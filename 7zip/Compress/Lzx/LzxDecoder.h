// LzxDecoder.h

#ifndef __LZXDECODER_H
#define __LZXDECODER_H

#include "../../ICoder.h"

#include "../../Compress/Huffman/HuffmanDecoder.h"
#include "../../Compress/LZ/LZOutWindow.h"
#include "../../Common/InBuffer.h"

#include "Lzx.h"
#include "Lzx86Converter.h"

namespace NCompress {
namespace NLzx {

namespace NBitStream {

const int kNumBigValueBits = 8 * 4;
const int kNumValueBits = 17;
const UInt32 kBitDecoderValueMask = (1 << kNumValueBits) - 1;

class CDecoder
{
  CInBuffer m_Stream;
  UInt32 m_Value;
  int m_BitPos;
public:
  CDecoder() {}
  bool Create(UInt32 bufferSize) { return m_Stream.Create(bufferSize); }

  void SetStream(ISequentialInStream *s) { m_Stream.SetStream(s); }
  void ReleaseStream() { m_Stream.ReleaseStream(); }

  void InitNormal()
  {
    m_Stream.Init();
    m_BitPos = kNumBigValueBits; 
    Normalize();
  }

  void InitDirect()
  {
    m_Stream.Init();
    m_BitPos = kNumBigValueBits; 
  }

  UInt64 GetProcessedSize() const 
    { return m_Stream.GetProcessedSize() - (kNumBigValueBits - m_BitPos) / 8; }
  
  int GetBitPosition() const { return m_BitPos & 0xF; }

  void Normalize()
  {
    for (;m_BitPos >= 16; m_BitPos -= 16)
    {
      Byte b0 = m_Stream.ReadByte();
      Byte b1 = m_Stream.ReadByte();
      m_Value = (m_Value << 8) | b1;
      m_Value = (m_Value << 8) | b0;
    }
  }

  UInt32 GetValue(int numBits) const
  {
    return ((m_Value >> ((32 - kNumValueBits) - m_BitPos)) & kBitDecoderValueMask) >> 
        (kNumValueBits - numBits);
  }
  
  void MovePos(UInt32 numBits)
  {
    m_BitPos += numBits;
    Normalize();
  }

  UInt32 ReadBits(int numBits)
  {
    UInt32 res = GetValue(numBits);
    MovePos(numBits);
    return res;
  }

  UInt32 ReadBitsBig(int numBits)
  {
    UInt32 numBits0 = numBits / 2;
    UInt32 numBits1 = numBits - numBits0;
    UInt32 res = ReadBits(numBits0) << numBits1;
    return res + ReadBits(numBits1);
  }

  bool ReadUInt32(UInt32 &v)
  {
    if (m_BitPos != 0)
      return false;
    v = ((m_Value >> 16) & 0xFFFF) | ((m_Value << 16) & 0xFFFF0000);
    m_BitPos = kNumBigValueBits;
    return true;
  }

  Byte DirectReadByte() { return m_Stream.ReadByte(); }

  void Align(int alignPos)
  {
    if (((m_Stream.GetProcessedSize() + alignPos) & 1) != 0)
      m_Stream.ReadByte();
    Normalize();
  }
};
}

class CDecoder : 
  public ICompressCoder,
  public CMyUnknownImp
{
  NBitStream::CDecoder m_InBitStream;
  CLZOutWindow m_OutWindowStream;

  UInt32 m_RepDistances[kNumRepDistances];
  UInt32 m_NumPosLenSlots;

  bool m_IsUncompressedBlock;
  bool m_AlignIsUsed;

  NCompress::NHuffman::CDecoder<kNumHuffmanBits, kMainTableSize> m_MainDecoder;
  NCompress::NHuffman::CDecoder<kNumHuffmanBits, kNumLenSymbols> m_LenDecoder;
  NCompress::NHuffman::CDecoder<kNumHuffmanBits, kAlignTableSize> m_AlignDecoder;
  NCompress::NHuffman::CDecoder<kNumHuffmanBits, kLevelTableSize> m_LevelDecoder;

  Byte m_LastMainLevels[kMainTableSize];
  Byte m_LastLenLevels[kNumLenSymbols];

  Cx86ConvertOutStream *m_x86ConvertOutStreamSpec;
  CMyComPtr<ISequentialOutStream> m_x86ConvertOutStream;

  UInt32 m_UnCompressedBlockSize;

  bool _keepHistory;
  int _remainLen;
  int m_AlignPos;

  UInt32 ReadBits(UInt32 numBits);
  bool ReadTable(Byte *lastLevels, Byte *newLevels, UInt32 numSymbols);
  bool ReadTables();
  void ClearPrevLevels();

  HRESULT CodeSpec(UInt32 size);

  HRESULT CodeReal(ISequentialInStream *inStream, 
      ISequentialOutStream *outStream, 
      const UInt64 *inSize, const UInt64 *outSize,
      ICompressProgressInfo *progress);
public:
  CDecoder();

  MY_UNKNOWN_IMP

  void ReleaseStreams();
  STDMETHOD(Flush)();

  // ICompressCoder interface
  STDMETHOD(Code)(ISequentialInStream *inStream, 
      ISequentialOutStream *outStream, 
      const UInt64 *inSize, const UInt64 *outSize,
      ICompressProgressInfo *progress);

  STDMETHOD(SetInStream)(ISequentialInStream *inStream);
  STDMETHOD(ReleaseInStream)();
  STDMETHOD(SetOutStreamSize)(const UInt64 *outSize);

  HRESULT SetParams(int numDictBits);
  void SetKeepHistory(bool keepHistory, int alignPos)
  {
    _keepHistory = keepHistory;
    m_AlignPos = alignPos;
  }
};

}}

#endif
