PROG=../../../../bin/7zCon.sfx

LOCAL_FLAGS= \
  -DEXTRACT_ONLY \
  -DNO_READ_FROM_CODER \
  -D_SFX

include ../../../../makefile.crc32
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
7zRegister.o\
LZMADecoder.o\
LZMARegister.o\
BranchCoder.o\
x86.o\
x86_2.o\
PPMDDecoder.o\
PPMDRegister.o\
LZOutWindow.o\
CopyCoder.o\
CopyRegister.o\
7zAES.o\
7zAESRegister.o\
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
../../../Common/CommandLineParser.o\
../../../Common/StdInStream.o\
../../../Common/StdOutStream.o\
../../../Common/StringConvert.o\
../../../Common/String.o\
../../../Common/CRC.o\
../../../Common/Vector.o\
../../../Common/Wildcard.o\
../../../Common/IntToString.o\
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
DefaultName.o\
LoadCodecs.o\
Extract.o\
ExtractingFilePath.o\
OpenArchive.o\
PropIDUtils.o\
CreateCoder.o\
Main.o\
Sha256.o \
aescrypt.o\
aeskey.o\
aestab.o \
BranchX86.o \
BCJ2Register.o\
BCJRegister.o\
$(OBJ_CRC32)

include ../../../../makefile.glb

