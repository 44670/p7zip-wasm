// CodecsPath.cpp

#include "StdAfx.h"
#include "../../../Common/String.h"

#ifdef _WIN32
extern HINSTANCE g_hInstance;

static CSysString GetLibraryPath()
{
  TCHAR fullPath[MAX_PATH + 1];
  ::GetModuleFileName(g_hInstance, fullPath, MAX_PATH);
  return fullPath;
}

static CSysString GetLibraryFolderPrefix()
{
  CSysString path = GetLibraryPath();
  int pos = path.ReverseFind(TEXT(CHAR_PATH_SEPARATOR));
  return path.Left(pos + 1);
}

CSysString GetBaseFolderPrefix()
{
  CSysString libPrefix = GetLibraryFolderPrefix();
  CSysString temp = libPrefix;
  temp.Delete(temp.Length() - 1);
  int pos = temp.ReverseFind(TEXT(CHAR_PATH_SEPARATOR));
  return temp.Left(pos + 1);
}

CSysString GetCodecsFolderPrefix()
{
  return GetBaseFolderPrefix() + (CSysString)(TEXT("Codecs")) + (CSysString)(TEXT(STRING_PATH_SEPARATOR));
}
#else

CSysString GetCodecsFolderPrefix()
{
  const char *p7zip_home_dir = getenv("P7ZIP_HOME_DIR");
  if (p7zip_home_dir == 0) p7zip_home_dir="./";
  return CSysString(p7zip_home_dir) + (CSysString)(TEXT("Codecs")) + (CSysString)(TEXT(STRING_PATH_SEPARATOR));
}
#endif

