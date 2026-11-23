#!/bin/sh
# Run the documented commands against the activated SDK in a fresh directory.
set -eu
export LC_ALL=C
source_root=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
output=$(mktemp -d)
trap 'rm -rf -- "$output"' EXIT
cd "$source_root"

expected='INFO 3
WARN 1
ERROR 1'
for optimization in O0 O2; do
  objc3c library/log-summary.objc3 "-$optimization" -Wall -Wextra -Werror -MMD \
    -MF "$output/log-summary.d" -o "$output/log-summary"
  test "$( "$output/log-summary" sample.log )" = "$expected"
  test -s "$output/log-summary.d"

  status=0
  "$output/log-summary" invalid.log >"$output/stdout" 2>"$output/stderr" || status=$?
  test "$status" = 1
  test ! -s "$output/stdout"
  test "$(cat "$output/stderr")" = 'invalid.log:2: expected INFO, WARN, or ERROR'

  status=0
  "$output/log-summary" "$output/missing.log" >"$output/stdout" 2>"$output/stderr" || status=$?
  test "$status" = 1
  test ! -s "$output/stdout"
  test "$(cat "$output/stderr")" = "$output/missing.log: No such file or directory"

  printf ' \t\r\n ERROR last line' >"$output/edge.log"
  test "$( "$output/log-summary" "$output/edge.log" )" = 'INFO 0
WARN 0
ERROR 1'
done

objc3c-build --workspace workspace.json --output-dir "$output/package" --optimization O2
test "$( "$output/package/application.bin" sample.log )" = "$expected"
printf '%s\n' "$expected"
