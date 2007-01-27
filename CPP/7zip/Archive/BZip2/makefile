PROG=../../../../bin/Formats/bz2.so

LOCAL_FLAGS=-DCOMPRESS_MT

MY_WINDOWS=$(MY_HOME)/myGetNumberOfProcessors.o

include ../../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS_DLL)

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o\
../../../Common/String.o\
../../../Common/StringConvert.o\
../../../Common/StringToInt.o\
DllExports.o\
DLL.o\
PropVariant.o\
CopyCoder.o\
CodecsPath.o\
ParseProperties.o\
DummyOutStream.o\
BZip2Handler.o\
BZip2HandlerOut.o\
BZip2Update.o\
StreamUtils.o \
ProgressUtils.o

include ../../../../makefile.glb

