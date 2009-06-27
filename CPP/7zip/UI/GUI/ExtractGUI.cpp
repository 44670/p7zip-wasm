// ExtractGUI.cpp

#include "StdAfx.h"

#include "Common/IntToString.h"
#include "Common/StringConvert.h"

#include "Windows/Error.h"
#include "Windows/FileDir.h"
#include "Windows/FileFind.h"
#include "Windows/Thread.h"

#include "../FileManager/ExtractCallback.h"
#include "../FileManager/FormatUtils.h"
#include "../FileManager/LangUtils.h"

#include "../Common/ArchiveExtractCallback.h"
#include "../Common/PropIDUtils.h"

#include "../Explorer/MyMessages.h"

#include "resource.h"
#include "ExtractRes.h"
#include "ExtractDialog.h"
#include "ExtractGUI.h"

using namespace NWindows;

static const wchar_t *kIncorrectOutDir = L"Incorrect output directory path";

struct CThreadExtracting
{
  CCodecs *codecs;
  CExtractCallbackImp *ExtractCallbackSpec;
  CIntVector FormatIndices;

  UStringVector *ArchivePaths;
  UStringVector *ArchivePathsFull;
  const NWildcard::CCensorNode *WildcardCensor;
  const CExtractOptions *Options;
  CMyComPtr<IExtractCallbackUI> ExtractCallback;
  CDecompressStat Stat;
  UString ErrorMessage;
  HRESULT Result;
  
  DWORD Process()
  {
    ExtractCallbackSpec->ProgressDialog.WaitCreating();
    try
    {
      Result = DecompressArchives(
          codecs, FormatIndices,
          *ArchivePaths, *ArchivePathsFull,
          *WildcardCensor, *Options, ExtractCallbackSpec, ExtractCallback, ErrorMessage, Stat);
    }
    catch(const UString &s)
    {
      ErrorMessage = s;
      Result = E_FAIL;
    }
    catch(const wchar_t *s)
    {
      ErrorMessage = s;
      Result = E_FAIL;
    }
    catch(const char *s)
    {
      ErrorMessage = GetUnicodeString(s);
      Result = E_FAIL;
    }
    catch(...)
    {
      Result = E_FAIL;
    }
    ExtractCallbackSpec->ProgressDialog.MyClose();
    return 0;
  }
  static THREAD_FUNC_DECL MyThreadFunction(void *param)
  {
    return ((CThreadExtracting *)param)->Process();
  }
};

#ifndef _SFX

static void AddValuePair(UINT resourceID, UInt32 langID, UInt64 value, UString &s)
{
  wchar_t sz[32];
  s += LangString(resourceID, langID);
  s += L" ";
  ConvertUInt64ToString(value, sz);
  s += sz;
  s += L"\n";
}

static void AddSizePair(UINT resourceID, UInt32 langID, UInt64 value, UString &s)
{
  wchar_t sz[32];
  s += LangString(resourceID, langID);
  s += L" ";
  ConvertUInt64ToString(value, sz);
  s += sz;
  ConvertUInt64ToString(value >> 20, sz);
  s += L" (";
  s += sz;
  s += L" MB)";
  s += L"\n";
}

#endif

HRESULT ExtractGUI(
    CCodecs *codecs,
    const CIntVector &formatIndices,
    UStringVector &archivePaths,
    UStringVector &archivePathsFull,
    const NWildcard::CCensorNode &wildcardCensor,
    CExtractOptions &options,
    bool showDialog,
    CExtractCallbackImp *extractCallback)
{
  CThreadExtracting extracter;
  extracter.codecs = codecs;
  extracter.FormatIndices = formatIndices;

  if (!options.TestMode)
  {
    UString outputDir = options.OutputDir;
    if (outputDir.IsEmpty())
      NFile::NDirectory::MyGetCurrentDirectory(outputDir);
    if (showDialog)
    {
      CExtractDialog dialog;
      if (!NFile::NDirectory::MyGetFullPathName(outputDir, dialog.DirectoryPath))
      {
        ShowErrorMessage(kIncorrectOutDir);
        return E_FAIL;
      }
      NFile::NName::NormalizeDirPathPrefix(dialog.DirectoryPath);
#ifndef _WIN32
      {
	extern const TCHAR * nameWindowToUnix(const TCHAR * lpFileName);
        UString tmpDirectoryPath = dialog.DirectoryPath;
	dialog.DirectoryPath = nameWindowToUnix(tmpDirectoryPath);
      }
#endif


      // dialog.OverwriteMode = options.OverwriteMode;
      // dialog.PathMode = options.PathMode;

      if(dialog.Create(0) != IDOK)
        return E_ABORT;
      outputDir = dialog.DirectoryPath;
      options.OverwriteMode = dialog.OverwriteMode;
      options.PathMode = dialog.PathMode;
      #ifndef _SFX
      extractCallback->Password = dialog.Password;
      extractCallback->PasswordIsDefined = !dialog.Password.IsEmpty();
      #endif
    }
    if (!NFile::NDirectory::MyGetFullPathName(outputDir, options.OutputDir))
    {
      ShowErrorMessage(kIncorrectOutDir);
      return E_FAIL;
    }
    NFile::NName::NormalizeDirPathPrefix(options.OutputDir);
    
    /*
    if(!NFile::NDirectory::CreateComplexDirectory(options.OutputDir))
    {
      UString s = GetUnicodeString(NError::MyFormatMessage(GetLastError()));
      UString s2 = MyFormatNew(IDS_CANNOT_CREATE_FOLDER,
      #ifdef LANG
      0x02000603,
      #endif
      options.OutputDir);
      MyMessageBox(s2 + UString(L"\n") + s);
      return E_FAIL;
    }
    */
  }
  
  UString title = LangStringSpec(options.TestMode ? IDS_PROGRESS_TESTING : IDS_PROGRESS_EXTRACTING,
      options.TestMode ? 0x02000F90: 0x02000890);

  extracter.ExtractCallbackSpec = extractCallback;
  extracter.ExtractCallback = extractCallback;
  extracter.ExtractCallbackSpec->Init();

  extracter.ArchivePaths = &archivePaths;
  extracter.ArchivePathsFull = &archivePathsFull;
  extracter.WildcardCensor = &wildcardCensor;
  extracter.Options = &options;

  NWindows::CThread thread;
  RINOK(thread.Create(CThreadExtracting::MyThreadFunction, &extracter));
  extracter.ExtractCallbackSpec->StartProgressDialog(title);
  if (extracter.Result == S_OK && options.TestMode &&
      extracter.ExtractCallbackSpec->Messages.IsEmpty() &&
      extracter.ExtractCallbackSpec->NumArchiveErrors == 0)
  {
    #ifndef _SFX
    UString s;
    AddValuePair(IDS_ARCHIVES_COLON, 0x02000324, extracter.Stat.NumArchives, s);
    AddValuePair(IDS_FOLDERS_COLON, 0x02000321, extracter.Stat.NumFolders, s);
    AddValuePair(IDS_FILES_COLON, 0x02000320, extracter.Stat.NumFiles, s);
    AddSizePair(IDS_SIZE_COLON, 0x02000322, extracter.Stat.UnpackSize, s);
    AddSizePair(IDS_COMPRESSED_COLON, 0x02000323, extracter.Stat.PackSize, s);

    if (options.CalcCrc)
    {
      wchar_t temp[16];
      ConvertUInt32ToHex(extracter.Stat.CrcSum, temp);
      s += L"CRC: ";
      s += temp;
      s += L"\n";
    }

    s += L"\n";
    s += LangString(IDS_MESSAGE_NO_ERRORS, 0x02000608);

    MessageBoxW(0, s, LangString(IDS_PROGRESS_TESTING, 0x02000F90), 0);
    #endif
  }
  if (extracter.Result != S_OK)
    if (!extracter.ErrorMessage.IsEmpty())
      throw extracter.ErrorMessage;
  return extracter.Result;
}
