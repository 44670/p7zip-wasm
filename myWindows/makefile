PROG=test_emul

LOCAL_FLAGS=-DENV_UNIX -I. -I.. -I../include_windows

include ../makefile.machine

LIBS=$(LOCAL_LIBS)

OBJS=myGetTickCount.o \
	myModuleFileName.o \
	wine_date_and_time.o \
	mySplitCommandLine.o \
	myAddExeFlag.o \
	test_emul.o \
	../Common/Vector.o \
	../Common/String.o \
	../Common/MyWindows.o

include ../makefile.glb

