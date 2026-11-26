#!/bin/sh
set -eu

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 SDK_ROOT NEW_OUTPUT_DIRECTORY" >&2
  exit 2
fi

example=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
sdk=$(CDPATH='' cd -- "$1" && pwd -P)
parent=$(CDPATH='' cd -- "$(dirname -- "$2")" && pwd -P)
output="$parent/$(basename -- "$2")"
case "$output/" in
  "$sdk/"*) echo 'The output directory must be outside the installed SDK.' >&2; exit 2 ;;
esac
producer="$output/producer"
mkdir -- "$output"
mkdir -- "$producer"

cp -R -- "$example/producer" "$producer/workspace"
"$sdk/bin/objc3c-build" --sdk-root "$sdk" --workspace "$producer/workspace/workspace.json" \
  --output-dir "$producer/build" --optimization O2 > "$output/producer.json"
"$sdk/bin/objc3c-package" --sdk-root "$sdk" publish --build-dir "$producer/build" \
  --store "$output/published" > "$output/publication.json"
content=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["content_sha256"])' "$output/publication.json")
"$sdk/bin/objc3c-package" --sdk-root "$sdk" install --package "$output/published/$content" \
  --content-sha256 "$content" --store "$output/installed" > "$output/installation.json"

# The consumer uses the installed objects and interface pair after the producer is removed.
rm -rf -- "$producer"
cp -R -- "$example/consumer" "$output/client"
"$sdk/bin/objc3c-package" --sdk-root "$sdk" consume --package "$output/installed/$content" \
  --content-sha256 "$content" --application "$output/client/application.json" \
  --output-dir "$output/consumer-build" > "$output/consumption.json"
cat -- "$output/consumer-build/execution.log"
