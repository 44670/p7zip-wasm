/* Alloc.h */

#ifndef __COMMON_ALLOC_H
#define __COMMON_ALLOC_H

#include <stddef.h>

#ifdef _WIN32

void *MyAlloc(size_t size);
void MyFree(void *address);


void SetLargePageSize();

void *MidAlloc(size_t size);
void MidFree(void *address);
void *BigAlloc(size_t size);
void BigFree(void *address);

#else

#define MyAlloc(size) malloc(size)
#define MyFree(address) free(address)
#define MidAlloc(size) malloc(size)
#define MidFree(address) free(address)
#define BigAlloc(size) malloc(size)
#define BigFree(address) free(address)

#endif

#endif
