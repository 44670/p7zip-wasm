PROG=../../../bin/Codecs/7zAES.so

LOCAL_FLAGS=

MY_WINDOWS= \
	$(MY_HOME)/wine_date_and_time.o \
	$(MY_HOME)/myModuleFileName.o

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS_DLL)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
../../../Common/StringConvert.o\
../../../Common/Vector.o\
../../../Common/String.o\
DllExports.o\
StreamObjects.o\
StreamUtils.o\
DLL.o\
Synchronization.o\
7zAES.o\
SHA256.o\
FileFind.o \
FileDir.o

include ../../../makefile.glb

