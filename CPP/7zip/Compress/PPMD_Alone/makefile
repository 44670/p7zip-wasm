include ../../../../makefile.machine

PROG = ppmd.exe

LIB = $(LOCAL_LIBS)
RM = rm -f
CFLAGS = -c -I. -I../../../  -I../../../myWindows  -I../../../include_windows -UENV_UNIX

OBJS = \
  PPMDAlone.o \
  PPMDDecoder.o \
  PPMDEncoder.o \
  CommandLineParser.o \
  MyString.o \
  StringConvert.o \
  MyVector.o \
  InBuffer.o \
  OutBuffer.o \
  FileStreams.o \
  C_FileIO.o \
  StreamUtils.o \
  StringToInt.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(LDFLAGS) $(OBJS) $(LIB)

PPMDAlone.o: PPMDAlone.cpp
	$(CXX) $(CFLAGS) PPMDAlone.cpp

PPMDDecoder.o: ../PPMD/PPMDDecoder.cpp
	$(CXX) $(CFLAGS) ../PPMD/PPMDDecoder.cpp

PPMDEncoder.o: ../PPMD/PPMDEncoder.cpp
	$(CXX) $(CFLAGS) ../PPMD/PPMDEncoder.cpp

InBuffer.o: ../../Common/InBuffer.cpp
	$(CXX) $(CFLAGS) ../../Common/InBuffer.cpp

OutBuffer.o: ../../Common/OutBuffer.cpp
	$(CXX) $(CFLAGS) ../../Common/OutBuffer.cpp

FileStreams.o: ../../Common/FileStreams.cpp
	$(CXX) $(CFLAGS) ../../Common/FileStreams.cpp

StreamUtils.o: ../../Common/StreamUtils.cpp
	$(CXX) $(CFLAGS) ../../Common/StreamUtils.cpp

C_FileIO.o: ../../../Common/C_FileIO.cpp
	$(CXX) $(CFLAGS) ../../../Common/C_FileIO.cpp

CommandLineParser.o: ../../../Common/CommandLineParser.cpp
	$(CXX) $(CFLAGS) ../../../Common/CommandLineParser.cpp

MyWindows.o: ../../../Common/MyWindows.cpp
	$(CXX) $(CFLAGS) ../../../Common/MyWindows.cpp

MyString.o: ../../../Common/MyString.cpp
	$(CXX) $(CFLAGS) ../../../Common/MyString.cpp

StringConvert.o: ../../../Common/StringConvert.cpp
	$(CXX) $(CFLAGS) ../../../Common/StringConvert.cpp

StringToInt.o: ../../../Common/StringToInt.cpp
	$(CXX) $(CFLAGS) ../../../Common/StringToInt.cpp

MyVector.o: ../../../Common/MyVector.cpp
	$(CXX) $(CFLAGS) ../../../Common/MyVector.cpp

clean:
	-$(RM) $(PROG) $(OBJS) testfile* *.exe ir.out
	-$(RM) -fr SunWS_cache


test: $(PROG)
	cp PPMDAlone.cpp testfile
	./$(PROG) e testfile testfile.ppmd
	./$(PROG) d testfile.ppmd testfile.ppmd.d 
	diff -s testfile testfile.ppmd.d
	./$(PROG) e -so testfile > testfile.ppmd.2
	./$(PROG) d -si -so < testfile.ppmd.2 > testfile.ppmd.2.d
	diff -s testfile testfile.ppmd.2.d
	./$(PROG) e -o32 -m256 testfile testfile.ppmd.3
	./$(PROG) d testfile.ppmd.3 testfile.ppmd.3.d 
	diff -s testfile testfile.ppmd.3.d
	@echo =========
	@echo All Done 
	@echo =========


