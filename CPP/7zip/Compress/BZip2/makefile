PROG=../../../../bin/Codecs/BZip2.so

LOCAL_FLAGS=-DCOMPRESS_BZIP2_MT

MY_WINDOWS=

include ../../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
InBuffer.o\
OutBuffer.o\
Synchronization.o\
DllExports.o\
BZip2Decoder.o\
BZip2Encoder.o\
BlockSort.o\
BZip2CRC.o \
Sort.o \
HuffmanEncode.o

include ../../../../makefile.glb

