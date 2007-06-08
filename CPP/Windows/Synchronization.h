// Windows/Synchronization.h

#ifndef __WINDOWS_SYNCHRONIZATION_H
#define __WINDOWS_SYNCHRONIZATION_H

#include "Defs.h"

extern "C" 
{ 
#include "../../C/Threads.h"
}

#ifdef _WIN32
#include "Handle.h"
#endif

#ifdef ENV_BEOS
#include <Locker.h>
#include <kernel/OS.h>
#include <list>
#endif

#undef DEBUG_SYNCHRO
// #define DEBUG_SYNCHRO 1

DWORD WINAPI WaitForMultipleObjects( DWORD count, const HANDLE *handles, BOOL wait_all, DWORD timeout );


#ifdef DEBUG_SYNCHRO
typedef struct 
{
  void *ctx1;
  // void *ctx2;
} t_SyncCXT;

void sync_GetContexts(t_SyncCXT & ctx);

#endif

namespace NWindows {
namespace NSynchronization {

struct CBaseHandle
{
#ifdef DEBUG_SYNCHRO
	t_SyncCXT ctx;
#endif

	typedef enum { EVENT , SEMAPHORE } t_type;

	CBaseHandle(t_type t) { 
		type = t;
#ifdef DEBUG_SYNCHRO
		sync_GetContexts(ctx);
#endif
	}

	t_type type;
	union
	{
		struct
		{
			bool _manual_reset;
			bool _state;
		} event;
		struct
		{
			LONG count;
			LONG maxCount;
		} sema;
	} u;
  operator HANDLE() { return ((HANDLE)this); }
};

class CBaseEvent : public CBaseHandle
{
  bool _created;
public:
  bool IsCreated() { return _created; }
  CBaseEvent() : CBaseHandle(CBaseHandle::EVENT), _created(false) {} 
  ~CBaseEvent() { Close(); }

  HRes Close() { _created = false ; return S_OK; }

  HRes Create(bool manualReset, bool initiallyOwn)
  {
    this->u.event._manual_reset = manualReset;
    this->u.event._state        = initiallyOwn;
    this->_created = true;
    return S_OK;
  }

  HRes Set();
  HRes Reset();
  HRes Lock();
};

class CManualResetEvent: public CBaseEvent
{
public:
  HRes Create(bool initiallyOwn = false)
  {
    return CBaseEvent::Create(true, initiallyOwn);
  }
  HRes CreateIfNotCreated()
  {
    if (IsCreated())
      return 0;
    return CBaseEvent::Create(true, false);
  }
};

class CAutoResetEvent: public CBaseEvent
{
public:
  HRes Create()
  {
    return CBaseEvent::Create(false, false);
  }
  HRes CreateIfNotCreated()
  {
    if (IsCreated())
      return 0;
    return CBaseEvent::Create(false, false);
  }
};

#ifdef ENV_BEOS
class CCriticalSection : BLocker
{
public:
  CCriticalSection() { }
  ~CCriticalSection() {}
  void Enter() { Lock(); }
  void Leave() { Unlock(); }
};
#else
#ifdef DEBUG_SYNCHRO

// #define TRACEN(u) u;
#define TRACEN(u)  /* */

class CCriticalSection
{
  pthread_mutex_t _object;
  void dump_error(int ligne,int ret,const char *text,void *param)
  {
    printf("\n##T%d#ERROR (l=%d) %s : param=%p ret = %d (%s)##\n",(int)pthread_self(),ligne,text,param,ret,strerror(ret));
    // abort();
  }
public:
  CCriticalSection() {
    pthread_mutexattr_t mutexattr;
    int ret = pthread_mutexattr_init(&mutexattr);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutexattr_init",&mutexattr);
    ret = pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_ERRORCHECK);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutexattr_settype",&mutexattr);
    ret = ::pthread_mutex_init(&_object,&mutexattr);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutex_init",&_object);
    TRACEN((printf("\nT%d : C1-Create(%p)\n",(int)pthread_self(),(void *)&_object)))
  }
  ~CCriticalSection() {
    TRACEN((printf("\nT%d : C1-FREE(%p)\n",(int)pthread_self(),(void *)&_object)))
    int ret = ::pthread_mutex_destroy(&_object);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutex_destroy",&_object);
  }
  void Enter(int ligne = 0) { 
    TRACEN((printf("\nT%d %d : C1-lock(%p)\n",(int)pthread_self(),ligne,(void *)&_object)))
    int ret = ::pthread_mutex_lock(&_object);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutex_lock",&_object);
    TRACEN((printf("\nT%d %d : C2-lock(%p)\n",(int)pthread_self(),ligne,(void *)&_object)))
  }
  void Leave(int ligne = 0) {
    TRACEN((printf("\nT%d %d : C1-unlock(%p)\n",(int)pthread_self(),ligne,(void *)&_object)))
    int ret = ::pthread_mutex_unlock(&_object);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutex_unlock",&_object);
    TRACEN((printf("\nT%d %d : C2-unlock(%p)\n",(int)pthread_self(),ligne,(void *)&_object)))
  }
};
#else
class CCriticalSection
{
  pthread_mutex_t _object;
public:
  CCriticalSection() {
    ::pthread_mutex_init(&_object,0);
  }
  ~CCriticalSection() {
    ::pthread_mutex_destroy(&_object);
  }
  void Enter() { ::pthread_mutex_lock(&_object); }
  void Leave() { ::pthread_mutex_unlock(&_object); }
};
#endif
#endif

class CSemaphore : public CBaseHandle
{
public:
  CSemaphore() : CBaseHandle(CBaseHandle::SEMAPHORE) {} 
  HRes Create(LONG initiallyCount, LONG maxCount)
  {
    if ((initiallyCount < 0) || (initiallyCount > maxCount) || (maxCount < 1)) return S_FALSE;
    this->u.sema.count    = initiallyCount;
    this->u.sema.maxCount = maxCount;
    return S_OK;
  }
  HRes Release(LONG releaseCount = 1);
  HRes Close() { return S_OK; }
};

class CCriticalSectionLock
{
  CCriticalSection &_object;
  void Unlock()  { _object.Leave(); }
public:
  CCriticalSectionLock(CCriticalSection &object): _object(object) 
    {_object.Enter(); } 
  ~CCriticalSectionLock() { Unlock(); }
};

}}

#endif

