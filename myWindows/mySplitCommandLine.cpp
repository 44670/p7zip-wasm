#include "StdAfx.h"

#include "../Common/StringConvert.h"

#include "myPrivate.h"

#ifdef HAVE_LOCALE
#include <locale.h>
#endif

extern int global_use_utf16_conversion;

void mySplitCommandLine(int numArguments,const char *arguments[],UStringVector &parts) {
  // define the module name for the main program
  mySetModuleFileNameA(arguments[0]);

#ifdef HAVE_LOCALE
  // set the program's current locale from the user's environment variables
  setlocale(LC_ALL,"");

  // auto-detect which conversion p7zip should use
  char *locale = setlocale(LC_CTYPE,0);
  if (locale) {
    size_t len = strlen(locale);
    char *locale_upper = (char *)malloc(len+1);
    if (locale_upper) {
      strcpy(locale_upper,locale);

      for(size_t i=0;i<len;i++)
        locale_upper[i] = toupper(locale_upper[i] & 255);

      if (    (strcmp(locale_upper,"") != 0)
              && (strcmp(locale_upper,"C") != 0)
              && (strcmp(locale_upper,"POSIX") != 0) ) {
        global_use_utf16_conversion = 1;
      }
      free(locale_upper);
    }
  }
#endif

  parts.Clear();
  for(int ind=0;ind < numArguments; ind++) {
    if ((ind <= 2) && (strcmp(arguments[ind],"-no-utf16") == 0)) {
      global_use_utf16_conversion = 0;
    } else if ((ind <= 2) && (strcmp(arguments[ind],"-utf16") == 0)) {
      global_use_utf16_conversion = 1;
    } else {
      UString tmp = MultiByteToUnicodeString(arguments[ind]);
      // tmp.Trim(); " " is a valid filename ...
      if (!tmp.IsEmpty()) {
        parts.Add(tmp);
      }
    }
  }
}

const char *my_getlocale(void) {
#ifdef HAVE_LOCALE
  const char* ret = setlocale(LC_CTYPE,0);
  if (ret == 0)
    ret ="C";
  return ret;
#else
  return "C";
#endif
}

