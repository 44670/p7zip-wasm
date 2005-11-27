PROG=../../../bin/Formats/chm.so

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
../../../Common/UTFConvert.o\
../../../Common/Vector.o\
../../../Common/MyWindows.o \
DllExports.o \
ChmHandler.o \
ChmHeader.o \
ChmIn.o \
PropVariant.o \
InBuffer.o \
LimitedStreams.o \
OutBuffer.o \
ProgressUtils.o \
StreamUtils.o \
ItemNameUtils.o \
LzxDecoder.o \
Lzx86Converter.o \
LZOutWindow.o \
CopyCoder.o


include ../../../makefile.glb

