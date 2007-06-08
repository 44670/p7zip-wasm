PROG=../../../../bin/7z

LOCAL_FLAGS=\
  -DCOMPRESS_MT \
  -DWIN_LONG_PATH \
  -DEXTERNAL_LZMA \
  -DEXTERNAL_CODECS \
  -DBREAK_HANDLER \
  -DBENCH_MT \
  -D_7ZIP_LARGE_PAGES

include ../../../../makefile.crc32
include ../../../../makefile.machine

LIBS=$(LOCAL_LIBS_DLL)

MY_WINDOWS=\
	$(MY_HOME)/mySplitCommandLine.o \
	$(MY_HOME)/myAddExeFlag.o \
	$(MY_HOME)/myGetNumberOfProcessors.o \
	$(MY_HOME)/wine_date_and_time.o

OBJS = \
../../../Common/MyWindows.o \
../../../Common/CommandLineParser.o \
../../../Common/CRC.o \
../../../Common/StdInStream.o \
../../../Common/StdOutStream.o \
../../../Common/ListFileUtils.o \
../../../Common/Vector.o \
../../../Common/Wildcard.o \
../../../Common/StringConvert.o \
../../../Common/String.o \
../../../Common/IntToString.o \
../../../Common/StringToInt.o \
ConsoleClose.o \
ExtractCallbackConsole.o \
List.o \
Main.o \
MainAr.o \
OpenCallbackConsole.o \
PercentPrinter.o \
UpdateCallbackConsole.o \
UserInputUtils.o \
DLL.o \
Error.o \
FileDir.o \
FileFind.o \
FileIO.o \
FileName.o \
PropVariant.o \
PropVariantConversions.o \
ArchiveCommandLine.o \
ArchiveExtractCallback.o \
ArchiveOpenCallback.o \
DefaultName.o \
EnumDirItems.o \
Extract.o \
SetProperties.o \
ExtractingFilePath.o \
LoadCodecs.o \
OpenArchive.o \
PropIDUtils.o \
SortUtils.o \
TempFiles.o \
Update.o \
UpdateAction.o \
UpdateCallback.o \
UpdatePair.o \
UpdateProduce.o \
FilePathAutoRename.o \
FileStreams.o \
CopyCoder.o \
StreamUtils.o \
LzmaBench.o \
LzmaBenchCon.o \
$(OBJ_CRC32)

include ../../../../makefile.glb

