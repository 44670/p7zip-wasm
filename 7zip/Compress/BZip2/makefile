PROG=../../../bin/Codecs/BZip2.so

LOCAL_FLAGS=

MY_WINDOWS=

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
InBuffer.o\
OutBuffer.o\
DllExports.o\
BZip2Decoder.o\
BZip2Encoder.o\
HuffmanEncoder.o\
BlockSort.o\
BZip2CRC.o

include ../../../makefile.glb

