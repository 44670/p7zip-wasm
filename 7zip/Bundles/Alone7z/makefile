PROG=../../../bin/7zr

LOCAL_FLAGS= \
  -DEXCLUDE_COM \
  -DNO_REGISTRY \
  -D_NO_CRYPTO \
  -DFORMAT_7Z \
  -DCOMPRESS_MT \
  -DCOMPRESS_BCJ_X86 \
  -DCOMPRESS_BCJ2 \
  -DCOMPRESS_COPY \
  -DCOMPRESS_LZMA \
  -DCOMPRESS_MF_MT 

include ../../../makefile.machine

LIBS=$(LOCAL_LIBS)

MY_WINDOWS= \
	$(MY_HOME)/wine_date_and_time.o \
	$(MY_HOME)/myAddExeFlag.o \
	$(MY_HOME)/myGetNumberOfProcessors.o \
	$(MY_HOME)/myModuleFileName.o \
	$(MY_HOME)/mySplitCommandLine.o

OBJS=\
ConsoleClose.o \
ExtractCallbackConsole.o \
List.o \
Main.o \
MainAr.o \
OpenCallbackConsole.o \
PercentPrinter.o \
UpdateCallbackConsole.o \
UserInputUtils.o \
../../../Common/Alloc.o \
../../../Common/CommandLineParser.o \
../../../Common/CRC.o \
../../../Common/IntToString.o \
../../../Common/ListFileUtils.o \
../../../Common/MyWindows.o \
../../../Common/StdInStream.o \
../../../Common/StdOutStream.o \
../../../Common/String.o \
../../../Common/StringConvert.o \
../../../Common/StringToInt.o \
../../../Common/UTFConvert.o \
../../../Common/Vector.o \
../../../Common/Wildcard.o \
Error.o \
FileDir.o \
FileFind.o \
FileIO.o \
FileName.o \
PropVariant.o \
PropVariantConversions.o \
Synchronization.o \
FilePathAutoRename.o \
FileStreams.o \
InBuffer.o \
InOutTempBuffer.o \
LimitedStreams.o \
LockedStream.o \
OffsetStream.o \
OutBuffer.o \
ProgressUtils.o \
StreamBinder.o \
StreamObjects.o \
StreamUtils.o \
ArchiveCommandLine.o \
ArchiveExtractCallback.o \
ArchiveOpenCallback.o \
ArchiverInfo.o \
DefaultName.o \
EnumDirItems.o \
Extract.o \
ExtractingFilePath.o \
OpenArchive.o \
PropIDUtils.o \
SetProperties.o \
SortUtils.o \
TempFiles.o \
Update.o \
UpdateAction.o \
UpdateCallback.o \
UpdatePair.o \
UpdateProduce.o \
CoderMixer2.o \
CoderMixer2MT.o \
CrossThreadProgress.o \
DummyOutStream.o \
FilterCoder.o \
InStreamWithCRC.o \
ItemNameUtils.o \
MultiStream.o \
OutStreamWithCRC.o \
ParseProperties.o \
7zCompressionMode.o \
7zDecode.o \
7zEncode.o \
7zExtract.o \
7zFolderInStream.o \
7zFolderOutStream.o \
7zHandler.o \
7zHandlerOut.o \
7zHeader.o \
7zIn.o \
7zMethodID.o \
7zOut.o \
7zProperties.o \
7zSpecStream.o \
7zUpdate.o \
BranchCoder.o \
x86.o \
x86_2.o \
LZInWindow.o \
LZOutWindow.o \
LZMADecoder.o \
LZMAEncoder.o \
CopyCoder.o \
MT.o \
RangeCoderBit.o \

include ../../../makefile.glb

