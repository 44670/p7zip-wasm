// Archive/GZip/OutHandler.cpp

#include "StdAfx.h"

#include "GZipHandler.h"
#include "GZipUpdate.h"

#include "Common/StringConvert.h"
#include "Common/StringToInt.h"

#include "Windows/Time.h"
#include "Windows/FileFind.h"
#include "Windows/PropVariant.h"

#include "../../Compress/Copy/CopyCoder.h"

using namespace NWindows;
using namespace NTime;

namespace NArchive {
namespace NGZip {

STDMETHODIMP CHandler::GetFileTimeType(UInt32 *timeType)
{
  *timeType = NFileTimeType::kUnix;
  return S_OK;
}

static HRESULT CopyStreams(ISequentialInStream *inStream, 
    ISequentialOutStream *outStream, IArchiveUpdateCallback *updateCallback)
{
  CMyComPtr<ICompressCoder> copyCoder = new NCompress::CCopyCoder;
  return copyCoder->Code(inStream, outStream, NULL, NULL, NULL);
}

STDMETHODIMP CHandler::UpdateItems(ISequentialOutStream *outStream, UInt32 numItems,
    IArchiveUpdateCallback *updateCallback)
{
  if (numItems != 1)
    return E_INVALIDARG;

  UInt64 size;
  Int32 newData;
  Int32 newProperties;
  UInt32 indexInArchive;
  UInt32 itemIndex = 0;
  if (!updateCallback)
    return E_FAIL;
  RINOK(updateCallback->GetUpdateItemInfo(0, 
      &newData, &newProperties, &indexInArchive));

  CItem newItem = m_Item;
  newItem.ExtraFlags = 0;
  newItem.Flags = 0;
  if (IntToBool(newProperties))
  {
    UInt32 attributes;
    FILETIME utcTime;
    UString name;
    bool isDirectory;
    {
      NCOM::CPropVariant propVariant;
      RINOK(updateCallback->GetProperty(itemIndex, kpidAttributes, &propVariant));
      if (propVariant.vt == VT_EMPTY)
        attributes = 0;
      else if (propVariant.vt != VT_UI4)
        return E_INVALIDARG;
      else
        attributes = propVariant.ulVal;
    }
    {
      NCOM::CPropVariant propVariant;
      RINOK(updateCallback->GetProperty(itemIndex, kpidLastWriteTime, &propVariant));
      if (propVariant.vt != VT_FILETIME)
        return E_INVALIDARG;
      utcTime = propVariant.filetime;
    }
    {
      NCOM::CPropVariant propVariant;
      RINOK(updateCallback->GetProperty(itemIndex, kpidPath, &propVariant));
      if (propVariant.vt == VT_EMPTY)
        name.Empty();
      else if (propVariant.vt != VT_BSTR)
        return E_INVALIDARG;
      else
        name = propVariant.bstrVal;
    }
    {
      NCOM::CPropVariant propVariant;
      RINOK(updateCallback->GetProperty(itemIndex, kpidIsFolder, &propVariant));
      if (propVariant.vt == VT_EMPTY)
        isDirectory = false;
      else if (propVariant.vt != VT_BOOL)
        return E_INVALIDARG;
      else
        isDirectory = (propVariant.boolVal != VARIANT_FALSE);
    }
    if (isDirectory || NFile::NFind::NAttributes::IsDirectory(attributes))
      return E_INVALIDARG;
    if(!FileTimeToUnixTime(utcTime, newItem.Time))
      return E_INVALIDARG;
    newItem.Name = UnicodeStringToMultiByte(name, CP_ACP);
    int dirDelimiterPos = newItem.Name.ReverseFind(CHAR_PATH_SEPARATOR);
    if (dirDelimiterPos >= 0)
      newItem.Name = newItem.Name.Mid(dirDelimiterPos + 1);
    
    newItem.SetNameIsPresentFlag(!newItem.Name.IsEmpty());
  }

  if (IntToBool(newData))
  {
    {
      NCOM::CPropVariant propVariant;
      RINOK(updateCallback->GetProperty(itemIndex, kpidSize, &propVariant));
      if (propVariant.vt != VT_UI8)
        return E_INVALIDARG;
      size = propVariant.uhVal.QuadPart;
    }
    newItem.UnPackSize32 = (UInt32)size;
    return UpdateArchive(m_Stream, size, outStream, newItem, 
        m_Method, itemIndex, updateCallback);
  }
    
  if (indexInArchive != 0)
    return E_INVALIDARG;

  if (IntToBool(newProperties))
  {
    COutArchive outArchive;
    outArchive.Create(outStream);
    outArchive.WriteHeader(newItem);
    RINOK(m_Stream->Seek(m_StreamStartPosition + m_DataOffset, STREAM_SEEK_SET, NULL));
  }
  else
  {
    RINOK(m_Stream->Seek(m_StreamStartPosition, STREAM_SEEK_SET, NULL));
  }
  return CopyStreams(m_Stream, outStream, updateCallback);
}

static const UInt32 kMatchFastLenNormal  = 32;
static const UInt32 kMatchFastLenMX  = 64;

static const UInt32 kNumPassesNormal = 1;
static const UInt32 kNumPassesMX  = 3;

STDMETHODIMP CHandler::SetProperties(const wchar_t **names, const PROPVARIANT *values, Int32 numProperties)
{
  InitMethodProperties();
  for (int i = 0; i < numProperties; i++)
  {
    UString name = UString(names[i]);
    name.MakeUpper();
    const PROPVARIANT &value = values[i];
    if (name[0] == 'X')
    {
      name.Delete(0);
      UInt32 level = 9;
      if (value.vt == VT_UI4)
      {
        if (!name.IsEmpty())
          return E_INVALIDARG;
        level = value.ulVal;
      }
      else if (value.vt == VT_EMPTY)
      {
        if(!name.IsEmpty())
        {
          const wchar_t *start = name;
          const wchar_t *end;
          UInt64 v = ConvertStringToUInt64(start, &end);
          if (end - start != name.Length())
            return E_INVALIDARG;
          level = (UInt32)v;
        }
      }
      else
        return E_INVALIDARG;
      if (level < 7)
      {
        InitMethodProperties();
      }
      else
      {
        m_Method.NumPasses = kNumPassesMX;
        m_Method.NumFastBytes = kMatchFastLenMX;
      }
      continue;
    }
    else if (name == L"PASS")
    {
      if (value.vt != VT_UI4)
        return E_INVALIDARG;
      m_Method.NumPasses = value.ulVal;
      if (m_Method.NumPasses < 1 || m_Method.NumPasses > 10)
        return E_INVALIDARG;
    }
    else if (name == L"FB")
    {
      if (value.vt != VT_UI4)
        return E_INVALIDARG;
      m_Method.NumFastBytes = value.ulVal;
      /*
      if (m_Method.NumFastBytes < 3 || m_Method.NumFastBytes > 255)
        return E_INVALIDARG;
      */
    }
    else
      return E_INVALIDARG;
  }
  return S_OK;
}  

}}
