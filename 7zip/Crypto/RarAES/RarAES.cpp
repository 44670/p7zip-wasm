// Crypto/RarAES/RarAES.h
// This code is based on UnRar sources

#include "StdAfx.h"

#include "RarAES.h"
#include "sha1.h"

extern void GetCryptoFolderPrefix(TCHAR *path);

// {23170F69-40C1-278B-0601-010000000000}
DEFINE_GUID(CLSID_CCrypto_AES128_Decoder, 
0x23170F69, 0x40C1, 0x278B, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00);

namespace NCrypto {
namespace NRar29 {

CDecoder::CDecoder():
  _thereIsSalt(false),
  _needCalculate(true),
  _rar350Mode(false)
{
  for (int i = 0; i < sizeof(_salt); i++)
    _salt[i] = 0;
}

STDMETHODIMP CDecoder::SetDecoderProperties2(const Byte *data, UInt32 size)
{
  bool thereIsSaltPrev = _thereIsSalt;
  _thereIsSalt = false;
  if (size == 0)
    return S_OK;
  if (size < 8)
    return E_INVALIDARG;
  _thereIsSalt = true;
  bool same = false;
  if (_thereIsSalt == thereIsSaltPrev)
  {
    same = true;
    if (_thereIsSalt)
    {
      for (int i = 0; i < sizeof(_salt); i++)
        if (_salt[i] != data[i])
        {
          same = false;
          break;
        }
    }
  }
  for (int i = 0; i < sizeof(_salt); i++)
    _salt[i] = data[i];
  if (!_needCalculate && !same)
    _needCalculate = true;
  return S_OK;
}

static const int kMaxPasswordLength = 127 * 2;

STDMETHODIMP CDecoder::CryptoSetPassword(const Byte *data, UInt32 size)
{
  if (size > kMaxPasswordLength)
    size = kMaxPasswordLength;
  bool same = false;
  if (size == buffer.GetCapacity())
  {
    same = true;
    for (UInt32 i = 0; i < size; i++)
      if (data[i] != buffer[i])
      {
        same = false;
        break;
      }
  }
  if (!_needCalculate && !same)
    _needCalculate = true;
  buffer.SetCapacity(size);
  memcpy(buffer, data, size);
  return S_OK;
}

STDMETHODIMP CDecoder::Init()
{
  Calculate();
  CreateFilter();
  CMyComPtr<ICryptoProperties> cp;
  RINOK(_aesFilter.QueryInterface(IID_ICryptoProperties, &cp));
  RINOK(cp->SetKey(aesKey, 16));
  RINOK(cp->SetInitVector(aesInit, 16));
  _aesFilter->Init();
  return S_OK;
}

HRESULT CDecoder::CreateFilter()
{
  if (_aesFilter)
    return S_OK;
  TCHAR aesLibPath[MAX_PATH + 64];
  GetCryptoFolderPrefix(aesLibPath);
  lstrcat(aesLibPath, TEXT("AES.dll"));
  return _aesLib.LoadAndCreateFilter(aesLibPath, CLSID_CCrypto_AES128_Decoder, &_aesFilter);
}

STDMETHODIMP_(UInt32) CDecoder::Filter(Byte *data, UInt32 size)
{
  return _aesFilter->Filter(data, size);
}

void CDecoder::Calculate()
{
  if (_needCalculate)
  {
    const int kSaltSize = 8;
    
    Byte rawPassword[kMaxPasswordLength + kSaltSize];
    
    memcpy(rawPassword, buffer, buffer.GetCapacity());
    
    int rawLength = buffer.GetCapacity();
    
    if (_thereIsSalt)
    {
      memcpy(rawPassword + rawLength, _salt, kSaltSize);
      rawLength += kSaltSize;
    }
    
    CSHA1 sha;
    sha.Init();

    // seems rar reverts hash for sha.
    const int hashRounds = 0x40000;
    int i;
    for (i = 0; i < hashRounds; i++)
    {
      sha.Update(rawPassword, rawLength, _rar350Mode);
      Byte pswNum[3];
      pswNum[0] = (Byte)i;
      pswNum[1] = (Byte)(i >> 8);
      pswNum[2] = (Byte)(i >> 16);
      sha.Update(pswNum, 3, _rar350Mode);
      if (i % (hashRounds / 16) == 0)
      {
        CSHA1 shaTemp = sha;
        Byte digest[20];
        shaTemp.Final(digest);
        aesInit[i / (hashRounds / 16)] = (Byte)digest[4 * 4 + 3];
      }
    }
    /*
    // it's test message for sha
    const char *message = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    sha.Update((const Byte *)message, strlen(message));
    */
    Byte digest[20];
    sha.Final(digest);
    for (i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        aesKey[i * 4 + j] = (digest[i * 4 + 3 - j]);
  }
  _needCalculate = false;
}



/*
STDMETHODIMP CDecoder::Code(ISequentialInStream *inStream,
      ISequentialOutStream *outStream, UInt64 const *inSize, 
      const UInt64 *outSize,ICompressProgressInfo *progress)
{
  Calculate();
  TCHAR aesLibPath[MAX_PATH + 64];
  GetCryptoFolderPrefix(aesLibPath);
  lstrcat(aesLibPath, TEXT("AES.dll"));
  CCoderLibrary aesLib;
  CMyComPtr<ICompressCoder2> aesDecoder;
  RINOK(aesLib.LoadAndCreateCoder2(aesLibPath, CLSID_CCrypto_AES128_Decoder, &aesDecoder));

  CSequentialInStreamImp *ivStreamSpec = new CSequentialInStreamImp;
  CMyComPtr<ISequentialInStream> ivStream(ivStreamSpec);
  ivStreamSpec->Init(aesInit, 16);

  CSequentialInStreamImp *keyStreamSpec = new CSequentialInStreamImp;
  CMyComPtr<ISequentialInStream> keyStream(keyStreamSpec);
  keyStreamSpec->Init(aesKey, 16);

  ISequentialInStream *inStreams[3] = { inStream, ivStream, keyStream };
  UInt64 ivSize = 16;
  UInt64 keySize = 16;
  const UInt64 *inSizes[3] = { inSize, &ivSize, &ivSize, };
  return aesDecoder->Code(inStreams, inSizes, 3, 
      &outStream, &outSize, 1, progress);
}
*/

}}
