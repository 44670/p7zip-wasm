PROG=../../../bin/Codecs/PPMD.so

LOCAL_FLAGS=

MY_WINDOWS=

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o \
DllExports.o\
PPMDDecoder.o\
PPMDEncoder.o\
StreamUtils.o\
InBuffer.o\
OutBuffer.o

include ../../../makefile.glb

