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

#include <windows.h>

#define NEED_NAME_WINDOWS_TO_UNIX
#include "myPrivate.h"

#include "Common/StringConvert.h"
#include "Common/StdOutStream.h"

#undef NDEBUG
#include <assert.h>

#include "Common/StringConvert.cpp"
#include "Common/StdOutStream.cpp"
#include "Common/IntToString.cpp"


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

 // Number of 100 nanosecond units from 1/1/1601 to 1/1/1970
#define EPOCH_BIAS  116444736000000000LL
static LARGE_INTEGER UnixTimeToUL(time_t tps_unx)
{
	LARGE_INTEGER ul;
	ul.QuadPart = tps_unx * 10000000LL + EPOCH_BIAS;
	return ul;
}

static LARGE_INTEGER FileTimeToUL(FILETIME fileTime)
{
	LARGE_INTEGER lFileTime;
	lFileTime.QuadPart = fileTime.dwHighDateTime;
	lFileTime.QuadPart = (lFileTime.QuadPart << 32) | fileTime.dwLowDateTime;
	return lFileTime;
}

static void display(const char *txt,SYSTEMTIME systime)
{
	FILETIME fileTime;
	BOOL ret = SystemTimeToFileTime(&systime,&fileTime);
	assert(ret == TRUE);
	LARGE_INTEGER ulFileTime = FileTimeToUL(fileTime);
	
	const char * day="";
	switch (systime.wDayOfWeek)
	{
        	case 0:day = "Sunday";break;
        	case 1:day = "Monday";break;
        	case 2:day = "Tuesday";break;
        	case 3:day = "Wednesday";break;
        	case 4:day = "Thursday";break;
        	case 5:day = "Friday";break;
        	case 6:day = "Saturday";break;
	}
	g_StdOut<< txt << day << " " 
		<< (int)systime.wYear << "/" <<  (int)systime.wMonth << "/" << (int)systime.wDay << " "
		<< (int)systime.wHour << ":" << (int)systime.wMinute << ":" <<  (int)systime.wSecond << ":" 
        	<<     (int)systime.wMilliseconds
		<< " (" << (UInt64)ulFileTime.QuadPart << ")\n";
}

static void test_time()
{
	time_t tps_unx = time(0);

	g_StdOut << "\nTEST TIME :\n";
	SYSTEMTIME systimeGM;
	GetSystemTime(&systimeGM);
	
	LARGE_INTEGER ul = UnixTimeToUL(tps_unx);
	g_StdOut<<"  unix time = " << (UInt64)tps_unx << " (" << (UInt64)ul.QuadPart << ")\n";

	g_StdOut<<"  gmtime    : " << asctime(gmtime(&tps_unx))<<"\n";
	g_StdOut<<"  localtime : " << asctime(localtime(&tps_unx))<<"\n";

	display("  GetSystemTime : ", systimeGM);
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

  test_time();

  return 0;
}

