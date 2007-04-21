
include ../../../../makefile.glb

ALLFLAGS=-fprofile-arcs -ftest-coverage \
    	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-DNDEBUG -D_REENTRANT -DENV_UNIX \
  -DWIN_LONG_PATH \
  -DCOMPRESS_MT \
  -DCOMPRESS_BZIP2_MT \
  -DCOMPRESS_MF_MT \
  -D_7ZIP_LARGE_PAGES \
  -DBREAK_HANDLER \
  -DBENCH_MT \
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
	-I../../Compress/LZMA_Alone \
	-I../../../../C/Compress/Branch \
-I../../../myWindows \
-I../../../ \
-I../../../include_windows

CXX=g++ $(ALLFLAGS)
CC=gcc $(ALLFLAGS)

SRCS=\
 ../../../myWindows/myGetTickCount.cpp \
 ../../../myWindows/wine_date_and_time.cpp \
 ../../../myWindows/myAddExeFlag.cpp \
 ../../../myWindows/myGetNumberOfProcessors.cpp \
 ../../../myWindows/mySplitCommandLine.cpp \
 ../../UI/Console/ConsoleClose.cpp \
 ../../UI/Console/ExtractCallbackConsole.cpp \
 ../../UI/Console/List.cpp \
 ../../UI/Console/Main.cpp \
 ../../UI/Console/MainAr.cpp \
 ../../UI/Console/OpenCallbackConsole.cpp \
 ../../UI/Console/PercentPrinter.cpp \
 ../../UI/Console/UpdateCallbackConsole.cpp \
 ../../UI/Console/UserInputUtils.cpp \
 ../../../Windows/Error.cpp \
 ../../../Windows/FileDir.cpp \
 ../../../Windows/FileFind.cpp \
 ../../../Windows/FileIO.cpp \
 ../../../Windows/FileName.cpp \
 ../../../Windows/PropVariant.cpp \
 ../../../Windows/PropVariantConversions.cpp \
 ../../../Windows/Synchronization.cpp \
 ../../Common/CreateCoder.cpp \
 ../../Archive/Common/CrossThreadProgress.cpp \
 ../../Compress/Branch/ARM.cpp \
 ../../Compress/Branch/ARMThumb.cpp \
 ../../Compress/Branch/BCJ2Register.cpp \
 ../../Compress/Branch/BCJRegister.cpp \
 ../../Compress/Branch/BranchCoder.cpp \
 ../../Compress/Branch/BranchRegister.cpp \
 ../../Compress/Branch/IA64.cpp \
 ../../Compress/Branch/PPC.cpp \
 ../../Compress/Branch/SPARC.cpp \
 ../../Compress/Branch/x86.cpp \
 ../../Compress/Branch/x86_2.cpp \
 ../../Compress/BZip2/BZip2CRC.cpp \
 ../../Compress/BZip2/BZip2Decoder.cpp \
 ../../Compress/BZip2/BZip2Encoder.cpp \
 ../../Compress/BZip2/BZip2Register.cpp \
 ../../Compress/Copy/CopyCoder.cpp \
 ../../Compress/Copy/CopyRegister.cpp \
 ../../Compress/Deflate/Deflate64Register.cpp \
 ../../Compress/Deflate/DeflateDecoder.cpp \
 ../../Compress/Deflate/DeflateEncoder.cpp \
 ../../Compress/Deflate/DeflateRegister.cpp \
 ../../Compress/Implode/ImplodeDecoder.cpp \
 ../../Compress/Implode/ImplodeHuffmanDecoder.cpp \
 ../../Compress/LZ/LZOutWindow.cpp \
 ../../Compress/LZMA/LZMADecoder.cpp \
 ../../Compress/LZMA/LZMAEncoder.cpp \
 ../../Compress/LZMA/LZMARegister.cpp \
 ../../Compress/PPMD/PPMDDecoder.cpp \
 ../../Compress/PPMD/PPMDEncoder.cpp \
 ../../Compress/PPMD/PPMDRegister.cpp \
 ../../Compress/RangeCoder/RangeCoderBit.cpp \
 ../../Compress/Shrink/ShrinkDecoder.cpp \
 ../../Compress/Z/ZDecoder.cpp \
 ../../Compress/BWT/BlockSort.cpp \
 ../../Compress/Lzx/Lzx86Converter.cpp \
 ../../Compress/Lzx/LzxDecoder.cpp \
 ../../Compress/Quantum/QuantumDecoder.cpp \
 ../../Compress/LZMA_Alone/LzmaBench.cpp \
 ../../Compress/LZMA_Alone/LzmaBenchCon.cpp \
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
 ../../Archive/7z/7zOut.cpp \
 ../../Archive/7z/7zProperties.cpp \
 ../../Archive/7z/7zRegister.cpp \
 ../../Archive/7z/7zSpecStream.cpp \
 ../../Archive/7z/7zUpdate.cpp \
 ../../Archive/BZip2/bz2Register.cpp \
 ../../Archive/BZip2/BZip2Handler.cpp \
 ../../Archive/BZip2/BZip2HandlerOut.cpp \
 ../../Archive/BZip2/BZip2Update.cpp \
 ../../Archive/GZip/GZipHandler.cpp \
 ../../Archive/GZip/GZipHandlerOut.cpp \
 ../../Archive/GZip/GZipHeader.cpp \
 ../../Archive/GZip/GZipIn.cpp \
 ../../Archive/GZip/GZipOut.cpp \
 ../../Archive/GZip/GZipRegister.cpp \
 ../../Archive/GZip/GZipUpdate.cpp \
 ../../Archive/Tar/TarHandler.cpp \
 ../../Archive/Tar/TarHandlerOut.cpp \
 ../../Archive/Tar/TarHeader.cpp \
 ../../Archive/Tar/TarIn.cpp \
 ../../Archive/Tar/TarOut.cpp \
 ../../Archive/Tar/TarRegister.cpp \
 ../../Archive/Tar/TarUpdate.cpp \
 ../../Archive/Zip/ZipAddCommon.cpp \
 ../../Archive/Zip/ZipHandler.cpp \
 ../../Archive/Zip/ZipHandlerOut.cpp \
 ../../Archive/Zip/ZipHeader.cpp \
 ../../Archive/Zip/ZipIn.cpp \
 ../../Archive/Zip/ZipItem.cpp \
 ../../Archive/Zip/ZipOut.cpp \
 ../../Archive/Zip/ZipRegister.cpp \
 ../../Archive/Zip/ZipUpdate.cpp \
 ../../Archive/Common/CoderMixer2.cpp \
 ../../Archive/Common/CoderMixer2MT.cpp \
 ../../Archive/Common/CoderMixer2ST.cpp \
 ../../Archive/Common/DummyOutStream.cpp \
 ../../Archive/Common/InStreamWithCRC.cpp \
 ../../Archive/Common/ItemNameUtils.cpp \
 ../../Archive/Common/MultiStream.cpp \
 ../../Archive/Common/OutStreamWithCRC.cpp \
 ../../Archive/Common/ParseProperties.cpp \
 ../../Archive/Split/SplitHandler.cpp \
 ../../Archive/Split/SplitRegister.cpp \
 ../../Archive/Z/ZHandler.cpp \
 ../../Archive/Z/ZRegister.cpp \
 ../../Archive/Cab/CabBlockInStream.cpp \
 ../../Archive/Cab/CabHandler.cpp \
 ../../Archive/Cab/CabHeader.cpp \
 ../../Archive/Cab/CabIn.cpp \
 ../../Archive/Cab/CabRegister.cpp \
 ../../UI/Common/ArchiveCommandLine.cpp \
 ../../UI/Common/ArchiveExtractCallback.cpp \
 ../../UI/Common/ArchiveOpenCallback.cpp \
 ../../UI/Common/DefaultName.cpp \
 ../../UI/Common/EnumDirItems.cpp \
 ../../UI/Common/Extract.cpp \
 ../../UI/Common/ExtractingFilePath.cpp \
 ../../UI/Common/LoadCodecs.cpp \
 ../../UI/Common/OpenArchive.cpp \
 ../../UI/Common/PropIDUtils.cpp \
 ../../UI/Common/SetProperties.cpp \
 ../../UI/Common/SortUtils.cpp \
 ../../UI/Common/TempFiles.cpp \
 ../../UI/Common/Update.cpp \
 ../../UI/Common/UpdateAction.cpp \
 ../../UI/Common/UpdateCallback.cpp \
 ../../UI/Common/UpdatePair.cpp \
 ../../UI/Common/UpdateProduce.cpp \
 ../../Crypto/Zip/ZipCipher.cpp \
 ../../Crypto/Zip/ZipCrypto.cpp \
 ../../Crypto/AES/MyAES.cpp \
 ../../Crypto/7zAES/7zAES.cpp \
 ../../Crypto/7zAES/7zAESRegister.cpp \
 ../../Crypto/WzAES/WzAES.cpp \
 ../../Crypto/Hash/HmacSha1.cpp \
 ../../Crypto/Hash/Pbkdf2HmacSha1.cpp \
 ../../Crypto/Hash/RandGen.cpp \
 ../../Crypto/Hash/Sha1.cpp \
 ../../Crypto/Hash/Sha256.cpp \
 ../../Common/FilePathAutoRename.cpp \
 ../../Common/FileStreams.cpp \
 ../../Common/FilterCoder.cpp \
 ../../Common/InBuffer.cpp \
 ../../Common/InOutTempBuffer.cpp \
 ../../Common/LimitedStreams.cpp \
 ../../Common/LockedStream.cpp \
 ../../Common/LSBFDecoder.cpp \
 ../../Common/LSBFEncoder.cpp \
 ../../Common/MemBlocks.cpp \
 ../../Common/MethodId.cpp \
 ../../Common/OffsetStream.cpp \
 ../../Common/OutBuffer.cpp \
 ../../Common/OutMemStream.cpp \
 ../../Common/ProgressMt.cpp \
 ../../Common/ProgressUtils.cpp \
 ../../Common/StreamBinder.cpp \
 ../../Common/StreamObjects.cpp \
 ../../Common/StreamUtils.cpp \
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
 ../../Crypto/AES/aeskey.c \
 ../../Crypto/AES/aestab.c \
 ../../../../C/Compress/Lz/MatchFinder.c \
 ../../../../C/Compress/Lz/MatchFinderMt.c \
 ../../../../C/Compress/Huffman/HuffmanEncode.c \
 ../../../../C/Compress/Branch/BranchARM.c \
 ../../../../C/Compress/Branch/BranchARMThumb.c \
 ../../../../C/Compress/Branch/BranchIA64.c \
 ../../../../C/Compress/Branch/BranchPPC.c \
 ../../../../C/Compress/Branch/BranchSPARC.c \
 ../../../../C/Compress/Branch/BranchX86.c \
 ../../../../C/7zCrc.c \
 ../../../../C/Alloc.c \
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

