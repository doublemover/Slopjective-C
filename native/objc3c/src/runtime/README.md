# ObjC3 Native Runtime Surface

`M251-D001` reserves `native/objc3c/src/runtime` as the canonical in-tree
runtime-library root for executable Objective-C 3 support.

Frozen surface:

- `objc3_runtime`
- `native/objc3c/src/runtime/objc3_runtime.h`
- `static`
- `objc3_runtime`
- `objc3_runtime_register_image`
- `objc3_runtime_lookup_selector`
- `objc3_runtime_dispatch_i32`
- `objc3_runtime_reset_for_testing`

Ownership boundary:

- the compiler owns source-derived metadata records and emitted IR/object
  payloads
- the runtime owns registration, lookup, and dispatch state once those payloads
  are ingested

`M251-D001` does not land the actual library skeleton or driver/link wiring.
`M251-D002` and `M251-D003` must preserve this frozen surface.
