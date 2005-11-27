PROG=../../../bin/Formats/gz.so

LOCAL_FLAGS=

MY_WINDOWS=$(MY_HOME)/myModuleFileName.o

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS_DLL)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/StringConvert.o\
../../../Common/Alloc.o\
../../../Common/String.o\
../../../Common/CRC.o\
../../../Common/StringToInt.o\
DllExports.o\
DLL.o\
PropVariant.o\
CopyCoder.o\
GZipHandler.o\
GZipHandlerOut.o\
GZipHeader.o\
GZipIn.o\
GZipOut.o\
GZipUpdate.o\
ProgressUtils.o\
StreamUtils.o \
CodecsPath.o\
InStreamWithCRC.o\
OutStreamWithCRC.o

include ../../../makefile.glb

