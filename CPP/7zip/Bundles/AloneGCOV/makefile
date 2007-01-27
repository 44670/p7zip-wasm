
ALLFLAGS=-fprofile-arcs -ftest-coverage \
    	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-DNDEBUG -D_REENTRANT -DENV_UNIX \
  -DEXCLUDE_COM \
  -DNO_REGISTRY \
  -DFORMAT_7Z \
  -DFORMAT_BZIP2 \
  -DFORMAT_CAB \
  -DFORMAT_GZIP \
  -DFORMAT_SPLIT \
  -DFORMAT_TAR \
  -DFORMAT_Z \
  -DFORMAT_ZIP \
  -DCOMPRESS_MT \
  -DCOMPRESS_BCJ_X86 \
  -DCOMPRESS_BCJ2 \
  -DCOMPRESS_BZIP2 \
  -DCOMPRESS_BZIP2_MT \
  -DCOMPRESS_COPY \
  -DCOMPRESS_DEFLATE \
  -DCOMPRESS_DEFLATE64 \
  -DCOMPRESS_IMPLODE \
  -DCOMPRESS_LZMA \
  -DCOMPRESS_MF_MT \
  -DCOMPRESS_PPMD \
  -DCRYPTO_7ZAES \
  -DCRYPTO_AES \
  -DCRYPTO_ZIP \
	-I../../Archive/7z -I../../Crypto/7zAES/ \
	-I../../Archive/BZip2 \
	-I../../Archive/Common \
	-I../../Archive/GZip \
	-I../../Archive/Cab \
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
	-I../../Compress/Lzx \
	-I../../Compress/LZ \
	-I../../Compress/LZ/MT \
	-I../../Compress/LZMA \
	-I../../Compress/PPMD \
	-I../../Compress/Quantum \
	-I../../Compress/RangeCoder \
	-I../../Compress/Shrink \
	-I../../Compress/Z \
	-I../../Crypto/AES \
	-I../../Crypto/Zip \
	-I../../Crypto/Hash \
	-I../../Crypto/WzAES \
	-I../../UI/Console \
	-I../../UI/Common \
	-I../../../Windows \
	-I../../../Common \
	-I../../../7zip/Common \
	-I../../../../C \
	-I../../../../C/Compress/Huffman \
	-I../../../../C/Compress/Lz \
-I../../../myWindows \
-I../../../ \
-I../../../include_windows

CXX=g++ $(ALLFLAGS)
CC=gcc $(ALLFLAGS)

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
 ../../Common/MemBlocks.cpp \
 ../../Common/OutMemStream.cpp \
 ../../Common/ProgressMt.cpp \
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
 ../../Compress/Implode/ImplodeDecoder.cpp \
 ../../Compress/Implode/ImplodeHuffmanDecoder.cpp \
 ../../Compress/LZ/LZOutWindow.cpp \
 ../../Compress/LZMA/LZMADecoder.cpp \
 ../../Compress/LZMA/LZMAEncoder.cpp \
 ../../Compress/PPMD/PPMDDecoder.cpp \
 ../../Compress/PPMD/PPMDEncoder.cpp \
 ../../Compress/RangeCoder/RangeCoderBit.cpp \
 ../../Compress/Shrink/ShrinkDecoder.cpp \
 ../../Compress/Z/ZDecoder.cpp \
 ../../Crypto/7zAES/7zAES.cpp \
 ../../Crypto/AES/MyAES.cpp \
 ../../Crypto/Hash/Pbkdf2HmacSha1.cpp \
 ../../Crypto/Hash/Sha256.cpp \
 ../../Crypto/Hash/HmacSha1.cpp \
 ../../Crypto/Hash/Sha1.cpp \
 ../../Crypto/Hash/RandGen.cpp \
 ../../Crypto/WzAES/WzAES.cpp \
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
 ../../Archive/Cab/CabHandler.cpp \
 ../../Archive/Cab/CabHeader.cpp \
 ../../Archive/Cab/CabIn.cpp \
 ../../Archive/Cab/CabBlockInStream.cpp \
 ../../Compress/Lzx/LzxDecoder.cpp \
 ../../Compress/Lzx/Lzx86Converter.cpp \
 ../../Compress/Quantum/QuantumDecoder.cpp \
../../../myWindows/myGetTickCount.cpp \
../../../myWindows/wine_date_and_time.cpp \
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
../../../Common/Vector.cpp \
../../../Common/StringConvert.cpp \
../../../Common/String.cpp \
../../../Common/CRC.cpp \
../../../Common/IntToString.cpp \
../../../Common/StringToInt.cpp \
../../../Common/UTFConvert.cpp

SRCS_C=\
../../Crypto/AES/aescrypt.c \
../../Crypto/AES/aestab.c \
../../Crypto/AES/aeskey.c \
../../../../C/Compress/Huffman/HuffmanEncode.c \
../../../../C/Compress/Lz/MatchFinder.c \
../../../../C/Compress/Lz/MatchFinderMt.c \
../../../../C/7zCrc.c \
../../../../C/Sort.c \
../../../../C/Threads.c

all : copy 7za.exe
	
7za.exe:
	$(CXX) -c *.cpp 
	$(CC)  -c *.c
	$(CXX) *.o -o 7za.exe -lpthread
copy:
	rm -f *.cpp *.c
	cp -p $(SRCS) $(SRCS_C) .

clean:
	rm -f 7za.exe *.cpp *.c *.gcov *.gcda *.gcno *.o core

