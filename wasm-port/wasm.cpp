#include <stdint.h>
#include <stdio.h>
#include <emscripten.h>

#include "StdAfx.h"

#include "Common/MyWindows.h"

#ifdef _WIN32
#include <Psapi.h>
#else
#include "myPrivate.h"
#endif

#include "../C/CpuArch.h"

#if defined(_7ZIP_LARGE_PAGES)
#include "../C/Alloc.h"
#endif

#include "Common/MyInitGuid.h"

#include "Common/CommandLineParser.h"
#include "Common/IntToString.h"
#include "Common/MyException.h"
#include "Common/StringConvert.h"
#include "Common/StringToInt.h"
#include "Common/UTFConvert.h"

#include "Windows/ErrorMsg.h"

#ifdef _WIN32
#include "Windows/MemoryLock.h"
#endif

#include "Windows/TimeUtils.h"

#include "7zip/UI/Common/ArchiveCommandLine.h"
#include "7zip/UI/Common/ExitCode.h"
#include "7zip/UI/Common/Extract.h"

#ifdef EXTERNAL_CODECS
#include "../Common/LoadCodecs.h"
#endif

#include "7zip/Common/RegisterCodec.h"
/*
#include "BenchCon.h"
#include "ConsoleClose.h"
#include "ExtractCallbackConsole.h"
#include "List.h"
#include "OpenCallbackConsole.h"
#include "UpdateCallbackConsole.h"

#include "HashCon.h"*/

#ifdef PROG_VARIANT_R
#include "../C/7zVersion.h"
#else
#include "7zip/MyVersion.h"
#endif

CCodecs *codecs = new CCodecs;
CMyComPtr<IUnknown> __codecsRef = codecs;
CArc *currentArc = NULL;
extern "C"
{

    static wchar_t listBuf[1 * 1024 * 1024];
    void *getSymbol(int id)
    {
        if (id == 0)
        {
            return listBuf;
        }
        return NULL;
    }

    char *getpass(const char *prompt)
    {
        printf("Getpass not implemented!\n");
        return "";
    }

    int utilWStrLen(const wchar_t *s)
    {
        return (int)wcslen(s);
    }

    int apiOpenArchive()
    {
        CArchiveLink *al = new CArchiveLink();
        COpenOptions op;
        op.filePath = L"archive";
        op.codecs = codecs;
        HRESULT ret = al->Open(op);
        printf("OpenArchive len: %d\n", al->Arcs.Size());
        if (al->Arcs.Size() <= 0)
        {
            return -1000;
        }
        currentArc = &(al->Arcs.Back());
        return (int)ret;
    }

    int apiListFiles()
    {
        listBuf[0] = 0;
        int bufPos = 0;
        if (currentArc == NULL)
        {
            return -1000;
        }
        // Iterate over all files in the archive
        UInt32 itemCount = 0;
        currentArc->Archive->GetNumberOfItems(&itemCount);
        printf("ItemCount: %d\n", itemCount);
        for (int i = 0; i < itemCount; i++)
        {
            UString result;
            currentArc->GetItemPath2(i, result);
            int len = (int)result.Len();
            if (bufPos + len + 1 > sizeof(listBuf))
            {
                return -1001;
            }
            memcpy(listBuf + bufPos, result.Ptr(), len * sizeof(wchar_t));
            listBuf[bufPos + len] = 0xCAFEBABE;
            bufPos += len + 1;
        }
        return bufPos;
    }

    int main(int argc, char **argv)
    {
        int ret = codecs->Load();
        printf("Codec load returned %d\n", ret);
        // print available codecs
        printf("Available codecs: %d\n", codecs->Formats.Size());
        for (int i = 0; i < codecs->Formats.Size(); i++)
        {
            const CArcInfoEx &ai = codecs->Formats[i];
            printf("%x\n", ai.Flags);
        }
        EM_ASM(
            wasmReady(););
    }
}