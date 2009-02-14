// LzOutWindow.h

#ifndef __LZ_OUT_WINDOW_H
#define __LZ_OUT_WINDOW_H

#include "../IStream.h"

#include "../Common/OutBuffer.h"

#ifndef _NO_EXCEPTIONS
typedef COutBufferException CLzOutWindowException;
#endif

class CLzOutWindow: public COutBuffer
{
public:
  void Init(bool solid = false);
  
  // distance >= 0, len > 0,
  bool CopyBlock(UInt32 distance, UInt32 len)
  {
    UInt32 pos = _pos - distance - 1;
    if (distance >= _pos)
    {
      if (!_overDict || distance >= _bufferSize)
        return false;
      pos += _bufferSize;
    }
    if (_limitPos - _pos > len && _bufferSize - pos > len)
    {
      const Byte *src = _buffer + pos;
      Byte *dest = _buffer + _pos;
      _pos += len;
      do
        *dest++ = *src++;
      while(--len != 0);
    }
    else do
    {
      if (pos == _bufferSize)
        pos = 0;
      _buffer[_pos++] = _buffer[pos++];
      if (_pos == _limitPos)
        FlushWithCheck();
    }
    while(--len != 0);
    return true;
  }
  
  void PutByte(Byte b)
  {
    _buffer[_pos++] = b;
    if (_pos == _limitPos)
      FlushWithCheck();
  }
  
  Byte GetByte(UInt32 distance) const
  {
    UInt32 pos = _pos - distance - 1;
    if (pos >= _bufferSize)
      pos += _bufferSize;
    return _buffer[pos];
  }
};

#endif
