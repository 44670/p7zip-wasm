#! /bin/sh

# global install
DEST_HOME=/usr/local
# for testing 
# DEST_HOME=${HOME}/INSTALL/usr/local
DEST_BIN=${DEST_HOME}/bin
DEST_SHARE=${DEST_HOME}/lib/p7zip
DEST_MAN=${DEST_HOME}/man
[ "$1" ] && DEST_BIN=$1
[ "$2" ] && DEST_SHARE=$2
[ "$3" ] && DEST_MAN=$3

use_share="n"

if [ -x bin/7zCon.sfx ]
then
  use_share="o"
fi

if [ -x bin/7z ]
then
  use_share="o"
fi

# cleaning previous install
rm -f ${DEST_BIN}/7z
rm -f ${DEST_BIN}/7za
rm -f ${DEST_SHARE}/7z
rm -f ${DEST_SHARE}/7za
rm -f ${DEST_SHARE}/7zCon.sfx
rm -fr ${DEST_SHARE}/Codecs ${DEST_SHARE}/Formats
rmdir ${DEST_SHARE} 2> /dev/null

if [ "${use_share}" = "o" ]
then
  mkdir -p ${DEST_BIN}
  mkdir -p ${DEST_SHARE}
  if [ -x bin/7za ]
  then
    echo "- installing ${DEST_BIN}/7za"
    cp bin/7za ${DEST_SHARE}/7za
    chmod 777 ${DEST_SHARE}/7za
    strip     ${DEST_SHARE}/7za
    chmod 555 ${DEST_SHARE}/7za
    echo "#! /bin/sh" > ${DEST_BIN}/7za
    echo "${DEST_SHARE}/7za \"\$@\"" >> ${DEST_BIN}/7za
    chmod 555 ${DEST_BIN}/7za
  fi

  if [ -x bin/7zCon.sfx ]
  then
    echo "- installing ${DEST_SHARE}/7zCon.sfx"
    cp bin/7zCon.sfx ${DEST_SHARE}/7zCon.sfx
    chmod 777 ${DEST_SHARE}/7zCon.sfx
    strip     ${DEST_SHARE}/7zCon.sfx
    chmod 555 ${DEST_SHARE}/7zCon.sfx
  fi

  if [ -x bin/7z ]
  then
    echo "- installing ${DEST_BIN}/7z"
    cp bin/7z ${DEST_SHARE}/7z
    chmod 777 ${DEST_SHARE}/7z
    strip     ${DEST_SHARE}/7z
    chmod 555 ${DEST_SHARE}/7z
    cp -r bin/Codecs bin/Formats ${DEST_SHARE}/
    chmod 555 ${DEST_SHARE}/*/*
    echo "#! /bin/sh" > ${DEST_BIN}/7z
    echo "${DEST_SHARE}/7z \"\$@\"" >> ${DEST_BIN}/7z
    chmod 555 ${DEST_BIN}/7z
  fi

else
  if [ -x bin/7za ]
  then
    echo "- installing ${DEST_BIN}/7za"
    mkdir -p ${DEST_BIN}
    cp bin/7za ${DEST_BIN}/7za
    chmod 555 ${DEST_BIN}/7za
  fi
fi

mkdir -p ${DEST_MAN}/man1
echo "- installing ${DEST_MAN}/man1/7z.1"
cp man1/7z.1 ${DEST_MAN}/man1/
chmod 444    ${DEST_MAN}/man1/7z.1
echo "- installing ${DEST_MAN}/man1/7za.1"
cp man1/7za.1 ${DEST_MAN}/man1/
chmod 444    ${DEST_MAN}/man1/7za.1

