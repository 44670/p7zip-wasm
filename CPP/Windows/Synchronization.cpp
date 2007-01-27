// Windows/Synchronization.cpp

#include "StdAfx.h"

#include "Synchronization.h"

static NWindows::NSynchronization::CCriticalSection gbl_criticalSection;

#define myEnter() gbl_criticalSection.Enter()
#define myLeave() gbl_criticalSection.Leave()
#define myYield() gbl_criticalSection.WaitCond()

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

  myEnter();
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
        myLeave();
        return WAIT_OBJECT_0;
      } else {
        wait_count -= wait_delta;
        if (wait_count) myYield();
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
            myLeave();
            return WAIT_OBJECT_0+i;
          }
          break;
	case NWindows::NSynchronization::CBaseHandle::SEMAPHORE :
          if (hitem->u.sema.count > 0) {
            hitem->u.sema.count--;
            myLeave();
            return WAIT_OBJECT_0+i;
          }
          break;
        default:
          printf("\n\n INTERNAL ERROR - WaitForMultipleObjects(...,true) with unknown type (%d)\n\n",hitem->type);
          abort();
        }
      }
      wait_count -= wait_delta;
      if (wait_count) myYield();
    }
  }
  myLeave();
  return WAIT_TIMEOUT;
}


namespace NWindows {
namespace NSynchronization {

bool CBaseEvent::Set()
{
  myEnter();
  this->u.event._state = true;
  myLeave();
  gbl_criticalSection.SignalCond();
  return true;
}

bool CBaseEvent::Reset()
{
  myEnter();
  this->u.event._state = false;
  myLeave();
  gbl_criticalSection.SignalCond();
  return true;
}

bool CBaseEvent::Lock()
{
  myEnter();
  while(true) {
    if (this->u.event._state == true) {
      if (this->u.event._manual_reset == false) {
        this->u.event._state = false;
      }
      myLeave();
      return true;
    }
    myYield();
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

  myEnter();
  LONG newCount = this->u.sema.count + releaseCount;
  if (newCount > this->u.sema.maxCount)
  {
    myLeave();
    return false;
  }
  this->u.sema.count = newCount;

  myLeave();

  gbl_criticalSection.SignalCond(); // wake up "WaitForMultipleObjects"

  return true;
}

}}

