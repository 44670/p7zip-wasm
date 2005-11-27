PROG=../../../bin/Formats/cab.so

LOCAL_FLAGS=

MY_WINDOWS=$(MY_HOME)/wine_date_and_time.o

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/Alloc.o\
../../../Common/IntToString.o\
../../../Common/String.o\
../../../Common/StringConvert.o\
../../../Common/StringToInt.o\
../../../Common/UTFConvert.o\
../../../Common/Vector.o\
../../../Common/MyWindows.o \
DllExports.o \
CabBlockInStream.o \
CabHandler.o \
CabHeader.o \
CabIn.o \
PropVariant.o \
InBuffer.o \
LSBFDecoder.o \
OutBuffer.o \
ProgressUtils.o \
StreamUtils.o \
LzxDecoder.o \
Lzx86Converter.o \
DeflateDecoder.o \
QuantumDecoder.o \
LZOutWindow.o \
CopyCoder.o

include ../../../makefile.glb

