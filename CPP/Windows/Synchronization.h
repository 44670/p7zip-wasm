// Windows/Synchronization.h

#ifndef __WINDOWS_SYNCHRONIZATION_H
#define __WINDOWS_SYNCHRONIZATION_H

#include "Defs.h"

#ifdef ENV_BEOS
#include <Locker.h>
#include <kernel/OS.h>
#include <list>
#endif

#undef DEBUG_SYNCHRO

DWORD WINAPI WaitForMultipleObjects( DWORD count, const HANDLE *handles, BOOL wait_all, DWORD timeout );

namespace NWindows {
namespace NSynchronization {

struct CBaseHandle
{
	typedef enum { EVENT , SEMAPHORE } t_type;

	CBaseHandle(t_type t) { type = t;  }

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
  bool Close() { return true; }
};

class CBaseEvent : public CBaseHandle
{
public:

  CBaseEvent() : CBaseHandle(CBaseHandle::EVENT) {} 
  ~CBaseEvent() { Close(); }

  bool Create(bool manualReset, bool initiallyOwn)
  {
    this->u.event._manual_reset = manualReset;
    this->u.event._state        = initiallyOwn;
    return true;
  }

  bool Set();
  bool Reset();

  bool Lock();
  
};

class CEvent: public CBaseEvent
{
public:
  CEvent() {};
  CEvent(bool manualReset, bool initiallyOwn);
};

class CManualResetEvent: public CEvent
{
public:
  CManualResetEvent(bool initiallyOwn = false):
    CEvent(true, initiallyOwn) {};
};

class CAutoResetEvent: public CEvent
{
public:
  CAutoResetEvent(bool initiallyOwn = false):
    CEvent(false, initiallyOwn) {};
};

#ifdef ENV_BEOS
class CCriticalSection : BLocker
{
#define MAX_THREAD 256
  thread_id _waiting[MAX_THREAD]; // std::list<thread_id> _waiting;
  int index_waiting;
public:
  CCriticalSection() { index_waiting = 0; }
  ~CCriticalSection() {}
  void Enter() { Lock(); }
  void Leave() { Unlock(); }
  void WaitCond() { 
    _waiting[index_waiting++] = find_thread(NULL); // _waiting.push_back(find_thread(NULL));
    thread_id sender;
    Unlock();
    int msg = receive_data(&sender, NULL, 0);
    Lock();
  }
  void SignalCond() {
    Lock();
    // for (std::list<thread_id>::iterator index = _waiting.begin(); index != _waiting.end(); index++)
    for(int index = 0 ; index < index_waiting ; index++)
    {
       send_data(_waiting[index], '7zCN', NULL, 0);
    }
    index_waiting = 0; // _waiting.clear();
    Unlock();
  }
};
#else
#ifdef DEBUG_SYNCHRO
class CCriticalSection
{
  pthread_mutex_t _object;
  pthread_cond_t _cond;
  void dump_error(int ret,const char *text)
  {
    printf("\n##ERROR %s : ret = %d (%s)##\n",text,ret,strerror(ret));
    // abort();
  }
public:
  CCriticalSection() {
    pthread_mutexattr_t mutexattr;
    int ret = pthread_mutexattr_init(&mutexattr);
    if (ret != 0) dump_error(ret,"pthread_mutexattr_init");
    ret = pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_ERRORCHECK);
    if (ret != 0) dump_error(ret,"pthread_mutexattr_settype");
    ret = ::pthread_mutex_init(&_object,&mutexattr);
    if (ret != 0) dump_error(ret,"pthread_mutex_init");
    ret = ::pthread_cond_init(&_cond,0);
    if (ret != 0) dump_error(ret,"pthread_cond_init");
  }
  ~CCriticalSection() {
    int ret = ::pthread_mutex_destroy(&_object);
    if (ret != 0) dump_error(ret,"pthread_mutex_destroy");
    ret = ::pthread_cond_destroy(&_cond);
    if (ret != 0) dump_error(ret,"pthread_cond_destroy");
  }
  void Enter() { 
    int ret = ::pthread_mutex_lock(&_object);
    if (ret != 0) dump_error(ret,"pthread_mutex_lock");
  }
  void Leave() {
    int ret = ::pthread_mutex_unlock(&_object);
    if (ret != 0) dump_error(ret,"pthread_mutex_unlock");
  }
  void WaitCond() {
    int ret = ::pthread_cond_wait(&_cond, &_object);
    if (ret != 0) dump_error(ret,"pthread_cond_wait");
  }
  void SignalCond() {
    int ret = ::pthread_cond_broadcast(&_cond);
    if (ret != 0) dump_error(ret,"pthread_cond_broadcast");
  }
};
#else
class CCriticalSection
{
  pthread_mutex_t _object;
  pthread_cond_t _cond;
public:
  CCriticalSection() {
    ::pthread_mutex_init(&_object,0);
    ::pthread_cond_init(&_cond,0);
  }
  ~CCriticalSection() {
    ::pthread_mutex_destroy(&_object);
    ::pthread_cond_destroy(&_cond);
  }
  void Enter() { ::pthread_mutex_lock(&_object); }
  void Leave() { ::pthread_mutex_unlock(&_object); }
  void WaitCond() { ::pthread_cond_wait(&_cond, &_object); }
  void SignalCond() { ::pthread_cond_broadcast(&_cond); }
};
#endif
#endif

class CSemaphore : public CBaseHandle
{
public:
  CSemaphore() : CBaseHandle(CBaseHandle::SEMAPHORE) {} 
  bool Create(LONG initiallyCount, LONG maxCount)
  {
    if ((initiallyCount < 0) || (initiallyCount > maxCount) || (maxCount < 1)) return false;
    this->u.sema.count    = initiallyCount;
    this->u.sema.maxCount = maxCount;
    return true;
  }
  bool Release(LONG releaseCount = 1);
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
