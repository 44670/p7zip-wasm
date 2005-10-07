PROG=../../../bin/Codecs/Branch.so

LOCAL_FLAGS=

MY_WINDOWS= 

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o \
DllExports.o\
ARM.o\
ARMThumb.o\
BranchCoder.o\
IA64.o\
PPC.o\
SPARC.o\
x86.o\
x86_2.o\
InBuffer.o\
OutBuffer.o

include ../../../makefile.glb

