
DEST_BIN=/usr/local/bin
DEST_SHARE=/usr/local/lib/p7zip
DEST_MAN=/usr/local/man

.PHONY: all all2 7za sfx 7z 7zr common clean tar_src tar_bin depend test test_7z test_7zr

all::7za

all2: 7za sfx 7z

all3: 7za sfx 7z 7zr

common:
	mkdir -p  bin
	cd Common       ; $(MAKE) all
	cd myWindows    ; $(MAKE) all

7za: common
	cd 7zip/Bundles/Alone ; $(MAKE) all

7zr: common
	cd 7zip/Bundles/Alone7z ; $(MAKE) all
depend:
	cd Common                 ; $(MAKE) depend
	cd myWindows              ; $(MAKE) depend
	cd 7zip/Bundles/Alone     ; $(MAKE) depend
	cd 7zip/Bundles/SFXCon    ; $(MAKE) depend
	cd 7zip/UI/Console        ; $(MAKE) depend
	cd 7zip/Archive/7z        ; $(MAKE) depend
	cd 7zip/Archive/Arj       ; $(MAKE) depend
	cd 7zip/Archive/BZip2     ; $(MAKE) depend
	cd 7zip/Archive/Cab       ; $(MAKE) depend
	cd 7zip/Archive/Chm       ; $(MAKE) depend
	cd 7zip/Archive/Cpio      ; $(MAKE) depend
	cd 7zip/Archive/Deb       ; $(MAKE) depend
	cd 7zip/Archive/GZip      ; $(MAKE) depend
	cd 7zip/Archive/Iso       ; $(MAKE) depend
	cd 7zip/Archive/Lzh       ; $(MAKE) depend
	cd 7zip/Archive/Nsis      ; $(MAKE) depend
	cd 7zip/Archive/Rar       ; $(MAKE) depend
	cd 7zip/Archive/RPM       ; $(MAKE) depend
	cd 7zip/Archive/Split     ; $(MAKE) depend
	cd 7zip/Archive/Tar       ; $(MAKE) depend
	cd 7zip/Archive/Z         ; $(MAKE) depend
	cd 7zip/Archive/Zip       ; $(MAKE) depend
	cd 7zip/Compress/Branch   ; $(MAKE) depend
	cd 7zip/Compress/ByteSwap ; $(MAKE) depend
	cd 7zip/Compress/BZip2    ; $(MAKE) depend
	cd 7zip/Compress/Copy     ; $(MAKE) depend
	cd 7zip/Compress/Deflate  ; $(MAKE) depend
	cd 7zip/Compress/Implode  ; $(MAKE) depend
	cd 7zip/Compress/LZMA     ; $(MAKE) depend
	cd 7zip/Compress/PPMD     ; $(MAKE) depend
	cd 7zip/Compress/Rar29    ; $(MAKE) depend
	cd 7zip/Crypto/7zAES      ; $(MAKE) depend
	cd 7zip/Crypto/AES        ; $(MAKE) depend
	cd 7zip/Bundles/Alone7z   ; $(MAKE) depend

sfx: common
	mkdir -p  bin
	cd 7zip/Bundles/SFXCon ; $(MAKE) all

7z: common
	mkdir -p  bin/Codecs bin/Formats
	cd 7zip/UI/Console        ; $(MAKE) all
	cd 7zip/Archive/7z        ; $(MAKE) all
	cd 7zip/Archive/Arj       ; $(MAKE) all
	cd 7zip/Archive/BZip2     ; $(MAKE) all
	cd 7zip/Archive/Cab       ; $(MAKE) all
	cd 7zip/Archive/Chm       ; $(MAKE) all
	cd 7zip/Archive/Cpio      ; $(MAKE) all
	cd 7zip/Archive/Deb       ; $(MAKE) all
	cd 7zip/Archive/GZip      ; $(MAKE) all
	cd 7zip/Archive/Iso       ; $(MAKE) all
	cd 7zip/Archive/Lzh       ; $(MAKE) all
	cd 7zip/Archive/Nsis      ; $(MAKE) all
	cd 7zip/Archive/Rar       ; $(MAKE) all
	cd 7zip/Archive/RPM       ; $(MAKE) all
	cd 7zip/Archive/Split     ; $(MAKE) all
	cd 7zip/Archive/Tar       ; $(MAKE) all
	cd 7zip/Archive/Z         ; $(MAKE) all
	cd 7zip/Archive/Zip       ; $(MAKE) all
	cd 7zip/Compress/Branch   ; $(MAKE) all
	cd 7zip/Compress/ByteSwap ; $(MAKE) all
	cd 7zip/Compress/BZip2    ; $(MAKE) all
	cd 7zip/Compress/Copy     ; $(MAKE) all
	cd 7zip/Compress/Deflate  ; $(MAKE) all
	cd 7zip/Compress/Implode  ; $(MAKE) all
	cd 7zip/Compress/LZMA     ; $(MAKE) all
	cd 7zip/Compress/PPMD     ; $(MAKE) all
	cd 7zip/Compress/Rar29    ; $(MAKE) all
	cd 7zip/Crypto/7zAES      ; $(MAKE) all
	cd 7zip/Crypto/AES        ; $(MAKE) all

clean:
	cd Common                 ; $(MAKE) clean
	cd myWindows              ; $(MAKE) clean
	cd 7zip/Bundles/Alone     ; $(MAKE) clean
	cd 7zip/Bundles/SFXCon    ; $(MAKE) clean
	cd 7zip/UI/Console        ; $(MAKE) clean
	cd 7zip/Archive/7z        ; $(MAKE) clean
	cd 7zip/Archive/Arj       ; $(MAKE) clean
	cd 7zip/Archive/BZip2     ; $(MAKE) clean
	cd 7zip/Archive/Cab       ; $(MAKE) clean
	cd 7zip/Archive/Chm       ; $(MAKE) clean
	cd 7zip/Archive/Cpio      ; $(MAKE) clean
	cd 7zip/Archive/Deb       ; $(MAKE) clean
	cd 7zip/Archive/GZip      ; $(MAKE) clean
	cd 7zip/Archive/Iso       ; $(MAKE) clean
	cd 7zip/Archive/Lzh       ; $(MAKE) clean
	cd 7zip/Archive/Nsis      ; $(MAKE) clean
	cd 7zip/Archive/Rar       ; $(MAKE) clean
	cd 7zip/Archive/RPM       ; $(MAKE) clean
	cd 7zip/Archive/Split     ; $(MAKE) clean
	cd 7zip/Archive/Tar       ; $(MAKE) clean
	cd 7zip/Archive/Z         ; $(MAKE) clean
	cd 7zip/Archive/Zip       ; $(MAKE) clean
	cd 7zip/Compress/Branch   ; $(MAKE) clean
	cd 7zip/Compress/ByteSwap ; $(MAKE) clean
	cd 7zip/Compress/BZip2    ; $(MAKE) clean
	cd 7zip/Compress/Copy     ; $(MAKE) clean
	cd 7zip/Compress/Deflate  ; $(MAKE) clean
	cd 7zip/Compress/Implode  ; $(MAKE) clean
	cd 7zip/Compress/LZMA     ; $(MAKE) clean
	cd 7zip/Compress/PPMD     ; $(MAKE) clean
	cd 7zip/Compress/Rar29    ; $(MAKE) clean
	cd 7zip/Crypto/7zAES      ; $(MAKE) clean
	cd 7zip/Crypto/AES        ; $(MAKE) clean
	cd 7zip/Bundles/Alone7z   ; $(MAKE) clean
	chmod +x contrib/VirtualFileSystemForMidnightCommander/u7z
	chmod +x contrib/gzip-like_CLI_wrapper_for_7z/p7zip
	chmod +x install.sh check/check.sh check/clean_all.sh check/check_7zr.sh 
	cd check                  ; ./clean_all.sh
	find . -name "*~" -exec rm -f {} \;
	find . -name "*.orig" -exec rm -f {} \;
	find . -name ".*.swp" -exec rm -f {} \;
	rm -fr bin

test: all
	cd check ; ./check.sh ../bin/7za

test_7z: all2
	cd check ; ./check.sh ../bin/7z

test_7zr: 7zr
	cd check ; ./check_7zr.sh ../bin/7zr
install:
	./install.sh $(DEST_BIN) $(DEST_SHARE) $(DEST_MAN)

REP=$(shell pwd)
ARCHIVE=$(shell basename $(REP))

.PHONY: tar_all tar_all2 tar_src tar_src_extra src_7z tar_bin tar_bin2

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

