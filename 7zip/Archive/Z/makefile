PROG=../../../bin/Formats/z.so

LOCAL_FLAGS=

MY_WINDOWS=$(MY_HOME)/myModuleFileName.o

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
../../../Common/String.o\
../../../Common/StringConvert.o\
DllExports.o \
ZHandler.o \
ZDecoder.o \
PropVariant.o\
InBuffer.o\
OutBuffer.o\
ProgressUtils.o\
StreamUtils.o\
CodecsPath.o\
DummyOutStream.o

include ../../../makefile.glb

