// Windows/Error.h

#include "StdAfx.h"

#include "Windows/Error.h"
#ifndef _UNICODE
#include "Common/StringConvert.h"
#endif

namespace NWindows {
namespace NError {

bool MyFormatMessage(DWORD messageID, CSysString &message)
{
  const char * txt = 0;

  switch(messageID) {
    case ERROR_NO_MORE_FILES   : txt = "No more files"; break ;
    case E_NOTIMPL             : txt = "E_NOTIMPL"; break ;
    case E_NOINTERFACE         : txt = "E_NOINTERFACE"; break ;
    case E_ABORT               : txt = "E_ABORT"; break ;
    case E_FAIL                : txt = "E_FAIL"; break ;
    case STG_E_INVALIDFUNCTION : txt = "STG_E_INVALIDFUNCTION"; break ;
    case E_OUTOFMEMORY         : txt = "E_OUTOFMEMORY"; break ;
    case E_INVALIDARG          : txt = "E_INVALIDARG"; break ;
    default:
      txt = strerror(messageID);
  }
  if (txt) {
    message = txt;
  } else {
    char msgBuf[256];
    sprintf(msgBuf,"error #%x",(unsigned)messageID);
    message = msgBuf;
  }
  
  message += "                ";
  return true;
}

#ifndef _UNICODE
bool MyFormatMessage(DWORD messageID, UString &message)
{
    CSysString messageSys;
    bool result = MyFormatMessage(messageID, messageSys);
    message = GetUnicodeString(messageSys);
    return result;
}
#endif

}}

