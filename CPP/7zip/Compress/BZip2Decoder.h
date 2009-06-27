// Compress/BZip2Decoder.h

#ifndef __COMPRESS_BZIP2_DECODER_H
#define __COMPRESS_BZIP2_DECODER_H

#include "../../Common/MyCom.h"

#ifdef COMPRESS_BZIP2_MT
#include "../../Windows/Synchronization.h"
#include "../../Windows/Thread.h"
#endif

#include "../ICoder.h"

#include "../Common/InBuffer.h"
#include "../Common/OutBuffer.h"

#include "BitmDecoder.h"
#include "BZip2Const.h"
#include "BZip2Crc.h"
#include "HuffmanDecoder.h"

namespace NCompress {
namespace NBZip2 {

typedef NCompress::NHuffman::CDecoder<kMaxHuffmanLen, kMaxAlphaSize> CHuffmanDecoder;

class CDecoder;

struct CState
{
  UInt32 *Counters;

  #ifdef COMPRESS_BZIP2_MT

  CDecoder *Decoder;
  NWindows::CThread Thread;
  bool m_OptimizeNumTables;

  NWindows::NSynchronization::CAutoResetEvent StreamWasFinishedEvent;
  NWindows::NSynchronization::CAutoResetEvent WaitingWasStartedEvent;

  // it's not member of this thread. We just need one event per thread
  NWindows::NSynchronization::CAutoResetEvent CanWriteEvent;

  Byte MtPad[1 << 8]; // It's pad for Multi-Threading. Must be >= Cache_Line_Size.

  HRESULT Create();
  void FinishStream();
  void ThreadFunc();

  #endif

  CState(): Counters(0) {}
  ~CState() { Free(); }
  bool Alloc();
  void Free();
};

class CDecoder :
  public ICompressCoder,
  #ifdef COMPRESS_BZIP2_MT
  public ICompressSetCoderMt,
  #endif
  public CMyUnknownImp
{
public:
  COutBuffer m_OutStream;
  Byte MtPad[1 << 8]; // It's pad for Multi-Threading. Must be >= Cache_Line_Size.
  NBitm::CDecoder<CInBuffer> m_InStream;
  Byte m_Selectors[kNumSelectorsMax];
  CHuffmanDecoder m_HuffmanDecoders[kNumTablesMax];
  UInt64 _inStart;

private:

  bool _needInStreamInit;

  UInt32 ReadBits(int numBits);
  Byte ReadByte();
  bool ReadBit();
  UInt32 ReadCrc();
  HRESULT PrepareBlock(CState &state);
  HRESULT DecodeFile(bool &isBZ, ICompressProgressInfo *progress);
  HRESULT CodeReal(ISequentialInStream *inStream, ISequentialOutStream *outStream,
      bool &isBZ, ICompressProgressInfo *progress);
  class CDecoderFlusher
  {
    CDecoder *_decoder;
  public:
    bool NeedFlush;
    bool ReleaseInStream;
    CDecoderFlusher(CDecoder *decoder, bool releaseInStream):
      _decoder(decoder),
      ReleaseInStream(releaseInStream),
      NeedFlush(true) {}
    ~CDecoderFlusher()
    {
      if (NeedFlush)
        _decoder->Flush();
      _decoder->ReleaseStreams(ReleaseInStream);
    }
  };

public:
  CBZip2CombinedCrc CombinedCrc;

  #ifdef COMPRESS_BZIP2_MT
  ICompressProgressInfo *Progress;
  CState *m_States;
  UInt32 m_NumThreadsPrev;

  NWindows::NSynchronization::CManualResetEvent CanProcessEvent;
  NWindows::NSynchronization::CCriticalSection CS;
  UInt32 NumThreads;
  bool MtMode;
  UInt32 NextBlockIndex;
  bool CloseThreads;
  bool StreamWasFinished1;
  bool StreamWasFinished2;
  NWindows::NSynchronization::CManualResetEvent CanStartWaitingEvent;

  HRESULT Result1;
  HRESULT Result2;

  UInt32 BlockSizeMax;
  ~CDecoder();
  HRESULT Create();
  void Free();

  #else
  CState m_States[1];
  #endif

  CDecoder();

  HRESULT ReadSignatures(bool &wasFinished, UInt32 &crc);


  HRESULT Flush() { return m_OutStream.Flush(); }
  void ReleaseStreams(bool releaseInStream)
  {
    if (releaseInStream)
      m_InStream.ReleaseStream();
    m_OutStream.ReleaseStream();
  }

  MY_QUERYINTERFACE_BEGIN
  #ifdef COMPRESS_BZIP2_MT
  MY_QUERYINTERFACE_ENTRY(ICompressSetCoderMt)
  #endif

  MY_QUERYINTERFACE_END
  MY_ADDREF_RELEASE

  
  STDMETHOD(Code)(ISequentialInStream *inStream, ISequentialOutStream *outStream,
      const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress);

  STDMETHOD(SetInStream)(ISequentialInStream *inStream);
  STDMETHOD(ReleaseInStream)();

  HRESULT CodeResume(ISequentialOutStream *outStream, bool &isBZ, ICompressProgressInfo *progress);
  UInt64 GetInputProcessedSize() const { return m_InStream.GetProcessedSize(); }
  
  #ifdef COMPRESS_BZIP2_MT
  STDMETHOD(SetNumberOfThreads)(UInt32 numThreads);
  #endif
};

}}

#endif
