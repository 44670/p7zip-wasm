// Tar/Handler.h

#ifndef __TAR_HANDLER_H
#define __TAR_HANDLER_H

#include "Common/MyCom.h"
#include "../IArchive.h"

#include "TarItem.h"

namespace NArchive {
namespace NTar {

class CHandler: 
  public IInArchive,
  public IOutArchive,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP2(
    IInArchive,
    IOutArchive
  )

  STDMETHOD(Open)(IInStream *stream, 
      const UInt64 *maxCheckStartPosition,
      IArchiveOpenCallback *openArchiveCallback);  
  STDMETHOD(Close)();  
  STDMETHOD(GetNumberOfItems)(UInt32 *numItems);  
  STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT *value);
  STDMETHOD(Extract)(const UInt32* indices, UInt32 numItems, 
      Int32 testMode, IArchiveExtractCallback *extractCallback);
  
  STDMETHOD(GetArchiveProperty)(PROPID propID, PROPVARIANT *value);

  STDMETHOD(GetNumberOfProperties)(UInt32 *numProperties);  
  STDMETHOD(GetPropertyInfo)(UInt32 index,     
      BSTR *name, PROPID *propID, VARTYPE *varType);

  STDMETHOD(GetNumberOfArchiveProperties)(UInt32 *numProperties);  
  STDMETHOD(GetArchivePropertyInfo)(UInt32 index,     
      BSTR *name, PROPID *propID, VARTYPE *varType);

  // IOutArchive
  STDMETHOD(UpdateItems)(ISequentialOutStream *outStream, UInt32 numItems,
      IArchiveUpdateCallback *updateCallback);
  STDMETHOD(GetFileTimeType)(UInt32 *type);  

private:
  CObjectVector<CItemEx> _items;
  CMyComPtr<IInStream> _inStream;
};

}}

#endif
