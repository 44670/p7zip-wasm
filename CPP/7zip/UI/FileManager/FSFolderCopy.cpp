// FSFolderCopy.cpp

#include "StdAfx.h"

// FIXME #include <Winbase.h>

#include "FSFolder.h"
#include "Windows/FileDir.h"
#include "Windows/Error.h"

#include "Common/StringConvert.h"

#include "../../Common/FilePathAutoRename.h"

using namespace NWindows;
using namespace NFile;
using namespace NFind;

#ifndef _UNICODE
extern bool g_IsNT;
#endif

namespace NFsFolder {

/*
static bool IsItWindows2000orHigher()
{
  OSVERSIONINFO versionInfo;
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (!::GetVersionEx(&versionInfo))
    return false;
  return (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
      (versionInfo.dwMajorVersion >= 5);
}
*/

struct CProgressInfo
{
  UInt64 StartPos;
  IProgress *Progress;
};

#ifdef _WIN32

static DWORD CALLBACK CopyProgressRoutine(
  LARGE_INTEGER /* TotalFileSize */,          // file size
  LARGE_INTEGER TotalBytesTransferred,  // bytes transferred
  LARGE_INTEGER /* StreamSize */,             // bytes in stream
  LARGE_INTEGER /* StreamBytesTransferred */, // bytes transferred for stream
  DWORD /* dwStreamNumber */,                 // current stream
  DWORD /* dwCallbackReason */,               // callback reason
  HANDLE /* hSourceFile */,                   // handle to source file
  HANDLE /* hDestinationFile */,              // handle to destination file
  LPVOID lpData                         // from CopyFileEx
)
{
  CProgressInfo &progressInfo = *(CProgressInfo *)lpData;
  UInt64 completed = progressInfo.StartPos + TotalBytesTransferred.QuadPart;
  if (progressInfo.Progress->SetCompleted(&completed) != S_OK)
    return PROGRESS_CANCEL;
  return PROGRESS_CONTINUE;
}

typedef BOOL (WINAPI * CopyFileExPointer)(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN LPBOOL pbCancel OPTIONAL,
    IN DWORD dwCopyFlags
    );

typedef BOOL (WINAPI * CopyFileExPointerW)(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN LPBOOL pbCancel OPTIONAL,
    IN DWORD dwCopyFlags
    );

#ifndef _UNICODE
static inline UINT GetCurrentCodePage() { return ::AreFileApisANSI() ? CP_ACP : CP_OEMCP; }
static CSysString GetSysPath(LPCWSTR sysPath)
  { return UnicodeStringToMultiByte(sysPath, GetCurrentCodePage()); }
#endif

#endif

static bool MyCopyFile(LPCWSTR existingFile, LPCWSTR newFile, IProgress *progress, UInt64 &completedSize)
{
#ifdef _WIN32
  CProgressInfo progressInfo;
  progressInfo.Progress = progress;
  progressInfo.StartPos = completedSize;
  BOOL CancelFlag = FALSE;
  #ifndef _UNICODE
  if (g_IsNT)
  #endif
  {
    CopyFileExPointerW copyFunctionW = (CopyFileExPointerW)
        ::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"),
        "CopyFileExW");
    if (copyFunctionW == 0)
      return false;
    if (copyFunctionW(existingFile, newFile, CopyProgressRoutine,
        &progressInfo, &CancelFlag, COPY_FILE_FAIL_IF_EXISTS))
      return true;
    #ifdef WIN_LONG_PATH
    UString longPathExisting, longPathNew;
    if (!NDirectory::GetLongPaths(existingFile, newFile, longPathExisting, longPathNew))
      return false;
    if (copyFunctionW(longPathExisting, longPathNew, CopyProgressRoutine,
        &progressInfo, &CancelFlag, COPY_FILE_FAIL_IF_EXISTS))
      return true;
    #endif
    return false;
  }
  #ifndef _UNICODE
  else
  {
    CopyFileExPointer copyFunction = (CopyFileExPointer)
        ::GetProcAddress(::GetModuleHandleA("kernel32.dll"),
        "CopyFileExA");
    if (copyFunction != 0)
    {
      if (copyFunction(GetSysPath(existingFile), GetSysPath(newFile),
          CopyProgressRoutine,&progressInfo, &CancelFlag, COPY_FILE_FAIL_IF_EXISTS))
        return true;
      if (::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
        return false;
    }
    return BOOLToBool(::CopyFile(GetSysPath(existingFile), GetSysPath(newFile), TRUE));
  }
  #endif
#else
  extern bool wxw_CopyFile(LPCWSTR existingFile, LPCWSTR newFile, bool overwrite);
  return wxw_CopyFile(existingFile, newFile, true);
#endif
}

#ifdef _WIN32
typedef BOOL (WINAPI * MoveFileWithProgressPointer)(
    IN LPCWSTR lpExistingFileName,
    IN LPCWSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN DWORD dwFlags
    );
#endif

static bool MyMoveFile(LPCWSTR existingFile, LPCWSTR newFile, IProgress *progress, UInt64 &completedSize)
{
#ifdef _WIN32
  // if (IsItWindows2000orHigher())
  // {
    CProgressInfo progressInfo;
    progressInfo.Progress = progress;
    progressInfo.StartPos = completedSize;

    MoveFileWithProgressPointer moveFunction = (MoveFileWithProgressPointer)
        ::GetProcAddress(::GetModuleHandle(TEXT("kernel32.dll")),
        "MoveFileWithProgressW");
    if (moveFunction != 0)
    {
      if (moveFunction(
          existingFile, newFile, CopyProgressRoutine,
          &progressInfo, MOVEFILE_COPY_ALLOWED))
        return true;
      if (::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
      {
        #ifdef WIN_LONG_PATH
        UString longPathExisting, longPathNew;
        if (!NDirectory::GetLongPaths(existingFile, newFile, longPathExisting, longPathNew))
          return false;
        if (moveFunction(longPathExisting, longPathNew, CopyProgressRoutine,
            &progressInfo, MOVEFILE_COPY_ALLOWED))
          return true;
        #endif
        if (::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
          return false;
      }
    }
  // }
  // else
#endif
    return NDirectory::MyMoveFile(existingFile, newFile);
}

static HRESULT MyCopyFile(
    const UString &srcPath,
    const CFileInfoW &srcFileInfo,
    const UString &destPathSpec,
    IFolderOperationsExtractCallback *callback,
    UInt64 &completedSize)
{
  UString destPath = destPathSpec;
  if (destPath.CompareNoCase(srcPath) == 0)
  {
    UString message = UString(L"can not move file \'") + destPath + UString(L"\' onto itself");
    RINOK(callback->ShowMessage(message));
    return E_ABORT;
  }

  INT32 writeAskResult;
  CMyComBSTR destPathResult;
  RINOK(callback->AskWrite(
      srcPath,
      BoolToInt(false),
      &srcFileInfo.MTime, &srcFileInfo.Size,
      destPath,
      &destPathResult,
      &writeAskResult));
  if (IntToBool(writeAskResult))
  {
    UString destPathNew = UString(destPathResult);
    RINOK(callback->SetCurrentFilePath(srcPath));
    if (!MyCopyFile(srcPath, destPathNew, callback, completedSize))
    {
      UString message = NError::MyFormatMessageW(GetLastError()) +
        UString(L" \'") +
        UString(destPathNew) +
        UString(L"\'");
      RINOK(callback->ShowMessage(message));
      return E_ABORT;
    }
  }
  completedSize += srcFileInfo.Size;
  return callback->SetCompleted(&completedSize);
}

static HRESULT CopyFolder(
    const UString &srcPath,
    const UString &destPathSpec,
    IFolderOperationsExtractCallback *callback,
    UInt64 &completedSize)
{
  RINOK(callback->SetCompleted(&completedSize));

  UString destPath = destPathSpec;
  int len = srcPath.Length();
  if (destPath.Length() >= len && srcPath.CompareNoCase(destPath.Left(len)) == 0)
  {
    if (destPath.Length() == len || destPath[len] == WCHAR_PATH_SEPARATOR)
    {
      UString message = UString(L"can not copy folder \'") +
          destPath + UString(L"\' onto itself");
      RINOK(callback->ShowMessage(message));
      return E_ABORT;
    }
  }

  if (!NDirectory::CreateComplexDirectory(destPath))
  {
    UString message = UString(L"can not create folder ") + destPath;
    RINOK(callback->ShowMessage(message));
    return E_ABORT;
  }
  CEnumeratorW enumerator(srcPath + UString(WSTRING_PATH_SEPARATOR L"*"));
  CFileInfoEx fileInfo;
  while (enumerator.Next(fileInfo))
  {
    const UString srcPath2 = srcPath + UString(WSTRING_PATH_SEPARATOR) + fileInfo.Name;
    const UString destPath2 = destPath + UString(WSTRING_PATH_SEPARATOR) + fileInfo.Name;
    if (fileInfo.IsDir())
    {
      RINOK(CopyFolder(srcPath2, destPath2, callback, completedSize));
    }
    else
    {
      RINOK(MyCopyFile(srcPath2, fileInfo, destPath2, callback, completedSize));
    }
  }
  return S_OK;
}


STDMETHODIMP CFSFolder::CopyTo(const UInt32 *indices, UInt32 numItems,
    const wchar_t *path, IFolderOperationsExtractCallback *callback)
{
  if (numItems == 0)
    return S_OK;
  
  UInt64 numFolders, numFiles, totalSize;
  GetItemsFullSize(indices, numItems, numFolders, numFiles, totalSize, callback);
  RINOK(callback->SetTotal(totalSize));
  RINOK(callback->SetNumFiles(numFiles));
  
  UString destPath = path;
  if (destPath.IsEmpty())
    return E_INVALIDARG;
  bool directName = (destPath[destPath.Length() - 1] != WCHAR_PATH_SEPARATOR);
  if (directName)
  {
    if (numItems > 1)
      return E_INVALIDARG;
  }
    /*
    // doesn't work in network
  else
    if (!NDirectory::CreateComplexDirectory(destPath)))
    {
      DWORD lastError = ::GetLastError();
      UString message = UString(L"can not create folder ") +
        destPath;
      RINOK(callback->ShowMessage(message));
      return E_ABORT;
    }
    */

  UInt64 completedSize = 0;
  RINOK(callback->SetCompleted(&completedSize));
  for (UInt32 i = 0; i < numItems; i++)
  {
    const CDirItem &fileInfo = *_refs[indices[i]];
    UString destPath2 = destPath;
    if (!directName)
      destPath2 += fileInfo.Name;
    UString srcPath = _path + GetPrefix(fileInfo) + fileInfo.Name;
    if (fileInfo.IsDir())
    {
      RINOK(CopyFolder(srcPath, destPath2, callback, completedSize));
    }
    else
    {
      RINOK(MyCopyFile(srcPath, fileInfo, destPath2, callback, completedSize));
    }
  }
  return S_OK;
}

/////////////////////////////////////////////////
// Move Operations

HRESULT MyMoveFile(
    const UString &srcPath,
    const CFileInfoW &srcFileInfo,
    const UString &destPathSpec,
    IFolderOperationsExtractCallback *callback,
    UInt64 &completedSize)
{
  UString destPath = destPathSpec;
  if (destPath.CompareNoCase(srcPath) == 0)
  {
    UString message = UString(L"can not move file \'")
         + destPath +
        UString(L"\' onto itself");
        RINOK(callback->ShowMessage(message));
    return E_ABORT;
  }

  INT32 writeAskResult;
  CMyComBSTR destPathResult;
  RINOK(callback->AskWrite(
      srcPath,
      BoolToInt(false),
      &srcFileInfo.MTime, &srcFileInfo.Size,
      destPath,
      &destPathResult,
      &writeAskResult));
  if (IntToBool(writeAskResult))
  {
    UString destPathNew = UString(destPathResult);
    RINOK(callback->SetCurrentFilePath(srcPath));
    if (!MyMoveFile(srcPath, destPathNew, callback, completedSize))
    {
      UString message = UString(L"can not move to file ") + destPathNew;
      RINOK(callback->ShowMessage(message));
    }
  }
  completedSize += srcFileInfo.Size;
  RINOK(callback->SetCompleted(&completedSize));
  return S_OK;
}

HRESULT MyMoveFolder(
    const UString &srcPath,
    const UString &destPathSpec,
    IFolderOperationsExtractCallback *callback,
    UInt64 &completedSize)
{
  UString destPath = destPathSpec;
  int len = srcPath.Length();
  if (destPath.Length() >= len && srcPath.CompareNoCase(destPath.Left(len)) == 0)
  {
    if (destPath.Length() == len || destPath[len] == WCHAR_PATH_SEPARATOR)
    {
      UString message = UString(L"can not move folder \'") +
          destPath + UString(L"\' onto itself");
      RINOK(callback->ShowMessage(message));
      return E_ABORT;
    }
  }

  if (MyMoveFile(srcPath, destPath, callback, completedSize))
    return S_OK;

  if (!NDirectory::CreateComplexDirectory(destPath))
  {
    UString message = UString(L"can not create folder ") +  destPath;
    RINOK(callback->ShowMessage(message));
    return E_ABORT;
  }
  {
    CEnumeratorW enumerator(srcPath + UString(WSTRING_PATH_SEPARATOR L"*"));
    CFileInfoEx fileInfo;
    while (enumerator.Next(fileInfo))
    {
      const UString srcPath2 = srcPath + UString(WSTRING_PATH_SEPARATOR) + fileInfo.Name;
      const UString destPath2 = destPath + UString(WSTRING_PATH_SEPARATOR) + fileInfo.Name;
      if (fileInfo.IsDir())
      {
        RINOK(MyMoveFolder(srcPath2, destPath2, callback, completedSize));
      }
      else
      {
        RINOK(MyMoveFile(srcPath2, fileInfo, destPath2, callback, completedSize));
      }
    }
  }
  if (!NDirectory::MyRemoveDirectory(srcPath))
  {
    UString message = UString(L"can not remove folder") + srcPath;
    RINOK(callback->ShowMessage(message));
    return E_ABORT;
  }
  return S_OK;
}

STDMETHODIMP CFSFolder::MoveTo(
    const UInt32 *indices,
    UInt32 numItems,
    const wchar_t *path,
    IFolderOperationsExtractCallback *callback)
{
  if (numItems == 0)
    return S_OK;

  UInt64 numFolders, numFiles, totalSize;
  GetItemsFullSize(indices, numItems, numFolders, numFiles, totalSize, callback);
  RINOK(callback->SetTotal(totalSize));
  RINOK(callback->SetNumFiles(numFiles));

  UString destPath = path;
  if (destPath.IsEmpty())
    return E_INVALIDARG;
  bool directName = (destPath[destPath.Length() - 1] != WCHAR_PATH_SEPARATOR);
  if (directName)
  {
    if (numItems > 1)
      return E_INVALIDARG;
  }
  else
    if (!NDirectory::CreateComplexDirectory(destPath))
    {
      UString message = UString(L"can not create folder ") +
        destPath;
      RINOK(callback->ShowMessage(message));
      return E_ABORT;
    }

  UInt64 completedSize = 0;
  RINOK(callback->SetCompleted(&completedSize));
  for (UInt32 i = 0; i < numItems; i++)
  {
    const CDirItem &fileInfo = *_refs[indices[i]];
    UString destPath2 = destPath;
    if (!directName)
      destPath2 += fileInfo.Name;
    UString srcPath = _path + GetPrefix(fileInfo) + fileInfo.Name;
    if (fileInfo.IsDir())
    {
      RINOK(MyMoveFolder(srcPath, destPath2, callback, completedSize));
    }
    else
    {
      RINOK(MyMoveFile(srcPath, fileInfo, destPath2, callback, completedSize));
    }
  }
  return S_OK;
}

STDMETHODIMP CFSFolder::CopyFrom(const wchar_t * /* fromFolderPath */,
    const wchar_t ** /* itemsPaths */, UInt32 /* numItems */, IProgress * /* progress */)
{
  /*
  UInt64 numFolders, numFiles, totalSize;
  numFiles = numFolders = totalSize = 0;
  UInt32 i;
  for (i = 0; i < numItems; i++)
  {
    UString path = (UString)fromFolderPath + itemsPaths[i];

    CFileInfoW fileInfo;
    if (!FindFile(path, fileInfo))
      return ::GetLastError();
    if (fileInfo.IsDir())
    {
      UInt64 subFolders, subFiles, subSize;
      RINOK(GetFolderSize(path + UString(WSTRING_PATH_SEPARATOR) + fileInfo.Name, subFolders, subFiles, subSize, progress));
      numFolders += subFolders;
      numFolders++;
      numFiles += subFiles;
      totalSize += subSize;
    }
    else
    {
      numFiles++;
      totalSize += fileInfo.Size;
    }
  }
  RINOK(progress->SetTotal(totalSize));
  RINOK(callback->SetNumFiles(numFiles));
  for (i = 0; i < numItems; i++)
  {
    UString path = (UString)fromFolderPath + itemsPaths[i];
  }
  return S_OK;
  */
  return E_NOTIMPL;
}
  
}
