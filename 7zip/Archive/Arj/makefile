PROG=../../../bin/Formats/arj.so

LOCAL_FLAGS=

MY_WINDOWS=$(MY_HOME)/wine_date_and_time.o

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
../../../Common/Vector.o\
../../../Common/StringConvert.o\
../../../Common/String.o\
../../../Common/CRC.o\
DllExports.o\
PropVariant.o\
ArjHandler.o\
ArjIn.o\
ArjDecoder1.o\
ArjDecoder2.o\
LZOutWindow.o\
CopyCoder.o\
InBuffer.o\
OutBuffer.o\
LimitedStreams.o\
ProgressUtils.o\
StreamUtils.o \
ItemNameUtils.o\
OutStreamWithCRC.o

include ../../../makefile.glb

