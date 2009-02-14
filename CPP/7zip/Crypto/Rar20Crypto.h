// Crypto/Rar20Crypto.h

#ifndef __CRYPTO_RAR20_CRYPTO_H
#define __CRYPTO_RAR20_CRYPTO_H

#include "Common/MyCom.h"

#include "../ICoder.h"
#include "../IPassword.h"

namespace NCrypto {
namespace NRar20 {

class CData
{
  Byte SubstTable[256];
  UInt32 Keys[4];
  
  UInt32 SubstLong(UInt32 t)
  {
    return (UInt32)SubstTable[(int)t & 255] |
           ((UInt32)SubstTable[(int)(t >> 8) & 255] << 8) |
           ((UInt32)SubstTable[(int)(t >> 16) & 255] << 16) |
           ((UInt32)SubstTable[(int)(t >> 24) & 255] << 24);
  }
  void UpdateKeys(const Byte *data);
  void CryptBlock(Byte *buf, bool encrypt);
public:
  void EncryptBlock(Byte *buf) { CryptBlock(buf, true); }
  void DecryptBlock(Byte *buf) { CryptBlock(buf, false); }
  void SetPassword(const Byte *password, UInt32 passwordLen);
};

class CDecoder:
  public ICompressFilter,
  public ICryptoSetPassword,
  public CMyUnknownImp
{
  CData _cipher;
public:
  MY_UNKNOWN_IMP1(ICryptoSetPassword)

  STDMETHOD(Init)();
  STDMETHOD_(UInt32, Filter)(Byte *data, UInt32 size);
  STDMETHOD(CryptoSetPassword)(const Byte *data, UInt32 size);
};

}}

#endif
