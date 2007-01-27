PROG=../../../../bin/Codecs/LZMA.so

LOCAL_FLAGS=-DCOMPRESS_MF_MT -D_ST_MODE

MY_WINDOWS=

include ../../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
../../../Common/CRC.o\
DllExports.o\
InBuffer.o\
OutBuffer.o\
StreamUtils.o\
RangeCoderBit.o\
LZOutWindow.o\
Synchronization.o\
LZMADecoder.o\
LZMAEncoder.o \
7zCrc.o \
Sort.o \
Threads.o \
MatchFinder.o \
MatchFinderMt.o

include ../../../../makefile.glb

