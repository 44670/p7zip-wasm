#include "StdAfx.h"

#include "../Common/StringConvert.h"

#include "myPrivate.h"

// #define TRACEN(u) u;
#define TRACEN(u)  /* */

void my_windows_split_path(const AString &p_path, AString &dir , AString &base) {
  int pos = p_path.ReverseFind('/');
  if (pos == -1) {
    // no separator
    dir  = ".";
    if (p_path.IsEmpty())
      base = ".";
    else
      base = p_path;
  } else if ((pos+1) < p_path.Length()) {
    // true separator
    base = p_path.Mid(pos+1);
    while ((pos >= 1) && (p_path[pos-1] == '/'))
      pos--;
    if (pos == 0)
      dir = "/";
    else
      dir = p_path.Left(pos);
  } else {
    // separator at the end of the path
    // pos = p_path.find_last_not_of("/");
    pos = -1;
    int ind = 0;
    while (p_path[ind]) {
      if (p_path[ind] != '/')
        pos = ind;
      ind++;
    }
    if (pos == -1) {
      base = "/";
      dir = "/";
    } else {
      my_windows_split_path(p_path.Left(pos+1),dir,base);
    }
  }
}

static const char * myModuleFileName = 0;
extern "C" void mySetModuleFileNameA(const char * moduleFileName) {
  char *ptr =  new char[strlen(moduleFileName)+1];
  strcpy(ptr,moduleFileName);
  myModuleFileName = (const char *)ptr;
  TRACEN((printf("mySetModuleFileNameA(%s) &myModuleFileName=%p\n",myModuleFileName,&myModuleFileName)))
}

DWORD GetModuleFileNameA( HMODULE hModule, LPSTR lpFilename, DWORD nSize) {
  TRACEN((printf("GetModuleFileNameA(%p) &myModuleFileName=%p\n",(void *)hModule,&myModuleFileName)))
  if (hModule != 0)
    throw "GetModuleFileNameA not implemented when hModule !=0";

  if ((nSize>=1) && (myModuleFileName)) {
    strncpy(lpFilename,myModuleFileName,nSize);
    lpFilename[nSize-1] = 0;
    TRACEN((printf("GetModuleFileNameA(%p) lpFilename='%s'\n",(void *)hModule,lpFilename)))
    return strlen(lpFilename);
  }
  return 0;
}

static DWORD mySearchPathA( LPCSTR path, LPCSTR name, LPCSTR ext,
                            DWORD buflen, LPSTR buffer, LPSTR *lastpart ) {
  if (path != 0) {
    printf("NOT EXPECTED : SearchPathA : path != NULL\n");
    exit(EXIT_FAILURE);
  }

  if (ext != 0) {
    printf("NOT EXPECTED : SearchPathA : ext != NULL\n");
    exit(EXIT_FAILURE);
  }

  TRACEN((printf("mySearchPathA() myModuleFileName=%p\n",myModuleFileName)))
  if (myModuleFileName) {
    TRACEN((printf("mySearchPathA() myModuleFileName='%s'\n",myModuleFileName)))
    FILE *file;
    file = fopen(name,"r");
    if (file) {
      DWORD ret = (DWORD)strlen(name);
      fclose(file);
      if (ret >= buflen) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return 0;
      }
      strcpy(buffer,name);
      if (lastpart)
        *lastpart = buffer;

      return ret;
    }
    AString module_path(myModuleFileName);
    AString dir,name2,dir_path;
    my_windows_split_path(module_path,dir,name2);
    dir_path = dir;
    dir_path += "/";
    dir_path += name;

    TRACEN((printf("mySearchPathA() fopen-2(%s)\n",(const char *)dir_path)))
    file = fopen((const char *)dir_path,"r");
    if (file) {
      DWORD ret = strlen((const char *)dir_path);
      fclose(file);
      if (ret >= buflen) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return 0;
      }
      strcpy(buffer,(const char *)dir_path);
      if (lastpart)
        *lastpart = buffer + (strlen((const char *)dir) + 1);

      return ret;
    }

  }

  return 0;
}

DWORD SearchPathA( LPCSTR path, LPCSTR name, LPCSTR ext,
                   DWORD buflen, LPSTR buffer, LPSTR *lastpart ) {
  if (buffer == 0) {
    printf("NOT EXPECTED : SearchPathA : buffer == NULL\n");
    exit(EXIT_FAILURE);
  }
  *buffer=0;
  DWORD ret = mySearchPathA(path,name,ext,buflen,buffer,lastpart);

  TRACEN((printf("SearchPathA(%s,%s,%s,%d,%s,%p)=%d\n",
                 (path ? path : "<NULL>"),
                 (name ? name : "<NULL>"),
                 (ext ? ext : "<NULL>"),
                 (int)buflen,
                 buffer,lastpart,(int)ret)));
  return ret;
}

