// stdafx.h

#ifndef __STDAFX_H
#define __STDAFX_H


#include "config.h"


#define NO_INLINE /* FIXME */

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#include "Common/MyWindows.h"
#include "Common/Types.h"

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <wchar.h>
#include <stddef.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

#undef CS /* fix for Solaris 10 x86 */

/***************************/
typedef void * HINSTANCE; // FIXME

#define CLASS_E_CLASSNOTAVAILABLE        ((HRESULT)0x80040111L)
#define DLL_PROCESS_ATTACH   1

/************************* LastError *************************/
inline DWORD WINAPI GetLastError(void) { return errno; }
inline void WINAPI SetLastError( DWORD err ) { errno = err; }

#define AreFileApisANSI() 1

#endif 

