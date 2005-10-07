PROG=../../../bin/Codecs/Deflate.so

LOCAL_FLAGS=

MY_WINDOWS=

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
../../../Common/CRC.o\
DllExports.o\
HuffmanEncoder.o\
InBuffer.o\
LSBFDecoder.o\
LSBFEncoder.o\
OutBuffer.o\
LZInWindow.o\
LZOutWindow.o\
DeflateDecoder.o\
DeflateEncoder.o

include ../../../makefile.glb

