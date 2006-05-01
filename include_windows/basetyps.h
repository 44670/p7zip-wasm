#ifndef _BASETYPS_H
#define _BASETYPS_H

#ifndef __OBJC__
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif  /* __cplusplus */ 

#ifdef HAVE_GCCVISIBILITYPATCH
    #define DLLEXPORT __attribute__ ((visibility("default")))
    #define DLLLOCAL __attribute__ ((visibility("hidden")))
  #else
    #define DLLEXPORT
    #define DLLLOCAL
  #endif

#define STDAPI      EXTERN_C DLLEXPORT HRESULT

#if defined(__cplusplus) && !defined(CINTERFACE)
#define THIS_
#define THIS	void
#define DECLARE_INTERFACE(i) struct i
#define DECLARE_INTERFACE_(i,b) struct i : public b
#endif
#define BEGIN_INTERFACE
#define END_INTERFACE

#endif /* __OBJC__ */

typedef GUID IID;
typedef GUID CLSID;
#endif
