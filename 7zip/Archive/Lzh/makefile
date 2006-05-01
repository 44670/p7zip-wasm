PROG=../../../bin/Formats/lzh.so

LOCAL_FLAGS=

MY_WINDOWS=$(MY_HOME)/wine_date_and_time.o

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o \
../../../Common/String.o \
../../../Common/StringConvert.o \
../../../Common/Vector.o \
DllExports.o \
LimitedStreams.o \
StreamUtils.o \
OutBuffer.o \
ProgressUtils.o \
PropVariant.o \
LzhCRC.o \
ItemNameUtils.o \
LzhHandler.o \
LzhIn.o \
LzhOutStreamWithCRC.o \
LzhDecoder.o \
LZOutWindow.o \
CopyCoder.o \
InBuffer.o

include ../../../makefile.glb

