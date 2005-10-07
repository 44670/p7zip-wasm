PROG=../../../bin/Formats/cpio.so

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
../../../Common/StringToInt.o\
DllExports.o\
CopyCoder.o\
CpioHandler.o\
CpioHeader.o\
CpioIn.o\
PropVariant.o\
ItemNameUtils.o\
LimitedStreams.o\
StreamUtils.o\
ProgressUtils.o

include ../../../makefile.glb

