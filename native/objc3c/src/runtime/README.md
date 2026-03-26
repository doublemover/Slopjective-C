# ObjC3 Native Runtime Surface

Live runtime surface:

- archive: `artifacts/lib/objc3_runtime.lib`
- public header: `native/objc3c/src/runtime/objc3_runtime.h`
- primary entrypoints:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_reset_for_testing`

Ownership boundary:

- the compiler owns source-derived metadata and emitted IR/object payloads
- the runtime owns installed registration, lookup, dispatch, and execution state once payloads are ingested

The live runtime docs describe the current executable surface only. Historical milestone freeze text is archived under `tmp/archive/`.
