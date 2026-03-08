# Objective-C 3 Runtime Shim

`objc3_msgsend_i32` is a deterministic test shim:

- `M = 2147483629`
- `selector_score = sum((byte_i * i), i starting at 1) mod M`, using selector bytes as unsigned 8-bit values
- If `selector == NULL`, `selector_score = 0`
- Return value:

`(41 + 97*receiver + 7*a0 + 11*a1 + 13*a2 + 17*a3 + 19*selector_score) mod M`

The implementation normalizes negative modulo results into `[0, M-1]`.

`M251-D001` reserves the real native runtime-library surface separately:

- `objc3_runtime`
- `native/objc3c/src/runtime`
- `native/objc3c/src/runtime/objc3_runtime.h`
- `objc3_runtime_register_image`
- `objc3_runtime_lookup_selector`
- `objc3_runtime_dispatch_i32`
- `objc3_runtime_reset_for_testing`

This shim is not that runtime library and remains test-only evidence.

`M251-D002` now instantiates the in-tree native runtime library skeleton:

- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `artifacts/lib/objc3_runtime.lib`
- `tests/tooling/runtime/m251_d002_runtime_library_probe.cpp`

The current native library intentionally preserves the deterministic
`objc3_msgsend_i32` arithmetic formula for `objc3_runtime_dispatch_i32` so the
driver can migrate from shim-backed execution to native-library linkage in
`M251-D003` without semantic drift.

`M251-D003` now makes that linkage real for emitted-object execution paths:

- `scripts/check_objc3c_native_execution_smoke.ps1` resolves the runtime
  archive from emitted manifest data and links against
  `artifacts/lib/objc3_runtime.lib`
- `native/objc3c/src/runtime/objc3_runtime.cpp` exports
  `objc3_msgsend_i32` as a compatibility bridge to
  `objc3_runtime_dispatch_i32`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains test-only evidence
  for explicit unresolved-symbol and formula-parity coverage

`M251-E003` publishes the canonical operator runbook for this runtime-foundation
slice at `docs/runbooks/m251_runtime_foundation_developer_runbook.md`.
