#!/bin/sh
# gzip-like CLI wrapper for p7zip
# version 3.0
#
# History
#  2.0 :
#    - support for -filename, "file name"
#    - p7zip file1 file2 ...
#  3.0 :  (robert@debian.org, March 2016)
#    - use 7za or 7zr, whichever one is available
#    - refactor the script for better readability
#    - remove `"$?" != 0 ' checks that do not work with `set -e'
#    - use stderr for error reporting
#    - add support for -c, -f, -k options

set -e

# detect 7z program to use
prg7z="`which 7za 2>/dev/null`" || \
  prg7z="`which 7zr 2>/dev/null`" || \
    { echo "$0: cannot find neither 7za nor 7zr command" >&2; exit 1; }

# global options
f_compress=true
f_keep=false
f_force=false
f_tostdout=false

usage()
{
  echo "Usage: $0 [options] [--] [ name ... ]"
  echo ""
  echo "Options:"
  echo "    -c --stdout --to-stdout      output data to stdout"
  echo "    -d --decompress --uncompress decompress file"
  echo "    -f --force                   do not ask questions"
  echo "    -k --keep                    keep original file"
  echo "    -h --help                    print this help"
  echo "    --                           treat subsequent arguments as file"
  echo "                                 names, even if they start with a dash"
  echo ""
  exit 0
}

has_7z_suffix()
{
  case "$1" in
    *.7z)
      return 0
      ;;
    *)
      return 1
      ;;
  esac;
}

make_tmp_file()
{
  P7ZIPTMP="${TMP:-/tmp}"
  mktemp "${P7ZIPTMP}/p7zip.XXXXXXXX"
}

check_not_a_tty()
{
  if ! ${f_force} && ${f_compress} && tty <&1 >/dev/null ; then
    echo "$0: compressed data not written to a terminal." >&2
    echo "For help, type: $0 -h" >&2
    exit 1
  fi
}

compress_file()
{
  file="$1"

  if ! ${f_force} && has_7z_suffix "${file}"; then
    echo "$0: $file already has the 7z suffix" >&2
    exit 1
  fi

  # compress to stdout via temporary file
  if ${f_tostdout}; then
    check_not_a_tty
    tmp="`make_tmp_file`"
    trap "rm -f -- ${tmp}" 0
    rm -f -- "${tmp}"
    "${prg7z}" a -si -- "${tmp}" < "${file}" >/dev/null && cat "${tmp}" || \
      { echo "$0: failed to compress data to temporary file" >&2;  exit 1; }
    rm -f -- "${tmp}"
    return 0
  fi

  # compress to a file
  if ! ${f_force} && [ -e "${file}.7z" ]; then
    echo "$0: destination file ${file}.7z already exists" >&2
    exit 1
  fi

  rm -f -- "${file}.7z"
  flags=""
  ${f_keep} || flags="$flags -sdel"
  ! ${f_force} || flags="$flags -y"
  "${prg7z}" a $flags -- "${file}.7z" "${file}" || {  rm -f -- "${file}.7z"; exit 1; }
}


decompress_file()
{
  file="$1"

  has_7z_suffix "${file}" || { echo "$0: ${file}: unknown suffix" >&2; exit 1; }

  # decompress to stdout
  if ${f_tostdout}; then
    # The following `| cat' pipe shouldn't be needed, however it is here to
    # trick 7z not to complain about writing data to terminal.
    "${prg7z}" x -so -- "${file}" | cat || exit 1
    return 0;
  fi

  flags=""
  ! ${f_force} || flags="$flags -y"
  "${prg7z}" x $flags -- "${file}" || exit 1

  # remove original file unless the archive contains more than one file
  if ! ${f_keep} && "${prg7z}" l --  "${file}" 2>/dev/null | grep -q '^1 file,' 2>/dev/null; then
    rm -f -- "${file}"
  fi
}

process_file()
{
  file="$1"

  # check if file exists and is readable
  [ -r "${file}" ] || { echo "$0: cannot read ${file}" >&2; exit 1; }

  if ${f_compress}; then
    compress_file "${file}"
  else
    decompress_file "${file}"
  fi
}

process_stdin()
{
  check_not_a_tty

  tmp="`make_tmp_file`"
  trap "rm -f -- ${tmp}" 0

  if ${f_compress}; then

    rm -f -- "${tmp}"
    "${prg7z}" a -si -- "${tmp}" >/dev/null && cat -- "${tmp}" || exit 1

  else # decompress

    cat > "${tmp}"
    # The following `| cat' pipe shouldn't be needed, however it is here to
    # trick 7z not to complain about writing data to terminal.
    "${prg7z}" x -so -- "${tmp}" | cat || exit 1
  fi

  rm -f -- "${tmp}"
}


## MAIN


# files and flags
while [ "$#" != "0" ] ; do
  case "$1" in
    -c|--stdout|--to-stdout)
      f_tostdout=true
      ;;
    -d|--decompress|--uncompress)
      f_compress=false # decompressing
      ;;
    -f|--force)
      f_force=true
      ;;
    -h|--help)
      usage
      ;;
    -k|--keep)
      f_keep=true
      ;;
    --)
      shift
      break
      ;;
    -*)
      echo "$0: ignoring unknown option $1" >&2
      ;;
    *)
      break
      ;;
  esac
  shift
done


# make sure they're present, before we screw up
for i in mktemp rm cat tty grep; do
  if ! which $i >/dev/null ; then
    echo "$0: $i: command not found" >&2
    exit 1
  fi
done

if [ "$#" = 0 ]; then
  # compressing/decompressing using standard I/O
  process_stdin
  exit 0
fi

# only files now
while [ "$#" != "0" ] ; do
  process_file  "$1"
  shift
done

exit 0
