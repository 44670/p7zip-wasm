// DLLExports.cpp

#include "StdAfx.h"

#include "Common/MyInitGuid.h"
#include "Common/ComTry.h"
#include "Windows/PropVariant.h"
#include "../../ICoder.h"
#include "NsisHandler.h"

// {23170F69-40C1-278A-1000-000110090000}
DEFINE_GUID(CLSID_CNsisHandler, 
  0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x09, 0x00, 0x00);

HINSTANCE g_hInstance;
#ifdef _WIN32
#ifndef _UNICODE
bool g_IsNT = false;
static bool IsItWindowsNT()
{
  OSVERSIONINFO versionInfo;
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (!::GetVersionEx(&versionInfo)) 
    return false;
  return (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}
#endif
#endif

extern "C"
DLLEXPORT BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    g_hInstance = hInstance;
#ifdef _WIN32
    #ifndef _UNICODE
    g_IsNT = IsItWindowsNT();
    #endif
#endif
  }
  return TRUE;
}

STDAPI CreateObject(
    const GUID *classID, 
    const GUID *interfaceID, 
    void **outObject)
{
  COM_TRY_BEGIN
  *outObject = 0;
  if (*classID != CLSID_CNsisHandler)
    return CLASS_E_CLASSNOTAVAILABLE;
  int needIn = *interfaceID == IID_IInArchive;
  // int needOut = *interfaceID == IID_IOutArchive;
  if (needIn /*|| needOut */)
  {
    NArchive::NNsis::CHandler *temp = new NArchive::NNsis::CHandler;
    if (needIn)
    {
      CMyComPtr<IInArchive> inArchive = (IInArchive *)temp;
      *outObject = inArchive.Detach();
    }
    /*
    else
    {
      CMyComPtr<IOutArchive> outArchive = (IOutArchive *)temp;
      *outObject = outArchive.Detach();
    }
    */
  }
  else
    return E_NOINTERFACE;
  COM_TRY_END
  return S_OK;
}

STDAPI GetHandlerProperty(PROPID propID, PROPVARIANT *value)
{
  NWindows::NCOM::CPropVariant propVariant;
  switch(propID)
  {
    case NArchive::kName:
      propVariant = L"Nsis";
      break;
    case NArchive::kClassID:
    {
      if ((value->bstrVal = ::SysAllocStringByteLen(
          (const char *)&CLSID_CNsisHandler, sizeof(GUID))) != 0)
        value->vt = VT_BSTR;
      return S_OK;
    }
    case NArchive::kExtension:
      propVariant = L"exe";
      break;
    case NArchive::kUpdate:
      propVariant = false;
      break;
    case NArchive::kStartSignature:
    {
      if ((value->bstrVal = ::SysAllocStringByteLen((const char *)NArchive::NNsis::kSignature, 
          NArchive::NNsis::kSignatureSize)) != 0)
        value->vt = VT_BSTR;
      return S_OK;
    }
    case NArchive::kAssociate:
    {
      propVariant = false;
      break;
    }
  }
  propVariant.Detach(value);
  return S_OK;
}
