/* Alloc.h -- Memory allocation functions
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __COMMON_ALLOC_H
#define __COMMON_ALLOC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *MyAlloc(size_t size);
void MyFree(void *address);

#ifdef _WIN32

void SetLargePageSize();

void *MidAlloc(size_t size);
void MidFree(void *address);
void *BigAlloc(size_t size);
void BigFree(void *address);

#else

#include <stdlib.h> /* malloc */

#define MyAlloc(size) malloc(size)
#define MyFree(address) free(address)
#define MidAlloc(size) malloc(size)
#define MidFree(address) free(address)
#define BigAlloc(size) malloc(size)
#define BigFree(address) free(address)

#endif

#ifdef __cplusplus
}
#endif

#endif
