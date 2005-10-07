PROG=../../../bin/Formats/7z.so

LOCAL_FLAGS=

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS_DLL)

MY_WINDOWS= \
	$(MY_HOME)/wine_date_and_time.o \
	$(MY_HOME)/myModuleFileName.o

include ../../../makefile.machine

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o \
../../../Common/Vector.o \
../../../Common/StringConvert.o \
../../../Common/String.o \
../../../Common/CRC.o \
../../../Common/IntToString.o \
../../../Common/StringToInt.o \
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
7zMethods.o \
7zOut.o \
7zProperties.o \
7zSpecStream.o \
7zUpdate.o \
CodecsPath.o \
CoderLoader.o \
CoderMixer2.o \
CoderMixer2MT.o \
CrossThreadProgress.o \
FilterCoder.o \
InStreamWithCRC.o \
ItemNameUtils.o \
MultiStream.o \
StreamUtils.o \
OutStreamWithCRC.o \
InOutTempBuffer.o \
LimitedStreams.o \
LockedStream.o \
OutBuffer.o \
ProgressUtils.o \
StreamBinder.o \
StreamObjects.o \
DLL.o \
FileDir.o \
FileFind.o \
FileIO.o \
PropVariant.o \
Synchronization.o \
CopyCoder.o

# 7zCompressionMode.o

include ../../../makefile.glb

