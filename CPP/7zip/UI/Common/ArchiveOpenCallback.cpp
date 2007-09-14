// ArchiveOpenCallback.cpp

#include "StdAfx.h"

#include "ArchiveOpenCallback.h"

#include "Common/StringConvert.h"
#include "Common/ComTry.h"
#include "Windows/PropVariant.h"

#include "../../Common/FileStreams.h"

using namespace NWindows;

STDMETHODIMP COpenCallbackImp::SetTotal(const UInt64 *files, const UInt64 *bytes)
{
  COM_TRY_BEGIN
  if (!Callback)
    return S_OK;
  return Callback->SetTotal(files, bytes);
  COM_TRY_END
}

STDMETHODIMP COpenCallbackImp::SetCompleted(const UInt64 *files, const UInt64 *bytes)
{
  COM_TRY_BEGIN
  if (!Callback)
    return S_OK;
  return Callback->SetTotal(files, bytes);
  COM_TRY_END
}
  
STDMETHODIMP COpenCallbackImp::GetProperty(PROPID propID, PROPVARIANT *value)
{
  COM_TRY_BEGIN
  NCOM::CPropVariant propVariant;
  if (_subArchiveMode)
  {
    switch(propID)
    {
      case kpidName:
        propVariant = _subArchiveName;
        break;
    }
    propVariant.Detach(value);
    return S_OK;
  }
  switch(propID)
  {
    case kpidName:
      propVariant = _fileInfo.Name;
      break;
    case kpidIsFolder:
      propVariant = _fileInfo.IsDirectory();
      break;
    case kpidSize:
      propVariant = _fileInfo.Size;
      break;
    case kpidAttributes:
      propVariant = (UInt32)_fileInfo.Attributes;
      break;
    case kpidLastAccessTime:
      propVariant = _fileInfo.LastAccessTime;
      break;
    case kpidCreationTime:
      propVariant = _fileInfo.CreationTime;
      break;
    case kpidLastWriteTime:
      propVariant = _fileInfo.LastWriteTime;
      break;
    }
  propVariant.Detach(value);
  return S_OK;
  COM_TRY_END
}

int COpenCallbackImp::FindName(const UString &name)
{
  for (int i = 0; i < FileNames.Size(); i++)
    if (name.CompareNoCase(FileNames[i]) == 0)
      return i;
  return -1;
}

struct CInFileStreamVol: public CInFileStream
{
  UString Name;
  COpenCallbackImp *OpenCallbackImp;
  CMyComPtr<IArchiveOpenCallback> OpenCallbackRef;
  ~CInFileStreamVol()
  {
    int index = OpenCallbackImp->FindName(Name);
    if (index >= 0)
      OpenCallbackImp->FileNames.Delete(index);
  }
};

STDMETHODIMP COpenCallbackImp::GetStream(const wchar_t *name, IInStream **inStream)
{
  COM_TRY_BEGIN
  if (_subArchiveMode)
    return S_FALSE;
  if (Callback)
  {
    RINOK(Callback->CheckBreak());
  }
  *inStream = NULL;
  UString fullPath = _folderPrefix + name;
  if (!NFile::NFind::FindFile(fullPath, _fileInfo))
    return S_FALSE;
  if (_fileInfo.IsDirectory())
    return S_FALSE;
  CInFileStreamVol *inFile = new CInFileStreamVol;
  CMyComPtr<IInStream> inStreamTemp = inFile;
  if (!inFile->Open(fullPath))
    return ::GetLastError();
  *inStream = inStreamTemp.Detach();
  inFile->Name = name;
  inFile->OpenCallbackImp = this;
  inFile->OpenCallbackRef = this;
  FileNames.Add(name);
  TotalSize += _fileInfo.Size;
  return S_OK;
  COM_TRY_END
}

#ifndef _NO_CRYPTO
STDMETHODIMP COpenCallbackImp::CryptoGetTextPassword(BSTR *password)
{
  COM_TRY_BEGIN
  if (!Callback)
    return E_NOTIMPL;
  return Callback->CryptoGetTextPassword(password);
  COM_TRY_END
}
#endif
  
