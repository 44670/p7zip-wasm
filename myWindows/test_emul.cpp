#undef BIG_ENDIAN
#undef LITTLE_ENDIAN

#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif
#ifdef HAVE_LOCALE
#include <locale.h>
#endif

#ifdef __linux__
#include <endian.h>
#include <byteswap.h>
#elif defined(__APPLE_CC__)
#include <machine/endian.h>
#elif defined(sparc)
#define PLATFORM_BYTE_ORDER AES_BIG_ENDIAN
#elif defined(BIG_ENDIAN)
#define PLATFORM_BYTE_ORDER AES_BIG_ENDIAN
#elif defined(LITTLE_ENDIAN)
#define PLATFORM_BYTE_ORDER AES_LITTLE_ENDIAN
// #else
// #include <sys/endian.h>
#endif


#include <windows.h>

#define NEED_NAME_WINDOWS_TO_UNIX
#include "myPrivate.h"

#include "Common/StringConvert.h"

#undef NDEBUG
#include <assert.h>

#include "Common/StringConvert.cpp"


#if  defined(HAVE_WCHAR_H) && defined(HAVE_MBSTOWCS) && defined(HAVE_WCSTOMBS)
void test_mbs(void) {
  wchar_t wstr1[256] = {
                         L'e',
                         0xE8, // latin small letter e with grave
                         0xE9, // latin small letter e with acute
                         L'a',
                         0xE0, // latin small letter a with grave
                         0x20AC, // euro sign
                         L'b',
                         0 };
  wchar_t wstr2[256];
  char    astr[256];
  extern int global_use_utf16_conversion;

  global_use_utf16_conversion = 1;

  size_t len1 = wcslen(wstr1);

  printf("wstr1 - %d - '%ls'\n",(int)len1,wstr1);

  size_t len0 = wcstombs(astr,wstr1,sizeof(astr));
  printf("astr - %d - '%s'\n",(int)len0,astr);

  size_t len2 = mbstowcs(wstr2,astr,sizeof(wstr2)/sizeof(*wstr2));
  printf("wstr - %d - '%ls'\n",(int)len2,wstr2);

  if (wcscmp(wstr1,wstr2) != 0) {
    printf("ERROR during conversions wcs -> mbs -> wcs\n");
    exit(EXIT_FAILURE);
  }

  char *ptr = astr;
  size_t len = 0;
  while (*ptr) {
    ptr = CharNextA(ptr);
    len += 1;
  }
  if (len != len1) {
    printf("ERROR CharNextA : len=%d, len1=%d\n",(int)len,(int)len1);
    exit(EXIT_FAILURE);
  }

  UString ustr(wstr1);
  assert(ustr.Length() == (int)len1);

  AString  ansistr(astr);
  assert(ansistr.Length() == (int)len0);

  ansistr = UnicodeStringToMultiByte(ustr);
  assert(ansistr.Length() == (int)len0);

  assert(strcmp(ansistr,astr) == 0);
  assert(wcscmp(ustr,wstr1) == 0);

  UString ustr2 = MultiByteToUnicodeString(astr);
  assert(ustr2.Length() == (int)len1);
  assert(wcscmp(ustr2,wstr1) == 0);
}
#endif

static void test_astring(int num) {
  AString strResult;

  strResult = "first part : ";
  char number[256];
  sprintf(number,"%d",num);
  strResult += AString(number);

  strResult += " : last part";

  printf("strResult -%s-\n",(const char *)strResult);

}


extern void my_windows_split_path(const AString &p_path, AString &dir , AString &base);

static struct {
  const char *path;
  const char *dir;
  const char *base;
}
tabSplit[]=
  {
    { "",".","." },
    { "/","/","/" },
    { ".",".","." },
    { "//","/","/" },
    { "///","/","/" },
    { "dir",".","dir" },
    { "/dir","/","dir" },
    { "/dir/","/","dir" },
    { "/dir/base","/dir","base" },
    { "/dir//base","/dir","base" },
    { "/dir///base","/dir","base" },
    { "//dir/base","//dir","base" },
    { "///dir/base","///dir","base" },
    { "/dir/base/","/dir","base" },
    { 0,0,0 }
  };

static void test_split_astring() {
  int ind = 0;
  while (tabSplit[ind].path) {
    AString path(tabSplit[ind].path);
    AString dir;
    AString base;

    my_windows_split_path(path,dir,base);

    if ((dir != tabSplit[ind].dir) || (base != tabSplit[ind].base)) {
      printf("ERROR : '%s' '%s' '%s'\n",(const char *)path,(const char *)dir,(const char *)base);
    }
    ind++;
  }
  printf("test_split_astring : done\n");
}


int main() {
#if defined(BIG_ENDIAN)
  printf("BIG_ENDIAN : %d\n",(int)BIG_ENDIAN);
#endif
#if defined(LITTLE_ENDIAN)

  printf("LITTLE_ENDIAN : %d\n",(int)LITTLE_ENDIAN);
#endif

  printf("sizeof(Byte)   : %d\n",(int)sizeof(Byte));
  printf("sizeof(UInt16) : %d\n",(int)sizeof(UInt16));
  printf("sizeof(UInt32) : %d\n",(int)sizeof(UInt32));
  printf("sizeof(UINT32) : %d\n",(int)sizeof(UINT32));
  printf("sizeof(UInt64) : %d\n",(int)sizeof(UInt64));
  printf("sizeof(UINT64) : %d\n",(int)sizeof(UINT64));
  printf("sizeof(void *) : %d\n",(int)sizeof(void *));

#if  defined(HAVE_WCHAR_H) && defined(HAVE_MBSTOWCS) && defined(HAVE_WCSTOMBS)
#ifdef HAVE_LOCALE

  setlocale(LC_ALL,"");
#endif

  test_mbs();
#endif

  test_astring(12345);
  test_split_astring();

  return 0;
}

