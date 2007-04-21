// Windows/Synchronization.cpp

#include "StdAfx.h"

#include "Synchronization.h"

#define MAGIC 0x1234CAFE
class CSynchroTest
{
  int _magic;
  public:
  CSynchroTest() {
    _magic = MAGIC;
  }
  void testConstructor() {
    if (_magic != MAGIC) {
      printf("ERROR : no constructors called during loading of plugins (please look at LINK_SHARED in makefile.machine)\n");
      exit(EXIT_FAILURE);
    }
  }
};

#ifdef ENV_BEOS
class CSynchro : BLocker , public CSynchroTest
{
#define MAX_THREAD 256
  thread_id _waiting[MAX_THREAD]; // std::list<thread_id> _waiting;
  int index_waiting;
public:
  CSynchro() { index_waiting = 0; }
  ~CSynchro() {}
  void Enter() { Lock(); }
  void Leave() { Unlock(); }
  void WaitCond() { 
    _waiting[index_waiting++] = find_thread(NULL); // _waiting.push_back(find_thread(NULL));
    thread_id sender;
    Unlock();
    int msg = receive_data(&sender, NULL, 0);
    Lock();
  }
  void LeaveAndSignal() {
    // Unlock();
    // Lock();
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
class CSynchro : public CSynchroTest
{
  pthread_mutex_t _object;
  pthread_cond_t _cond;
  void dump_error(int ligne,int ret,const char *text,void *param)
  {
    printf("\n##T%d#ERROR2 (l=%d) %s : param=%p ret = %d (%s)##\n",(int)pthread_self(),ligne,text,param,ret,strerror(ret));
    // abort();
  }
public:
  CSynchro() {
    pthread_mutexattr_t mutexattr;
    int ret = pthread_mutexattr_init(&mutexattr);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutexattr_init",&mutexattr);
    ret = pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_ERRORCHECK);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutexattr_settype",&mutexattr);
    ret = ::pthread_mutex_init(&_object,&mutexattr);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutex_init",&_object);
    ret = ::pthread_cond_init(&_cond,0);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_cond_init",&_cond);
    TRACEN((printf("\nT%d : E1-Create(%p,%p)\n",(int)pthread_self(),(void *)&_object,(void *)&_cond)))
  }
  ~CSynchro() {
    int ret = ::pthread_mutex_destroy(&_object);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutex_destroy",&_object);
    ret = ::pthread_cond_destroy(&_cond);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_cond_destroy",&_cond);
  }
  void Enter() { 
    TRACEN((printf("\nT%d : E1-lock(%p)\n",(int)pthread_self(),(void *)&_object)))
    int ret = ::pthread_mutex_lock(&_object);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_mutex_lock",&_object);
    TRACEN((printf("\nT%d : E2-lock(%p)\n",(int)pthread_self(),(void *)&_object)))
  }
  void Leave() {
    TRACEN((printf("\nT%d : E1-unlock(%p)\n",(int)pthread_self(),(void *)&_object)))
    int ret = ::pthread_mutex_unlock(&_object);
    if (ret != 0) dump_error(__LINE__,ret,"Leave::pthread_mutex_unlock",&_object);
    TRACEN((printf("\nT%d : E2-unlock(%p)\n",(int)pthread_self(),(void *)&_object)))
  }
  void WaitCond() {
    TRACEN((printf("\nT%d : E1-cond_wait(%p,%p)\n",(int)pthread_self(),(void *)&_cond,(void *)&_object)))
    int ret = ::pthread_cond_wait(&_cond, &_object);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_cond_wait",&_cond);
    TRACEN((printf("\nT%d : E2-cond_wait(%p,%p)\n",(int)pthread_self(),(void *)&_cond,(void *)&_object)))
  }
  void LeaveAndSignal() {
    TRACEN((printf("\nT%d : E0-unlock(%p)\n",(int)pthread_self(),(void *)&_object)))
    int ret = ::pthread_mutex_unlock(&_object);
    if (ret != 0) dump_error(__LINE__,ret,"LeaveAndSignal::pthread_mutex_unlock",&_object);
    TRACEN((printf("\nT%d : E1-cond_broadcast(%p)\n",(int)pthread_self(),(void *)&_cond)))
    ret = ::pthread_cond_broadcast(&_cond);
    if (ret != 0) dump_error(__LINE__,ret,"pthread_cond_broadcast",&_cond);
    TRACEN((printf("\nT%d : E2-cond_broadcast(%p)\n",(int)pthread_self(),(void *)&_cond)))
  }
};
#else
class CSynchro : public CSynchroTest
{
  pthread_mutex_t _object;
  pthread_cond_t _cond;
public:
  CSynchro() {
    ::pthread_mutex_init(&_object,0);
    ::pthread_cond_init(&_cond,0);
  }
  ~CSynchro() {
    ::pthread_mutex_destroy(&_object);
    ::pthread_cond_destroy(&_cond);
  }
  void Enter() { 
     ::pthread_mutex_lock(&_object);
  }
  void Leave() {
    ::pthread_mutex_unlock(&_object);
  }
  void WaitCond() { 
    ::pthread_cond_wait(&_cond, &_object);
  }
  void LeaveAndSignal() { 
    ::pthread_mutex_unlock(&_object);
    ::pthread_cond_broadcast(&_cond);
  }
};
#endif
#endif

static CSynchro gbl_synchro;

#ifdef DEBUG_SYNCHRO
void sync_GetContexts(t_SyncCXT & ctx)
{
	ctx.ctx1 = (void *)(&gbl_synchro);
}

void sync_VerifContexts(const char *txt, const t_SyncCXT & ctx)
{
	if (ctx.ctx1 != (void *)(&gbl_synchro)) {
		printf("\n%s : #ctx1\n",txt);
	}
}
void sync_VerifContexts(const char *txt, const t_SyncCXT & ctx,const t_SyncCXT & ctx1)
{
	if (ctx.ctx1 != (void *)(ctx1.ctx1)) {
		printf("\n%s : #ctx1#\n",txt);
		exit(EXIT_FAILURE);
	}
}
#endif

extern "C" void sync_TestConstructor(void) {
	gbl_synchro.testConstructor();
}


DWORD WINAPI WaitForMultipleObjects( DWORD count, const HANDLE *handles, BOOL wait_all, DWORD timeout )
{
  unsigned wait_count = 1;
  unsigned wait_delta;

  switch (timeout)
  {
    case 0        : wait_delta = 1; break; // trick - one "while"
    case INFINITE : wait_delta = 0; break; // trick - infinite "while"
    default:
      printf("\n\n INTERNAL ERROR - WaitForMultipleObjects(...) timeout(%u) != 0 or INFINITE\n\n",(unsigned)timeout);
      abort();
  }

  gbl_synchro.Enter();

#ifdef DEBUG_SYNCHRO
  for(DWORD i=0;i<count;i++) {
    NWindows::NSynchronization::CBaseHandle* hitem = (NWindows::NSynchronization::CBaseHandle*)handles[i];
    sync_VerifContexts("WFMO",hitem->ctx);
  }
  if (count >= 2)
  {
    NWindows::NSynchronization::CBaseHandle* hitem1 = (NWindows::NSynchronization::CBaseHandle*)handles[0];
    for(DWORD i=1;i<count;i++) {
      NWindows::NSynchronization::CBaseHandle* hitem = (NWindows::NSynchronization::CBaseHandle*)handles[i];
      sync_VerifContexts("WFMO",hitem->ctx,hitem1->ctx);
    }
  }
#endif
  if (wait_all) {
    while(wait_count) {
      bool found_all = true;
      for(DWORD i=0;i<count;i++) {
        NWindows::NSynchronization::CBaseHandle* hitem = (NWindows::NSynchronization::CBaseHandle*)handles[i];

        switch (hitem->type)
        {
	case NWindows::NSynchronization::CBaseHandle::EVENT :
          if (hitem->u.event._state == false) {
            found_all = false;
          }
          break;
	case NWindows::NSynchronization::CBaseHandle::SEMAPHORE :
          if (hitem->u.sema.count == 0) {
            found_all = false;
          }
          break;
        default:
          printf("\n\n INTERNAL ERROR - WaitForMultipleObjects(...,true) with unknown type (%d)\n\n",hitem->type);
          abort();
        }
      }
      if (found_all) {
        for(DWORD i=0;i<count;i++) {
          NWindows::NSynchronization::CBaseHandle* hitem = (NWindows::NSynchronization::CBaseHandle*)handles[i];

          switch (hitem->type)
          {
	  case NWindows::NSynchronization::CBaseHandle::EVENT :
            if (hitem->u.event._manual_reset == false) {
              hitem->u.event._state = false;
            }
            break;
	  case NWindows::NSynchronization::CBaseHandle::SEMAPHORE :
            hitem->u.sema.count--;
            break;
          default:
            printf("\n\n INTERNAL ERROR - WaitForMultipleObjects(...,true) with unknown type (%d)\n\n",hitem->type);
            abort();
          }
        }
        gbl_synchro.Leave();
        return WAIT_OBJECT_0;
      } else {
        wait_count -= wait_delta;
        if (wait_count) gbl_synchro.WaitCond();
      }
    }
  } else {
    while(wait_count) {
      for(DWORD i=0;i<count;i++) {
        NWindows::NSynchronization::CBaseHandle* hitem = (NWindows::NSynchronization::CBaseHandle*)handles[i];
	
        switch (hitem->type)
        {
	case NWindows::NSynchronization::CBaseHandle::EVENT :
          if (hitem->u.event._state == true) {
            if (hitem->u.event._manual_reset == false) {
              hitem->u.event._state = false;
            }
            gbl_synchro.Leave();
            return WAIT_OBJECT_0+i;
          }
          break;
	case NWindows::NSynchronization::CBaseHandle::SEMAPHORE :
          if (hitem->u.sema.count > 0) {
            hitem->u.sema.count--;
            gbl_synchro.Leave();
            return WAIT_OBJECT_0+i;
          }
          break;
        default:
          printf("\n\n INTERNAL ERROR - WaitForMultipleObjects(...,true) with unknown type (%d)\n\n",hitem->type);
          abort();
        }
      }
      wait_count -= wait_delta;
      if (wait_count) gbl_synchro.WaitCond();
    }
  }
  gbl_synchro.Leave();
  return WAIT_TIMEOUT;
}


namespace NWindows {
namespace NSynchronization {

bool CBaseEvent::Set()
{
  gbl_synchro.Enter();
#ifdef DEBUG_SYNCHRO
  sync_VerifContexts("CBE::Set",this->ctx);
#endif
  this->u.event._state = true;
  gbl_synchro.LeaveAndSignal();
  return true;
}

bool CBaseEvent::Reset()
{
  gbl_synchro.Enter();
#ifdef DEBUG_SYNCHRO
  sync_VerifContexts("CBE::Reset",this->ctx);
#endif
  this->u.event._state = false;
  gbl_synchro.LeaveAndSignal();
  return true;
}

bool CBaseEvent::Lock()
{
  gbl_synchro.Enter();
#ifdef DEBUG_SYNCHRO
  sync_VerifContexts("CBE::Lock",this->ctx);
#endif
  while(true) {
    if (this->u.event._state == true) {
      if (this->u.event._manual_reset == false) {
        this->u.event._state = false;
      }
      gbl_synchro.Leave();
      return true;
    }
    gbl_synchro.WaitCond();
  }
  // dead code
}

CEvent::CEvent(bool manualReset, bool initiallyOwn)
{
  if (!Create(manualReset, initiallyOwn))
    throw "CreateEvent error";
}

bool CSemaphore::Release(LONG releaseCount)
{
  if (releaseCount < 1) return false;

  gbl_synchro.Enter();
#ifdef DEBUG_SYNCHRO
  sync_VerifContexts("CSem::Release",this->ctx);
#endif
  LONG newCount = this->u.sema.count + releaseCount;
  if (newCount > this->u.sema.maxCount)
  {
    gbl_synchro.Leave();
    return false;
  }
  this->u.sema.count = newCount;

  gbl_synchro.LeaveAndSignal();

  return true;
}

}}

