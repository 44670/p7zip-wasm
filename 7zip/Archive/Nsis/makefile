PROG=../../../bin/Formats/nsis.so

LOCAL_FLAGS=

include ../../../makefile.machine

LOCAL_SHARED=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS_DLL)

MY_WINDOWS= \
 $(MY_HOME)/myModuleFileName.o \
 $(MY_HOME)/wine_date_and_time.o \

include ../../../makefile.machine

OBJS = \
../../../Common/MyWindows.o \
../../../Common/Alloc.o \
../../../Common/IntToString.o \
../../../Common/String.o \
../../../Common/StringConvert.o \
../../../Common/Vector.o \
DllExports.o \
NsisDecode.o \
NsisHandler.o \
NsisIn.o \
DLL.o \
FileFind.o \
PropVariant.o \
LimitedStreams.o \
ProgressUtils.o \
StreamUtils.o \
CodecsPath.o \
CoderLoader.o \
ItemNameUtils.o \
FilterCoder.o \
7zMethodID.o \
7zMethods.o

include ../../../makefile.glb

