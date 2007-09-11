// Tar/Handler.cpp

#include "StdAfx.h"

#include "TarHandler.h"
#include "TarIn.h"

#include "Common/Defs.h"
#include "Common/StringConvert.h"
#include "Common/NewHandler.h"
#include "Common/ComTry.h"

#include "Windows/Time.h"
#include "Windows/PropVariant.h"

#include "../../Common/ProgressUtils.h"
#include "../../Common/LimitedStreams.h"

#include "../../Compress/Copy/CopyCoder.h"

#include "../Common/ItemNameUtils.h"

using namespace NWindows;
using namespace NTime;

namespace NArchive {
namespace NTar {

STATPROPSTG kProps[] = 
{
  { NULL, kpidPath, VT_BSTR},
  { NULL, kpidIsFolder, VT_BOOL},
  { NULL, kpidSize, VT_UI8},
  { NULL, kpidPackedSize, VT_UI8},
  { NULL, kpidLastWriteTime, VT_FILETIME},
  { NULL, kpidUser, VT_BSTR},
  { NULL, kpidGroup, VT_BSTR},
};









IMP_IInArchive_Props
IMP_IInArchive_ArcProps_NO

STDMETHODIMP CHandler::Open(IInStream *stream, 
    const UInt64 * /* maxCheckStartPosition */,
    IArchiveOpenCallback *openArchiveCallback)
{
  COM_TRY_BEGIN
  // try
  {
    CInArchive archive;

    if (archive.Open(stream) != S_OK)
      return S_FALSE;

    _items.Clear();

    if (openArchiveCallback != NULL)
    {
      RINOK(openArchiveCallback->SetTotal(NULL, NULL));
      UInt64 numFiles = _items.Size();
      RINOK(openArchiveCallback->SetCompleted(&numFiles, NULL));
    }

    for (;;)
    {
      CItemEx item;
      bool filled;
      HRESULT result = archive.GetNextItem(filled, item);
      if (result == S_FALSE)
        return S_FALSE;
      if (result != S_OK)
        return S_FALSE;
      if (!filled)
        break;
      _items.Add(item);
      archive.SkeepDataRecords(item.Size);
      if (openArchiveCallback != NULL)
      {
        UInt64 numFiles = _items.Size();
        RINOK(openArchiveCallback->SetCompleted(&numFiles, NULL));
      }
    }
    if (_items.Size() == 0)
    {
      CMyComPtr<IArchiveOpenVolumeCallback> openVolumeCallback;
      if (!openArchiveCallback)
        return S_FALSE;
      openArchiveCallback->QueryInterface(IID_IArchiveOpenVolumeCallback, (void **)&openVolumeCallback);
      if (!openVolumeCallback)
        return S_FALSE;
      NCOM::CPropVariant prop;
      RINOK(openVolumeCallback->GetProperty(kpidName, &prop));
      if (prop.vt != VT_BSTR)
        return S_FALSE;
      UString baseName = prop.bstrVal;
      baseName = baseName.Right(4);
      if (baseName.CompareNoCase(L".tar") != 0)
        return S_FALSE;
    }
    _inStream = stream;
  }
  /*
  catch(...)
  {
    return S_FALSE;
  }
  */
  return S_OK;
  COM_TRY_END
}

STDMETHODIMP CHandler::Close()
{
  _items.Clear();
  _inStream.Release();
  return S_OK;
}

STDMETHODIMP CHandler::GetNumberOfItems(UInt32 *numItems)
{
  *numItems = _items.Size();
  return S_OK;
}

STDMETHODIMP CHandler::GetProperty(UInt32 index, PROPID propID, PROPVARIANT *value)
{
  COM_TRY_BEGIN
  NWindows::NCOM::CPropVariant prop;
  const CItemEx &item = _items[index];

  switch(propID)
  {
    case kpidPath:
      prop = (const wchar_t *)NItemName::GetOSName2(
          MultiByteToUnicodeString(item.Name, CP_OEMCP));
      break;
    case kpidIsFolder:
      prop = item.IsDirectory();
      break;
    case kpidSize:
    case kpidPackedSize:
      prop = (UInt64)item.Size;
      break;
    case kpidLastWriteTime:
    {
      FILETIME utcFileTime;
      if (item.ModificationTime != 0)
        NTime::UnixTimeToFileTime(item.ModificationTime, utcFileTime);
      else
      {
        utcFileTime.dwLowDateTime = 0;
        utcFileTime.dwHighDateTime = 0;
      }
      prop = utcFileTime;
      break;
    }
    case kpidUser:
      prop = (const wchar_t *)
          MultiByteToUnicodeString(item.UserName, CP_OEMCP);
      break;
    case kpidGroup:
      prop = (const wchar_t *)
          MultiByteToUnicodeString(item.GroupName, CP_OEMCP);
      break;
  }
  prop.Detach(value);
  return S_OK;
  COM_TRY_END
}

STDMETHODIMP CHandler::Extract(const UInt32* indices, UInt32 numItems,
    Int32 _aTestMode, IArchiveExtractCallback *extractCallback)
{
  COM_TRY_BEGIN
  bool testMode = (_aTestMode != 0);
  bool allFilesMode = (numItems == UInt32(-1));
  if (allFilesMode)
    numItems = _items.Size();
  if (numItems == 0)
    return S_OK;
  UInt64 totalSize = 0;
  UInt32 i;
  for (i = 0; i < numItems; i++)
    totalSize += _items[allFilesMode ? i : indices[i]].Size;
  extractCallback->SetTotal(totalSize);

  UInt64 currentTotalSize = 0;
  UInt64 currentItemSize;
  
  NCompress::CCopyCoder *copyCoderSpec = new NCompress::CCopyCoder();
  CMyComPtr<ICompressCoder> copyCoder = copyCoderSpec;

  CLocalProgress *lps = new CLocalProgress;
  CMyComPtr<ICompressProgressInfo> progress = lps;
  lps->Init(extractCallback, false);

  CLimitedSequentialInStream *streamSpec = new CLimitedSequentialInStream;
  CMyComPtr<ISequentialInStream> inStream(streamSpec);
  streamSpec->SetStream(_inStream);

  for (i = 0; i < numItems; i++, currentTotalSize += currentItemSize)
  {
    lps->InSize = lps->OutSize = currentTotalSize;
    RINOK(lps->SetCur());
    CMyComPtr<ISequentialOutStream> realOutStream;
    Int32 askMode = testMode ? 
        NArchive::NExtract::NAskMode::kTest :
        NArchive::NExtract::NAskMode::kExtract;
    Int32 index = allFilesMode ? i : indices[i];
    const CItemEx &item = _items[index];
    RINOK(extractCallback->GetStream(index, &realOutStream, askMode));
    currentItemSize = item.Size;
    if (item.IsDirectory())
    {
      RINOK(extractCallback->PrepareOperation(askMode));
      RINOK(extractCallback->SetOperationResult(NArchive::NExtract::NOperationResult::kOK));
      continue;
    }
    if (!testMode && (!realOutStream))
      continue;
    RINOK(extractCallback->PrepareOperation(askMode));
    if (testMode)
    {
      RINOK(extractCallback->SetOperationResult(NArchive::NExtract::NOperationResult::kOK));
      continue;
    }
    RINOK(_inStream->Seek(item.GetDataPosition(), STREAM_SEEK_SET, NULL));
    streamSpec->Init(item.Size);
    RINOK(copyCoder->Code(inStream, realOutStream, NULL, NULL, progress));
    realOutStream.Release();
    RINOK(extractCallback->SetOperationResult((copyCoderSpec->TotalSize == item.Size) ? 
        NArchive::NExtract::NOperationResult::kOK:
        NArchive::NExtract::NOperationResult::kDataError));
  }
  return S_OK;
  COM_TRY_END
}

}}
