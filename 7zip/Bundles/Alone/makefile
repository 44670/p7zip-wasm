
PROG=../../../bin/7za
PROGS=lib7za.a testlib testlib.exe

LOCAL_FLAGS= \
-DEXCLUDE_COM -DNO_REGISTRY -DFORMAT_7Z -DFORMAT_BZIP2 -DFORMAT_ZIP \
-DFORMAT_TAR -DFORMAT_GZIP -DCOMPRESS_LZMA -DCOMPRESS_BCJ_X86 \
-DFORMAT_SPLIT -DFORMAT_Z \
-DCOMPRESS_BCJ2 -DCOMPRESS_COPY -DCOMPRESS_MF_MT -DCOMPRESS_PPMD \
-DCOMPRESS_DEFLATE -DCOMPRESS_DEFLATE64 -DCOMPRESS_IMPLODE -DCOMPRESS_BZIP2 \
-DCRYPTO_ZIP -DCRYPTO_7ZAES -DCRYPTO_AES

include ../../../makefile.machine

LIBS=$(LOCAL_LIBS)

MY_WINDOWS= \
	$(MY_HOME)/myGetTickCount.o \
	$(MY_HOME)/wine_date_and_time.o \
	$(MY_HOME)/myModuleFileName.o \
	$(MY_HOME)/myAddExeFlag.o \
	$(MY_HOME)/mySplitCommandLine.o

OBJS=\
../../../Common/MyWindows.o \
7zAES.o \
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
../../../Common/AlignedBuffer.o \
../../../Common/Alloc.o \
../../../Common/CommandLineParser.o \
../../../Common/ListFileUtils.o \
../../../Common/StdInStream.o \
../../../Common/StdOutStream.o \
../../../Common/Wildcard.o \
../../../Common/UTFConvert.o \
../../../Common/Vector.o \
../../../Common/StringConvert.o \
../../../Common/String.o \
../../../Common/CRC.o \
../../../Common/IntToString.o \
../../../Common/StringToInt.o \
../../../Common/Random.o \
BZip2Decoder.o \
BZip2Encoder.o \
BZip2Handler.o \
BZip2HandlerOut.o \
BZip2Update.o \
BZip2CRC.o \
BranchCoder.o \
ConsoleClose.o \
CopyCoder.o \
DefaultName.o \
DeflateDecoder.o \
DeflateEncoder.o \
DummyOutStream.o \
ExtractingFilePath.o \
FileDir.o \
FileFind.o \
FileName.o \
FilePathAutoRename.o \
FileStreams.o \
FilterCoder.o \
GZipHandler.o \
GZipHandlerOut.o \
GZipHeader.o \
GZipIn.o \
GZipOut.o \
GZipUpdate.o \
HuffmanEncoder.o \
ImplodeDecoder.o \
ImplodeHuffmanDecoder.o \
InBuffer.o \
InStreamWithCRC.o \
ItemNameUtils.o \
LSBFDecoder.o \
LSBFEncoder.o \
LZInWindow.o \
LZMADecoder.o \
LZMAEncoder.o \
LZOutWindow.o \
LimitedStreams.o \
List.o \
LockedStream.o \
MT.o \
MainAr.o \
Main.o \
MultiStream.o \
MyAES.o \
OffsetStream.o \
StreamUtils.o \
OpenCallbackConsole.o \
OutBuffer.o \
OutStreamWithCRC.o \
PPMDDecoder.o \
PPMDEncoder.o \
PercentPrinter.o \
ProgressUtils.o \
PropIDUtils.o \
PropVariant.o \
PropVariantConversions.o \
RangeCoderBit.o \
SHA256.o \
SortUtils.o \
SplitHandler.o \
StreamBinder.o \
StreamObjects.o \
Synchronization.o \
TarHandler.o \
TarHandlerOut.o \
TarHeader.o \
TarIn.o \
TarOut.o \
TarUpdate.o \
TempFiles.o \
UpdateAction.o \
UpdateCallbackConsole.o \
UpdatePair.o \
UpdateProduce.o \
UserInputUtils.o \
ZipAddCommon.o \
ZipCipher.o \
ZipCrypto.o \
ZipHandlerOut.o \
ZipHeader.o \
ZipIn.o \
ZipItem.o \
ZipOut.o \
ZipUpdate.o \
aescrypt.o \
aeskey.o \
aestab.o \
x86.o \
x86_2.o \
ArchiveCommandLine.o \
ArchiverInfo.o \
CoderMixer2.o \
CoderMixer2MT.o \
CrossThreadProgress.o \
EnumDirItems.o \
Extract.o \
InOutTempBuffer.o \
OpenArchive.o \
Error.o \
ArchiveOpenCallback.o \
ExtractCallbackConsole.o \
ArchiveExtractCallback.o \
UpdateCallback.o \
Update.o \
ZipHandler.o \
FileIO.o \
ShrinkDecoder.o \
ZDecoder.o \
ZHandler.o \
BlockSort.o

include ../../../makefile.glb

OBJ7ZA= \
../../../Common/AlignedBuffer.o \
../../../Common/Alloc.o \
../../../Common/Vector.o \
../../../Common/StringConvert.o \
../../../Common/String.o \
../../../Common/Wildcard.o \
../../../Common/StringToInt.o \
../../../Common/CRC.o \
../../../Common/IntToString.o \
DllExports.o \
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
DllExports.o\
HuffmanEncoder.o\
InBuffer.o\
LSBFDecoder.o\
LSBFEncoder.o\
OutBuffer.o\
LZInWindow.o\
LZOutWindow.o\
DeflateDecoder.o\
DeflateEncoder.o\
BZip2Decoder.o\
BZip2Encoder.o\
BZip2CRC.o \
CoderMixer2.o \
CoderMixer2MT.o \
CrossThreadProgress.o \
FilterCoder.o \
InStreamWithCRC.o \
ItemNameUtils.o \
OutStreamWithCRC.o \
InBuffer.o \
InOutTempBuffer.o \
LimitedStreams.o \
LockedStream.o \
OutBuffer.o \
ProgressUtils.o \
StreamBinder.o \
StreamObjects.o \
BranchCoder.o \
x86_2.o \
x86.o \
CopyCoder.o \
LZInWindow.o \
LZOutWindow.o \
LZMADecoder.o \
LZMAEncoder.o \
MT.o \
PPMDDecoder.o \
PPMDEncoder.o \
RangeCoderBit.o \
ZDecoder.o \
ZHandler.o \
7zAES.o \
SHA256.o \
aescrypt.o \
aeskey.o \
aestab.o \
MyAES.o \
FileDir.o \
FileFind.o \
FileIO.o \
PropVariant.o \
Synchronization.o \
BlockSort.o\
../../../Common/MyWindows.o \
	$(MY_HOME)/myGetTickCount.o \
	$(MY_HOME)/wine_date_and_time.o \
	$(MY_HOME)/myModuleFileName.o \
	$(MY_HOME)/mySplitCommandLine.o

lib7za.a: $(OBJ7ZA)
	rm -f lib7za.a
	ar cr lib7za.a $(OBJ7ZA)


testlib : testlib.o ./lib7za.a
	$(CXX) $(LOCAL_SHARED) $(LDFLAGS) testlib.o lib7za.a -o testlib $(LIBS)

testlib.o : testlib.cpp
	$(CXX) $(CFLAGS) testlib.cpp

DllExports.o : ../../Archive/7z/DllExports.cpp
	$(CXX) $(CFLAGS) ../../Archive/7z/DllExports.cpp

