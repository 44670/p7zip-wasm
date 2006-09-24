PROG=../../../bin/Formats/7za.so

LOCAL_FLAGS=\
  -DEXCLUDE_COM \
  -DNO_REGISTRY \
  -DFORMAT_7Z \
  -DCOMPRESS_MT \
  -DCOMPRESS_BCJ_X86 \
  -DCOMPRESS_BCJ2 \
  -DCOMPRESS_BZIP2_DECODER \
  -DCOMPRESS_BZIP2_MT \
  -DCOMPRESS_COPY \
  -DCOMPRESS_DEFLATE_DECODER \
  -DCOMPRESS_LZMA \
  -DCOMPRESS_MF_MT \
  -DCOMPRESS_PPMD \
  -DCRYPTO_7ZAES \
  -DCRYPTO_AES

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS_DLL)

MY_WINDOWS= $(MY_HOME)/myModuleFileName.o \
	$(MY_HOME)/wine_date_and_time.o \
	$(MY_HOME)/myGetNumberOfProcessors.o

include ../../../makefile.machine

OBJS = \
../../../Common/Alloc.o \
../../../Common/CRC.o \
../../../Common/IntToString.o \
../../../Common/String.o \
../../../Common/StringConvert.o \
../../../Common/StringToInt.o \
../../../Common/Vector.o \
../../../Common/MyWindows.o \
../../../Common/Wildcard.o \
FileDir.o \
FileFind.o \
FileIO.o \
PropVariant.o \
Synchronization.o \
InBuffer.o \
InOutTempBuffer.o \
LimitedStreams.o \
LockedStream.o \
LSBFDecoder.o \
OutBuffer.o \
ProgressUtils.o \
StreamBinder.o \
StreamObjects.o \
StreamUtils.o \
CoderMixer2.o \
CoderMixer2MT.o \
CrossThreadProgress.o \
FilterCoder.o \
InStreamWithCRC.o \
ItemNameUtils.o \
OutStreamWithCRC.o \
ParseProperties.o \
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
BranchCoder.o \
x86.o \
x86_2.o \
BZip2CRC.o \
BZip2Decoder.o \
DeflateDecoder.o \
LZInWindow.o \
LZOutWindow.o \
LZMADecoder.o \
LZMAEncoder.o \
PPMDDecoder.o \
PPMDEncoder.o \
7zAES.o \
MyAES.o \
aescrypt.o \
aeskey.o \
aestab.o \
CopyCoder.o \
MT.o \
RangeCoderBit.o \
Sha256.o

include ../../../makefile.glb

