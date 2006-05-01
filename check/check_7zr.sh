#! /bin/sh

sure()
{
  eval $*
  if [ "$?" != "0" ]
  then
    echo "ERROR during : $*"
    echo "ERROR during : $*" > last_error
    exit 1
  fi
}

PZIP7="$1"


sure rm -fr 7za433_ref 7za433_7zip_bzip2 7za433_7zip_lzma 7za433_7zip_lzma_crypto 7za433_7zip_ppmd 7za433_tar
sure rm -fr 7za433_7zip_bzip2.7z 7za433_7zip_lzma.7z 7za433_7zip_lzma_crypto.7z 7za433_7zip_ppmd.7z 7za433_tar.tar

echo ""
echo "# TESTING ..."
echo "#############"

sure ${PZIP7} t test/7za433_7zip_lzma.7z


echo ""
echo "# EXTRACTING ..."
echo "################"

sure tar xf test/7za433_tar.tar
sure mv 7za433_tar 7za433_ref

sure ${PZIP7} x test/7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

echo ""
echo "# Archiving ..."
echo "###############"

sure ${PZIP7} a 7za433_7zip_lzma.7z 7za433_7zip_lzma

echo ""
echo "# EXTRACTING (PASS 2) ..."
echo "#########################"

sure rm -fr 7za433_7zip_lzma

sure ${PZIP7} x 7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

./clean_all.sh

echo ""
echo "========"
echo "ALL DONE"
echo "========"
echo ""
