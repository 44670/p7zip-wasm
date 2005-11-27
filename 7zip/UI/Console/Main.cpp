// Main.cpp

#include "StdAfx.h"

#include <io.h>

#include "Common/MyInitGuid.h"
#include "Common/CommandLineParser.h"
#include "Common/StdOutStream.h"
#include "Common/Wildcard.h"
#include "Common/ListFileUtils.h"
#include "Common/StringConvert.h"
#include "Common/StdInStream.h"
#include "Common/StringToInt.h"
#include "Common/Exception.h"

#include "Windows/FileDir.h"
#include "Windows/FileName.h"
#include "Windows/Defs.h"
#include "Windows/Error.h"

#include "../../IPassword.h"
#include "../../ICoder.h"
#include "../../Compress/LZ/IMatchFinder.h"
#include "../Common/ArchiverInfo.h"
#include "../Common/UpdateAction.h"
#include "../Common/Update.h"
#include "../Common/Extract.h"
#include "../Common/ArchiveCommandLine.h"
#include "../Common/ExitCode.h"

#include "List.h"
#include "OpenCallbackConsole.h"
#include "ExtractCallbackConsole.h"
#include "UpdateCallbackConsole.h"

#include "../../MyVersion.h"

#ifndef EXCLUDE_COM
#include "Windows/DLL.h"
#endif

using namespace NWindows;
using namespace NFile;
using namespace NCommandLineParser;

HINSTANCE g_hInstance = 0;
extern CStdOutStream *g_StdStream;

static const char *kCopyrightString = "\n7-Zip"
#ifdef EXCLUDE_COM
" (A)"
#endif

#ifdef UNICODE
" [NT]"
#endif

" " MY_VERSION_COPYRIGHT_DATE "\n"
"p7zip Version " P7ZIP_VERSION ;

static const char *kHelpString = 
    "\nUsage: 7z"
#ifdef EXCLUDE_COM
    "a"
#endif
    " <command> [<switches>...] <archive_name> [<file_names>...]\n"
    "       [<@listfiles...>]\n"
    "\n"
    "<Commands>\n"
    "  a: Add files to archive\n"
    "  d: Delete files from archive\n"
    "  e: Extract files from archive (without using directory names)\n"
    "  l: List contents of archive\n"
//    "  l[a|t][f]: List contents of archive\n"
//    "    a - with Additional fields\n"
//    "    t - with all fields\n"
//    "    f - with Full pathnames\n"
    "  t: Test integrity of archive\n"
    "  u: Update files to archive\n"
    "  x: eXtract files with full paths\n"
    "<Switches>\n"
    "  -ai[r[-|0]]{@listfile|!wildcard}: Include archives\n"
    "  -ax[r[-|0]]{@listfile|!wildcard}: eXclude archives\n"
    "  -bd: Disable percentage indicator\n"
    "  -i[r[-|0]]{@listfile|!wildcard}: Include filenames\n"
#ifdef HAVE_LSTAT
    "  -l: don't store symlinks; store the files/directories they point to\n"
    "  CAUTION : the scanning stage can never end because of symlinks like '..'\n"
    "            (ex:  ln -s .. ldir)\n"
#endif
    "  -m{Parameters}: set compression Method\n"
    "  -o{Directory}: set Output directory\n"
    "  -p{Password}: set Password\n"
    "  -r[-|0]: Recurse subdirectories\n"
    "  (CAUTION: this flag does not do what you think, avoid using it)\n"
    "  -sfx[{name}]: Create SFX archive\n"
    "  -si[{name}]: read data from stdin\n"
    "  -so: write data to stdout\n"
    "  -t{Type}: Set type of archive\n"
    "  -v{Size}[b|k|m|g]: Create volumes\n"
    "  -u[-][p#][q#][r#][x#][y#][z#][!newArchiveName]: Update options\n"
    "  -w[{path}]: assign Work directory. Empty path means a temporary directory\n"
    "  -x[r[-|0]]]{@listfile|!wildcard}: eXclude filenames\n"
    "  -y: assume Yes on all queries\n";

// ---------------------------
// exception messages

static const char *kProcessArchiveMessage = " archive: ";
static const char *kEverythingIsOk = "Everything is Ok";
static const char *kUserErrorMessage  = "Incorrect command line"; // NExitCode::kUserError

static const wchar_t *kDefaultSfxModule = L"7zCon.sfx";

static void PrintHelp(CStdOutStream &s)
{
  s << kHelpString;
}

static void ShowMessageAndThrowException(CStdOutStream &s, LPCSTR message, NExitCode::EEnum code)
{
  s << message << endl;
  throw code;
}

static void PrintHelpAndExit(CStdOutStream &s) // yyy
{
  PrintHelp(s);
  ShowMessageAndThrowException(s, kUserErrorMessage, NExitCode::kUserError);
}

static void PrintProcessTitle(CStdOutStream &s, const AString &processTitle, const UString &archiveName)
{
  s << endl << processTitle << kProcessArchiveMessage << archiveName << endl << endl;
}

#ifndef _WIN32
static void GetArguments(int numArguments, const char *arguments[], UStringVector &parts)
{
  parts.Clear();
  for(int i = 0; i < numArguments; i++)
  {
    UString s = MultiByteToUnicodeString(arguments[i]);
    parts.Add(s);
  }
}
#endif

static void showCopyrightAndHelp(CStdOutStream &out,bool needHelp)
{
    out << kCopyrightString << " (locale=" << my_getlocale() <<",Utf16=";
    if (global_use_utf16_conversion) out << "on";
    else                             out << "off";
    out << ",HugeFiles=";
    if (sizeof(off_t) >= 8) out << "on)\n";
    else                    out << "off)\n";
    if (needHelp) out << kHelpString;
}

int Main2(
  #ifndef _WIN32  
  int numArguments, const char *arguments[]
  #endif
)
{
  #ifdef _WIN32  
  SetFileApisToOEM();
  #endif
  
  UStringVector commandStrings;
  #ifdef _WIN32  
  NCommandLineParser::SplitCommandLine(GetCommandLineW(), commandStrings);
  #else
  // GetArguments(numArguments, arguments, commandStrings);
  extern void mySplitCommandLine(int numArguments,const char *arguments[],UStringVector &parts);
  mySplitCommandLine(numArguments,arguments,commandStrings);
  #endif

  if(commandStrings.Size() == 1)
  {
    showCopyrightAndHelp(g_StdOut,true);
    return 0;
  }
  commandStrings.Delete(0);

  CArchiveCommandLineOptions options;

  CArchiveCommandLineParser parser;

  parser.Parse1(commandStrings, options);

  if(options.HelpMode)
  {
    showCopyrightAndHelp(g_StdOut,true);
    return 0;
  }

  #ifdef _WIN32
  if (options.LargePages)
    NSecurity::EnableLockMemoryPrivilege();
  #endif

  CStdOutStream &stdStream = options.StdOutMode ? g_StdErr : g_StdOut;
  g_StdStream = &stdStream;

  if (options.EnableHeaders)
    showCopyrightAndHelp(stdStream,false);

  parser.Parse2(options);

  bool isExtractGroupCommand = options.Command.IsFromExtractGroup();
  if(isExtractGroupCommand || 
      options.Command.CommandType == NCommandType::kList)
  {
    if(isExtractGroupCommand)
    {
      CExtractCallbackConsole *ecs = new CExtractCallbackConsole;
      CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;

      ecs->OutStream = &stdStream;
      ecs->PasswordIsDefined = options.PasswordEnabled;
      ecs->Password = options.Password;
      ecs->Init();

      COpenCallbackConsole openCallback;
      openCallback.OutStream = &stdStream;
      openCallback.PasswordIsDefined = options.PasswordEnabled;
      openCallback.Password = options.Password;

      CExtractOptions eo;
      eo.StdOutMode = options.StdOutMode;
      eo.PathMode = options.Command.GetPathMode();
      eo.TestMode = options.Command.IsTestMode();
      eo.OverwriteMode = options.OverwriteMode;
      eo.OutputDir = options.OutputDir;
      eo.YesToAll = options.YesToAll;
      HRESULT result = DecompressArchives(
          options.ArchivePathsSorted, 
          options.ArchivePathsFullSorted,
          options.WildcardCensor.Pairs.Front().Head, 
          eo, &openCallback, ecs);

      if (ecs->NumArchives > 1)
      {
        stdStream << endl << endl << "Total:" << endl;
        stdStream << "Archives: " << ecs->NumArchives << endl;
      }
      if (ecs->NumArchiveErrors != 0 || ecs->NumFileErrors != 0)
      {
        if (ecs->NumArchives > 1)
        {
          if (ecs->NumArchiveErrors != 0)
            stdStream << "Archive Errors: " << ecs->NumArchiveErrors << endl;
          if (ecs->NumFileErrors != 0)
            stdStream << "Sub items Errors: " << ecs->NumFileErrors << endl;
        }
        if (result != S_OK)
          throw CSystemException(result);
        return NExitCode::kFatalError;
      }
      if (result != S_OK)
        throw CSystemException(result);
    }
    else
    {
      HRESULT result = ListArchives(
          options.ArchivePathsSorted, 
          options.ArchivePathsFullSorted,
          options.WildcardCensor.Pairs.Front().Head, 
          options.EnableHeaders, 
          options.PasswordEnabled, 
          options.Password);
      if (result != S_OK)
        throw CSystemException(result);
    }
  }
  else if(options.Command.IsFromUpdateGroup())
  {
    UString workingDir;

    CUpdateOptions &uo = options.UpdateOptions;
    if (uo.SfxMode && uo.SfxModule.IsEmpty())
      uo.SfxModule = kDefaultSfxModule;

    bool passwordIsDefined = 
        options.PasswordEnabled && !options.Password.IsEmpty();

    COpenCallbackConsole openCallback;
    openCallback.OutStream = &stdStream;
    openCallback.PasswordIsDefined = passwordIsDefined;
    openCallback.Password = options.Password;

    CUpdateCallbackConsole callback;
    callback.EnablePercents = options.EnablePercents;
    callback.PasswordIsDefined = passwordIsDefined;
    callback.AskPassword = options.PasswordEnabled && options.Password.IsEmpty();
    callback.Password = options.Password;
    callback.StdOutMode = uo.StdOutMode;
    callback.Init(&stdStream);

    CUpdateErrorInfo errorInfo;

    HRESULT result = UpdateArchive(options.WildcardCensor, uo, 
        errorInfo, &openCallback, &callback);

#ifdef ENV_UNIX
    if (uo.SfxMode)
    {
        void myAddExeFlag(const UString &name);
        for(int i = 0; i < uo.Commands.Size(); i++)
        {
            CUpdateArchiveCommand &command = uo.Commands[i];
            if (!uo.StdOutMode)
            {
                myAddExeFlag(command.ArchivePath.GetFinalPath());
            }
        }
    }
#endif

    int exitCode = NExitCode::kSuccess;
    if (callback.CantFindFiles.Size() > 0)
    {
      stdStream << endl;
      stdStream << "WARNINGS for files:" << endl << endl;
      int numErrors = callback.CantFindFiles.Size();
      for (int i = 0; i < numErrors; i++)
      {
        stdStream << callback.CantFindFiles[i] << " : ";
        stdStream << NError::MyFormatMessageW(callback.CantFindCodes[i]) << endl;
      }
      stdStream << "----------------" << endl;
      stdStream << "WARNING: Cannot find " << numErrors << " file";
      if (numErrors > 1)
        stdStream << "s";
      stdStream << endl;
      exitCode = NExitCode::kWarning;
    }

    if (result != S_OK)
    {
      stdStream << "\nError:\n";
      if (!errorInfo.Message.IsEmpty())
        stdStream << errorInfo.Message << endl;
      if (!errorInfo.FileName.IsEmpty())
        stdStream << errorInfo.FileName << endl;
      if (!errorInfo.FileName2.IsEmpty())
        stdStream << errorInfo.FileName2 << endl;
      if (errorInfo.SystemError != 0)
        stdStream << NError::MyFormatMessageW(errorInfo.SystemError) << endl;
      throw CSystemException(result);
    }
    int numErrors = callback.FailedFiles.Size();
    if (numErrors == 0)
    {
      if (callback.CantFindFiles.Size() == 0)
        stdStream << kEverythingIsOk << endl;
    }
    else
    {
      stdStream << endl;
      stdStream << "WARNINGS for files:" << endl << endl;
      for (int i = 0; i < numErrors; i++)
      {
        stdStream << callback.FailedFiles[i] << " : ";
        stdStream << NError::MyFormatMessageW(callback.FailedCodes[i]) << endl;
      }
      stdStream << "----------------" << endl;
      stdStream << "WARNING: Cannot open " << numErrors << " file";
      if (numErrors > 1)
        stdStream << "s";
      stdStream << endl;
      exitCode = NExitCode::kWarning;
    }
    return exitCode;
  }
  else 
    PrintHelpAndExit(stdStream);
  return 0;
}
