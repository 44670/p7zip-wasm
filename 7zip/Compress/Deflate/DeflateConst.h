// DeflateConst.h

#ifndef __DEFLATE_CONST_H
#define __DEFLATE_CONST_H

#include "DeflateExtConst.h"

namespace NCompress {
namespace NDeflate {

const UInt32 kLenTableSize = 29;

const UInt32 kStaticDistTableSize = 32;
const UInt32 kStaticLenTableSize = 31;

const UInt32 kReadTableNumber = 0x100;
const UInt32 kMatchNumber = kReadTableNumber + 1;

const UInt32 kMainTableSize = kMatchNumber + kLenTableSize; //298;
const UInt32 kStaticMainTableSize = kMatchNumber + kStaticLenTableSize; //298;

const UInt32 kDistTableStart = kMainTableSize;

const UInt32 kHeapTablesSizesSum32 = kMainTableSize + kDistTableSize32;
const UInt32 kHeapTablesSizesSum64 = kMainTableSize + kDistTableSize64;

const UInt32 kLevelTableSize = 19;

const UInt32 kMaxTableSize32 = kHeapTablesSizesSum32; // test it
const UInt32 kMaxTableSize64 = kHeapTablesSizesSum64; // test it

const UInt32 kStaticMaxTableSize = kStaticMainTableSize + kStaticDistTableSize;

const UInt32 kTableDirectLevels = 16;
const UInt32 kTableLevelRepNumber = kTableDirectLevels;
const UInt32 kTableLevel0Number = kTableLevelRepNumber + 1;
const UInt32 kTableLevel0Number2 = kTableLevel0Number + 1;

const UInt32 kLevelMask = 0xF;

const Byte kLenStart32[kLenTableSize] = 
  {0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,112,128,160,192,224, 255};
const Byte kLenStart64[kLenTableSize] = 
  {0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,112,128,160,192,224, 0};

const Byte kLenDirectBits32[kLenTableSize] = 
  {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5, 0};
const Byte kLenDirectBits64[kLenTableSize] = 
  {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5, 16};

const UInt32 kDistStart[kDistTableSize64]  = 
  {0,1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256,384,512,768,
  1024,1536,2048,3072,4096,6144,8192,12288,16384,24576, 32768, 49152};
const Byte kDistDirectBits[kDistTableSize64] = 
  {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14};

const Byte kLevelDirectBits[kLevelTableSize] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7};

const Byte kCodeLengthAlphabetOrder[kLevelTableSize] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

const UInt32 kMatchMinLen = 3; 
const UInt32 kMatchMaxLen32 = kNumLenCombinations32 + kMatchMinLen - 1; //256 + 2; test it
const UInt32 kMatchMaxLen64 = kNumLenCombinations64 + kMatchMinLen - 1; //255 + 2; test it

const int kFinalBlockFieldSize = 1;

namespace NFinalBlockField
{
enum
{
  kNotFinalBlock = 0,
  kFinalBlock = 1
};
}

const int kBlockTypeFieldSize = 2;

namespace NBlockType
{
  enum
  {
    kStored = 0,
    kFixedHuffman = 1,
    kDynamicHuffman = 2,
    kReserved = 3
  };
}

const UInt32 kDeflateNumberOfLengthCodesFieldSize  = 5;
const UInt32 kDeflateNumberOfDistanceCodesFieldSize  = 5;
const UInt32 kDeflateNumberOfLevelCodesFieldSize  = 4;

const UInt32 kDeflateNumberOfLitLenCodesMin = 257;

const UInt32 kDeflateNumberOfDistanceCodesMin = 1;
const UInt32 kDeflateNumberOfLevelCodesMin = 4;

const UInt32 kDeflateLevelCodeFieldSize  = 3;

const UInt32 kDeflateStoredBlockLengthFieldSizeSize = 16;

}}

#endif
