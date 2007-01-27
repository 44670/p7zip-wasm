PROG=../../../../bin/7zCon.sfx

LOCAL_FLAGS= \
  -DEXCLUDE_COM \
  -DNO_REGISTRY \
  -DEXTRACT_ONLY \
  -D_SFX \
  -DFORMAT_7Z \
  -DCOMPRESS_BCJ_X86 \
  -DCOMPRESS_BCJ2 \
  -DCOMPRESS_COPY \
  -DCOMPRESS_LZMA \
  -DCOMPRESS_PPMD \
  -DCRYPTO_7ZAES \
  -DCRYPTO_AES

include ../../../../makefile.machine

LIBS=$(LOCAL_LIBS)

MY_WINDOWS= \
	$(MY_HOME)/myGetTickCount.o\
	$(MY_HOME)/wine_date_and_time.o\
	$(MY_HOME)/mySplitCommandLine.o\
	$(MY_HOME)/myGetNumberOfProcessors.o

OBJS=\
CoderMixer2.o\
CoderMixer2MT.o\
CoderMixer2ST.o\
CrossThreadProgress.o\
FilterCoder.o\
ItemNameUtils.o\
OutStreamWithCRC.o\
ParseProperties.o \
ConsoleClose.o\
ExtractCallbackConsole.o\
List.o\
MainAr.o\
OpenCallbackConsole.o\
UserInputUtils.o\
7zDecode.o\
7zExtract.o\
7zFolderOutStream.o\
7zHandler.o\
7zHeader.o\
7zIn.o\
7zMethodID.o\
LZMADecoder.o\
BranchCoder.o\
x86.o\
x86_2.o\
PPMDDecoder.o\
LZOutWindow.o\
CopyCoder.o\
7zAES.o\
MyAES.o\
Error.o\
FileDir.o\
FileFind.o\
FileIO.o\
FileName.o\
PropVariant.o\
PropVariantConversions.o\
Synchronization.o\
../../../Common/MyWindows.o\
../../../Common/Alloc.o\
../../../Common/CommandLineParser.o\
../../../Common/StdInStream.o\
../../../Common/StdOutStream.o\
../../../Common/StringConvert.o\
../../../Common/String.o\
../../../Common/CRC.o\
../../../Common/Vector.o\
../../../Common/Wildcard.o\
../../../Common/IntToString.o\
../../../Common/StringToInt.o\
FilePathAutoRename.o\
FileStreams.o\
InBuffer.o\
LimitedStreams.o\
LockedStream.o\
StreamUtils.o\
OutBuffer.o\
ProgressUtils.o\
StreamBinder.o\
StreamObjects.o\
ArchiveExtractCallback.o\
ArchiveOpenCallback.o\
ArchiverInfo.o\
DefaultName.o\
Extract.o\
SetProperties.o\
ExtractingFilePath.o\
OpenArchive.o\
PropIDUtils.o\
Main.o\
Sha256.o \
aescrypt.o\
aeskey.o\
aestab.o

include ../../../../makefile.glb

