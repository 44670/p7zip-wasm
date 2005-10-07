// BZip2Handler.cpp

#include "StdAfx.h"

#include "BZip2Handler.h"

#include "Common/Defs.h"

#include "../../Common/ProgressUtils.h"
#include "../../Common/StreamUtils.h"

#include "Windows/PropVariant.h"
#include "Windows/Defs.h"
#include "Common/ComTry.h"

#include "../Common/DummyOutStream.h"

#ifdef COMPRESS_BZIP2
#include "../../Compress/BZip2/BZip2Decoder.h"
#else
// {23170F69-40C1-278B-0402-020000000000}
DEFINE_GUID(CLSID_CCompressBZip2Decoder, 
0x23170F69, 0x40C1, 0x278B, 0x04, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00);
#include "../Common/CoderLoader.h"
extern CSysString GetBZip2CodecPath();
#endif

using namespace NWindows;

namespace NArchive {
namespace NBZip2 {

STATPROPSTG kProperties[] = 
{
  { NULL, kpidPath, VT_BSTR},
  // { NULL, kpidIsFolder, VT_BOOL},
  // { NULL, kpidSize, VT_UI8},
  { NULL, kpidPackedSize, VT_UI8},
};

STDMETHODIMP CHandler::GetArchiveProperty(PROPID propID, PROPVARIANT *value)
{
  value->vt = VT_EMPTY;
  return S_OK;
}

STDMETHODIMP CHandler::GetNumberOfProperties(UInt32 *numProperties)
{
  *numProperties = sizeof(kProperties) / sizeof(kProperties[0]);
  return S_OK;
}

STDMETHODIMP CHandler::GetPropertyInfo(UInt32 index,     
      BSTR *name, PROPID *propID, VARTYPE *varType)
{
  if(index >= sizeof(kProperties) / sizeof(kProperties[0]))
    return E_INVALIDARG;
  const STATPROPSTG &srcItem = kProperties[index];
  *propID = srcItem.propid;
  *varType = srcItem.vt;
  *name = 0;
  return S_OK;
}

STDMETHODIMP CHandler::GetNumberOfArchiveProperties(UInt32 *numProperties)
{
  *numProperties = 0;
  return S_OK;
}

STDMETHODIMP CHandler::GetArchivePropertyInfo(UInt32 index,     
      BSTR *name, PROPID *propID, VARTYPE *varType)
{
  return E_INVALIDARG;
}

STDMETHODIMP CHandler::GetNumberOfItems(UInt32 *numItems)
{
  *numItems = 1;
  return S_OK;
}

STDMETHODIMP CHandler::GetProperty(UInt32 index, PROPID propID,  PROPVARIANT *value)
{
  COM_TRY_BEGIN
  NWindows::NCOM::CPropVariant propVariant;
  if (index != 0)
    return E_INVALIDARG;
  switch(propID)
  {
    case kpidIsFolder:
      propVariant = false;
      break;
    case kpidPackedSize:
      propVariant = _item.PackSize;
      break;
  }
  propVariant.Detach(value);
  return S_OK;
  COM_TRY_END
}

STDMETHODIMP CHandler::Open(IInStream *stream, 
    const UInt64 *maxCheckStartPosition,
    IArchiveOpenCallback *openArchiveCallback)
{
  COM_TRY_BEGIN
  try
  {
    RINOK(stream->Seek(0, STREAM_SEEK_CUR, &_streamStartPosition));
    const int kSignatureSize = 3;
    Byte buffer[kSignatureSize];
    UInt32 processedSize;
    RINOK(ReadStream(stream, buffer, kSignatureSize, &processedSize));
    if (processedSize != kSignatureSize)
      return S_FALSE;
    if (buffer[0] != 'B' || buffer[1] != 'Z' || buffer[2] != 'h')
      return S_FALSE;

    UInt64 endPosition;
    RINOK(stream->Seek(0, STREAM_SEEK_END, &endPosition));
    _item.PackSize = endPosition - _streamStartPosition;
    
    _stream = stream;
  }
  catch(...)
  {
    return S_FALSE;
  }
  return S_OK;
  COM_TRY_END
}

STDMETHODIMP CHandler::Close()
{
  _stream.Release();
  return S_OK;
}


STDMETHODIMP CHandler::Extract(const UInt32* indices, UInt32 numItems,
    Int32 testModeSpec, IArchiveExtractCallback *extractCallback)
{
  COM_TRY_BEGIN
  bool allFilesMode = (numItems == UInt32(-1));
  if (!allFilesMode)
  {
    if (numItems == 0)
      return S_OK;
    if (numItems != 1)
      return E_INVALIDARG;
    if (indices[0] != 0)
      return E_INVALIDARG;
  }

  bool testMode = (testModeSpec != 0);

  extractCallback->SetTotal(_item.PackSize);

  UInt64 currentTotalPacked = 0, currentTotalUnPacked = 0;
  
  RINOK(extractCallback->SetCompleted(&currentTotalPacked));
  
  CMyComPtr<ISequentialOutStream> realOutStream;
  Int32 askMode;
  askMode = testMode ? NArchive::NExtract::NAskMode::kTest :
  NArchive::NExtract::NAskMode::kExtract;
  
  RINOK(extractCallback->GetStream(0, &realOutStream, askMode));
    
  if(!testMode && !realOutStream)
    return S_OK;


  extractCallback->PrepareOperation(askMode);

  #ifndef COMPRESS_BZIP2
  CCoderLibrary lib;
  #endif
  CMyComPtr<ICompressCoder> decoder;
  #ifdef COMPRESS_BZIP2
  decoder = new NCompress::NBZip2::CDecoder;
  #else
  HRESULT loadResult = lib.LoadAndCreateCoder(
      GetBZip2CodecPath(),
      CLSID_CCompressBZip2Decoder, &decoder);
  if (loadResult != S_OK)
  {
    RINOK(extractCallback->SetOperationResult(NArchive::NExtract::NOperationResult::kUnSupportedMethod));
    return S_OK;
  }
  #endif


  CDummyOutStream *outStreamSpec = new CDummyOutStream;
  CMyComPtr<ISequentialOutStream> outStream(outStreamSpec);
  outStreamSpec->Init(realOutStream);
  
  realOutStream.Release();

  CLocalProgress *localProgressSpec = new CLocalProgress;
  CMyComPtr<ICompressProgressInfo> progress = localProgressSpec;
  localProgressSpec->Init(extractCallback, true);

  CLocalCompressProgressInfo *localCompressProgressSpec = 
      new CLocalCompressProgressInfo;
  CMyComPtr<ICompressProgressInfo> compressProgress = localCompressProgressSpec;
  
  RINOK(_stream->Seek(_streamStartPosition, STREAM_SEEK_SET, NULL));


  HRESULT result;

  bool firstItem = true;
  while(true)
  {
    localCompressProgressSpec->Init(progress, 
      &currentTotalPacked,
      &currentTotalUnPacked);

    const int kSignatureSize = 3;
    Byte buffer[kSignatureSize];
    UInt32 processedSize;
    RINOK(ReadStream(_stream, buffer, kSignatureSize, &processedSize));
    if (processedSize < kSignatureSize)
    {
      if (firstItem)
        return E_FAIL;
      break;
    }
    if (buffer[0] != 'B' || buffer[1] != 'Z' || buffer[2] != 'h')
    {
      if (firstItem)
        return E_FAIL;
      outStream.Release();
      RINOK(extractCallback->SetOperationResult(NArchive::NExtract::NOperationResult::kOK))
      return S_OK;
    }
    firstItem = false;

    UInt64 dataStartPos;
    RINOK(_stream->Seek((UInt64)(Int64)(-3), STREAM_SEEK_CUR, &dataStartPos));

    result = decoder->Code(_stream, outStream, NULL, NULL, compressProgress);

    if (result != S_OK)
      break;

    CMyComPtr<ICompressGetInStreamProcessedSize> getInStreamProcessedSize;
    decoder.QueryInterface(IID_ICompressGetInStreamProcessedSize, 
        &getInStreamProcessedSize);
    if (!getInStreamProcessedSize)
      break;

    UInt64 packSize;
    RINOK(getInStreamProcessedSize->GetInStreamProcessedSize(&packSize));
    UInt64 pos;
    RINOK(_stream->Seek(dataStartPos + packSize, STREAM_SEEK_SET, &pos));
    currentTotalPacked = pos - _streamStartPosition;
  }
  outStream.Release();

  int retResult;
  if (result == S_OK)
    retResult = NArchive::NExtract::NOperationResult::kOK;
  else
    retResult = NArchive::NExtract::NOperationResult::kDataError;

  RINOK(extractCallback->SetOperationResult(retResult));
 
  return S_OK;
  COM_TRY_END
}

}}
