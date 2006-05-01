// Common/Alloc.cpp

#include "StdAfx.h"

#ifdef _WIN32
#include "MyWindows.h"
#else
#include <stdlib.h>
#endif

#include "Alloc.h"

/* #define _SZ_ALLOC_DEBUG */
/* use _SZ_ALLOC_DEBUG to debug alloc/free operations */
#ifdef _SZ_ALLOC_DEBUG
#include <stdio.h>
int g_allocCount = 0;
int g_allocCountBig = 0;
#endif

void *MyAlloc(size_t size) throw()
{
  if (size == 0)
    return 0;
  #ifdef _SZ_ALLOC_DEBUG
  fprintf(stderr, "\nAlloc %10d bytes; count = %10d", size, g_allocCount++);
  #endif
  return ::malloc(size);
}

void MyFree(void *address) throw()
{
  #ifdef _SZ_ALLOC_DEBUG
  if (address != 0)
    fprintf(stderr, "\nFree; count = %10d", --g_allocCount);
  #endif
  
  ::free(address);
}

