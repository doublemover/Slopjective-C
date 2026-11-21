# Source-bound native diagnostic edits

Source 2.225 / System 2.151 retains Clang fix-it sets in native compilation results
and exposes `objc3c.native-diagnostics.v1` JSON through the C embedding API and
`--diagnostics-json`. Previously the custom diagnostic consumer kept messages and
locations but dropped every native replacement hint.

Each edit identifies an absolute physical/VFS filename, exact source SHA-256,
exclusive byte offsets, physical line/byte-column positions, replacement bytes
and Clang's insertion-order flag. Token ranges become complete character ranges;
copy-from-range insertions retain their actual bytes. Virtual inputs resolve
against the request filesystem's working directory. `#line` affects displayed
diagnostic locations, while edit offsets/locations still address the actual
source buffer. Compiler-only buffers are not editable files.

All hints for one diagnostic form one set. If any hint cannot be mapped safely,
the consumer withholds that complete set and records an unavailable reason. It
never guesses an edit inside a macro expansion. Consumers must check the source
hash, all ranges and ordering before applying the complete set. The compiler does
not modify input source files. One repair may expose another error; an edit set
does not promise that an otherwise invalid program will compile without further
diagnostics.

JSON includes native diagnostic IDs/severities/messages, spelling and expansion
locations, edit sets, import/publication error codes and stages, the compiler
identity and overall success/failure. `file_bytes_hex` and
`replacement_bytes_hex` preserve exact bytes, including embedded NUL or invalid
UTF-8. The corresponding text fields are display strings and repair invalid
UTF-8. The JSON record itself is UTF-8.

For in-process syntax, object, LLVM and interface operations:

```sh
objc3c source.m --objc3 --diagnostics-json -- -std=c17 > diagnostics.json
objc3c source.m --objc3 --diagnostics-json --emit-object source.o \
  -- -std=c17 > diagnostics.json
```

This mode emits one JSON object on stdout and keeps human diagnostics on stderr.
Use explicit `--emit-object`/`--emit-llvm` for artifacts; this is a frontend mode,
not the multi-input link driver. Artifact output to stdout is rejected to prevent
mixed binary/JSON output. Existing status codes retain their meaning.

C embedders call `objc3c_frontend_v3_result_diagnostics_json(result)`. The returned
string belongs to the result and remains valid until its destruction, including
after input strings have gone away. A null result returns null. No existing
caller-allocated C option structure changes.

Replay tests repair and recompile C17/C23 and C++20/C++23 source, including token
insertions/replacements/removals, multiple diagnostics, multibyte prefixes, virtual
inputs and `#line`. Macro-body errors acquire no guessed edit. C ownership,
source guards, raw byte encoding and structured module errors have controls.
The CLI test repairs a source file from JSON and compiles its object separately;
it remains active under Python optimization. Matching SDK and full diagnostics/
profile acceptance remain separate work.

The native edit replay test, existing C API/import and module-resolution tests,
CLI replay under `python -O` and 90 tooling checks pass; four SDK execution
checks remain skipped. These focused results do not qualify a replacement SDK.
