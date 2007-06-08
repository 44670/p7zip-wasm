// Windows/Thread.h

#ifndef __WINDOWS_THREAD_H
#define __WINDOWS_THREAD_H

#include "Defs.h"

extern "C" 
{ 
#include "../../C/Threads.h"
}

#ifdef ENV_BEOS
#include <kernel/OS.h>
#endif

namespace NWindows {

#ifdef ENV_BEOS

class CThread
{
	thread_id _tid;
public:
   CThread() : _tid(B_BAD_THREAD_ID) {}
  ~CThread() { Close(); }

  HRes Close()
  {
    if (_tid < B_OK) return S_FALSE;
   
	/* http://www.opengroup.org/onlinepubs/007908799/xsh/pthread_detach.html */
	/* says that detach will not case thread to terminate, so we just cleanup values here */
    _tid = B_BAD_THREAD_ID;
    return S_OK;
  }

  HRes Create(DWORD (*startAddress)(void *), LPVOID parameter)
  {
	_tid = spawn_thread((int32 (*)(void *))startAddress, "CThread", B_LOW_PRIORITY, parameter);
	if (_tid >= B_OK) {
		resume_thread(_tid);
	} else {
		_tid = B_BAD_THREAD_ID;
	}

	if (_tid >= B_OK) return S_OK;
        return S_FALSE;
  }

  HRes Wait()
  {
	if (_tid >= B_OK) 
	{
		status_t exit_value;
		wait_for_thread(_tid, &exit_value);
		_tid = B_BAD_THREAD_ID;
	}
    return S_OK;
  }
};

#else

class CThread
{
	pthread_t _tid;
	bool _created;
public:
   CThread() : _created(false) {}
  ~CThread() { Close(); }

  HRes Close()
  {
    if (!_created) return S_OK;
    
    pthread_detach(_tid);
    _tid = 0;
    _created = false;
    return S_OK;
  }

  HRes Create(DWORD (*startAddress)(void *), LPVOID parameter)
  {
	pthread_attr_t attr;
	int ret;

	_created = false;

	ret = pthread_attr_init(&attr);
	if (ret) return S_FALSE;

	ret = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	if (ret) return S_FALSE;

	ret = pthread_create(&_tid, &attr, (void * (*)(void *))startAddress, parameter);

	/* ret2 = */ pthread_attr_destroy(&attr);

	if (ret) return S_FALSE;
	
	_created = true;

	return S_OK;
  }

  HRes Wait()
  {
	if (_created) 
	{
		void *thread_return;
		pthread_join(_tid,&thread_return);
		_created = false;
	}
        return S_OK;
  }
};

#endif

}

#endif

