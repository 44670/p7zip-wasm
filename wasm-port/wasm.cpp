#include <emscripten.h>
#include <stdint.h>
#include <stdio.h>
//
#include "StdAfx.h"
//
#include "Common/MyWindows.h"

#ifdef _WIN32
#include <Psapi.h>
#else
#include "myPrivate.h"
#endif
//
#include "../C/CpuArch.h"

#if defined(_7ZIP_LARGE_PAGES)
#include "../C/Alloc.h"
#endif
//
#include "Common/MyInitGuid.h"
//
#include "Common/CommandLineParser.h"
#include "Common/IntToString.h"
#include "Common/MyException.h"
#include "Common/StringConvert.h"
#include "Common/StringToInt.h"
#include "Common/UTFConvert.h"
//
#include "Windows/ErrorMsg.h"
//
#ifdef _WIN32
#include "Windows/MemoryLock.h"
#endif
//
#include "Windows/TimeUtils.h"
//
#include "7zip/UI/Common/ArchiveCommandLine.h"
#include "7zip/UI/Common/ExitCode.h"
#include "7zip/UI/Common/Extract.h"
//
#ifdef EXTERNAL_CODECS
#include "../Common/LoadCodecs.h"
#endif
//
#include "7zip/Common/RegisterCodec.h"
//
#ifdef PROG_VARIANT_R
#include "../C/7zVersion.h"
#else
#include "7zip/MyVersion.h"
#endif
//
EM_ASYNC_JS(int, jsWrite, (int idx, const void *data, size_t len),
            { return await wasmOnWrite(idx, data, len); });
EM_ASYNC_JS(int, jsClose, (int idx), { return await wasmOnClose(idx); });

extern "C" {

uint32_t apiExtractOneFileTarget = 0;

CCodecs *codecs = new CCodecs;
CMyComPtr<IUnknown> __codecsRef = codecs;

class MyOutStream : public ISequentialOutStream, public CMyUnknownImp {
 public:
  int idx;
  int refCount;
  MyOutStream(int i) {
    idx = i;
    refCount = 1;
  }
  STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize) {
    // Call wasmOnWrite on the JS side
    jsWrite(idx, data, size);
    *processedSize = size;
    return S_OK;
  }
  STDMETHOD(QueryInterface)
  (REFGUID iid, void **outObject) { return E_NOINTERFACE; }
  STDMETHOD_(ULONG, AddRef)
  () {
    refCount++;
    printf("MyOutStream::AddRef: %d\n", refCount);
    return 1;
  }
  STDMETHOD_(ULONG, Release)
  () {
    refCount--;
    printf("MyOutStream::Release: %d\n", refCount);
    if (refCount == 0) {
        jsClose(idx);
    }
    return 1;
  }
};

class MyCompressProgressInfo : public ICompressProgressInfo,
                               public CMyUnknownImp {
  STDMETHOD(SetRatioInfo)
  (const UInt64 *inSize, const UInt64 *outSize) { return S_OK; }
  STDMETHOD(QueryInterface)
  (REFIID iid, void **outObject) { return E_NOINTERFACE; }
  STDMETHOD_(ULONG, AddRef)
  () { return 1; }
  STDMETHOD_(ULONG, Release)
  () { return 1; }
};
MyCompressProgressInfo g_myCompressProgressInfo;

class MyExtractCallback : public IArchiveExtractCallback, public CMyUnknownImp {
  STDMETHOD(SetTotal)
  (UInt64 size) { return S_OK; }
  STDMETHOD(SetCompleted)
  (const UInt64 *completeValue) { return S_OK; }
  STDMETHOD(GetStream)
  (UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode) {
    if (apiExtractOneFileTarget != -1) {
      if (index != apiExtractOneFileTarget) {
        printf("Skipping idx: %d\n", index);
        return S_OK;
      }
    }

    *outStream = new MyOutStream(index);
    return S_OK;
  }
  STDMETHOD(PrepareOperation)
  (Int32 askExtractMode) { return S_OK; }
  STDMETHOD(SetOperationResult)
  (Int32 resultEOperationResult) { return S_OK; }
  STDMETHOD(QueryInterface)
  (REFGUID iid, void **outObject) {
    int groupID = iid.Data4[3];
    int subID = iid.Data4[5];
    printf("QueryInterface: %d %d\n", groupID, subID);
    if (groupID == 4) {
      if (subID == 4) {
        *outObject = &g_myCompressProgressInfo;
        return S_OK;
      }
    }
    return E_NOTIMPL;
  }
  virtual bool SetFileSymLinkAttrib() { return true; }
  STDMETHOD_(ULONG, AddRef)
  () { return 1; }
  STDMETHOD_(ULONG, Release)
  () { return 1; }
};

MyExtractCallback myExtractCallback;

#define API_GENERAL_ERROR (-1000)
#define API_NO_MEMORY (-1001)

static wchar_t listBuf[1 * 1024 * 1024];
CArc *currentArc;

void *getSymbol(int id) {
  if (id == 0) {
    return listBuf;
  }
  return NULL;
}

char *getpass(const char *prompt) {
  printf("Getpass not implemented!\n");
  return "";
}

int utilWStrLen(const wchar_t *s) { return (int)wcslen(s); }

int apiOpenArchive() {
  CArchiveLink *al = new CArchiveLink();
  COpenOptions op;
  op.filePath = L"/worker/archive";
  op.codecs = codecs;
  HRESULT ret = al->Open(op);
  printf("OpenArchive len: %d\n", al->Arcs.Size());
  if (al->Arcs.Size() <= 0) {
    return API_GENERAL_ERROR;
  }
  currentArc = &(al->Arcs.Back());
  return (int)ret;
}

int apiListFiles() {
  listBuf[0] = 0;
  int bufPos = 0;
  if (currentArc == NULL) {
    return API_GENERAL_ERROR;
  }
  // Iterate over all files in the archive
  UInt32 itemCount = 0;
  currentArc->Archive->GetNumberOfItems(&itemCount);
  printf("ItemCount: %d\n", itemCount);
  for (int i = 0; i < itemCount; i++) {
    CReadArcItem item;
    currentArc->GetItem(i, item);
    bool defined = false;
    uint64_t size = 0;
    currentArc->GetItemSize(i, size, defined);
    FILETIME time = {0};
    currentArc->GetItemMTime(i, time, defined);
    size_t dataLen = 7 + item.Path.Len() + 1;
    if (bufPos + dataLen > sizeof(listBuf)) {
      return API_NO_MEMORY;
    }
    listBuf[bufPos] = 0xCAFEBABE;
    listBuf[bufPos + 1] = item.IsDir ? 1 : 0;
    listBuf[bufPos + 2] = (uint32_t)size;
    listBuf[bufPos + 3] = (uint32_t)(size >> 32);
    listBuf[bufPos + 4] = time.dwLowDateTime;
    listBuf[bufPos + 5] = time.dwHighDateTime;
    listBuf[bufPos + 6] = item.Path.Len();
    bufPos += 7;
    memcpy(listBuf + bufPos, item.Path.Ptr(),
           item.Path.Len() * sizeof(wchar_t));
    bufPos += item.Path.Len();
  }
  return bufPos;
}

int apiExtractOneFile(uint32_t idx) {
  apiExtractOneFileTarget = idx;
  if (currentArc == NULL) {
    return API_GENERAL_ERROR;
  }
  CMyComPtr<IInArchive> archive = currentArc->Archive;
  uint32_t indicies[1] = {(uint32_t)idx};

  archive->Extract(indicies, 1, 0, &myExtractCallback);
  return 0;
}

int apiExtractAll() {
  apiExtractOneFileTarget = -1;
  if (currentArc == NULL) {
    return API_GENERAL_ERROR;
  }
  CMyComPtr<IInArchive> archive = currentArc->Archive;
  archive->Extract(NULL, -1, 0, &myExtractCallback);
  return 0;
}

int main(int argc, char **argv) {
  int ret = codecs->Load();
  printf("Codec load returned %d\n", ret);
  // print available codecs
  printf("Available codecs: %d\n", codecs->Formats.Size());
  for (int i = 0; i < codecs->Formats.Size(); i++) {
    const CArcInfoEx &ai = codecs->Formats[i];
    printf("%x\n", ai.Flags);
  }
  EM_ASM(wasmReady(););
}
}
