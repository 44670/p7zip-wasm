// Zip/Update.h

#ifndef __ZIP_UPDATE_H
#define __ZIP_UPDATE_H

#include "Common/Vector.h"
#include "Common/Types.h"

#include "../../ICoder.h"
#include "../IArchive.h"

#include "ZipCompressionMode.h"
#include "ZipIn.h"

namespace NArchive {
namespace NZip {

struct CUpdateRange
{
  UInt64 Position; 
  UInt64 Size;
  CUpdateRange() {};
  CUpdateRange(UInt64 position, UInt64 size): Position(position), Size(size) {};
};

struct CUpdateItem
{
  bool NewData;
  bool NewProperties;
  bool IsDirectory;
  int IndexInArchive;
  int IndexInClient;
  UInt32 Attributes;
  UInt32 Time;
  UInt64 Size;
  AString Name;
  // bool Commented;
  // CUpdateRange CommentRange;
};

HRESULT Update(
    const CObjectVector<CItemEx> &inputItems,
    const CObjectVector<CUpdateItem> &updateItems,
    ISequentialOutStream *seqOutStream,
    CInArchive *inArchive,
    CCompressionMethodMode *compressionMethodMode,
    IArchiveUpdateCallback *updateCallback);

}}

#endif
