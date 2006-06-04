
CXX=g++ \
-fprofile-arcs -ftest-coverage \
    	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-DNDEBUG -D_REENTRANT -DENV_UNIX \
-DCOMPRESS_BZIP2_MT \
-DCOMPRESS_MT \
-DEXCLUDE_COM -DNO_REGISTRY -DFORMAT_7Z -DFORMAT_BZIP2 -DFORMAT_ZIP \
-DFORMAT_TAR -DFORMAT_GZIP -DCOMPRESS_LZMA -DCOMPRESS_BCJ_X86 \
-DFORMAT_SPLIT -DFORMAT_Z \
-DCOMPRESS_BCJ2 -DCOMPRESS_COPY -DCOMPRESS_MF_MT -DCOMPRESS_PPMD \
-DCOMPRESS_DEFLATE -DCOMPRESS_DEFLATE64 -DCOMPRESS_IMPLODE -DCOMPRESS_BZIP2 \
-DCRYPTO_ZIP -DCRYPTO_7ZAES -DCRYPTO_AES \
	-I../../Archive/7z -I../../Crypto/7zAES/ \
	-I../../Archive/BZip2 \
	-I../../Archive/Common \
	-I../../Archive/GZip \
	-I../../Archive/Tar \
	-I../../Archive/Zip \
	-I../../Archive/Split \
	-I../../Archive/Z \
	-I../../Compress/BZip2 \
	-I../../Compress/BWT \
	-I../../Compress/Branch \
	-I../../Compress/Copy \
	-I../../Compress/Deflate \
	-I../../Compress/Huffman \
	-I../../Compress/Implode \
	-I../../Compress/LZ \
	-I../../Compress/LZ/MT \
	-I../../Compress/LZMA \
	-I../../Compress/PPMD \
	-I../../Compress/RangeCoder \
	-I../../Compress/Shrink \
	-I../../Compress/Z \
	-I../../Crypto/AES \
	-I../../Crypto/Zip \
	-I../../UI/Console \
	-I../../UI/Common \
	-I../../../Windows \
	-I../../../Common \
	-I../../../7zip/Common \
-I../../../myWindows \
-I../../../ \
-I../../../include_windows



SRCS=\
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
 ../../Archive/BZip2/BZip2Handler.cpp \
 ../../Archive/BZip2/BZip2HandlerOut.cpp \
 ../../Archive/BZip2/BZip2Update.cpp \
 ../../Archive/Common/CoderMixer2.cpp \
 ../../Archive/Common/CoderMixer2MT.cpp \
 ../../Archive/Common/CrossThreadProgress.cpp \
 ../../Archive/Common/DummyOutStream.cpp \
 ../../Archive/Common/FilterCoder.cpp \
 ../../Archive/Common/InStreamWithCRC.cpp \
 ../../Archive/Common/ItemNameUtils.cpp \
 ../../Archive/Common/MultiStream.cpp \
 ../../Archive/Common/OutStreamWithCRC.cpp \
 ../../Archive/Common/ParseProperties.cpp \
 ../../Archive/GZip/GZipHandler.cpp \
 ../../Archive/GZip/GZipHandlerOut.cpp \
 ../../Archive/GZip/GZipHeader.cpp \
 ../../Archive/GZip/GZipIn.cpp \
 ../../Archive/GZip/GZipOut.cpp \
 ../../Archive/GZip/GZipUpdate.cpp \
 ../../Archive/Split/SplitHandler.cpp \
 ../../Archive/Tar/TarHandler.cpp \
 ../../Archive/Tar/TarHandlerOut.cpp \
 ../../Archive/Tar/TarHeader.cpp \
 ../../Archive/Tar/TarIn.cpp \
 ../../Archive/Tar/TarOut.cpp \
 ../../Archive/Tar/TarUpdate.cpp \
 ../../Archive/Zip/ZipAddCommon.cpp \
 ../../Archive/Zip/ZipHandler.cpp \
 ../../Archive/Zip/ZipHandlerOut.cpp \
 ../../Archive/Zip/ZipHeader.cpp \
 ../../Archive/Zip/ZipIn.cpp \
 ../../Archive/Zip/ZipItem.cpp \
 ../../Archive/Zip/ZipOut.cpp \
 ../../Archive/Zip/ZipUpdate.cpp \
 ../../Archive/Z/ZHandler.cpp \
 ../../Common/FilePathAutoRename.cpp \
 ../../Common/FileStreams.cpp \
 ../../Common/InBuffer.cpp \
 ../../Common/InOutTempBuffer.cpp \
 ../../Common/LimitedStreams.cpp \
 ../../Common/LockedStream.cpp \
 ../../Common/LSBFDecoder.cpp \
 ../../Common/LSBFEncoder.cpp \
 ../../Common/OffsetStream.cpp \
 ../../Common/StreamUtils.cpp \
 ../../Common/OutBuffer.cpp \
 ../../Common/ProgressUtils.cpp \
 ../../Common/StreamBinder.cpp \
 ../../Common/StreamObjects.cpp \
 ../../Compress/Branch/BranchCoder.cpp \
 ../../Compress/Branch/x86_2.cpp \
 ../../Compress/Branch/x86.cpp \
 ../../Compress/BWT/BlockSort.cpp \
 ../../Compress/BZip2/BZip2CRC.cpp \
 ../../Compress/BZip2/BZip2Decoder.cpp \
 ../../Compress/BZip2/BZip2Encoder.cpp \
 ../../Compress/Copy/CopyCoder.cpp \
 ../../Compress/Deflate/DeflateDecoder.cpp \
 ../../Compress/Deflate/DeflateEncoder.cpp \
 ../../Compress/Huffman/HuffmanEncoder.cpp \
 ../../Compress/Implode/ImplodeDecoder.cpp \
 ../../Compress/Implode/ImplodeHuffmanDecoder.cpp \
 ../../Compress/LZ/LZInWindow.cpp \
 ../../Compress/LZ/LZOutWindow.cpp \
 ../../Compress/LZMA/LZMADecoder.cpp \
 ../../Compress/LZMA/LZMAEncoder.cpp \
 ../../Compress/LZ/MT/MT.cpp \
 ../../Compress/PPMD/PPMDDecoder.cpp \
 ../../Compress/PPMD/PPMDEncoder.cpp \
 ../../Compress/RangeCoder/RangeCoderBit.cpp \
 ../../Compress/Shrink/ShrinkDecoder.cpp \
 ../../Compress/Z/ZDecoder.cpp \
 ../../Crypto/7zAES/7zAES.cpp \
 ../../Crypto/7zAES/SHA256.cpp \
 ../../Crypto/AES/MyAES.cpp \
 ../../Crypto/Zip/ZipCipher.cpp \
 ../../Crypto/Zip/ZipCrypto.cpp \
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
 ../../UI/Console/ConsoleClose.cpp \
 ../../UI/Console/ExtractCallbackConsole.cpp \
 ../../UI/Console/List.cpp \
 ../../UI/Console/MainAr.cpp \
 ../../UI/Console/Main.cpp \
 ../../UI/Console/OpenCallbackConsole.cpp \
 ../../UI/Console/PercentPrinter.cpp \
 ../../UI/Console/UpdateCallbackConsole.cpp \
 ../../UI/Console/UserInputUtils.cpp \
 ../../../Windows/Error.cpp \
 ../../../Windows/FileDir.cpp \
 ../../../Windows/FileFind.cpp \
 ../../../Windows/FileIO.cpp \
 ../../../Windows/FileName.cpp \
 ../../../Windows/PropVariantConversions.cpp \
 ../../../Windows/PropVariant.cpp \
 ../../../Windows/Synchronization.cpp \
../../../myWindows/myGetTickCount.cpp \
../../../myWindows/wine_date_and_time.cpp \
../../../myWindows/myModuleFileName.cpp \
../../../myWindows/myAddExeFlag.cpp \
../../../myWindows/myGetNumberOfProcessors.cpp \
../../../myWindows/mySplitCommandLine.cpp \
../../../Common/Alloc.cpp \
../../../Common/MyWindows.cpp \
../../../Common/CommandLineParser.cpp \
../../../Common/ListFileUtils.cpp \
../../../Common/StdInStream.cpp \
../../../Common/StdOutStream.cpp \
../../../Common/Wildcard.cpp \
../../../Common/UTFConvert.cpp \
../../../Common/Vector.cpp \
../../../Common/StringConvert.cpp \
../../../Common/String.cpp \
../../../Common/CRC.cpp \
../../../Common/IntToString.cpp \
../../../Common/StringToInt.cpp \
../../../Common/Random.cpp

 SRCS_C=../../Crypto/AES/aescrypt.c \
 ../../Crypto/AES/aeskey.c \
 ../../Crypto/AES/aestab.c 

all : copy 7za.exe
	
7za.exe:
	$(CXX) *.cpp *.c -o 7za.exe -lpthread
copy:
	rm -f *.cpp *.c
	cp -p $(SRCS) $(SRCS_C) .
	cp MyAES.cpp_without_extern_C MyAES.cpp

clean:
	rm -f 7za.exe *.cpp *.c *.gcov *.gcda *.gcno

