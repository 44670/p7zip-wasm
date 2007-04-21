// NSisHandler.h

#ifndef __NSIS_HANDLER_H
#define __NSIS_HANDLER_H

#include "Common/MyCom.h"
#include "../IArchive.h"

#include "NsisIn.h"

#include "../../Common/CreateCoder.h"

namespace NArchive {
namespace NNsis {

class CHandler: 
  public IInArchive,
  PUBLIC_ISetCompressCodecsInfo
  public CMyUnknownImp
{
  CMyComPtr<IInStream> _inStream;
  CInArchive _archive;

  DECL_EXTERNAL_CODECS_VARS

  bool GetUncompressedSize(int index, UInt32 &size);
  bool GetCompressedSize(int index, UInt32 &size);

public:
  MY_QUERYINTERFACE_BEGIN
  MY_QUERYINTERFACE_ENTRY(IInArchive)
  QUERY_ENTRY_ISetCompressCodecsInfo
  MY_QUERYINTERFACE_END
  MY_ADDREF_RELEASE

  STDMETHOD(Open)(IInStream *stream, const UInt64 *maxCheckStartPosition, IArchiveOpenCallback *openArchiveCallback);  
  STDMETHOD(Close)();  
  STDMETHOD(GetNumberOfItems)(UInt32 *numItems);  
  STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT *value);
  STDMETHOD(Extract)(const UInt32* indices, UInt32 numItems, Int32 testMode, IArchiveExtractCallback *extractCallback);
  STDMETHOD(GetArchiveProperty)(PROPID propID, PROPVARIANT *value);
  STDMETHOD(GetNumberOfProperties)(UInt32 *numProperties);  
  STDMETHOD(GetPropertyInfo)(UInt32 index, BSTR *name, PROPID *propID, VARTYPE *varType);
  STDMETHOD(GetNumberOfArchiveProperties)(UInt32 *numProperties);  
  STDMETHOD(GetArchivePropertyInfo)(UInt32 index, BSTR *name, PROPID *propID, VARTYPE *varType);

  DECL_ISetCompressCodecsInfo
};

}}

#endif
