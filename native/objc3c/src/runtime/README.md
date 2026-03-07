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

`M251-D002` now instantiates that reserved surface and lands:

- `native/objc3c/src/runtime/objc3_runtime.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `artifacts/lib/objc3_runtime.lib` via `npm run build:objc3c-native`

`M251-D003` must preserve this surface while wiring the native driver/link path.

`M251-D003` now wires emitted-object consumers to the real archive without
changing the canonical runtime API surface:

- driver/manifest publication continues to expose the runtime archive path
- `scripts/check_objc3c_native_execution_smoke.ps1` links emitted objects
  against `artifacts/lib/objc3_runtime.lib`
- `native/objc3c/src/runtime/objc3_runtime.cpp` exports the compatibility
  bridge symbol `objc3_msgsend_i32`, which forwards to
  `objc3_runtime_dispatch_i32`
