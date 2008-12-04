// SysIconUtils.h

#include "StdAfx.h"

#include "SysIconUtils.h"
#ifndef _UNICODE
#include "Common/StringConvert.h"
#endif

#ifndef _UNICODE
extern bool g_IsNT;
#endif

int GetIconIndexForCSIDL(int csidl)
{
#ifdef _WIN32
  LPITEMIDLIST pidl = 0;
  SHGetSpecialFolderLocation(NULL, csidl, &pidl);
  if (pidl)
  {
    SHFILEINFO shellInfo;
    SHGetFileInfo(LPCTSTR(pidl),  FILE_ATTRIBUTE_NORMAL,
      &shellInfo, sizeof(shellInfo),
      SHGFI_PIDL | SHGFI_SYSICONINDEX);
    IMalloc  *pMalloc;
    SHGetMalloc(&pMalloc);
    if(pMalloc)
    {
      pMalloc->Free(pidl);
      pMalloc->Release();
    }
    return shellInfo.iIcon;
  }
#endif // FIXME -1 ?
  return 0;
}

DWORD_PTR GetRealIconIndex(LPCTSTR path, DWORD attributes, int &iconIndex)
{
#ifdef _WIN32
  SHFILEINFO shellInfo;
  DWORD_PTR res = ::SHGetFileInfo(path, FILE_ATTRIBUTE_NORMAL | attributes, &shellInfo,
      sizeof(shellInfo), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX);
  iconIndex = shellInfo.iIcon;
  return res;
#else
  return -1;
#endif
}


#ifdef _WIN32

#ifndef _UNICODE
typedef int (WINAPI * SHGetFileInfoWP)(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW *psfi, UINT cbFileInfo, UINT uFlags);

struct CSHGetFileInfoInit
{
  SHGetFileInfoWP shGetFileInfoW;
  CSHGetFileInfoInit()
  {
    shGetFileInfoW = (SHGetFileInfoWP)
    ::GetProcAddress(::GetModuleHandleW(L"shell32.dll"), "SHGetFileInfoW");
  }
} g_SHGetFileInfoInit;
#endif

DWORD_PTR MySHGetFileInfoW(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW *psfi, UINT cbFileInfo, UINT uFlags)
{
  #ifdef _UNICODE
  return SHGetFileInfoW(
  #else
  if (g_SHGetFileInfoInit.shGetFileInfoW == 0)
    return 0;
  return g_SHGetFileInfoInit.shGetFileInfoW(
  #endif
  pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
}

#ifndef _UNICODE
// static inline UINT GetCurrentCodePage() { return ::AreFileApisANSI() ? CP_ACP : CP_OEMCP; }
DWORD_PTR GetRealIconIndex(LPCWSTR path, DWORD attributes, int &iconIndex)
{
  if(g_IsNT)
  {
    SHFILEINFOW shellInfo;
    DWORD_PTR res = ::MySHGetFileInfoW(path, FILE_ATTRIBUTE_NORMAL | attributes, &shellInfo,
      sizeof(shellInfo), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX);
    iconIndex = shellInfo.iIcon;
    return res;
  }
  else
    return GetRealIconIndex(UnicodeStringToMultiByte(path), attributes, iconIndex);
}
#endif

DWORD_PTR GetRealIconIndex(const UString &fileName, DWORD attributes,
    int &iconIndex, UString &typeName)
{
  #ifndef _UNICODE
  if(!g_IsNT)
  {
    SHFILEINFO shellInfo;
    shellInfo.szTypeName[0] = 0;
    DWORD_PTR res = ::SHGetFileInfoA(GetSystemString(fileName), FILE_ATTRIBUTE_NORMAL | attributes, &shellInfo,
      sizeof(shellInfo), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX
      | SHGFI_TYPENAME);
    typeName = GetUnicodeString(shellInfo.szTypeName);
    iconIndex = shellInfo.iIcon;
    return res;
  }
  else
  #endif
  {
    SHFILEINFOW shellInfo;
    shellInfo.szTypeName[0] = 0;
    DWORD_PTR res = ::MySHGetFileInfoW(fileName, FILE_ATTRIBUTE_NORMAL | attributes, &shellInfo,
      sizeof(shellInfo), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX
      | SHGFI_TYPENAME);
    typeName = shellInfo.szTypeName;
    iconIndex = shellInfo.iIcon;
    return res;
  }
}

int CExtToIconMap::GetIconIndex(UINT32 attributes, const UString &fileNameSpec, UString &typeName)
{
  UString fileName = fileNameSpec;
  if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
  {
    fileName = L"__Fldr__";
    if (_dirIconIndex < 0)
      GetRealIconIndex(fileName, attributes, _dirIconIndex, _dirTypeName);
    typeName = _dirTypeName;
    return _dirIconIndex;
  }
  int dotPos = fileName.ReverseFind('.');
  if (dotPos < 0)
  {
    fileName = L"__File__";
    if (_noExtIconIndex < 0)
    {
      int iconIndexTemp;
      GetRealIconIndex(fileName, attributes, iconIndexTemp, _noExtTypeName);
    }
    typeName = _noExtTypeName;
    return _noExtIconIndex;
  }
  CExtIconPair extIconPair;
  extIconPair.Ext = fileName.Mid(dotPos + 1);
  int anIndex = _map.FindInSorted(extIconPair);
  if (anIndex >= 0)
    return _map[anIndex].IconIndex;
  fileName = fileName.Mid(dotPos);
  GetRealIconIndex(fileName, attributes, extIconPair.IconIndex, extIconPair.TypeName);
  _map.AddToSorted(extIconPair);
  typeName = extIconPair.TypeName;
  return extIconPair.IconIndex;
}

int CExtToIconMap::GetIconIndex(UINT32 attributes, const UString &fileName)
{
  UString typeName;
  return GetIconIndex(attributes, fileName, typeName);
}
#endif

