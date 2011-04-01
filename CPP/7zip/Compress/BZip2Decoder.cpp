// BZip2Decoder.cpp

#include "StdAfx.h"

#include "../../../C/Alloc.h"

#include "BZip2Decoder.h"
#include "Mtf8.h"

namespace NCompress {
namespace NBZip2 {

#undef NO_INLINE
#define NO_INLINE
  
static const UInt32 kNumThreadsMax = 4;

static const UInt32 kBufferSize = (1 << 17);

static const UInt16 kRandNums[512] = {
   619, 720, 127, 481, 931, 816, 813, 233, 566, 247,
   985, 724, 205, 454, 863, 491, 741, 242, 949, 214,
   733, 859, 335, 708, 621, 574, 73, 654, 730, 472,
   419, 436, 278, 496, 867, 210, 399, 680, 480, 51,
   878, 465, 811, 169, 869, 675, 611, 697, 867, 561,
   862, 687, 507, 283, 482, 129, 807, 591, 733, 623,
   150, 238, 59, 379, 684, 877, 625, 169, 643, 105,
   170, 607, 520, 932, 727, 476, 693, 425, 174, 647,
   73, 122, 335, 530, 442, 853, 695, 249, 445, 515,
   909, 545, 703, 919, 874, 474, 882, 500, 594, 612,
   641, 801, 220, 162, 819, 984, 589, 513, 495, 799,
   161, 604, 958, 533, 221, 400, 386, 867, 600, 782,
   382, 596, 414, 171, 516, 375, 682, 485, 911, 276,
   98, 553, 163, 354, 666, 933, 424, 341, 533, 870,
   227, 730, 475, 186, 263, 647, 537, 686, 600, 224,
   469, 68, 770, 919, 190, 373, 294, 822, 808, 206,
   184, 943, 795, 384, 383, 461, 404, 758, 839, 887,
   715, 67, 618, 276, 204, 918, 873, 777, 604, 560,
   951, 160, 578, 722, 79, 804, 96, 409, 713, 940,
   652, 934, 970, 447, 318, 353, 859, 672, 112, 785,
   645, 863, 803, 350, 139, 93, 354, 99, 820, 908,
   609, 772, 154, 274, 580, 184, 79, 626, 630, 742,
   653, 282, 762, 623, 680, 81, 927, 626, 789, 125,
   411, 521, 938, 300, 821, 78, 343, 175, 128, 250,
   170, 774, 972, 275, 999, 639, 495, 78, 352, 126,
   857, 956, 358, 619, 580, 124, 737, 594, 701, 612,
   669, 112, 134, 694, 363, 992, 809, 743, 168, 974,
   944, 375, 748, 52, 600, 747, 642, 182, 862, 81,
   344, 805, 988, 739, 511, 655, 814, 334, 249, 515,
   897, 955, 664, 981, 649, 113, 974, 459, 893, 228,
   433, 837, 553, 268, 926, 240, 102, 654, 459, 51,
   686, 754, 806, 760, 493, 403, 415, 394, 687, 700,
   946, 670, 656, 610, 738, 392, 760, 799, 887, 653,
   978, 321, 576, 617, 626, 502, 894, 679, 243, 440,
   680, 879, 194, 572, 640, 724, 926, 56, 204, 700,
   707, 151, 457, 449, 797, 195, 791, 558, 945, 679,
   297, 59, 87, 824, 713, 663, 412, 693, 342, 606,
   134, 108, 571, 364, 631, 212, 174, 643, 304, 329,
   343, 97, 430, 751, 497, 314, 983, 374, 822, 928,
   140, 206, 73, 263, 980, 736, 876, 478, 430, 305,
   170, 514, 364, 692, 829, 82, 855, 953, 676, 246,
   369, 970, 294, 750, 807, 827, 150, 790, 288, 923,
   804, 378, 215, 828, 592, 281, 565, 555, 710, 82,
   896, 831, 547, 261, 524, 462, 293, 465, 502, 56,
   661, 821, 976, 991, 658, 869, 905, 758, 745, 193,
   768, 550, 608, 933, 378, 286, 215, 979, 792, 961,
   61, 688, 793, 644, 986, 403, 106, 366, 905, 644,
   372, 567, 466, 434, 645, 210, 389, 550, 919, 135,
   780, 773, 635, 389, 707, 100, 626, 958, 165, 504,
   920, 176, 193, 713, 857, 265, 203, 50, 668, 108,
   645, 990, 626, 197, 510, 357, 358, 850, 858, 364,
   936, 638
};

bool CState::Alloc()
{
  if (!Counters)
    Counters = (UInt32 *)::BigAlloc((256 + kBlockSizeMax) * sizeof(UInt32));
  return (Counters != 0);
}

void CState::Free()
{
  ::BigFree(Counters);
  Counters = 0;
}

UInt32 CDecoder::ReadBits(unsigned numBits) { return m_InStream.ReadBits(numBits); }
Byte CDecoder::ReadByte() {return (Byte)ReadBits(8); }
bool CDecoder::ReadBit() { return ReadBits(1) != 0; }

UInt32 CDecoder::ReadCrc()
{
  UInt32 crc = 0;
  for (int i = 0; i < 4; i++)
  {
    crc <<= 8;
    crc |= ReadByte();
  }
  return crc;
}

static UInt32 NO_INLINE ReadBits(NBitm::CDecoder<CInBuffer> *m_InStream, unsigned num)
{
  return m_InStream->ReadBits(num);
}

static UInt32 NO_INLINE ReadBit(NBitm::CDecoder<CInBuffer> *m_InStream)
{
  return m_InStream->ReadBits(1);
}

static HRESULT NO_INLINE ReadBlock(NBitm::CDecoder<CInBuffer> *m_InStream,
    UInt32 *CharCounters, UInt32 blockSizeMax, Byte *m_Selectors, CHuffmanDecoder *m_HuffmanDecoders,
    UInt32 *blockSizeRes, UInt32 *origPtrRes, bool *randRes)
{
  if (randRes)
    *randRes = ReadBit(m_InStream) ? true : false;
  *origPtrRes = ReadBits(m_InStream, kNumOrigBits);
  
  // in original code it compares OrigPtr to (UInt32)(10 + blockSizeMax)) : why ?
  if (*origPtrRes >= blockSizeMax)
    return S_FALSE;

  CMtf8Decoder mtf;
  mtf.StartInit();
  
  int numInUse = 0;
  {
    Byte inUse16[16];
    int i;
    for (i = 0; i < 16; i++)
      inUse16[i] = (Byte)ReadBit(m_InStream);
    for (i = 0; i < 256; i++)
      if (inUse16[i >> 4])
      {
        if (ReadBit(m_InStream))
          mtf.Add(numInUse++, (Byte)i);
      }
    if (numInUse == 0)
      return S_FALSE;
    // mtf.Init(numInUse);
  }
  int alphaSize = numInUse + 2;

  int numTables = ReadBits(m_InStream, kNumTablesBits);
  if (numTables < kNumTablesMin || numTables > kNumTablesMax)
    return S_FALSE;
  
  UInt32 numSelectors = ReadBits(m_InStream, kNumSelectorsBits);
  if (numSelectors < 1 || numSelectors > kNumSelectorsMax)
    return S_FALSE;

  {
    Byte mtfPos[kNumTablesMax];
    int t = 0;
    do
      mtfPos[t] = (Byte)t;
    while(++t < numTables);
    UInt32 i = 0;
    do
    {
      int j = 0;
      while (ReadBit(m_InStream))
        if (++j >= numTables)
          return S_FALSE;
      Byte tmp = mtfPos[j];
      for (;j > 0; j--)
        mtfPos[j] = mtfPos[j - 1];
      m_Selectors[i] = mtfPos[0] = tmp;
    }
    while(++i < numSelectors);
  }

  int t = 0;
  do
  {
    Byte lens[kMaxAlphaSize];
    int len = (int)ReadBits(m_InStream, kNumLevelsBits);
    int i;
    for (i = 0; i < alphaSize; i++)
    {
      for (;;)
      {
        if (len < 1 || len > kMaxHuffmanLen)
          return S_FALSE;
        if (!ReadBit(m_InStream))
          break;
        len += 1 - (int)(ReadBit(m_InStream) << 1);
      }
      lens[i] = (Byte)len;
    }
    for (; i < kMaxAlphaSize; i++)
      lens[i] = 0;
    if(!m_HuffmanDecoders[t].SetCodeLengths(lens))
      return S_FALSE;
  }
  while(++t < numTables);

  {
    for (int i = 0; i < 256; i++)
      CharCounters[i] = 0;
  }
  
  UInt32 blockSize = 0;
  {
    UInt32 groupIndex = 0;
    UInt32 groupSize = 0;
    CHuffmanDecoder *huffmanDecoder = 0;
    int runPower = 0;
    UInt32 runCounter = 0;
    
    for (;;)
    {
      if (groupSize == 0)
      {
        if (groupIndex >= numSelectors)
          return S_FALSE;
        groupSize = kGroupSize;
        huffmanDecoder = &m_HuffmanDecoders[m_Selectors[groupIndex++]];
      }
      groupSize--;
        
      UInt32 nextSym = huffmanDecoder->DecodeSymbol(m_InStream);
      
      if (nextSym < 2)
      {
        runCounter += ((UInt32)(nextSym + 1) << runPower++);
        if (blockSizeMax - blockSize < runCounter)
          return S_FALSE;
        continue;
      }
      if (runCounter != 0)
      {
        UInt32 b = (UInt32)mtf.GetHead();
        CharCounters[b] += runCounter;
        do
          CharCounters[256 + blockSize++] = b;
        while(--runCounter != 0);
        runPower = 0;
      }
      if (nextSym <= (UInt32)numInUse)
      {
        UInt32 b = (UInt32)mtf.GetAndMove((int)nextSym - 1);
        if (blockSize >= blockSizeMax)
          return S_FALSE;
        CharCounters[b]++;
        CharCounters[256 + blockSize++] = b;
      }
      else if (nextSym == (UInt32)numInUse + 1)
        break;
      else
        return S_FALSE;
    }
  }
  *blockSizeRes = blockSize;
  return (*origPtrRes < blockSize) ? S_OK : S_FALSE;
}

static void NO_INLINE DecodeBlock1(UInt32 *charCounters, UInt32 blockSize)
{
  {
    UInt32 sum = 0;
    for (UInt32 i = 0; i < 256; i++)
    {
      sum += charCounters[i];
      charCounters[i] = sum - charCounters[i];
    }
  }
  
  UInt32 *tt = charCounters + 256;
  // Compute the T^(-1) vector
  UInt32 i = 0;
  do
    tt[charCounters[tt[i] & 0xFF]++] |= (i << 8);
  while(++i < blockSize);
}

static UInt32 NO_INLINE DecodeBlock2(const UInt32 *tt, UInt32 blockSize, UInt32 OrigPtr, COutBuffer &m_OutStream)
{
  CBZip2Crc crc;

  // it's for speed optimization: prefetch & prevByte_init;
  UInt32 tPos = tt[tt[OrigPtr] >> 8];
  unsigned prevByte = (unsigned)(tPos & 0xFF);
  
  unsigned numReps = 0;

  do
  {
    unsigned b = (unsigned)(tPos & 0xFF);
    tPos = tt[tPos >> 8];
    
    if (numReps == kRleModeRepSize)
    {
      for (; b > 0; b--)
      {
        crc.UpdateByte(prevByte);
        m_OutStream.WriteByte((Byte)prevByte);
      }
      numReps = 0;
      continue;
    }
    if (b != prevByte)
      numReps = 0;
    numReps++;
    prevByte = b;
    crc.UpdateByte(b);
    m_OutStream.WriteByte((Byte)b);

    /*
    prevByte = b;
    crc.UpdateByte(b);
    m_OutStream.WriteByte((Byte)b);
    for (; --blockSize != 0;)
    {
      b = (unsigned)(tPos & 0xFF);
      tPos = tt[tPos >> 8];
      crc.UpdateByte(b);
      m_OutStream.WriteByte((Byte)b);
      if (b != prevByte)
      {
        prevByte = b;
        continue;
      }
      if (--blockSize == 0)
        break;
      
      b = (unsigned)(tPos & 0xFF);
      tPos = tt[tPos >> 8];
      crc.UpdateByte(b);
      m_OutStream.WriteByte((Byte)b);
      if (b != prevByte)
      {
        prevByte = b;
        continue;
      }
      if (--blockSize == 0)
        break;
      
      b = (unsigned)(tPos & 0xFF);
      tPos = tt[tPos >> 8];
      crc.UpdateByte(b);
      m_OutStream.WriteByte((Byte)b);
      if (b != prevByte)
      {
        prevByte = b;
        continue;
      }
      --blockSize;
      break;
    }
    if (blockSize == 0)
      break;

    b = (unsigned)(tPos & 0xFF);
    tPos = tt[tPos >> 8];
    
    for (; b > 0; b--)
    {
      crc.UpdateByte(prevByte);
      m_OutStream.WriteByte((Byte)prevByte);
    }
    */
  }
  while(--blockSize != 0);
  return crc.GetDigest();
}

static UInt32 NO_INLINE DecodeBlock2Rand(const UInt32 *tt, UInt32 blockSize, UInt32 OrigPtr, COutBuffer &m_OutStream)
{
  CBZip2Crc crc;
  
  UInt32 randIndex = 1;
  UInt32 randToGo = kRandNums[0] - 2;
  
  unsigned numReps = 0;

  // it's for speed optimization: prefetch & prevByte_init;
  UInt32 tPos = tt[tt[OrigPtr] >> 8];
  unsigned prevByte = (unsigned)(tPos & 0xFF);
  
  do
  {
    unsigned b = (unsigned)(tPos & 0xFF);
    tPos = tt[tPos >> 8];
    
    {
      if (randToGo == 0)
      {
        b ^= 1;
        randToGo = kRandNums[randIndex++];
        randIndex &= 0x1FF;
      }
      randToGo--;
    }
    
    if (numReps == kRleModeRepSize)
    {
      for (; b > 0; b--)
      {
        crc.UpdateByte(prevByte);
        m_OutStream.WriteByte((Byte)prevByte);
      }
      numReps = 0;
      continue;
    }
    if (b != prevByte)
      numReps = 0;
    numReps++;
    prevByte = b;
    crc.UpdateByte(b);
    m_OutStream.WriteByte((Byte)b);
  }
  while(--blockSize != 0);
  return crc.GetDigest();
}


CDecoder::CDecoder()
{
  #ifndef _7ZIP_ST
  m_States = 0;
  m_NumThreadsPrev = 0;
  NumThreads = 1;
  #endif
  _needInStreamInit = true;
}

#ifndef _7ZIP_ST

CDecoder::~CDecoder()
{
  Free();
}

#define RINOK_THREAD(x) { WRes __result_ = (x); if(__result_ != 0) return __result_; }

HRESULT CDecoder::Create()
{
  RINOK_THREAD(CanProcessEvent.CreateIfNotCreated());
  RINOK_THREAD(CanStartWaitingEvent.CreateIfNotCreated());
  if (m_States != 0 && m_NumThreadsPrev == NumThreads)
    return S_OK;
  Free();
  MtMode = (NumThreads > 1);
  m_NumThreadsPrev = NumThreads;
  try
  {
    m_States = new CState[NumThreads];
    if (!m_States)
      return E_OUTOFMEMORY;
  }
  catch(...) { return E_OUTOFMEMORY; }
  for (UInt32 t = 0; t < NumThreads; t++)
  {
    CState &ti = m_States[t];
    ti.Decoder = this;
    if (MtMode)
    {
      HRESULT res = ti.Create();
      if (res != S_OK)
      {
        NumThreads = t;
        Free();
        return res;
      }
    }
  }
  return S_OK;
}

void CDecoder::Free()
{
  if (!m_States)
    return;
  CloseThreads = true;
  CanProcessEvent.Set();
  for (UInt32 t = 0; t < NumThreads; t++)
  {
    CState &s = m_States[t];
    if (MtMode)
      s.Thread.Wait();
    s.Free();
  }
  delete []m_States;
  m_States = 0;
}

#endif

HRESULT CDecoder::ReadSignatures(bool &wasFinished, UInt32 &crc)
{
  wasFinished = false;
  Byte s[6];
  for (int i = 0; i < 6; i++)
    s[i] = ReadByte();
  crc = ReadCrc();
  if (s[0] == kFinSig0)
  {
    if (s[1] != kFinSig1 ||
        s[2] != kFinSig2 ||
        s[3] != kFinSig3 ||
        s[4] != kFinSig4 ||
        s[5] != kFinSig5)
      return S_FALSE;
    
    wasFinished = true;
    return (crc == CombinedCrc.GetDigest()) ? S_OK : S_FALSE;
  }
  if (s[0] != kBlockSig0 ||
      s[1] != kBlockSig1 ||
      s[2] != kBlockSig2 ||
      s[3] != kBlockSig3 ||
      s[4] != kBlockSig4 ||
      s[5] != kBlockSig5)
    return S_FALSE;
  CombinedCrc.Update(crc);
  return S_OK;
}

HRESULT CDecoder::DecodeFile(bool &isBZ, ICompressProgressInfo *progress)
{
  Progress = progress;
  #ifndef _7ZIP_ST
  RINOK(Create());
  for (UInt32 t = 0; t < NumThreads; t++)
  {
    CState &s = m_States[t];
    if (!s.Alloc())
      return E_OUTOFMEMORY;
    if (MtMode)
    {
      RINOK(s.StreamWasFinishedEvent.Reset());
      RINOK(s.WaitingWasStartedEvent.Reset());
      RINOK(s.CanWriteEvent.Reset());
    }
  }
  #else
  if (!m_States[0].Alloc())
    return E_OUTOFMEMORY;
  #endif

  isBZ = false;
  Byte s[6];
  int i;
  for (i = 0; i < 4; i++)
    s[i] = ReadByte();
  if (s[0] != kArSig0 ||
      s[1] != kArSig1 ||
      s[2] != kArSig2 ||
      s[3] <= kArSig3 ||
      s[3] > kArSig3 + kBlockSizeMultMax)
    return S_OK;
  isBZ = true;
  UInt32 dicSize = (UInt32)(s[3] - kArSig3) * kBlockSizeStep;

  CombinedCrc.Init();
  #ifndef _7ZIP_ST
  if (MtMode)
  {
    NextBlockIndex = 0;
    StreamWasFinished1 = StreamWasFinished2 = false;
    CloseThreads = false;
    CanStartWaitingEvent.Reset();
    m_States[0].CanWriteEvent.Set();
    BlockSizeMax = dicSize;
    Result1 = Result2 = S_OK;
    CanProcessEvent.Set();
    UInt32 t;
    for (t = 0; t < NumThreads; t++)
      m_States[t].StreamWasFinishedEvent.Lock();
    CanProcessEvent.Reset();
    CanStartWaitingEvent.Set();
    for (t = 0; t < NumThreads; t++)
      m_States[t].WaitingWasStartedEvent.Lock();
    CanStartWaitingEvent.Reset();
    RINOK(Result2);
    RINOK(Result1);
  }
  else
  #endif
  {
    CState &state = m_States[0];
    for (;;)
    {
      RINOK(SetRatioProgress(m_InStream.GetProcessedSize()));
      bool wasFinished;
      UInt32 crc;
      RINOK(ReadSignatures(wasFinished, crc));
      if (wasFinished)
        return S_OK;

      UInt32 blockSize, origPtr;
      bool randMode;
      RINOK(ReadBlock(&m_InStream, state.Counters, dicSize,
        m_Selectors, m_HuffmanDecoders,
        &blockSize, &origPtr, &randMode));
      DecodeBlock1(state.Counters, blockSize);
      if ((randMode ?
          DecodeBlock2Rand(state.Counters + 256, blockSize, origPtr, m_OutStream) :
          DecodeBlock2(state.Counters + 256, blockSize, origPtr, m_OutStream)) != crc)
        return S_FALSE;
    }
  }
  return SetRatioProgress(m_InStream.GetProcessedSize());
}

HRESULT CDecoder::CodeReal(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    bool &isBZ, ICompressProgressInfo *progress)
{
  isBZ = false;
  try
  {

  if (!m_InStream.Create(kBufferSize))
    return E_OUTOFMEMORY;
  if (!m_OutStream.Create(kBufferSize))
    return E_OUTOFMEMORY;

  if (inStream)
    m_InStream.SetStream(inStream);

  CDecoderFlusher flusher(this, inStream != NULL);

  if (_needInStreamInit)
  {
    m_InStream.Init();
    _needInStreamInit = false;
  }
  _inStart = m_InStream.GetProcessedSize();

  m_InStream.AlignToByte();

  m_OutStream.SetStream(outStream);
  m_OutStream.Init();

  RINOK(DecodeFile(isBZ, progress));
  flusher.NeedFlush = false;
  return Flush();

  }
  catch(const CInBufferException &e)  { return e.ErrorCode; }
  catch(const COutBufferException &e) { return e.ErrorCode; }
  catch(...) { return E_FAIL; }
}

STDMETHODIMP CDecoder::Code(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64 * /* inSize */, const UInt64 * /* outSize */, ICompressProgressInfo *progress)
{
  _needInStreamInit = true;
  bool isBZ;
  RINOK(CodeReal(inStream, outStream, isBZ, progress));
  return isBZ ? S_OK : S_FALSE;
}

HRESULT CDecoder::CodeResume(ISequentialOutStream *outStream, bool &isBZ, ICompressProgressInfo *progress)
{
  return CodeReal(NULL, outStream, isBZ, progress);
}

STDMETHODIMP CDecoder::SetInStream(ISequentialInStream *inStream) { m_InStream.SetStream(inStream); return S_OK; }
STDMETHODIMP CDecoder::ReleaseInStream() { m_InStream.ReleaseStream(); return S_OK; }

#ifndef _7ZIP_ST

static THREAD_FUNC_DECL MFThread(void *p) { ((CState *)p)->ThreadFunc(); return 0; }

HRESULT CState::Create()
{
  RINOK_THREAD(StreamWasFinishedEvent.CreateIfNotCreated());
  RINOK_THREAD(WaitingWasStartedEvent.CreateIfNotCreated());
  RINOK_THREAD(CanWriteEvent.CreateIfNotCreated());
  RINOK_THREAD(Thread.Create(MFThread, this));
  return S_OK;
}

void CState::FinishStream()
{
  Decoder->StreamWasFinished1 = true;
  StreamWasFinishedEvent.Set();
  Decoder->CS.Leave();
  Decoder->CanStartWaitingEvent.Lock();
  WaitingWasStartedEvent.Set();
}

void CState::ThreadFunc()
{
  for (;;)
  {
    Decoder->CanProcessEvent.Lock();
    Decoder->CS.Enter();
    if (Decoder->CloseThreads)
    {
      Decoder->CS.Leave();
      return;
    }
    if (Decoder->StreamWasFinished1)
    {
      FinishStream();
      continue;
    }
    HRESULT res = S_OK;

    UInt32 blockIndex = Decoder->NextBlockIndex;
    UInt32 nextBlockIndex = blockIndex + 1;
    if (nextBlockIndex == Decoder->NumThreads)
      nextBlockIndex = 0;
    Decoder->NextBlockIndex = nextBlockIndex;
    UInt32 crc;
    UInt64 packSize = 0;
    UInt32 blockSize = 0, origPtr = 0;
    bool randMode = false;

    try
    {
      bool wasFinished;
      res = Decoder->ReadSignatures(wasFinished, crc);
      if (res != S_OK)
      {
        Decoder->Result1 = res;
        FinishStream();
        continue;
      }
      if (wasFinished)
      {
        Decoder->Result1 = res;
        FinishStream();
        continue;
      }

      res = ReadBlock(&Decoder->m_InStream, Counters, Decoder->BlockSizeMax,
          Decoder->m_Selectors, Decoder->m_HuffmanDecoders,
          &blockSize, &origPtr, &randMode);
      if (res != S_OK)
      {
        Decoder->Result1 = res;
        FinishStream();
        continue;
      }
      packSize = Decoder->m_InStream.GetProcessedSize();
    }
    catch(const CInBufferException &e) { res = e.ErrorCode;  if (res != S_OK) res = E_FAIL; }
    catch(...) { res = E_FAIL; }
    if (res != S_OK)
    {
      Decoder->Result1 = res;
      FinishStream();
      continue;
    }

    Decoder->CS.Leave();

    DecodeBlock1(Counters, blockSize);

    bool needFinish = true;
    try
    {
      Decoder->m_States[blockIndex].CanWriteEvent.Lock();
      needFinish = Decoder->StreamWasFinished2;
      if (!needFinish)
      {
        if ((randMode ?
            DecodeBlock2Rand(Counters + 256, blockSize, origPtr, Decoder->m_OutStream) :
            DecodeBlock2(Counters + 256, blockSize, origPtr, Decoder->m_OutStream)) == crc)
          res = Decoder->SetRatioProgress(packSize);
        else
          res = S_FALSE;
      }
    }
    catch(const COutBufferException &e) { res = e.ErrorCode; if (res != S_OK) res = E_FAIL; }
    catch(...) { res = E_FAIL; }
    if (res != S_OK)
    {
      Decoder->Result2 = res;
      Decoder->StreamWasFinished2 = true;
    }
    Decoder->m_States[nextBlockIndex].CanWriteEvent.Set();
    if (res != S_OK || needFinish)
    {
      StreamWasFinishedEvent.Set();
      Decoder->CanStartWaitingEvent.Lock();
      WaitingWasStartedEvent.Set();
    }
  }
}

STDMETHODIMP CDecoder::SetNumberOfThreads(UInt32 numThreads)
{
  NumThreads = numThreads;
  if (NumThreads < 1)
    NumThreads = 1;
  if (NumThreads > kNumThreadsMax)
    NumThreads = kNumThreadsMax;
  return S_OK;
}

#endif

HRESULT CDecoder::SetRatioProgress(UInt64 packSize)
{
  if (!Progress)
    return S_OK;
  packSize -= _inStart;
  UInt64 unpackSize = m_OutStream.GetProcessedSize();
  return Progress->SetRatioInfo(&packSize, &unpackSize);
}


// ---------- NSIS ----------

enum
{
  NSIS_STATE_INIT,
  NSIS_STATE_NEW_BLOCK,
  NSIS_STATE_DATA,
  NSIS_STATE_FINISHED,
  NSIS_STATE_ERROR
};

STDMETHODIMP CNsisDecoder::SetInStream(ISequentialInStream *inStream) { m_InStream.SetStream(inStream); return S_OK; }
STDMETHODIMP CNsisDecoder::ReleaseInStream() { m_InStream.ReleaseStream(); return S_OK; }

STDMETHODIMP CNsisDecoder::SetOutStreamSize(const UInt64 * /* outSize */)
{
  _nsisState = NSIS_STATE_INIT;
  return S_OK;
}

STDMETHODIMP CNsisDecoder::Read(void *data, UInt32 size, UInt32 *processedSize)
{
  try {

  *processedSize = 0;
  if (_nsisState == NSIS_STATE_FINISHED)
    return S_OK;
  if (_nsisState == NSIS_STATE_ERROR)
    return S_FALSE;
  if (size == 0)
    return S_OK;

  CState &state = m_State;

  if (_nsisState == NSIS_STATE_INIT)
  {
    if (!m_InStream.Create(kBufferSize))
      return E_OUTOFMEMORY;
    if (!state.Alloc())
      return E_OUTOFMEMORY;
    m_InStream.Init();
    _nsisState = NSIS_STATE_NEW_BLOCK;
  }

  if (_nsisState == NSIS_STATE_NEW_BLOCK)
  {
    Byte b = (Byte)m_InStream.ReadBits(8);
    if (b == kFinSig0)
    {
      _nsisState = NSIS_STATE_FINISHED;
      return S_OK;
    }
    if (b != kBlockSig0)
    {
      _nsisState = NSIS_STATE_ERROR;
      return S_FALSE;
    }
    UInt32 origPtr;
    RINOK(ReadBlock(&m_InStream, state.Counters, 9 * kBlockSizeStep,
        m_Selectors, m_HuffmanDecoders, &_blockSize, &origPtr, NULL));
    DecodeBlock1(state.Counters, _blockSize);
    const UInt32 *tt = state.Counters + 256;
    _tPos = tt[tt[origPtr] >> 8];
    _prevByte = (unsigned)(_tPos & 0xFF);
    _numReps = 0;
    _repRem = 0;
    _nsisState = NSIS_STATE_DATA;
  }

  UInt32 tPos = _tPos;
  unsigned prevByte = _prevByte;
  unsigned numReps = _numReps;
  UInt32 blockSize = _blockSize;
  const UInt32 *tt = state.Counters + 256;

  while (_repRem)
  {
    _repRem--;
    *(Byte *)data = (Byte)prevByte;
    data = (Byte *)data + 1;
    (*processedSize)++;
    if (--size == 0)
      return S_OK;
  }

  if (blockSize == 0)
  {
    _nsisState = NSIS_STATE_NEW_BLOCK;
    return S_OK;
  }

  do
  {
    unsigned b = (unsigned)(tPos & 0xFF);
    tPos = tt[tPos >> 8];
    blockSize--;
    
    if (numReps == kRleModeRepSize)
    {
      numReps = 0;
      while (b)
      {
        b--;
        *(Byte *)data = (Byte)prevByte;
        data = (Byte *)data + 1;
        (*processedSize)++;
        if (--size == 0)
          break;
      }
      _repRem = b;
      continue;
    }
    if (b != prevByte)
      numReps = 0;
    numReps++;
    prevByte = b;
    *(Byte *)data = (Byte)b;
    data = (Byte *)data + 1;
    (*processedSize)++;
    size--;
  }
  while (size && blockSize);
  _tPos = tPos;
  _prevByte = prevByte;
  _numReps = numReps;
  _blockSize = blockSize;
  return S_OK;

  }
  catch(const CInBufferException &e)  { return e.ErrorCode; }
  catch(...) { return S_FALSE; }
}

}}
