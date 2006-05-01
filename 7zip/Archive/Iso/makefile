PROG=../../../bin/Formats/iso.so

LOCAL_FLAGS=

MY_WINDOWS=$(MY_HOME)/wine_date_and_time.o

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS_DLL)

OBJS= \
../../../Common/MyWindows.o \
../../../Common/Alloc.o \
../../../Common/IntToString.o\
../../../Common/String.o\
../../../Common/StringConvert.o\
../../../Common/Vector.o\
DllExports.o\
IsoHandler.o\
IsoHeader.o\
IsoIn.o\
PropVariant.o\
LimitedStreams.o\
ProgressUtils.o\
StreamUtils.o \
ItemNameUtils.o \
CopyCoder.o

include ../../../makefile.glb

