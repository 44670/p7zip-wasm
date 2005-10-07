// ConsoleClose.cpp

#include "StdAfx.h"

#include "ConsoleClose.h"

#ifdef ENV_UNIX
#include <signal.h>
#endif

static int g_BreakCounter = 0;
static const int kBreakAbortThreshold = 2;

namespace NConsoleClose {

#ifdef ENV_UNIX
static void HandlerRoutine(int)
{
  g_BreakCounter++;
  if (g_BreakCounter < kBreakAbortThreshold)
    return ;
  exit(EXIT_FAILURE);
}
#else
static BOOL WINAPI HandlerRoutine(DWORD aCtrlType)
{
  g_BreakCounter++;
  if (g_BreakCounter < kBreakAbortThreshold)
    return TRUE;
  return FALSE;
  /*
  switch(aCtrlType)
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      if (g_BreakCounter < kBreakAbortThreshold)
      return TRUE;
  }
  return FALSE;
  */
}
#endif

bool TestBreakSignal()
{
  /*
  if (g_BreakCounter > 0)
    return true;
  */
  return (g_BreakCounter > 0);
}

void CheckCtrlBreak()
{
  if (TestBreakSignal())
    throw CCtrlBreakException();
}

CCtrlHandlerSetter::CCtrlHandlerSetter()
{
#ifdef ENV_UNIX
   memo_sig_int = signal(SIGINT,HandlerRoutine); // CTRL-C
   if (memo_sig_int == SIG_ERR)
    throw "SetConsoleCtrlHandler fails (SIGINT)";
   memo_sig_term = signal(SIGTERM,HandlerRoutine); // for kill -15 (before "kill -9")
   if (memo_sig_term == SIG_ERR)
    throw "SetConsoleCtrlHandler fails (SIGTERM)";
#else
  if(!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
    throw "SetConsoleCtrlHandler fails";
#endif
}

CCtrlHandlerSetter::~CCtrlHandlerSetter()
{
#ifdef ENV_UNIX
   signal(SIGINT,memo_sig_int); // CTRL-C
   signal(SIGTERM,memo_sig_term); // kill {pid}
#else
  if(!SetConsoleCtrlHandler(HandlerRoutine, FALSE))
    throw "SetConsoleCtrlHandler fails";
#endif
}

}
