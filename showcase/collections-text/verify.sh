#!/bin/sh
# Replay the published Linux SDK collections and text tutorial.
set -eu
export LC_ALL=C
: "${OBJC3C_SDK_ROOT:?activate the installed Linux SDK first}"
source_root=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
output=$(mktemp -d)
trap 'rm -rf -- "$output"' EXIT
cp "$source_root/collections-text.objc3" "$output/"
cd "$output"

expected='text: scalar count 6, UTF-8 byte count 10
prefix (up to 4 scalars): café
entry 7: length 6, map size 1
entry 99: NOT_FOUND, fallback -1'
short='text: scalar count 1, UTF-8 byte count 2
prefix (up to 4 scalars): é
entry 7: length 1, map size 1
entry 99: NOT_FOUND, fallback -1'
for optimization in O0 O2; do
  "$OBJC3C_SDK_ROOT/bin/objc3c" collections-text.objc3 "-$optimization" -Wall -Wextra -Werror \
    -fmodules -fmodules-cache-path=./modules "$OBJC3C_SDK_ROOT/lib/libobjc3_runtime_services.a" \
    -lstdc++ -pthread -o collections-text
  test "$(./collections-text)" = "$expected"
  test "$(./collections-text 'é')" = "$short"

  status=0
  ./collections-text "$(printf '\303')" >stdout.txt 2>stderr.txt || status=$?
  test "$status" = 1
  test ! -s stdout.txt
  printf 'input is not valid UTF-8\n' | cmp - stderr.txt

  status=0
  ./collections-text one two >stdout.txt 2>stderr.txt || status=$?
  test "$status" = 2
  test ! -s stdout.txt
  printf 'usage: collections-text [TEXT]\n' | cmp - stderr.txt
done
printf '%s\n' "$expected"
