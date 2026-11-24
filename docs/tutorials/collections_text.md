# Index UTF-8 text lengths

This program stores the Unicode scalar count of a text value in an integer
map. It prints a short preview, updates an existing entry, and shows how to
handle a missing key and invalid UTF-8.

Activate the published Linux SDK as shown in [Getting started](getting_started.md).
Save [collections-text.objc3](../../showcase/collections-text/collections-text.objc3)
in an empty directory. Build and run it with the installed compiler and
standard-library services archive.

```sh
"$OBJC3C_SDK_ROOT/bin/objc3c" collections-text.objc3 -O2 -Wall -Wextra -Werror \
  -fmodules -fmodules-cache-path=./modules \
  "$OBJC3C_SDK_ROOT/lib/libobjc3_runtime_services.a" -lstdc++ -pthread \
  -o collections-text
./collections-text
```

```text
text: scalar count 6, UTF-8 byte count 10
prefix (up to 4 scalars): café
entry 7: length 6, map size 1
entry 99: NOT_FOUND, fallback -1
```

The `.objc3` source imports `objc3.collections` and `objc3.text` from the SDK.
`objc3_text_prefix` takes up to four Unicode scalars. The first four scalars
of `café 🌍` occupy five bytes, including two for `é`. A displayed character
can contain multiple scalars. The prefix is an owned snapshot, and `defer`
releases it and the original text at scope exit.

`objc3_map_i32_insert_count` first adds key `7` and then replaces its value.
The replacement leaves one entry. `objc3_map_i32_get_or` writes the caller's
fallback for missing key `99` and returns `OBJC3_STDLIB_V2_NOT_FOUND`. Check
the status to distinguish a missing key from a stored value equal to the
fallback. The map also has a `defer` release.

Pass your own UTF-8 text as one quoted argument. The first example below
prints a one-scalar prefix. The second supplies a truncated UTF-8 sequence.

```sh
./collections-text 'é'
./collections-text "$(printf '\303')"
```

The second command exits with status 1 and prints `input is not valid UTF-8`
to standard error. Passing more than one argument exits with status 2 and
prints `usage: collections-text [TEXT]`.

Run `sh verify.sh` from the [example directory](../../showcase/collections-text/)
with the SDK active to verify the outputs and exit statuses at `-O0` and `-O2`.
