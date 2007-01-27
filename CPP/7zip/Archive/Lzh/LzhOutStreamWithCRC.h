// LzhOutStreamWithCRC.h

#ifndef __LZHOUTSTREAMWITHCRC_H
#define __LZHOUTSTREAMWITHCRC_H

#include "LzhCRC.h"
#include "../../../Common/MyCom.h"
#include "../../IStream.h"

namespace NArchive {
namespace NLzh {

class COutStreamWithCRC: 
  public ISequentialOutStream,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP

  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
private:
  CCRC _crc;
  CMyComPtr<ISequentialOutStream> _stream;
public:
  void Init(ISequentialOutStream *stream)
  {
    _stream = stream;
    _crc.Init();
  }
  void ReleaseStream() { _stream.Release(); }
  UInt32 GetCRC() const { return _crc.GetDigest(); }
  void InitCRC() { _crc.Init(); }

};

}}

#endif
