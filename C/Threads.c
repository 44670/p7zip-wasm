/* Threads.c */

#include "Threads.h"

#ifdef ENV_BEOS
#include <kernel/OS.h>
#else
#include <pthread.h>
#include <stdlib.h>
#endif

/* #define DEBUG_SYNCHRO 1 */
#undef DEBUG_SYNCHRO

#ifdef ENV_BEOS

/* TODO : optimize the code and verify the returned values */ 

HRes Thread_Create(CThread *thread, unsigned (StdCall *startAddress)(void *), LPVOID parameter)
{ 
	thread->_tid = spawn_thread((int32 (*)(void *))startAddress, "CThread", B_LOW_PRIORITY, parameter);
	if (thread->_tid >= B_OK) {
		resume_thread(thread->_tid);
	} else {
		thread->_tid = B_BAD_THREAD_ID;
	}
	thread->_created = 1;
	return 0; // SZ_OK;
}

HRes Thread_Wait(CThread *thread)
{
  int ret;

  if (thread->_created == 0)
    return EINVAL;

  if (thread->_tid >= B_OK) 
  {
    status_t exit_value;
    wait_for_thread(thread->_tid, &exit_value);
    thread->_tid = B_BAD_THREAD_ID;
  } else {
    return EINVAL;
  }
  
  thread->_created = 0;
  
  return 0;
}

HRes Thread_Close(CThread *thread)
{
    if (!thread->_created) return SZ_OK;
    
    thread->_tid = B_BAD_THREAD_ID;
    thread->_created = 0;
    return SZ_OK;
}


HRes AutoResetEvent_Create(CAutoResetEvent *p, int initialSignaled)
{
  p->index_waiting = 0;
  p->_manual_reset = FALSE;
  p->_state        = (initialSignaled ? TRUE : FALSE);
  p->_created = 1;
  p->_sem = create_sem(1,"event");
  return 0;
}

HRes AutoResetEvent_CreateNotSignaled(CAutoResetEvent *p) { return AutoResetEvent_Create(p, 0); }

HRes Event_Set(CEvent *p) {
  int index;
  acquire_sem(p->_sem);
  p->_state = TRUE;
  for(index = 0 ; index < p->index_waiting ; index++)
  {
     send_data(p->_waiting[index], '7zCN', NULL, 0);
  }
  p->index_waiting = 0;
  release_sem(p->_sem);
  return 0;
}

HRes Event_Reset(CEvent *p) {
  acquire_sem(p->_sem);
  p->_state = FALSE;
  release_sem(p->_sem);
  return 0;
}
 
HRes Event_Wait(CEvent *p) {
  acquire_sem(p->_sem);
  while (p->_state == FALSE)
  {
    thread_id sender; 
    p->_waiting[p->index_waiting++] = find_thread(NULL);
    release_sem(p->_sem);
    /* int msg = */ receive_data(&sender, NULL, 0);
    acquire_sem(p->_sem);
  }
  if (p->_manual_reset == FALSE)
  {
     p->_state = FALSE;
  }
  release_sem(p->_sem);
  return 0;
}

HRes Event_Close(CEvent *p) { 
  if (p->_created)
  {
    p->_created = 0;
    delete_sem(p->_sem);
  }
  return 0;
}

HRes Semaphore_Create(CSemaphore *p, UInt32 initiallyCount, UInt32 maxCount)
{
  p->index_waiting = 0;
  p->_count    = initiallyCount;
  p->_maxCount = maxCount;
  p->_created = 1;
  p->_sem = create_sem(1,"sem");
  return 0;
}

HRes Semaphore_ReleaseN(CSemaphore *p, UInt32 releaseCount)
{
  UInt32 newCount;
  int index;
  
  if (releaseCount < 1) return EINVAL;

  acquire_sem(p->_sem);
  newCount = p->_count + releaseCount;
  if (newCount > p->_maxCount)
  {
    release_sem(p->_sem);
    return EINVAL;
  }
  p->_count = newCount;
  for(index = 0 ; index < p->index_waiting ; index++)
  {
     send_data(p->_waiting[index], '7zCN', NULL, 0);
  }
  p->index_waiting = 0;
  release_sem(p->_sem);
  return 0;
}

HRes Semaphore_Release1(CSemaphore *p)
{
  return Semaphore_ReleaseN(p, 1);
}


HRes Semaphore_Wait(CSemaphore *p) {
  acquire_sem(p->_sem);
  while (p->_count < 1)
  {
    thread_id sender;  
    p->_waiting[p->index_waiting++] = find_thread(NULL);
    release_sem(p->_sem);
    /* int msg = */ receive_data(&sender, NULL, 0);
    acquire_sem(p->_sem);
  }
  p->_count--;
  release_sem(p->_sem); 
  return 0;
}

HRes Semaphore_Close(CSemaphore *p) {
  if (p->_created)
  {
    p->_created = 0;
    delete_sem(p->_sem);
  }
  return 0;
}

HRes CriticalSection_Init(CCriticalSection * lpCriticalSection)
{
  if (lpCriticalSection)
	lpCriticalSection->_sem = create_sem(1,"cc");
}

void CriticalSection_Enter(CCriticalSection * lpCriticalSection)
{
  if (lpCriticalSection)
	acquire_sem(lpCriticalSection->_sem);
}

void CriticalSection_Leave(CCriticalSection * lpCriticalSection)
{
  if (lpCriticalSection)
	release_sem(lpCriticalSection->_sem);
}

void CriticalSection_Delete(CCriticalSection * lpCriticalSection)
{
  if (lpCriticalSection)
	delete_sem(lpCriticalSection->_sem);
}

#else

HRes Thread_Create(CThread *thread, unsigned (StdCall *startAddress)(void *), LPVOID parameter)
{ 
	pthread_attr_t attr;
	int ret;

	thread->_created = 0;

	ret = pthread_attr_init(&attr);
	if (ret) return ret;

	ret = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	if (ret) return ret;

	ret = pthread_create(&thread->_tid, &attr, (void * (*)(void *))startAddress, parameter);

	/* ret2 = */ pthread_attr_destroy(&attr);

	if (ret) return ret;
	
	thread->_created = 1;

	return 0; // SZ_OK;
}

HRes Thread_Wait(CThread *thread)
{
  void *thread_return;
  int ret;

  if (thread->_created == 0)
    return EINVAL;

  ret = pthread_join(thread->_tid,&thread_return);
  thread->_created = 0;
  
  return ret;
}

HRes Thread_Close(CThread *thread)
{
    if (!thread->_created) return SZ_OK;
    
    pthread_detach(thread->_tid);
    thread->_tid = 0;
    thread->_created = 0;
    return SZ_OK;
}

#ifdef DEBUG_SYNCHRO

#include <stdio.h>

static void dump_error(int ligne,int ret,const char *text,void *param)
{
  printf("\n##T%d#ERROR2 (l=%d) %s : param=%p ret = %d (%s)##\n",(int)pthread_self(),ligne,text,param,ret,strerror(ret));
    // abort();
}


HRes AutoResetEvent_Create(CAutoResetEvent *p, int initialSignaled)
{
  /* memset(&p->_mutex,0,sizeof(p->_mutex)); */
  pthread_mutexattr_t mutexattr;
  int ret = pthread_mutexattr_init(&mutexattr);
  if (ret != 0) dump_error(__LINE__,ret,"AREC::pthread_mutexattr_init",&mutexattr);
  ret = pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_ERRORCHECK_NP);
  if (ret != 0) dump_error(__LINE__,ret,"AREC::pthread_mutexattr_settype",&mutexattr);
  ret = pthread_mutex_init(&p->_mutex,&mutexattr);
  if (ret != 0) dump_error(__LINE__,ret,"AREC::pthread_mutexattr_init",&p->_mutex);
  if (ret == 0)
  {
    /* memset(&p->_cond,0,sizeof(p->_cond)); */
    ret = pthread_cond_init(&p->_cond,0);
    if (ret != 0) dump_error(__LINE__,ret,"AREC::pthread_cond_init",&p->_cond);
    p->_manual_reset = FALSE;
    p->_state        = (initialSignaled ? TRUE : FALSE);
    p->_created = 1;
  }
  return ret;
}

HRes AutoResetEvent_CreateNotSignaled(CAutoResetEvent *p) { return AutoResetEvent_Create(p, 0); }

HRes Event_Set(CEvent *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret != 0) dump_error(__LINE__,ret,"ES::pthread_mutex_lock",&p->_mutex);
  if (ret == 0)
  {
    p->_state = TRUE;
    ret = pthread_mutex_unlock(&p->_mutex);
    if (ret != 0) dump_error(__LINE__,ret,"ES::pthread_mutex_unlock",&p->_mutex);
    if (ret == 0)
    {
       ret = pthread_cond_broadcast(&p->_cond);
       if (ret != 0) dump_error(__LINE__,ret,"ES::pthread_cond_broadcast",&p->_cond);
    }
  }
  return ret;
}

HRes Event_Reset(CEvent *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret != 0) dump_error(__LINE__,ret,"ER::pthread_mutex_lock",&p->_mutex);
  if (ret == 0)
  {
    p->_state = FALSE;
    ret = pthread_mutex_unlock(&p->_mutex);
    if (ret != 0) dump_error(__LINE__,ret,"ER::pthread_mutex_unlock",&p->_mutex);
  }
  return ret;

}
 
HRes Event_Wait(CEvent *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret != 0) dump_error(__LINE__,ret,"EW::pthread_mutex_lock",&p->_mutex);
  if (ret == 0)
  {
    while ((p->_state == FALSE) && (ret == 0))
    {
       ret = pthread_cond_wait(&p->_cond, &p->_mutex);
       if (ret != 0) dump_error(__LINE__,ret,"EW::pthread_cond_wait",&p->_mutex);
    }
    if (ret == 0)
    {
       if (p->_manual_reset == FALSE)
       {
         p->_state = FALSE;
       }
       ret = pthread_mutex_unlock(&p->_mutex);
       if (ret != 0) dump_error(__LINE__,ret,"EW::pthread_mutex_unlock",&p->_mutex);
    }
  }
  return ret;
}

HRes Event_Close(CEvent *p) { 
  if (p->_created)
  {
    p->_created = 0;
    int ret = pthread_mutex_destroy(&p->_mutex);
    if (ret != 0) dump_error(__LINE__,ret,"EC::pthread_mutex_destroy",&p->_mutex);
    ret = pthread_cond_destroy(&p->_cond);
    if (ret != 0) dump_error(__LINE__,ret,"EC::pthread_cond_destroy",&p->_cond);
  }
  return 0;
}

HRes Semaphore_Create(CSemaphore *p, UInt32 initiallyCount, UInt32 maxCount)
{
  /* memset(&p->_mutex,0,sizeof(p->_mutex)); */
  pthread_mutexattr_t mutexattr;
  int ret = pthread_mutexattr_init(&mutexattr);
  if (ret != 0) dump_error(__LINE__,ret,"SemC::pthread_mutexattr_init",&mutexattr);
  ret = pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_ERRORCHECK_NP);
  if (ret != 0) dump_error(__LINE__,ret,"SemC::pthread_mutexattr_settype",&mutexattr);
  ret = pthread_mutex_init(&p->_mutex,&mutexattr);
  if (ret != 0) dump_error(__LINE__,ret,"SemC::pthread_mutexattr_init",&p->_mutex);
  if (ret == 0)
  {
    /* memset(&p->_cond,0,sizeof(p->_cond)); */
    ret = pthread_cond_init(&p->_cond,0);
    if (ret != 0) dump_error(__LINE__,ret,"SemC::pthread_cond_init",&p->_mutex);
    p->_count    = initiallyCount;
    p->_maxCount = maxCount;
    p->_created = 1;
  }
  return ret;
}

HRes Semaphore_ReleaseN(CSemaphore *p, UInt32 releaseCount)
{
  int ret;
  if (releaseCount < 1) return EINVAL;

  ret = pthread_mutex_lock(&p->_mutex);
  if (ret != 0) dump_error(__LINE__,ret,"SemR::pthread_mutex_lock",&p->_mutex);
  if (ret == 0)
  {
    UInt32 newCount = p->_count + releaseCount;
    if (newCount > p->_maxCount)
    {
      ret = pthread_mutex_unlock(&p->_mutex);
      if (ret != 0) dump_error(__LINE__,ret,"SemR::pthread_mutex_unlock",&p->_mutex);
      return EINVAL;
    }
    p->_count = newCount;
    ret = pthread_mutex_unlock(&p->_mutex);
    if (ret != 0) dump_error(__LINE__,ret,"SemR::pthread_mutex_unlock",&p->_mutex);
    if (ret == 0)
    {
       ret = pthread_cond_broadcast(&p->_cond);
       if (ret != 0) dump_error(__LINE__,ret,"SemR::pthread_cond_broadcast",&p->_cond);
    }
  }
  return ret;
}

HRes Semaphore_Release1(CSemaphore *p)
{
  return Semaphore_ReleaseN(p, 1);
}


HRes Semaphore_Wait(CSemaphore *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret != 0) dump_error(__LINE__,ret,"SemW::pthread_mutex_lock",&p->_mutex);
  if (ret == 0)
  {
    while ((p->_count < 1) && (ret == 0))
    {
       ret = pthread_cond_wait(&p->_cond, &p->_mutex);
       if (ret != 0) dump_error(__LINE__,ret,"SemW::pthread_cond_wait",&p->_mutex);
    }
    if (ret == 0)
    {
      p->_count--;
      ret = pthread_mutex_unlock(&p->_mutex);
      if (ret != 0) dump_error(__LINE__,ret,"SemW::pthread_mutex_unlock",&p->_mutex);
    }
  }
  return ret;
}

HRes Semaphore_Close(CSemaphore *p) {
  if (p->_created)
  {
    p->_created = 0;
    int ret = pthread_mutex_destroy(&p->_mutex);
    if (ret != 0) dump_error(__LINE__,ret,"Semc::pthread_mutex_destroy",&p->_mutex);
    ret = pthread_cond_destroy(&p->_cond);
    if (ret != 0) dump_error(__LINE__,ret,"Semc::pthread_cond_destroy",&p->_cond);
  }
  return 0;
}

HRes CriticalSection_Init(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		pthread_mutexattr_t mutexattr;
		int ret = pthread_mutexattr_init(&mutexattr);
		if (ret != 0) dump_error(__LINE__,ret,"CS I::pthread_mutexattr_init",&mutexattr);
		ret = pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_ERRORCHECK_NP);
		if (ret != 0) dump_error(__LINE__,ret,"CS I::pthread_mutexattr_settype",&mutexattr);
		ret = pthread_mutex_init(&lpCriticalSection->_mutex,&mutexattr);
		if (ret != 0) dump_error(__LINE__,ret,"CS I::pthread_mutexattr_init",&lpCriticalSection->_mutex);
		return ret;
	}
	return EINTR;
}

void CriticalSection_Enter(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		int ret = pthread_mutex_lock(&(lpCriticalSection->_mutex));
                if (ret != 0) dump_error(__LINE__,ret,"CS::pthread_mutex_lock",&(lpCriticalSection->_mutex));
	}
}

void CriticalSection_Leave(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		int ret = pthread_mutex_unlock(&(lpCriticalSection->_mutex));
                if (ret != 0) dump_error(__LINE__,ret,"CS::pthread_mutex_unlock",&(lpCriticalSection->_mutex));
	}
}

void CriticalSection_Delete(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		int ret = pthread_mutex_destroy(&(lpCriticalSection->_mutex));
                if (ret != 0) dump_error(__LINE__,ret,"CS::pthread_mutex_destroy",&(lpCriticalSection->_mutex));
	}
}

#else

HRes AutoResetEvent_Create(CAutoResetEvent *p, int initialSignaled)
{
  /* memset(&p->_mutex,0,sizeof(p->_mutex)); */
  int ret = pthread_mutex_init(&p->_mutex,0);
  if (ret == 0)
  {
    /* memset(&p->_cond,0,sizeof(p->_cond)); */
    ret = pthread_cond_init(&p->_cond,0);
    p->_manual_reset = FALSE;
    p->_state        = (initialSignaled ? TRUE : FALSE);
    p->_created = 1;
  }
  return ret;
}

HRes AutoResetEvent_CreateNotSignaled(CAutoResetEvent *p) { return AutoResetEvent_Create(p, 0); }

HRes Event_Set(CEvent *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret == 0)
  {
    p->_state = TRUE;
    ret = pthread_mutex_unlock(&p->_mutex);
    if (ret == 0)
    {
       ret = pthread_cond_broadcast(&p->_cond);
    }
  }
  return ret;
}

HRes Event_Reset(CEvent *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret == 0)
  {
    p->_state = FALSE;
    ret = pthread_mutex_unlock(&p->_mutex);
  }
  return ret;

}
 
HRes Event_Wait(CEvent *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret == 0)
  {
    while ((p->_state == FALSE) && (ret == 0))
    {
       ret = pthread_cond_wait(&p->_cond, &p->_mutex);
    }
    if (ret == 0)
    {
       if (p->_manual_reset == FALSE)
       {
         p->_state = FALSE;
       }
       ret = pthread_mutex_unlock(&p->_mutex);
    }
  }
  return ret;
}

HRes Event_Close(CEvent *p) { 
  if (p->_created)
  {
    p->_created = 0;
    pthread_mutex_destroy(&p->_mutex);
    pthread_cond_destroy(&p->_cond);
  }
  return 0;
}

HRes Semaphore_Create(CSemaphore *p, UInt32 initiallyCount, UInt32 maxCount)
{
  /* memset(&p->_mutex,0,sizeof(p->_mutex)); */
  int ret = pthread_mutex_init(&p->_mutex,0);
  if (ret == 0)
  {
    /* memset(&p->_cond,0,sizeof(p->_cond)); */
    ret = pthread_cond_init(&p->_cond,0);
    p->_count    = initiallyCount;
    p->_maxCount = maxCount;
    p->_created = 1;
  }
  return ret;
}

HRes Semaphore_ReleaseN(CSemaphore *p, UInt32 releaseCount)
{
  int ret;
  if (releaseCount < 1) return EINVAL;

  ret = pthread_mutex_lock(&p->_mutex);
  if (ret == 0)
  {
    UInt32 newCount = p->_count + releaseCount;
    if (newCount > p->_maxCount)
    {
      ret = pthread_mutex_unlock(&p->_mutex);
      return EINVAL;
    }
    p->_count = newCount;
    ret = pthread_mutex_unlock(&p->_mutex);
    if (ret == 0)
    {
       ret = pthread_cond_broadcast(&p->_cond);
    }
  }
  return ret;
}

HRes Semaphore_Release1(CSemaphore *p)
{
  return Semaphore_ReleaseN(p, 1);
}


HRes Semaphore_Wait(CSemaphore *p) {
  int ret = pthread_mutex_lock(&p->_mutex);
  if (ret == 0)
  {
    while ((p->_count < 1) && (ret == 0))
    {
       ret = pthread_cond_wait(&p->_cond, &p->_mutex);
    }
    if (ret == 0)
    {
      p->_count--;
      ret = pthread_mutex_unlock(&p->_mutex);
    }
  }
  return ret;
}

HRes Semaphore_Close(CSemaphore *p) {
  if (p->_created)
  {
    p->_created = 0;
    pthread_mutex_destroy(&p->_mutex);
    pthread_cond_destroy(&p->_cond);
  }
  return 0;
}

HRes CriticalSection_Init(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		return pthread_mutex_init(&(lpCriticalSection->_mutex),0);
	}
	return EINTR;
}

void CriticalSection_Enter(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		pthread_mutex_lock(&(lpCriticalSection->_mutex));
	}
}

void CriticalSection_Leave(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		pthread_mutex_unlock(&(lpCriticalSection->_mutex));
	}
}

void CriticalSection_Delete(CCriticalSection * lpCriticalSection)
{
	if (lpCriticalSection)
	{
		pthread_mutex_destroy(&(lpCriticalSection->_mutex));
	}
}

#endif /* DEBUG_SYNCHRO */

#endif

