// Archive/ZipIn.cpp

#include "StdAfx.h"

#include "ZipIn.h"
#include "Windows/Defs.h"
#include "Common/StringConvert.h"
#include "Common/DynamicBuffer.h"
#include "../../Common/LimitedStreams.h"
#include "../../Common/StreamUtils.h"

extern "C"
{
  #include "../../../../C/CpuArch.h"
}

#define Get16(p) GetUi16(p)
#define Get32(p) GetUi32(p)
#define Get64(p) GetUi64(p)

namespace NArchive {
namespace NZip {
 
// static const char kEndOfString = '\0';

HRESULT CInArchive::Open(IInStream *stream, const UInt64 *searchHeaderSizeLimit)
{
  Close();
  RINOK(stream->Seek(0, STREAM_SEEK_CUR, &m_StreamStartPosition));
  m_Position = m_StreamStartPosition;
  RINOK(FindAndReadMarker(stream, searchHeaderSizeLimit));
  RINOK(stream->Seek(m_Position, STREAM_SEEK_SET, NULL));
  m_Stream = stream;
  return S_OK;
}

void CInArchive::Close()
{
  m_Stream.Release();
}

HRESULT CInArchive::Seek(UInt64 offset)
{
  return m_Stream->Seek(offset, STREAM_SEEK_SET, NULL);
}

//////////////////////////////////////
// Markers

static inline bool TestMarkerCandidate(const Byte *p, UInt32 &value)
{
  value = Get32(p);
  return
    (value == NSignature::kLocalFileHeader) ||
    (value == NSignature::kEndOfCentralDir);
}

static const UInt32 kNumMarkerAddtionalBytes = 2;
static inline bool TestMarkerCandidate2(const Byte *p, UInt32 &value)
{
  value = Get32(p);
  if (value == NSignature::kEndOfCentralDir)
    return (Get16(p + 4) == 0);
  return (value == NSignature::kLocalFileHeader && p[4] < 128);
}

HRESULT CInArchive::FindAndReadMarker(IInStream *stream, const UInt64 *searchHeaderSizeLimit)
{
  m_ArchiveInfo.Clear();
  m_Position = m_StreamStartPosition;

  Byte marker[NSignature::kMarkerSize];
  RINOK(ReadStream_FALSE(stream, marker, NSignature::kMarkerSize));
  m_Position += NSignature::kMarkerSize;
  if (TestMarkerCandidate(marker, m_Signature))
    return S_OK;

  CByteDynamicBuffer dynamicBuffer;
  const UInt32 kSearchMarkerBufferSize = 0x10000;
  dynamicBuffer.EnsureCapacity(kSearchMarkerBufferSize);
  Byte *buffer = dynamicBuffer;
  UInt32 numBytesPrev = NSignature::kMarkerSize - 1;
  memcpy(buffer, marker + 1, numBytesPrev);
  UInt64 curTestPos = m_StreamStartPosition + 1;
  for (;;)
  {
    if (searchHeaderSizeLimit != NULL)
      if (curTestPos - m_StreamStartPosition > *searchHeaderSizeLimit)
        break;
    size_t numReadBytes = kSearchMarkerBufferSize - numBytesPrev;
    RINOK(ReadStream(stream, buffer + numBytesPrev, &numReadBytes));
    m_Position += numReadBytes;
    UInt32 numBytesInBuffer = numBytesPrev + (UInt32)numReadBytes;
    const UInt32 kMarker2Size = NSignature::kMarkerSize + kNumMarkerAddtionalBytes;
    if (numBytesInBuffer < kMarker2Size)
      break;
    UInt32 numTests = numBytesInBuffer - kMarker2Size + 1;
    for (UInt32 pos = 0; pos < numTests; pos++)
    {
      if (buffer[pos] != 0x50)
        continue;
      if (TestMarkerCandidate2(buffer + pos, m_Signature))
      {
        curTestPos += pos;
        m_ArchiveInfo.StartPosition = curTestPos;
        m_Position = curTestPos + NSignature::kMarkerSize;
        return S_OK;
      }
    }
    curTestPos += numTests;
    numBytesPrev = numBytesInBuffer - numTests;
    memmove(buffer, buffer + numTests, numBytesPrev);
  }
  return S_FALSE;
}

HRESULT CInArchive::ReadBytes(void *data, UInt32 size, UInt32 *processedSize)
{
  size_t realProcessedSize = size;
  HRESULT result = ReadStream(m_Stream, data, &realProcessedSize);
  if (processedSize != NULL)
    *processedSize = (UInt32)realProcessedSize;
  m_Position += realProcessedSize;
  return result;
}

void CInArchive::IncreaseRealPosition(UInt64 addValue)
{
  if (m_Stream->Seek(addValue, STREAM_SEEK_CUR, &m_Position) != S_OK)
    throw CInArchiveException(CInArchiveException::kSeekStreamError);
}

bool CInArchive::ReadBytesAndTestSize(void *data, UInt32 size)
{
  UInt32 realProcessedSize;
  if (ReadBytes(data, size, &realProcessedSize) != S_OK)
    throw CInArchiveException(CInArchiveException::kReadStreamError);
  return (realProcessedSize == size);
}

void CInArchive::SafeReadBytes(void *data, UInt32 size)
{
  if (!ReadBytesAndTestSize(data, size))
    throw CInArchiveException(CInArchiveException::kUnexpectedEndOfArchive);
}

void CInArchive::ReadBuffer(CByteBuffer &buffer, UInt32 size)
{
  buffer.SetCapacity(size);
  if (size > 0)
    SafeReadBytes(buffer, size);
}

Byte CInArchive::ReadByte()
{
  Byte b;
  SafeReadBytes(&b, 1);
  return b;
}

UInt16 CInArchive::ReadUInt16()
{
  UInt16 value = 0;
  for (int i = 0; i < 2; i++)
    value |= (((UInt16)ReadByte()) << (8 * i));
  return value;
}

UInt32 CInArchive::ReadUInt32()
{
  UInt32 value = 0;
  for (int i = 0; i < 4; i++)
    value |= (((UInt32)ReadByte()) << (8 * i));
  return value;
}

UInt64 CInArchive::ReadUInt64()
{
  UInt64 value = 0;
  for (int i = 0; i < 8; i++)
    value |= (((UInt64)ReadByte()) << (8 * i));
  return value;
}

bool CInArchive::ReadUInt32(UInt32 &value)
{
  value = 0;
  for (int i = 0; i < 4; i++)
  {
    Byte b;
    if (!ReadBytesAndTestSize(&b, 1))
      return false;
    value |= (UInt32(b) << (8 * i));
  }
  return true;
}


AString CInArchive::ReadFileName(UInt32 nameSize)
{
  if (nameSize == 0)
    return AString();
  char *p = m_NameBuffer.GetBuffer(nameSize);
  SafeReadBytes(p, nameSize);
  p[nameSize] = 0;
  m_NameBuffer.ReleaseBuffer();
  return m_NameBuffer;
}

void CInArchive::GetArchiveInfo(CInArchiveInfo &archiveInfo) const
{
  archiveInfo = m_ArchiveInfo;
}

/*
void CInArchive::ThrowIncorrectArchiveException()
{
  throw CInArchiveException(CInArchiveException::kIncorrectArchive);
}
*/

static UInt32 GetUInt32(const Byte *data)
{
  return
      ((UInt32)(Byte)data[0]) |
      (((UInt32)(Byte)data[1]) << 8) |
      (((UInt32)(Byte)data[2]) << 16) |
      (((UInt32)(Byte)data[3]) << 24);
}

/*
static UInt16 GetUInt16(const Byte *data)
{
  return
      ((UInt16)(Byte)data[0]) |
      (((UInt16)(Byte)data[1]) << 8);
}
*/

static UInt64 GetUInt64(const Byte *data)
{
  return GetUInt32(data) | ((UInt64)GetUInt32(data + 4) << 32);
}



void CInArchive::ReadExtra(UInt32 extraSize, CExtraBlock &extraBlock,
    UInt64 &unpackSize, UInt64 &packSize, UInt64 &localHeaderOffset, UInt32 &diskStartNumber)
{
  extraBlock.Clear();
  UInt32 remain = extraSize;
  while(remain >= 4)
  {
    CExtraSubBlock subBlock;
    subBlock.ID = ReadUInt16();
    UInt32 dataSize = ReadUInt16();
    remain -= 4;
    if (dataSize > remain) // it's bug
      dataSize = remain;
    if (subBlock.ID == NFileHeader::NExtraID::kZip64)
    {
      if (unpackSize == 0xFFFFFFFF)
      {
        if (dataSize < 8)
          break;
        unpackSize = ReadUInt64();
        remain -= 8;
        dataSize -= 8;
      }
      if (packSize == 0xFFFFFFFF)
      {
        if (dataSize < 8)
          break;
        packSize = ReadUInt64();
        remain -= 8;
        dataSize -= 8;
      }
      if (localHeaderOffset == 0xFFFFFFFF)
      {
        if (dataSize < 8)
          break;
        localHeaderOffset = ReadUInt64();
        remain -= 8;
        dataSize -= 8;
      }
      if (diskStartNumber == 0xFFFF)
      {
        if (dataSize < 4)
          break;
        diskStartNumber = ReadUInt32();
        remain -= 4;
        dataSize -= 4;
      }
      for (UInt32 i = 0; i < dataSize; i++)
        ReadByte();
    }
    else
    {
      ReadBuffer(subBlock.Data, dataSize);
      extraBlock.SubBlocks.Add(subBlock);
    }
    remain -= dataSize;
  }
  IncreaseRealPosition(remain);
}

HRESULT CInArchive::ReadLocalItem(CItemEx &item)
{
  item.ExtractVersion.Version = ReadByte();
  item.ExtractVersion.HostOS = ReadByte();
  item.Flags = ReadUInt16();
  item.CompressionMethod = ReadUInt16();
  item.Time =  ReadUInt32();
  item.FileCRC = ReadUInt32();
  item.PackSize = ReadUInt32();
  item.UnPackSize = ReadUInt32();
  UInt32 fileNameSize = ReadUInt16();
  item.LocalExtraSize = ReadUInt16();
  item.Name = ReadFileName(fileNameSize);
  item.FileHeaderWithNameSize = 4 + NFileHeader::kLocalBlockSize + fileNameSize;
  if (item.LocalExtraSize > 0)
  {
    UInt64 localHeaderOffset = 0;
    UInt32 diskStartNumber = 0;
    ReadExtra(item.LocalExtraSize, item.LocalExtra, item.UnPackSize, item.PackSize,
      localHeaderOffset, diskStartNumber);
  }
  /*
  if (item.IsDir())
    item.UnPackSize = 0;       // check It
  */
  return S_OK;
}

HRESULT CInArchive::ReadLocalItemAfterCdItem(CItemEx &item)
{
  if (item.FromLocal)
    return S_OK;
  try
  {
    RINOK(Seek(m_ArchiveInfo.Base + item.LocalHeaderPosition));
    CItemEx localItem;
    if (ReadUInt32() != NSignature::kLocalFileHeader)
      return S_FALSE;
    RINOK(ReadLocalItem(localItem));
    if (item.Flags != localItem.Flags)
    {
      if (
          (item.CompressionMethod != NFileHeader::NCompressionMethod::kDeflated ||
            (item.Flags & 0x7FF9) != (localItem.Flags & 0x7FF9)) &&
          (item.CompressionMethod != NFileHeader::NCompressionMethod::kStored ||
            (item.Flags & 0x7FFF) != (localItem.Flags & 0x7FFF)) &&
          (item.CompressionMethod != NFileHeader::NCompressionMethod::kImploded ||
            (item.Flags & 0x7FFF) != (localItem.Flags & 0x7FFF))
        )
        return S_FALSE;
    }

    if (item.CompressionMethod != localItem.CompressionMethod ||
        // item.Time != localItem.Time ||
        (!localItem.HasDescriptor() &&
          (
            item.FileCRC != localItem.FileCRC ||
            item.PackSize != localItem.PackSize ||
            item.UnPackSize != localItem.UnPackSize
          )
        ) ||
        item.Name.Length() != localItem.Name.Length()
        )
      return S_FALSE;
    item.FileHeaderWithNameSize = localItem.FileHeaderWithNameSize;
    item.LocalExtraSize = localItem.LocalExtraSize;
    item.LocalExtra = localItem.LocalExtra;
    item.FromLocal = true;
  }
  catch(...) { return S_FALSE; }
  return S_OK;
}

HRESULT CInArchive::ReadLocalItemDescriptor(CItemEx &item)
{
  if (item.HasDescriptor())
  {
    const int kBufferSize = (1 << 12);
    Byte buffer[kBufferSize];
    
    UInt32 numBytesInBuffer = 0;
    UInt32 packedSize = 0;
    
    bool descriptorWasFound = false;
    for (;;)
    {
      UInt32 processedSize;
      RINOK(ReadBytes(buffer + numBytesInBuffer, kBufferSize - numBytesInBuffer, &processedSize));
      numBytesInBuffer += processedSize;
      if (numBytesInBuffer < NFileHeader::kDataDescriptorSize)
        return S_FALSE;
      UInt32 i;
      for (i = 0; i <= numBytesInBuffer - NFileHeader::kDataDescriptorSize; i++)
      {
        // descriptorSignature field is Info-ZIP's extension
        // to Zip specification.
        UInt32 descriptorSignature = GetUInt32(buffer + i);
        
        // !!!! It must be fixed for Zip64 archives
        UInt32 descriptorPackSize = GetUInt32(buffer + i + 8);
        if (descriptorSignature== NSignature::kDataDescriptor && descriptorPackSize == packedSize + i)
        {
          descriptorWasFound = true;
          item.FileCRC = GetUInt32(buffer + i + 4);
          item.PackSize = descriptorPackSize;
          item.UnPackSize = GetUInt32(buffer + i + 12);
          IncreaseRealPosition(Int64(Int32(0 - (numBytesInBuffer - i - NFileHeader::kDataDescriptorSize))));
          break;
        }
      }
      if (descriptorWasFound)
        break;
      packedSize += i;
      int j;
      for (j = 0; i < numBytesInBuffer; i++, j++)
        buffer[j] = buffer[i];
      numBytesInBuffer = j;
    }
  }
  else
    IncreaseRealPosition(item.PackSize);
  return S_OK;
}

HRESULT CInArchive::ReadLocalItemAfterCdItemFull(CItemEx &item)
{
  if (item.FromLocal)
    return S_OK;
  try
  {
    RINOK(ReadLocalItemAfterCdItem(item));
    if (item.HasDescriptor())
    {
      RINOK(Seek(m_ArchiveInfo.Base + item.GetDataPosition() + item.PackSize));
      if (ReadUInt32() != NSignature::kDataDescriptor)
        return S_FALSE;
      UInt32 crc = ReadUInt32();
      UInt64 packSize, unpackSize;

      /*
      if (IsZip64)
      {
        packSize = ReadUInt64();
        unpackSize = ReadUInt64();
      }
      else
      */
      {
        packSize = ReadUInt32();
        unpackSize = ReadUInt32();
      }

      if (crc != item.FileCRC || item.PackSize != packSize || item.UnPackSize != unpackSize)
        return S_FALSE;
    }
  }
  catch(...) { return S_FALSE; }
  return S_OK;
}
  
HRESULT CInArchive::ReadCdItem(CItemEx &item)
{
  item.FromCentral = true;
  const int kBufSize = 42;
  Byte p[kBufSize];
  SafeReadBytes(p, kBufSize);
  item.MadeByVersion.Version = p[0];
  item.MadeByVersion.HostOS = p[1];
  item.ExtractVersion.Version = p[2];
  item.ExtractVersion.HostOS = p[3];
  item.Flags = Get16(p + 4);
  item.CompressionMethod = Get16(p + 6);
  item.Time = Get32(p + 8);
  item.FileCRC = Get32(p + 12);
  item.PackSize = Get32(p + 16);
  item.UnPackSize = Get32(p + 20);
  UInt16 headerNameSize = Get16(p + 24);
  UInt16 headerExtraSize = Get16(p + 26);
  UInt16 headerCommentSize = Get16(p + 28);
  UInt32 headerDiskNumberStart = Get16(p + 30);
  item.InternalAttributes = Get16(p + 32);
  item.ExternalAttributes = Get32(p + 34);
  item.LocalHeaderPosition = Get32(p + 38);
  item.Name = ReadFileName(headerNameSize);
  
  if (headerExtraSize > 0)
  {
    ReadExtra(headerExtraSize, item.CentralExtra, item.UnPackSize, item.PackSize,
        item.LocalHeaderPosition, headerDiskNumberStart);
  }

  if (headerDiskNumberStart != 0)
    throw CInArchiveException(CInArchiveException::kMultiVolumeArchiveAreNotSupported);
  
  // May be these strings must be deleted
  /*
  if (item.IsDir())
    item.UnPackSize = 0;
  */
  
  ReadBuffer(item.Comment, headerCommentSize);
  return S_OK;
}

HRESULT CInArchive::TryEcd64(UInt64 offset, CCdInfo &cdInfo)
{
  RINOK(Seek(offset));
  const UInt32 kEcd64Size = 56;
  Byte buf[kEcd64Size];
  if (!ReadBytesAndTestSize(buf, kEcd64Size))
    return S_FALSE;
  if (GetUInt32(buf) != NSignature::kZip64EndOfCentralDir)
    return S_FALSE;
  // cdInfo.NumEntries = GetUInt64(buf + 24);
  cdInfo.Size = GetUInt64(buf + 40);
  cdInfo.Offset = GetUInt64(buf + 48);
  return S_OK;
}

HRESULT CInArchive::FindCd(CCdInfo &cdInfo)
{
  UInt64 endPosition;
  RINOK(m_Stream->Seek(0, STREAM_SEEK_END, &endPosition));
  const UInt32 kBufSizeMax = (1 << 16) + kEcdSize + kZip64EcdLocatorSize;
  Byte buf[kBufSizeMax];
  UInt32 bufSize = (endPosition < kBufSizeMax) ? (UInt32)endPosition : kBufSizeMax;
  if (bufSize < kEcdSize)
    return S_FALSE;
  UInt64 startPosition = endPosition - bufSize;
  RINOK(m_Stream->Seek(startPosition, STREAM_SEEK_SET, &m_Position));
  if (m_Position != startPosition)
    return S_FALSE;
  if (!ReadBytesAndTestSize(buf, bufSize))
    return S_FALSE;
  for (int i = (int)(bufSize - kEcdSize); i >= 0; i--)
  {
    if (GetUInt32(buf + i) == NSignature::kEndOfCentralDir)
    {
      if (i >= kZip64EcdLocatorSize)
      {
        const Byte *locator = buf + i - kZip64EcdLocatorSize;
        if (GetUInt32(locator) == NSignature::kZip64EndOfCentralDirLocator)
        {
          UInt64 ecd64Offset = GetUInt64(locator + 8);
          if (TryEcd64(ecd64Offset, cdInfo) == S_OK)
            return S_OK;
          if (TryEcd64(m_ArchiveInfo.StartPosition + ecd64Offset, cdInfo) == S_OK)
          {
            m_ArchiveInfo.Base = m_ArchiveInfo.StartPosition;
            return S_OK;
          }
        }
      }
      if (GetUInt32(buf + i + 4) == 0)
      {
        // cdInfo.NumEntries = GetUInt16(buf + i + 10);
        cdInfo.Size = GetUInt32(buf + i + 12);
        cdInfo.Offset = GetUInt32(buf + i + 16);
        UInt64 curPos = endPosition - bufSize + i;
        UInt64 cdEnd = cdInfo.Size + cdInfo.Offset;
        if (curPos > cdEnd)
          m_ArchiveInfo.Base = curPos - cdEnd;
        return S_OK;
      }
    }
  }
  return S_FALSE;
}

HRESULT CInArchive::TryReadCd(CObjectVector<CItemEx> &items, UInt64 cdOffset, UInt64 cdSize, CProgressVirt *progress)
{
  items.Clear();
  RINOK(m_Stream->Seek(cdOffset, STREAM_SEEK_SET, &m_Position));
  if (m_Position != cdOffset)
    return S_FALSE;
  while(m_Position - cdOffset < cdSize)
  {
    if (ReadUInt32() != NSignature::kCentralFileHeader)
      return S_FALSE;
    CItemEx cdItem;
    RINOK(ReadCdItem(cdItem));
    items.Add(cdItem);
    if (progress && items.Size() % 1000 == 0)
      RINOK(progress->SetCompleted(items.Size()));
  }
  return (m_Position - cdOffset == cdSize) ? S_OK : S_FALSE;
}

HRESULT CInArchive::ReadCd(CObjectVector<CItemEx> &items, UInt64 &cdOffset, UInt64 &cdSize, CProgressVirt *progress)
{
  m_ArchiveInfo.Base = 0;
  CCdInfo cdInfo;
  RINOK(FindCd(cdInfo));
  HRESULT res = S_FALSE;
  cdSize = cdInfo.Size;
  cdOffset = cdInfo.Offset;
  res = TryReadCd(items, m_ArchiveInfo.Base + cdOffset, cdSize, progress);
  if (res == S_FALSE && m_ArchiveInfo.Base == 0)
  {
    res = TryReadCd(items, cdInfo.Offset + m_ArchiveInfo.StartPosition, cdSize, progress);
    if (res == S_OK)
      m_ArchiveInfo.Base = m_ArchiveInfo.StartPosition;
  }
  if (!ReadUInt32(m_Signature))
    return S_FALSE;
  return res;
}

HRESULT CInArchive::ReadLocalsAndCd(CObjectVector<CItemEx> &items, CProgressVirt *progress, UInt64 &cdOffset)
{
  items.Clear();
  while (m_Signature == NSignature::kLocalFileHeader)
  {
    // FSeek points to next byte after signature
    // NFileHeader::CLocalBlock localHeader;
    CItemEx item;
    item.LocalHeaderPosition = m_Position - m_StreamStartPosition - 4; // points to signature;
    RINOK(ReadLocalItem(item));
    item.FromLocal = true;
    ReadLocalItemDescriptor(item);
    items.Add(item);
    if (progress && items.Size() % 100 == 0)
      RINOK(progress->SetCompleted(items.Size()));
    if (!ReadUInt32(m_Signature))
      break;
  }
  cdOffset = m_Position - 4;
  for (int i = 0; i < items.Size(); i++)
  {
    if (progress && i % 1000 == 0)
      RINOK(progress->SetCompleted(items.Size()));
    if (m_Signature != NSignature::kCentralFileHeader)
      return S_FALSE;

    CItemEx cdItem;
    RINOK(ReadCdItem(cdItem));

    if (i == 0)
    {
      if (cdItem.LocalHeaderPosition == 0)
        m_ArchiveInfo.Base = m_ArchiveInfo.StartPosition;
    }

    int index;
    int left = 0, right = items.Size();
    for (;;)
    {
      if (left >= right)
        return S_FALSE;
      index = (left + right) / 2;
      UInt64 position = items[index].LocalHeaderPosition - m_ArchiveInfo.Base;
      if (cdItem.LocalHeaderPosition == position)
        break;
      if (cdItem.LocalHeaderPosition < position)
        right = index;
      else
        left = index + 1;
    }
    CItemEx &item = items[index];
    item.LocalHeaderPosition = cdItem.LocalHeaderPosition;
    item.MadeByVersion = cdItem.MadeByVersion;
    item.CentralExtra = cdItem.CentralExtra;

    if (
        // item.ExtractVersion != cdItem.ExtractVersion ||
        item.Flags != cdItem.Flags ||
        item.CompressionMethod != cdItem.CompressionMethod ||
        // item.Time != cdItem.Time ||
        item.FileCRC != cdItem.FileCRC)
      return S_FALSE;

    if (item.Name.Length() != cdItem.Name.Length() ||
        item.PackSize != cdItem.PackSize ||
        item.UnPackSize != cdItem.UnPackSize
      )
      return S_FALSE;
    item.Name = cdItem.Name;
    item.InternalAttributes = cdItem.InternalAttributes;
    item.ExternalAttributes = cdItem.ExternalAttributes;
    item.Comment = cdItem.Comment;
    item.FromCentral = cdItem.FromCentral;
    if (!ReadUInt32(m_Signature))
      return S_FALSE;
  }
  return S_OK;
}

struct CEcd
{
  UInt16 thisDiskNumber;
  UInt16 startCDDiskNumber;
  UInt16 numEntriesInCDOnThisDisk;
  UInt16 numEntriesInCD;
  UInt32 cdSize;
  UInt32 cdStartOffset;
  UInt16 commentSize;
  void Parse(const Byte *p);
};

void CEcd::Parse(const Byte *p)
{
  thisDiskNumber = Get16(p);
  startCDDiskNumber = Get16(p + 2);
  numEntriesInCDOnThisDisk = Get16(p + 4);
  numEntriesInCD = Get16(p + 6);
  cdSize = Get32(p + 8);
  cdStartOffset = Get32(p + 12);
  commentSize = Get16(p + 16);
}

struct CEcd64
{
  UInt16 versionMade;
  UInt16 versionNeedExtract;
  UInt32 thisDiskNumber;
  UInt32 startCDDiskNumber;
  UInt64 numEntriesInCDOnThisDisk;
  UInt64 numEntriesInCD;
  UInt64 cdSize;
  UInt64 cdStartOffset;
  void Parse(const Byte *p);
  CEcd64() { memset(this, 0, sizeof(*this)); }
};

void CEcd64::Parse(const Byte *p)
{
  versionMade = Get16(p);
  versionNeedExtract = Get16(p + 2);
  thisDiskNumber = Get32(p + 4);
  startCDDiskNumber = Get32(p + 8);
  numEntriesInCDOnThisDisk = Get64(p + 12);
  numEntriesInCD = Get64(p + 20);
  cdSize = Get64(p + 28);
  cdStartOffset = Get64(p + 36);
}

#define COPY_ECD_ITEM_16(n) if (!isZip64 || ecd. n != 0xFFFF)     ecd64. n = ecd. n;
#define COPY_ECD_ITEM_32(n) if (!isZip64 || ecd. n != 0xFFFFFFFF) ecd64. n = ecd. n;

HRESULT CInArchive::ReadHeaders(CObjectVector<CItemEx> &items, CProgressVirt *progress)
{
  // m_Signature must be kLocalFileHeaderSignature or
  // kEndOfCentralDirSignature
  // m_Position points to next byte after signature

  IsZip64 = false;
  items.Clear();

  UInt64 cdSize, cdStartOffset;
  HRESULT res = ReadCd(items, cdStartOffset, cdSize, progress);
  if (res != S_FALSE && res != S_OK)
    return res;

  /*
  if (res != S_OK)
    return res;
  res = S_FALSE;
  */

  if (res == S_FALSE)
  {
    m_ArchiveInfo.Base = 0;
    RINOK(m_Stream->Seek(m_ArchiveInfo.StartPosition, STREAM_SEEK_SET, &m_Position));
    if (m_Position != m_ArchiveInfo.StartPosition)
      return S_FALSE;
    if (!ReadUInt32(m_Signature))
      return S_FALSE;
    RINOK(ReadLocalsAndCd(items, progress, cdStartOffset));
    cdSize = (m_Position - 4) - cdStartOffset;
    cdStartOffset -= m_ArchiveInfo.Base;
  }

  CEcd64 ecd64;
  bool isZip64 = false;
  UInt64 zip64EcdStartOffset = m_Position - 4 - m_ArchiveInfo.Base;
  if (m_Signature == NSignature::kZip64EndOfCentralDir)
  {
    IsZip64 = isZip64 = true;
    UInt64 recordSize = ReadUInt64();

    const int kBufSize = kZip64EcdSize;
    Byte buf[kBufSize];
    SafeReadBytes(buf, kBufSize);
    ecd64.Parse(buf);

    IncreaseRealPosition(recordSize - kZip64EcdSize);
    if (!ReadUInt32(m_Signature))
      return S_FALSE;
    if (ecd64.thisDiskNumber != 0 || ecd64.startCDDiskNumber != 0)
      throw CInArchiveException(CInArchiveException::kMultiVolumeArchiveAreNotSupported);
    if (ecd64.numEntriesInCDOnThisDisk != items.Size() ||
        ecd64.numEntriesInCD != items.Size() ||
        ecd64.cdSize != cdSize ||
        (ecd64.cdStartOffset != cdStartOffset &&
        (!items.IsEmpty())))
      return S_FALSE;
  }
  if (m_Signature == NSignature::kZip64EndOfCentralDirLocator)
  {
    /* UInt32 startEndCDDiskNumber = */ ReadUInt32();
    UInt64 endCDStartOffset = ReadUInt64();
    /* UInt32 numberOfDisks = */ ReadUInt32();
    if (zip64EcdStartOffset != endCDStartOffset)
      return S_FALSE;
    if (!ReadUInt32(m_Signature))
      return S_FALSE;
  }
  if (m_Signature != NSignature::kEndOfCentralDir)
      return S_FALSE;

  const int kBufSize = kEcdSize - 4;
  Byte buf[kBufSize];
  SafeReadBytes(buf, kBufSize);
  CEcd ecd;
  ecd.Parse(buf);

  COPY_ECD_ITEM_16(thisDiskNumber);
  COPY_ECD_ITEM_16(startCDDiskNumber);
  COPY_ECD_ITEM_16(numEntriesInCDOnThisDisk);
  COPY_ECD_ITEM_16(numEntriesInCD);
  COPY_ECD_ITEM_32(cdSize);
  COPY_ECD_ITEM_32(cdStartOffset);

  ReadBuffer(m_ArchiveInfo.Comment, ecd.commentSize);

  if (ecd64.thisDiskNumber != 0 || ecd64.startCDDiskNumber != 0)
    throw CInArchiveException(CInArchiveException::kMultiVolumeArchiveAreNotSupported);
  if ((UInt16)ecd64.numEntriesInCDOnThisDisk != ((UInt16)items.Size()) ||
      (UInt16)ecd64.numEntriesInCD != ((UInt16)items.Size()) ||
      (UInt32)ecd64.cdSize != (UInt32)cdSize ||
      ((UInt32)(ecd64.cdStartOffset) != (UInt32)cdStartOffset &&
        (!items.IsEmpty())))
      return S_FALSE;
  
  return S_OK;
}

ISequentialInStream* CInArchive::CreateLimitedStream(UInt64 position, UInt64 size)
{
  CLimitedSequentialInStream *streamSpec = new CLimitedSequentialInStream;
  CMyComPtr<ISequentialInStream> stream(streamSpec);
  SeekInArchive(m_ArchiveInfo.Base + position);
  streamSpec->SetStream(m_Stream);
  streamSpec->Init(size);
  return stream.Detach();
}

IInStream* CInArchive::CreateStream()
{
  CMyComPtr<IInStream> stream = m_Stream;
  return stream.Detach();
}

bool CInArchive::SeekInArchive(UInt64 position)
{
  UInt64 newPosition;
  if (m_Stream->Seek(position, STREAM_SEEK_SET, &newPosition) != S_OK)
    return false;
  return (newPosition == position);
}

}}
