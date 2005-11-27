// Archive/GZipIn.h

#ifndef __ARCHIVE_GZIP_IN_H
#define __ARCHIVE_GZIP_IN_H

#include "GZipHeader.h"
#include "GZipItem.h"
#include "Common/CRC.h"
#include "../../IStream.h"

namespace NArchive {
namespace NGZip {
  
class CInArchive
{
  UInt64 m_Position;
  
  HRESULT ReadBytes(ISequentialInStream *inStream, void *data, UInt32 size);
  HRESULT ReadZeroTerminatedString(ISequentialInStream *inStream, AString &resString, CCRC &crc);
  HRESULT ReadByte(ISequentialInStream *inStream, Byte &value);
  HRESULT ReadUInt16(ISequentialInStream *inStream, UInt16 &value);
  HRESULT ReadUInt32(ISequentialInStream *inStream, UInt32 &value);
public:
  HRESULT ReadHeader(ISequentialInStream *inStream, CItem &item);
  HRESULT ReadPostHeader(ISequentialInStream *inStream, CItem &item);
  UInt64 GetOffset() const { return m_Position; }
};
  
}}
  
#endif
