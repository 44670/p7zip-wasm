PROG=../../../../bin/Codecs/Deflate.so

LOCAL_FLAGS=-D_ST_MODE

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
LSBFDecoder.o\
LSBFEncoder.o\
OutBuffer.o\
LZOutWindow.o\
DeflateDecoder.o\
DeflateEncoder.o \
7zCrc.o \
Sort.o \
MatchFinder.o \
HuffmanEncode.o

include ../../../../makefile.glb

