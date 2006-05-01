// Crypto/ZipCrypto.cpp

#include "StdAfx.h"

#include "ZipCipher.h"
#include "../../../Common/CRC.h"

namespace NCrypto {
namespace NZip {

static inline UInt32 ZipCRC32(UInt32 c, Byte  b)
{
  return CCRC::Table[(c ^ b) & 0xFF] ^ (c >> 8);
}

void CCipher::UpdateKeys(Byte b)
{
  Keys[0] = ZipCRC32(Keys[0], b);
  Keys[1] += Keys[0] & 0xff;
  Keys[1] = Keys[1] * 134775813L + 1;
  Keys[2] = ZipCRC32(Keys[2], Keys[1] >> 24);
}

void CCipher::SetPassword(const Byte *password, UInt32 passwordLength)
{
  Keys[0] = 305419896L;
  Keys[1] = 591751049L;
  Keys[2] = 878082192L;
  for (UInt32 i = 0; i < passwordLength; i++)
    UpdateKeys(password[i]);
}

Byte CCipher::DecryptByteSpec()
{
  UInt32 temp = Keys[2] | 2;
  return (temp * (temp ^ 1)) >> 8;
}

Byte CCipher::DecryptByte(Byte encryptedByte)
{
  Byte c = encryptedByte ^ DecryptByteSpec();
  UpdateKeys(c);
  return c;
}

Byte CCipher::EncryptByte(Byte b)
{
  Byte c = b ^ DecryptByteSpec();
  UpdateKeys(b);
  return c;
}

void CCipher::DecryptHeader(Byte *buffer)
{
  for (int i = 0; i < 12; i++)
    buffer[i] = DecryptByte(buffer[i]);
}

void CCipher::EncryptHeader(Byte *buffer)
{
  for (int i = 0; i < 12; i++)
    buffer[i] = EncryptByte(buffer[i]);
}

}}
