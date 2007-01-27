
CXX=g++ \
-fprofile-arcs -ftest-coverage \
    	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-DNDEBUG -D_REENTRANT -DENV_UNIX \
	-DEXCLUDE_COM \
	-DNO_REGISTRY \
	-D_NO_CRYPTO \
	-DFORMAT_7Z \
	-DCOMPRESS_MT \
	-DCOMPRESS_BCJ_X86 \
	-DCOMPRESS_BCJ2 \
	-DCOMPRESS_COPY \
	-DCOMPRESS_LZMA \
	-DCOMPRESS_MF_MT \
	-I../../Archive/7z \
	-I../../Archive/Common \
	-I../../Compress/BZip2 \
	-I../../Compress/BWT \
	-I../../Compress/Branch \
	-I../../Compress/Copy \
	-I../../Compress/Deflate \
	-I../../Compress/Huffman \
	-I../../Compress/LZ \
	-I../../Compress/LZ/MT \
	-I../../Compress/LZMA \
	-I../../Compress/PPMD \
	-I../../Compress/RangeCoder \
	-I../../UI/Console \
	-I../../UI/Common \
	-I../../../Windows \
	-I../../../Common \
	-I../../../7zip/Common \
-I../../../myWindows \
-I../../../ \
-I../../../include_windows

SRCS=\
 ../../UI/Console/ConsoleClose.cpp \
 ../../UI/Console/ExtractCallbackConsole.cpp \
 ../../UI/Console/List.cpp \
 ../../UI/Console/Main.cpp \
 ../../UI/Console/MainAr.cpp \
 ../../UI/Console/OpenCallbackConsole.cpp \
 ../../UI/Console/PercentPrinter.cpp \
 ../../UI/Console/UpdateCallbackConsole.cpp \
 ../../UI/Console/UserInputUtils.cpp \
 ../../Archive/7z/7zCompressionMode.cpp \
 ../../Archive/7z/7zDecode.cpp \
 ../../Archive/7z/7zEncode.cpp \
 ../../Archive/7z/7zExtract.cpp \
 ../../Archive/7z/7zFolderInStream.cpp \
 ../../Archive/7z/7zFolderOutStream.cpp \
 ../../Archive/7z/7zHandler.cpp \
 ../../Archive/7z/7zHandlerOut.cpp \
 ../../Archive/7z/7zHeader.cpp \
 ../../Archive/7z/7zIn.cpp \
 ../../Archive/7z/7zMethodID.cpp \
 ../../Archive/7z/7zOut.cpp \
 ../../Archive/7z/7zProperties.cpp \
 ../../Archive/7z/7zSpecStream.cpp \
 ../../Archive/7z/7zUpdate.cpp \
 ../../Archive/Common/CoderMixer2.cpp \
 ../../Archive/Common/CoderMixer2MT.cpp \
 ../../Archive/Common/CrossThreadProgress.cpp \
 ../../Archive/Common/FilterCoder.cpp \
 ../../Archive/Common/InStreamWithCRC.cpp \
 ../../Archive/Common/ItemNameUtils.cpp \
 ../../Archive/Common/MultiStream.cpp \
 ../../Archive/Common/OutStreamWithCRC.cpp \
 ../../Archive/Common/ParseProperties.cpp \
 ../../Common/FilePathAutoRename.cpp \
 ../../Common/FileStreams.cpp \
 ../../Common/InBuffer.cpp \
 ../../Common/InOutTempBuffer.cpp \
 ../../Common/LimitedStreams.cpp \
 ../../Common/LockedStream.cpp \
 ../../Common/StreamUtils.cpp \
 ../../Common/OutBuffer.cpp \
 ../../Common/ProgressUtils.cpp \
 ../../Common/StreamBinder.cpp \
 ../../Common/StreamObjects.cpp \
 ../../Compress/Branch/BranchCoder.cpp \
 ../../Compress/Branch/x86_2.cpp \
 ../../Compress/Branch/x86.cpp \
 ../../Compress/Copy/CopyCoder.cpp \
 ../../Compress/LZ/LZInWindow.cpp \
 ../../Compress/LZ/LZOutWindow.cpp \
 ../../Compress/LZMA/LZMADecoder.cpp \
 ../../Compress/LZMA/LZMAEncoder.cpp \
 ../../Compress/LZ/MT/MT.cpp \
 ../../Compress/RangeCoder/RangeCoderBit.cpp \
 ../../UI/Common/ArchiveCommandLine.cpp \
 ../../UI/Common/ArchiveExtractCallback.cpp \
 ../../UI/Common/ArchiveOpenCallback.cpp \
 ../../UI/Common/ArchiverInfo.cpp \
 ../../UI/Common/DefaultName.cpp \
 ../../UI/Common/EnumDirItems.cpp \
 ../../UI/Common/Extract.cpp \
 ../../UI/Common/ExtractingFilePath.cpp \
 ../../UI/Common/OpenArchive.cpp \
 ../../UI/Common/PropIDUtils.cpp \
 ../../UI/Common/SetProperties.cpp \
 ../../UI/Common/SortUtils.cpp \
 ../../UI/Common/TempFiles.cpp \
 ../../UI/Common/UpdateAction.cpp \
 ../../UI/Common/UpdateCallback.cpp \
 ../../UI/Common/Update.cpp \
 ../../UI/Common/UpdatePair.cpp \
 ../../UI/Common/UpdateProduce.cpp \
 ../../../Windows/Error.cpp \
 ../../../Windows/FileDir.cpp \
 ../../../Windows/FileFind.cpp \
 ../../../Windows/FileIO.cpp \
 ../../../Windows/FileName.cpp \
 ../../../Windows/PropVariantConversions.cpp \
 ../../../Windows/PropVariant.cpp \
 ../../../Windows/Synchronization.cpp \
../../../myWindows/wine_date_and_time.cpp \
../../../myWindows/myAddExeFlag.cpp \
../../../myWindows/myGetNumberOfProcessors.cpp \
../../../myWindows/mySplitCommandLine.cpp \
../../../Common/Alloc.cpp \
../../../Common/CommandLineParser.cpp \
../../../Common/CRC.cpp \
../../../Common/IntToString.cpp \
../../../Common/ListFileUtils.cpp \
../../../Common/MyWindows.cpp \
../../../Common/StdInStream.cpp \
../../../Common/StdOutStream.cpp \
../../../Common/String.cpp \
../../../Common/StringConvert.cpp \
../../../Common/StringToInt.cpp \
../../../Common/Vector.cpp \
../../../Common/Wildcard.cpp

all : copy 7zr.exe
	
7zr.exe:
	$(CXX) *.cpp -o 7zr.exe -lpthread
copy:
	rm -f *.cpp *.c
	cp -p $(SRCS) $(SRCS_C) .

clean:
	rm -f 7zr.exe *.cpp *.c *.gcov *.gcda *.gcno
	rm -fr 7za433_7zip_lzma 7za433_7zip_lzma*.7z

test :
	rm -fr 7za433_7zip_lzma 7za433_7zip_lzma*.7z
	./7zr.exe t ../../../check/test/7za433_7zip_lzma.7z
	./7zr.exe x ../../../check/test/7za433_7zip_lzma.7z
	./7zr.exe a -mx=1 7za433_7zip_lzma_1.7z 7za433_7zip_lzma
	./7zr.exe a -mx=2 7za433_7zip_lzma_2.7z 7za433_7zip_lzma
	./7zr.exe a -mx=3 7za433_7zip_lzma_3.7z 7za433_7zip_lzma
	./7zr.exe a -mx=4 7za433_7zip_lzma_4.7z 7za433_7zip_lzma
	./7zr.exe a -mx=5 -ms=on 7za433_7zip_lzma_5.7z 7za433_7zip_lzma
	./7zr.exe a -mx=6 7za433_7zip_lzma_6.7z 7za433_7zip_lzma
	./7zr.exe a -mx=7 7za433_7zip_lzma_7.7z 7za433_7zip_lzma
	./7zr.exe a -mx=8 7za433_7zip_lzma_8.7z 7za433_7zip_lzma
	./7zr.exe a -mx=9 7za433_7zip_lzma_9.7z 7za433_7zip_lzma
	./7zr.exe a -ms=off 7za433_7zip_lzma.7z 7za433_7zip_lzma
	echo "dummy" > 7za433_7zip_lzma/dummy
	./7zr.exe u -ms=off 7za433_7zip_lzma.7z 7za433_7zip_lzma
	./7zr.exe d 7za433_7zip_lzma.7z 7za433_7zip_lzma/dummy
	./7zr.exe a -ms=off -mmt=on -mmf=HC4 7za433_7zip_lzma_mt_hc4.7z 7za433_7zip_lzma
	./7zr.exe a -ms=off -mmt=on -mmf=BT2 7za433_7zip_lzma_mt_bt2.7z 7za433_7zip_lzma
	./7zr.exe a -ms=off -mmt=on -mmf=BT3 7za433_7zip_lzma_mt_bt3.7z 7za433_7zip_lzma
	./7zr.exe a -ms=off -mmt=on -mmf=BT4 7za433_7zip_lzma_mt_bt4.7z 7za433_7zip_lzma

gcov:
	ls -1 *.cpp | xargs gcov


