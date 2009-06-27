// DeltaFilter.cpp

#include "StdAfx.h"

#include "../../../C/Delta.h"

#include "../Common/RegisterCodec.h"

#include "BranchCoder.h"

struct CDelta
{
  unsigned _delta;
  Byte _state[DELTA_STATE_SIZE];
  CDelta(): _delta(1) {}
  void DeltaInit() { Delta_Init(_state); }
};

class CDeltaEncoder:
  public ICompressFilter,
  public ICompressSetCoderProperties,
  public ICompressWriteCoderProperties,
  CDelta,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP2(ICompressSetCoderProperties, ICompressWriteCoderProperties)
  STDMETHOD(Init)();
  STDMETHOD_(UInt32, Filter)(Byte *data, UInt32 size);
  STDMETHOD(SetCoderProperties)(const PROPID *propIDs, const PROPVARIANT *props, UInt32 numProps);
  STDMETHOD(WriteCoderProperties)(ISequentialOutStream *outStream);
};

class CDeltaDecoder:
  public ICompressFilter,
  public ICompressSetDecoderProperties2,
  CDelta,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP1(ICompressSetDecoderProperties2)
  STDMETHOD(Init)();
  STDMETHOD_(UInt32, Filter)(Byte *data, UInt32 size);
  STDMETHOD(SetDecoderProperties2)(const Byte *data, UInt32 size);
};

STDMETHODIMP CDeltaEncoder::Init()
{
  DeltaInit();
  return S_OK;
}

STDMETHODIMP_(UInt32) CDeltaEncoder::Filter(Byte *data, UInt32 size)
{
  Delta_Encode(_state, _delta, data, size);
  return size;
}

STDMETHODIMP CDeltaEncoder::SetCoderProperties(const PROPID *propIDs, const PROPVARIANT *props, UInt32 numProps)
{
  UInt32 delta = _delta;
  for (UInt32 i = 0; i < numProps; i++)
  {
    const PROPVARIANT &prop = props[i];
    if (propIDs[i] != NCoderPropID::kDefaultProp || prop.vt != VT_UI4 || prop.ulVal < 1 || prop.ulVal > 256)
      return E_INVALIDARG;
    delta = prop.ulVal;
  }
  _delta = delta;
  return S_OK;
}

STDMETHODIMP CDeltaEncoder::WriteCoderProperties(ISequentialOutStream *outStream)
{
  Byte prop = (Byte)(_delta - 1);
  return outStream->Write(&prop, 1, NULL);
}

STDMETHODIMP CDeltaDecoder::Init()
{
  DeltaInit();
  return S_OK;
}

STDMETHODIMP_(UInt32) CDeltaDecoder::Filter(Byte *data, UInt32 size)
{
  Delta_Decode(_state, _delta, data, size);
  return size;
}

STDMETHODIMP CDeltaDecoder::SetDecoderProperties2(const Byte *props, UInt32 size)
{
  if (size != 1)
    return E_INVALIDARG;
  _delta = (unsigned)props[0] + 1;
  return S_OK;
}

#define CREATE_CODEC(x) \
  static void *CreateCodec ## x() { return (void *)(ICompressFilter *)(new C ## x ## Decoder); } \
  static void *CreateCodec ## x ## Out() { return (void *)(ICompressFilter *)(new C ## x ## Encoder); }

CREATE_CODEC(Delta)

#define METHOD_ITEM(x, id, name) { CreateCodec ## x, CreateCodec ## x ## Out, id, name, 1, true  }

static CCodecInfo g_CodecsInfo[] =
{
  METHOD_ITEM(Delta, 3, L"Delta")
};

REGISTER_CODECS(Delta)
