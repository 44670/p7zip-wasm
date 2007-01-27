PROG=../../../../bin/Client7z

LOCAL_FLAGS=-DCOMPRESS_MT

include ../../../../makefile.machine

LIBS=$(LOCAL_LIBS_DLL)

MY_WINDOWS=\
	$(MY_HOME)/wine_date_and_time.o

OBJS = \
../../../Common/MyWindows.o \
DLL.o \
FileDir.o \
FileFind.o \
FileIO.o \
FileName.o \
PropVariant.o \
PropVariantConversions.o \
IntToString.o \
String.o \
StringConvert.o \
Vector.o \
Wildcard.o \
FileStreams.o \
Client7z.o

include ../../../../makefile.glb

