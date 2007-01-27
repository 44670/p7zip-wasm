// Thresds.h

#ifndef __7Z_THRESDS_H
#define __7Z_THRESDS_H

#include <windows.h>

#include "Types.h"

#ifdef ENV_BEOS
#include <kernel/OS.h>
#define MAX_THREAD 256
#else
#include <pthread.h>
#endif


typedef struct {
#ifdef ENV_BEOS
	sem_id _sem;
#else
        pthread_mutex_t _mutex;
#endif
} CCriticalSection;

HRes CriticalSection_Init(CCriticalSection *p);
void CriticalSection_Enter(CCriticalSection *p);
void CriticalSection_Leave(CCriticalSection *p);
void CriticalSection_Delete(CCriticalSection *);

typedef struct _CThread
{
#ifdef ENV_BEOS
	thread_id _tid;
#else
	pthread_t _tid;
#endif
	int _created;

} CThread;

#define Thread_Construct(thread) (thread)->_created = 0
#define Thread_WasCreated(thread) ((thread)->_created != 0)
 
HRes Thread_Create(CThread *thread, unsigned (StdCall *startAddress)(void *), LPVOID parameter);
HRes Thread_Wait(CThread *thread);
HRes Thread_Close(CThread *thread);

typedef struct _CEvent
{
  int _created;
  int _manual_reset;
  int _state;
#ifdef ENV_BEOS
  thread_id _waiting[MAX_THREAD];
  int index_waiting;
  sem_id _sem;
#else
  pthread_mutex_t _mutex;
  pthread_cond_t _cond;
#endif
} CEvent;

typedef CEvent CAutoResetEvent;

#define Event_Construct(event) (event)->_created = 0

HRes AutoResetEvent_Create(CAutoResetEvent *event, int initialSignaled);
HRes AutoResetEvent_CreateNotSignaled(CAutoResetEvent *event);
HRes Event_Set(CEvent *event);
HRes Event_Reset(CEvent *event);
HRes Event_Wait(CEvent *event);
HRes Event_Close(CEvent *event);


typedef struct _CSemaphore
{
  int _created;
  UInt32 _count;
  UInt32 _maxCount;
#ifdef ENV_BEOS
  thread_id _waiting[MAX_THREAD];
  int index_waiting;
  sem_id _sem;
#else
  pthread_mutex_t _mutex;
  pthread_cond_t _cond;
#endif
} CSemaphore;

#define Semaphore_Construct(p) (p)->_created = 0

HRes Semaphore_Create(CSemaphore *p, UInt32 initiallyCount, UInt32 maxCount);
HRes Semaphore_Release1(CSemaphore *p);
HRes Semaphore_Wait(CSemaphore *p);
HRes Semaphore_Close(CSemaphore *p);

#endif

