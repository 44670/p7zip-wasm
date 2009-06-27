// AgentProxy.h

#ifndef __AGENT_PROXY_H
#define __AGENT_PROXY_H

#include "Common/MyString.h"

#include "../Common/OpenArchive.h"

struct CProxyFile
{
  UInt32 Index;
  UString Name;
};

struct CProxyFolder: public CProxyFile
{
  CProxyFolder *Parent;
  CObjectVector<CProxyFolder> Folders;
  CObjectVector<CProxyFile> Files;
  bool IsLeaf;

  bool CrcIsDefined;
  UInt64 Size;
  UInt64 PackSize;
  UInt32 Crc;
  UInt32 NumSubFolders;
  UInt32 NumSubFiles;

  CProxyFolder(): Parent(NULL) {};
  int FindDirSubItemIndex(const UString &name, int &insertPos) const;
  int FindDirSubItemIndex(const UString &name) const;
  CProxyFolder* AddDirSubItem(UInt32 index, bool leaf, const UString &name);
  void AddFileSubItem(UInt32 index, const UString &name);
  void Clear();

  void GetPathParts(UStringVector &pathParts) const;
  UString GetFullPathPrefix() const;
  UString GetItemName(UInt32 index) const;
  void AddRealIndices(CUIntVector &realIndices) const;
  void GetRealIndices(const UInt32 *indices, UInt32 numItems, CUIntVector &realIndices) const;
  void CalculateSizes(IInArchive *archive);
};

struct CProxyArchive
{
  CProxyFolder RootFolder;
  bool ThereIsPathProp;

  HRESULT Load(const CArc &arc, IProgress *progress);
};

#endif
