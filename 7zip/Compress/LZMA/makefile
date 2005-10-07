PROG=../../../bin/Codecs/LZMA.so

LOCAL_FLAGS=-DCOMPRESS_MF_BT -DCOMPRESS_MF_PAT -DCOMPRESS_MF_HC -DCOMPRESS_MF_MT

MY_WINDOWS=

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
../../../Common/CRC.o\
../../../Common/AlignedBuffer.o\
DllExports.o\
InBuffer.o\
OutBuffer.o\
StreamUtils.o\
RangeCoderBit.o\
MT.o\
LZInWindow.o\
LZOutWindow.o\
Synchronization.o\
LZMADecoder.o\
LZMAEncoder.o

include ../../../makefile.glb

