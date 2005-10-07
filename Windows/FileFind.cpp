// Windows/FileFind.cpp

#include "StdAfx.h"

#include "FileFind.h"
#ifndef _UNICODE
#include "../Common/StringConvert.h"
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_LSTAT
extern int global_use_lstat;
int global_use_lstat=1; // default behaviour : p7zip stores symlinks instead of dumping the files they point to
#endif

extern void my_windows_split_path(const AString &p_path, AString &dir , AString &base);

#define NEED_NAME_WINDOWS_TO_UNIX
#include "myPrivate.h"

// #define TRACEN(u) u;
#define TRACEN(u)  /* */

static int filter_pattern(const char *string , const char *pattern , int flags_nocase) {
  if ((string == 0) || (*string==0)) {
    if (pattern == 0)
      return 1;
    while (*pattern=='*')
      ++pattern;
    return (!*pattern);
  }

  switch (*pattern) {
  case '*':
    if (!filter_pattern(string+1,pattern,flags_nocase))
      return filter_pattern(string,pattern+1,flags_nocase);
    return 1;
  case 0:
    if (*string==0)
      return 1;
    break;
  case '?':
    return filter_pattern(string+1,pattern+1,flags_nocase);
  default:
    if (   ((flags_nocase) && (tolower(*pattern)==tolower(*string)))
           || (*pattern == *string)
       ) {
      return filter_pattern(string+1,pattern+1,flags_nocase);
    }
    break;
  }
  return 0;
}


namespace NWindows {
namespace NFile {
namespace NFind {

static const TCHAR kDot = TEXT('.');

bool CFileInfo::IsDots() const
{ 
  if (!IsDirectory() || Name.IsEmpty())
    return false;
  if (Name[0] != kDot)
    return false;
  return Name.Length() == 1 || (Name[1] == kDot && Name.Length() == 2);
}

#ifndef _UNICODE
bool CFileInfoW::IsDots() const
{ 
  if (!IsDirectory() || Name.IsEmpty())
    return false;
  if (Name[0] != kDot)
    return false;
  return Name.Length() == 1 || (Name[1] == kDot && Name.Length() == 2);
}
#endif

static int fillin_CFileInfo(CFileInfo &fileInfo,const char *dir,const char *name) {
  struct stat stat_info;
  char filename[MAX_PATHNAME_LEN];

  size_t dir_len = strlen(dir);
  size_t name_len = strlen(name);
  size_t total = dir_len + 1 + name_len; // 1 = strlen("/");
  if (total >= MAX_PATHNAME_LEN) throw "fillin_CFileInfo - internal error - MAX_PATHNAME_LEN";
  memcpy(filename,dir,dir_len);
  filename[dir_len] = CHAR_PATH_SEPARATOR;
  memcpy(filename+(dir_len+1),name,name_len+1); // copy also final '\0'

  int ret;
#ifdef HAVE_LSTAT
  if (global_use_lstat) {
    ret = lstat(filename,&stat_info);
  } else
#endif
  {
     ret = stat(filename,&stat_info);
  }
  if (ret != 0) {
	AString err_msg = "stat error for ";
        err_msg += filename;
        err_msg += " (";
        err_msg += strerror(errno);
        err_msg += ")";
        throw err_msg;
  }

  /* FIXME : FILE_ATTRIBUTE_HIDDEN ? */
  if (S_ISDIR(stat_info.st_mode)) {
    fileInfo.Attributes = FILE_ATTRIBUTE_DIRECTORY;
  } else {
    fileInfo.Attributes = FILE_ATTRIBUTE_ARCHIVE;
  }

  if (!(stat_info.st_mode & S_IWUSR))
    fileInfo.Attributes |= FILE_ATTRIBUTE_READONLY;

  fileInfo.Attributes |= FILE_ATTRIBUTE_UNIX_EXTENSION + ((stat_info.st_mode & 0xFFFF) << 16);

  RtlSecondsSince1970ToFileTime( stat_info.st_ctime, &fileInfo.CreationTime );
  RtlSecondsSince1970ToFileTime( stat_info.st_mtime, &fileInfo.LastWriteTime );
  RtlSecondsSince1970ToFileTime( stat_info.st_atime, &fileInfo.LastAccessTime );

  if (S_ISDIR(stat_info.st_mode)) {
    fileInfo.Size = 0;
  } else { // file or symbolic link
    fileInfo.Size = stat_info.st_size; // for a symbolic link, size = size of filename
  }
  fileInfo.Name = name;
  return 0;
}

////////////////////////////////
// CFindFile

bool CFindFile::Close()
{

  if(_dirp == 0)
    return true;
  int ret = closedir(_dirp);
  if (ret == 0)
  {
    _dirp = 0;
    return true;
  }
  return false;
}
           
bool CFindFile::FindFirst(LPCTSTR wildcard, CFileInfo &fileInfo)
{
  Close();

  if ((!wildcard) || (wildcard[0]==0)) {
    SetLastError(ERROR_PATH_NOT_FOUND);
    return false;
  }
 
  my_windows_split_path(nameWindowToUnix(wildcard),_directory,_pattern);
  
  TRACEN((printf("CFindFile::FindFirst : %s (dirname=%s,pattern=%s)\n",wildcard,(const char *)_directory,(const char *)_pattern)))

  _dirp = ::opendir((const char *)_directory);
  TRACEN((printf("CFindFile::FindFirst : _dirp=%p\n",_dirp)))

  if (_dirp == 0) return false;

  struct dirent *dp;
  while ((dp = readdir(_dirp)) != NULL) {
    if (filter_pattern(dp->d_name,(const char *)_pattern,0) == 1) {
      int retf = fillin_CFileInfo(fileInfo,(const char *)_directory,dp->d_name);
      if (retf)
      {
         TRACEN((printf("CFindFile::FindFirst : closedir-1(dirp=%p)\n",_dirp)))
         closedir(_dirp);
         _dirp = 0;
         SetLastError( ERROR_NO_MORE_FILES );
         return false;
      }
      TRACEN((printf("CFindFile::FindFirst -%s- true\n",dp->d_name)))
      return true;
    }
  }

  TRACEN((printf("CFindFile::FindFirst : closedir-2(dirp=%p)\n",_dirp)))
  closedir(_dirp);
  _dirp = 0;
  SetLastError( ERROR_NO_MORE_FILES );
  return false;
}

bool CFindFile::FindFirst(LPCWSTR wildcard, CFileInfoW &fileInfo)
{
  Close();
  CFileInfo fileInfo0;
  bool bret = FindFirst((LPCTSTR)UnicodeStringToMultiByte(wildcard, CP_ACP), fileInfo0);
  if (bret)
  {
     fileInfo.Attributes = fileInfo0.Attributes;
     fileInfo.CreationTime = fileInfo0.CreationTime;
     fileInfo.LastAccessTime = fileInfo0.LastAccessTime;
     fileInfo.LastWriteTime = fileInfo0.LastWriteTime;
     fileInfo.Size = fileInfo0.Size;
     fileInfo.Name = GetUnicodeString(fileInfo0.Name, CP_ACP);
  }
  return bret;
}

bool CFindFile::FindNext(CFileInfo &fileInfo)
{
  if (_dirp == 0)
  {
    SetLastError( ERROR_INVALID_HANDLE );
    return false;
  }

  struct dirent *dp;
  while ((dp = readdir(_dirp)) != NULL) {
      if (filter_pattern(dp->d_name,(const char *)_pattern,0) == 1) {
        int retf = fillin_CFileInfo(fileInfo,(const char *)_directory,dp->d_name);
        if (retf)
        {
           TRACEN((printf("FindNextFileA -%s- ret_handle=FALSE (errno=%d)\n",dp->d_name,errno)))
           return false;

        }
        TRACEN((printf("FindNextFileA -%s- true\n",dp->d_name)))
        return true;
      }
    }
  TRACEN((printf("FindNextFileA ret_handle=FALSE (ERROR_NO_MORE_FILES)\n")))
  SetLastError( ERROR_NO_MORE_FILES );
  return false;
}

bool CFindFile::FindNext(CFileInfoW &fileInfo)
{
  CFileInfo fileInfo0;
  bool bret = FindNext(fileInfo0);
  if (bret)
  {
     fileInfo.Attributes = fileInfo0.Attributes;
     fileInfo.CreationTime = fileInfo0.CreationTime;
     fileInfo.LastAccessTime = fileInfo0.LastAccessTime;
     fileInfo.LastWriteTime = fileInfo0.LastWriteTime;
     fileInfo.Size = fileInfo0.Size;
     fileInfo.Name = GetUnicodeString(fileInfo0.Name, CP_ACP);
  }
  return bret;
}

bool FindFile(LPCTSTR wildcard, CFileInfo &fileInfo)
{
  CFindFile finder;
  return finder.FindFirst(wildcard, fileInfo);
}

#ifndef _UNICODE
bool FindFile(LPCWSTR wildcard, CFileInfoW &fileInfo)
{
  CFindFile finder;
  return finder.FindFirst(wildcard, fileInfo);
}
#endif

bool DoesFileExist(LPCTSTR name)
{
  CFileInfo fileInfo;
  return FindFile(name, fileInfo);
}

#ifndef _UNICODE
bool DoesFileExist(LPCWSTR name)
{
  CFileInfoW fileInfo;
  return FindFile(name, fileInfo);
}
#endif

/////////////////////////////////////
// CEnumerator

bool CEnumerator::NextAny(CFileInfo &fileInfo)
{
  if(_findFile.IsHandleAllocated())
    return _findFile.FindNext(fileInfo);
  else
    return _findFile.FindFirst(_wildcard, fileInfo);
}

bool CEnumerator::Next(CFileInfo &fileInfo)
{
  while(true)
  {
    if(!NextAny(fileInfo))
      return false;
    if(!fileInfo.IsDots())
      return true;
  }
}

bool CEnumerator::Next(CFileInfo &fileInfo, bool &found)
{
  if (Next(fileInfo))
  {
    found = true;
    return true;
  }
  found = false;
  return (::GetLastError() == ERROR_NO_MORE_FILES);
}

#ifndef _UNICODE
bool CEnumeratorW::NextAny(CFileInfoW &fileInfo)
{
  if(_findFile.IsHandleAllocated())
    return _findFile.FindNext(fileInfo);
  else
    return _findFile.FindFirst(_wildcard, fileInfo);
}

bool CEnumeratorW::Next(CFileInfoW &fileInfo)
{
  while(true)
  {
    if(!NextAny(fileInfo))
      return false;
    if(!fileInfo.IsDots())
      return true;
  }
}

bool CEnumeratorW::Next(CFileInfoW &fileInfo, bool &found)
{
  if (Next(fileInfo))
  {
    found = true;
    return true;
  }
  found = false;
  return (::GetLastError() == ERROR_NO_MORE_FILES);
}

#endif

}}}
