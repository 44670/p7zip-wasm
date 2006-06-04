// DeflateEncoder.h

#ifndef __DEFLATE_ENCODER_H
#define __DEFLATE_ENCODER_H

#include "Common/MyCom.h"

#include "../../ICoder.h"
#include "../../Common/LSBFEncoder.h"
#include "../LZ/IMatchFinder.h"
#include "../Huffman/HuffmanEncoder.h"

#include "DeflateConst.h"

namespace NCompress {
namespace NDeflate {
namespace NEncoder {

struct CCodeValue
{
  UInt16 Len;
  UInt16 Pos;
  void SetAsLiteral() { Len = (1 << 15); }
  bool IsLiteral() const { return ((Len & (1 << 15)) != 0); }
};

struct COptimal
{
  UInt32 Price;
  UInt16 PosPrev;
  UInt16 BackPrev;
};

const UInt32 kNumOptsBase = 1 << 12;
const UInt32 kNumOpts = kNumOptsBase + kMatchMaxLen;

class CCoder;

struct CTables: public CLevels
{
  bool UseSubBlocks;
  bool StoreMode;
  bool StaticMode;
  UInt32 BlockSizeRes;
  UInt32 m_Pos;
  void InitStructures();
};

class CCoder
{
  CMyComPtr<IMatchFinder> m_MatchFinder;
  NStream::NLSBF::CEncoder m_OutStream;

public:
  CCodeValue *m_Values;

  UInt16 *m_MatchDistances;
  UInt32 m_NumFastBytes;

  UInt16 *m_OnePosMatchesMemory;
  UInt16 *m_DistanceMemory;

  UInt32 m_Pos;

  int m_NumPasses;
  int m_NumDivPasses;
  bool m_CheckStatic;
  bool m_IsMultiPass;
  UInt32 m_ValueBlockSize;

  UInt32 m_NumLenCombinations;
  UInt32 m_MatchMaxLen;
  const Byte *m_LenStart;
  const Byte *m_LenDirectBits;

  bool m_Created;
  bool m_Deflate64Mode;

  NCompression::NHuffman::CEncoder MainCoder;
  NCompression::NHuffman::CEncoder DistCoder;
  NCompression::NHuffman::CEncoder LevelCoder;

  Byte m_LevelLevels[kLevelTableSize];
  int m_NumLitLenLevels;
  int m_NumDistLevels;
  UInt32 m_NumLevelCodes;
  UInt32 m_ValueIndex;

  bool m_SecondPass;
  UInt32 m_AdditionalOffset;

  UInt32 m_OptimumEndIndex;
  UInt32 m_OptimumCurrentIndex;
  
  Byte  m_LiteralPrices[256];
  Byte  m_LenPrices[kNumLenSymbolsMax];
  Byte  m_PosPrices[kDistTableSize64];

  CLevels m_NewLevels;
  UInt32 BlockSizeRes;

  CTables *m_Tables;
  COptimal m_Optimum[kNumOpts];

  UInt32 m_MatchFinderCycles;
  IMatchFinderSetNumPasses *m_SetMfPasses;

  void GetMatches();
  void MovePos(UInt32 num);
  UInt32 Backward(UInt32 &backRes, UInt32 cur);
  UInt32 GetOptimal(UInt32 &backRes);

  void CodeLevelTable(NStream::NLSBF::CEncoder *outStream, const Byte *levels, int numLevels);

  void MakeTables();
  UInt32 GetLzBlockPrice();
  void TryBlock(bool staticMode);
  UInt32 TryDynBlock(int tableIndex, UInt32 numPasses);

  UInt32 TryFixedBlock(int tableIndex);

  void SetPrices(const CLevels &levels);
  void WriteBlock();
  void WriteDynBlock(bool finalBlock);
  void WriteFixedBlock(bool finalBlock);



  HRESULT Create();
  void Free();

  void WriteStoreBlock(UInt32 blockSize, UInt32 additionalOffset, bool finalBlock);
  void WriteTables(bool writeMode, bool finalBlock);
  
  void WriteBlockData(bool writeMode, bool finalBlock);

  void ReleaseStreams()
  {
    // m_MatchFinder.ReleaseStream();
    m_OutStream.ReleaseStream();
  }
  class CCoderReleaser
  {
    CCoder *m_Coder;
  public:
    CCoderReleaser(CCoder *coder): m_Coder(coder) {}
    ~CCoderReleaser() { m_Coder->ReleaseStreams(); }
  };
  friend class CCoderReleaser;

  UInt32 GetBlockPrice(int tableIndex, int numDivPasses);
  void CodeBlock(int tableIndex, bool finalBlock);

public:
  CCoder(bool deflate64Mode = false);
  ~CCoder();

  HRESULT CodeReal(ISequentialInStream *inStream,
      ISequentialOutStream *outStream, const UInt64 *inSize, const UInt64 *outSize,
      ICompressProgressInfo *progress);

  HRESULT BaseCode(ISequentialInStream *inStream,
      ISequentialOutStream *outStream, const UInt64 *inSize, const UInt64 *outSize,
      ICompressProgressInfo *progress);

  // ICompressSetCoderProperties
  HRESULT BaseSetEncoderProperties2(const PROPID *propIDs, 
      const PROPVARIANT *properties, UInt32 numProperties);
};

///////////////////////////////////////////////////////////////

class CCOMCoder :
  public ICompressCoder,
  public ICompressSetCoderProperties, 
  public CMyUnknownImp,
  public CCoder
{
public:
  MY_UNKNOWN_IMP1(ICompressSetCoderProperties)
  CCOMCoder(): CCoder(false) {};
  STDMETHOD(Code)(ISequentialInStream *inStream,
      ISequentialOutStream *outStream, const UInt64 *inSize, const UInt64 *outSize,
      ICompressProgressInfo *progress);
  // ICompressSetCoderProperties
  STDMETHOD(SetCoderProperties)(const PROPID *propIDs, 
      const PROPVARIANT *properties, UInt32 numProperties);
};

class CCOMCoder64 :
  public ICompressCoder,
  public ICompressSetCoderProperties,
  public CMyUnknownImp,
  public CCoder
{
public:
  MY_UNKNOWN_IMP1(ICompressSetCoderProperties)
  CCOMCoder64(): CCoder(true) {};
  STDMETHOD(Code)(ISequentialInStream *inStream,
      ISequentialOutStream *outStream, const UInt64 *inSize, const UInt64 *outSize,
      ICompressProgressInfo *progress);
  // ICompressSetCoderProperties
  STDMETHOD(SetCoderProperties)(const PROPID *propIDs, 
      const PROPVARIANT *properties, UInt32 numProperties);
};


}}}

#endif
