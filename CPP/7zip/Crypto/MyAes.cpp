// Crypto/MyAes.cpp

#include "StdAfx.h"

#include "MyAes.h"

namespace NCrypto {

struct CAesTabInit { CAesTabInit() { AesGenTables();} } g_AesTabInit;

STDMETHODIMP CAesCbcEncoder::Init() { return S_OK; }

STDMETHODIMP_(UInt32) CAesCbcEncoder::Filter(Byte *data, UInt32 size)
{
  return (UInt32)AesCbc_Encode(&Aes, data, size);
}

STDMETHODIMP CAesCbcEncoder::SetKey(const Byte *data, UInt32 size)
{
  if ((size & 0x7) != 0 || size < 16 || size > 32)
    return E_INVALIDARG;
  Aes_SetKeyEncode(&Aes.aes, data, size);
  return S_OK;
}

STDMETHODIMP CAesCbcEncoder::SetInitVector(const Byte *data, UInt32 size)
{
  if (size != AES_BLOCK_SIZE)
    return E_INVALIDARG;
  AesCbc_Init(&Aes, data);
  return S_OK;
}

STDMETHODIMP CAesCbcDecoder::Init() { return S_OK; }

STDMETHODIMP_(UInt32) CAesCbcDecoder::Filter(Byte *data, UInt32 size)
{
  return (UInt32)AesCbc_Decode(&Aes, data, size);
}

STDMETHODIMP CAesCbcDecoder::SetKey(const Byte *data, UInt32 size)
{
  if ((size & 0x7) != 0 || size < 16 || size > 32)
    return E_INVALIDARG;
  Aes_SetKeyDecode(&Aes.aes, data, size);
  return S_OK;
}

STDMETHODIMP CAesCbcDecoder::SetInitVector(const Byte *data, UInt32 size)
{
  if (size != AES_BLOCK_SIZE)
    return E_INVALIDARG;
  AesCbc_Init(&Aes, data);
  return S_OK;
}

}
