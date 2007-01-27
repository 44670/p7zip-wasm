
DEST_BIN=/usr/local/bin
DEST_SHARE=/usr/local/lib/p7zip
DEST_MAN=/usr/local/man

.PHONY: default all all2 7za sfx 7z 7zr Client7z common clean tar_bin depend test test_7z test_7zr test_Client7z all_test

default:7za

all:7za sfx

all2: 7za sfx 7z

all3: 7za sfx 7z 7zr

all_test : test test_7z test_7zr test_Client7z

common:
	mkdir -p  bin
	cd CPP/Common       ; $(MAKE) all
	cd CPP/myWindows    ; $(MAKE) all

7za: common
	cd CPP/7zip/Bundles/Alone ; $(MAKE) all

7zr: common
	cd CPP/7zip/Bundles/Alone7z ; $(MAKE) all

Client7z: common
	mkdir -p  bin/Formats
	cd CPP/7zip/Bundles/Format7z ; $(MAKE) all
	cd CPP/7zip/UI/Client7z      ; $(MAKE) all

depend:
	cd CPP/Common                 ; $(MAKE) depend
	cd CPP/myWindows              ; $(MAKE) depend
	cd CPP/7zip/Bundles/Alone     ; $(MAKE) depend
	cd CPP/7zip/Bundles/SFXCon    ; $(MAKE) depend
	cd CPP/7zip/UI/Console        ; $(MAKE) depend
	cd CPP/7zip/UI/Client7z       ; $(MAKE) depend
	cd CPP/7zip/Archive/7z        ; $(MAKE) depend
	cd CPP/7zip/Archive/Arj       ; $(MAKE) depend
	cd CPP/7zip/Archive/BZip2     ; $(MAKE) depend
	cd CPP/7zip/Archive/Cab       ; $(MAKE) depend
	cd CPP/7zip/Archive/Chm       ; $(MAKE) depend
	cd CPP/7zip/Archive/Cpio      ; $(MAKE) depend
	cd CPP/7zip/Archive/Deb       ; $(MAKE) depend
	cd CPP/7zip/Archive/GZip      ; $(MAKE) depend
	cd CPP/7zip/Archive/Iso       ; $(MAKE) depend
	cd CPP/7zip/Archive/Lzh       ; $(MAKE) depend
	cd CPP/7zip/Archive/Nsis      ; $(MAKE) depend
	cd CPP/7zip/Archive/Rar       ; $(MAKE) depend
	cd CPP/7zip/Archive/RPM       ; $(MAKE) depend
	cd CPP/7zip/Archive/Split     ; $(MAKE) depend
	cd CPP/7zip/Archive/Tar       ; $(MAKE) depend
	cd CPP/7zip/Archive/Z         ; $(MAKE) depend
	cd CPP/7zip/Archive/Zip       ; $(MAKE) depend
	cd CPP/7zip/Compress/Branch   ; $(MAKE) depend
	cd CPP/7zip/Compress/ByteSwap ; $(MAKE) depend
	cd CPP/7zip/Compress/BZip2    ; $(MAKE) depend
	cd CPP/7zip/Compress/Copy     ; $(MAKE) depend
	cd CPP/7zip/Compress/Deflate  ; $(MAKE) depend
	cd CPP/7zip/Compress/LZMA     ; $(MAKE) depend
	cd CPP/7zip/Compress/PPMD     ; $(MAKE) depend
	cd CPP/7zip/Compress/Rar      ; $(MAKE) depend
	cd CPP/7zip/Crypto/7zAES      ; $(MAKE) depend
	cd CPP/7zip/Crypto/AES        ; $(MAKE) depend
	cd CPP/7zip/Bundles/Alone7z   ; $(MAKE) depend
	cd CPP/7zip/Bundles/Format7z  ; $(MAKE) depend

sfx: common
	mkdir -p  bin
	cd CPP/7zip/Bundles/SFXCon ; $(MAKE) all

7z: common
	mkdir -p  bin/Codecs bin/Formats
	cd CPP/7zip/UI/Console        ; $(MAKE) all
	cd CPP/7zip/Archive/7z        ; $(MAKE) all
	cd CPP/7zip/Archive/Arj       ; $(MAKE) all
	cd CPP/7zip/Archive/BZip2     ; $(MAKE) all
	cd CPP/7zip/Archive/Cab       ; $(MAKE) all
	cd CPP/7zip/Archive/Chm       ; $(MAKE) all
	cd CPP/7zip/Archive/Cpio      ; $(MAKE) all
	cd CPP/7zip/Archive/Deb       ; $(MAKE) all
	cd CPP/7zip/Archive/GZip      ; $(MAKE) all
	cd CPP/7zip/Archive/Iso       ; $(MAKE) all
	cd CPP/7zip/Archive/Lzh       ; $(MAKE) all
	cd CPP/7zip/Archive/Nsis      ; $(MAKE) all
	cd CPP/7zip/Archive/Rar       ; $(MAKE) all
	cd CPP/7zip/Archive/RPM       ; $(MAKE) all
	cd CPP/7zip/Archive/Split     ; $(MAKE) all
	cd CPP/7zip/Archive/Tar       ; $(MAKE) all
	cd CPP/7zip/Archive/Z         ; $(MAKE) all
	cd CPP/7zip/Archive/Zip       ; $(MAKE) all
	cd CPP/7zip/Compress/Branch   ; $(MAKE) all
	cd CPP/7zip/Compress/ByteSwap ; $(MAKE) all
	cd CPP/7zip/Compress/BZip2    ; $(MAKE) all
	cd CPP/7zip/Compress/Copy     ; $(MAKE) all
	cd CPP/7zip/Compress/Deflate  ; $(MAKE) all
	cd CPP/7zip/Compress/LZMA     ; $(MAKE) all
	cd CPP/7zip/Compress/PPMD     ; $(MAKE) all
	cd CPP/7zip/Compress/Rar      ; $(MAKE) all
	cd CPP/7zip/Crypto/7zAES      ; $(MAKE) all
	cd CPP/7zip/Crypto/AES        ; $(MAKE) all

clean:
	cd CPP/Common                 ; $(MAKE) clean
	cd CPP/myWindows              ; $(MAKE) clean
	cd CPP/7zip/Bundles/Alone     ; $(MAKE) clean
	cd CPP/7zip/Bundles/SFXCon    ; $(MAKE) clean
	cd CPP/7zip/UI/Console        ; $(MAKE) clean
	cd CPP/7zip/UI/Client7z        ; $(MAKE) clean
	cd CPP/7zip/Archive/7z        ; $(MAKE) clean
	cd CPP/7zip/Archive/Arj       ; $(MAKE) clean
	cd CPP/7zip/Archive/BZip2     ; $(MAKE) clean
	cd CPP/7zip/Archive/Cab       ; $(MAKE) clean
	cd CPP/7zip/Archive/Chm       ; $(MAKE) clean
	cd CPP/7zip/Archive/Cpio      ; $(MAKE) clean
	cd CPP/7zip/Archive/Deb       ; $(MAKE) clean
	cd CPP/7zip/Archive/GZip      ; $(MAKE) clean
	cd CPP/7zip/Archive/Iso       ; $(MAKE) clean
	cd CPP/7zip/Archive/Lzh       ; $(MAKE) clean
	cd CPP/7zip/Archive/Nsis      ; $(MAKE) clean
	cd CPP/7zip/Archive/Rar       ; $(MAKE) clean
	cd CPP/7zip/Archive/RPM       ; $(MAKE) clean
	cd CPP/7zip/Archive/Split     ; $(MAKE) clean
	cd CPP/7zip/Archive/Tar       ; $(MAKE) clean
	cd CPP/7zip/Archive/Z         ; $(MAKE) clean
	cd CPP/7zip/Archive/Zip       ; $(MAKE) clean
	cd CPP/7zip/Compress/Branch   ; $(MAKE) clean
	cd CPP/7zip/Compress/ByteSwap ; $(MAKE) clean
	cd CPP/7zip/Compress/BZip2    ; $(MAKE) clean
	cd CPP/7zip/Compress/Copy     ; $(MAKE) clean
	cd CPP/7zip/Compress/Deflate  ; $(MAKE) clean
	cd CPP/7zip/Compress/LZMA     ; $(MAKE) clean
	cd CPP/7zip/Compress/PPMD     ; $(MAKE) clean
	cd CPP/7zip/Compress/Rar      ; $(MAKE) clean
	cd CPP/7zip/Crypto/7zAES      ; $(MAKE) clean
	cd CPP/7zip/Crypto/AES        ; $(MAKE) clean
	cd CPP/7zip/Bundles/Alone7z   ; $(MAKE) clean
	cd CPP/7zip/Bundles/Format7z  ; $(MAKE) clean
	rm -fr bin
	rm -f make.log
	find . -name "*~" -exec rm -f {} \;
	find . -name "*.orig" -exec rm -f {} \;
	find . -name ".*.swp" -exec rm -f {} \;
	find . -name "*.[ch]" -exec chmod -x {} \;
	find . -name "*.cpp" -exec chmod -x {} \;
	find . -name "makefile*" -exec chmod -x {} \;
	chmod +x contrib/VirtualFileSystemForMidnightCommander/u7z
	chmod +x contrib/gzip-like_CLI_wrapper_for_7z/p7zip
	chmod +x install.sh check/check.sh check/clean_all.sh check/check_7zr.sh 
	cd check                  ; ./clean_all.sh

test: all
	cd check ; ./check.sh ../bin/7za

test_7z: all2
	cd check ; ./check.sh ../bin/7z

test_7zr: 7zr
	cd check ; ./check_7zr.sh ../bin/7zr

test_Client7z: Client7z
	cd check ; ./check_Client7z.sh ../bin/Client7z

install:
	./install.sh $(DEST_BIN) $(DEST_SHARE) $(DEST_MAN)

REP=$(shell pwd)
ARCHIVE=$(shell basename $(REP))

.PHONY: tar_all tar_all2 src_7z tar_bin tar_bin2

tar_all : clean
	rm -f  ../$(ARCHIVE)_src_all.tar.bz2
	cp makefile.linux_x86_ppc_alpha makefile.machine
	cd .. ; (tar cf - $(ARCHIVE) | bzip2 -9 > $(ARCHIVE)_src_all.tar.bz2)

tar_all2 : clean
	rm -f  ../$(ARCHIVE)_src_all.tar.bz2
	cp makefile.linux_x86_ppc_alpha makefile.machine
	cd .. ; (tar cf - $(ARCHIVE) | 7za a -mx=9 -tbzip2 -si $(ARCHIVE)_src_all.tar.bz2 )

src_7z : clean
	rm -f  ../$(ARCHIVE)_src.7z
	cd .. ; 7za a -mx=9 -m0=ppmd:mem=128m:o=32 $(ARCHIVE)_src.7z $(ARCHIVE)

tar_bin:
	rm -f  ../$(ARCHIVE)_x86_linux_bin.tar.bz2
	chmod +x install.sh contrib/VirtualFileSystemForMidnightCommander/u7z contrib/gzip-like_CLI_wrapper_for_7z/p7zip
	cd .. ; (tar cf - $(ARCHIVE)/bin $(ARCHIVE)/contrib $(ARCHIVE)/man1 $(ARCHIVE)/install.sh $(ARCHIVE)/ChangeLog $(ARCHIVE)/DOCS $(ARCHIVE)/README $(ARCHIVE)/TODO | bzip2 -9 > $(ARCHIVE)_x86_linux_bin.tar.bz2)

tar_bin2:
	rm -f  ../$(ARCHIVE)_x86_linux_bin.tar.bz2
	chmod +x install.sh contrib/VirtualFileSystemForMidnightCommander/u7z contrib/gzip-like_CLI_wrapper_for_7z/p7zip
	cd .. ; (tar cf - $(ARCHIVE)/bin $(ARCHIVE)/contrib $(ARCHIVE)/man1 $(ARCHIVE)/install.sh $(ARCHIVE)/ChangeLog $(ARCHIVE)/DOCS $(ARCHIVE)/README $(ARCHIVE)/TODO | 7za a -mx=9 -tbzip2 -si $(ARCHIVE)_x86_linux_bin.tar.bz2)

