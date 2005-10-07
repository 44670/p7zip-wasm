
#if !defined(__DJGPP__)

  #if !defined(ENV_MACOSX) && !defined(ENV_BEOS)

    /* <wchar.h> */
    #define HAVE_WCHAR_H

    /* <wctype.h> */
    #define HAVE_WCTYPE_H

    /* mbrtowc */
    #define HAVE_MBRTOWC

    /* towupper */
    #define HAVE_TOWUPPER

  #endif /* !ENV_MACOSX && !ENV_BEOS */

  /* lstat and readlink */
  #define HAVE_LSTAT

  /* <locale.h> */
  #define HAVE_LOCALE

  /* mbstowcs */
  #define HAVE_MBSTOWCS

  /* wcstombs */
  #define HAVE_WCSTOMBS

  #if !defined(__CYGWIN__) && !defined(sparc) && !defined(sun) && !defined(__APPLE_CC__) && !defined(ENV_BEOS)
  /* timegm */
  #define HAVE_TIMEGM
  #endif

#endif /* !__DJGPP__ */

#ifndef ENV_BEOS
#define HAVE_PTHREAD
#endif

#define MAX_PATHNAME_LEN   1024

