// Windows/DLL.h

#ifndef __WINDOWS_DLL_H
#define __WINDOWS_DLL_H

#include "../Common/MyString.h"

typedef void * HMODULE;

typedef int (*FARPROC)();

namespace NWindows {
namespace NDLL {

class CLibrary
{
  bool LoadOperations(HMODULE newModule);
  HMODULE _module;
  bool Free();
public:

  CLibrary():_module(NULL) {};
  ~CLibrary();

  bool Load(LPCTSTR fileName);
  FARPROC GetProcAddress(LPCSTR procName) const;
};

}}

#endif
