// AES_CBC.h

#ifndef __AES_CBC_H
#define __AES_CBC_H

#include "aescpp.h"

class CAES_CBC: public AESclass
{
protected:
  Byte _prevBlock[16];
public:
  void Init(const Byte *iv)
  {
    for (int i = 0; i < 16; i++)
      _prevBlock[i] = iv[i];
  }
  void Encode(const Byte *inBlock, Byte *outBlock)
  {
    int i;
    for (i = 0; i < 16; i++)
      _prevBlock[i] ^= inBlock[i];
    enc_blk(_prevBlock, outBlock);
    for (i = 0; i < 16; i++)
      _prevBlock[i] = outBlock[i];
  }

  void Decode(const Byte *inBlock, Byte *outBlock)
  {
    dec_blk(inBlock, outBlock);
    int i;
    for (i = 0; i < 16; i++)
      outBlock[i] ^= _prevBlock[i];
    for (i = 0; i < 16; i++)
      _prevBlock[i] = inBlock[i];
  }
};

#endif
