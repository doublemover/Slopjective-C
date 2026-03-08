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

`M254-A001` freezes the next startup-registration handoff without emitting it
yet:

- translation-unit preregistration inventory is the emitted
  `module.runtime-metadata.bin` payload plus
  `module.runtime-metadata-linker-options.rsp` and
  `module.runtime-metadata-discovery.json`
- the reserved constructor-root ownership model is
  `compiler-emits-constructor-root-runtime-owns-registration-state`
- the runtime-owned entrypoint remains `objc3_runtime_register_image`
- no startup constructor/root emission or bootstrap execution lands in
  `M254-A001`

`M254-A002` now emits one real `module.runtime-registration-manifest.json`
artifact on the native object-emission path:

- the manifest is authoritative for constructor-root shape under
  `registration-manifest-authoritative-for-constructor-root-shape`
- init-stub ownership remains
  `lowering-emits-init-stub-from-registration-manifest`
- the emitted manifest carries forward the same runtime-owned payload
  inventory, runtime archive path `artifacts/lib/objc3_runtime.lib`, and
  translation-unit identity key from linker-retention discovery
- init-stub emission and runtime bootstrap execution are still deferred to
  later `M254` issues

`M254-B001` freezes the startup/bootstrap semantic contract over that emitted
manifest:

- duplicate registration is defined against the translation-unit identity key
  and must fail closed rather than silently merging
- realization order is defined as constructor-root handoff followed by
  registration-manifest order
- bootstrap failure mode is
  `abort-before-user-main-no-partial-registration-commit`
- image-local initialization remains runtime-owned and image-local rather than
  driver-owned global mutable state

`M254-B002` turns that freeze into a live runtime contract:

- `objc3_runtime_register_image` now rejects duplicate translation-unit
  identity keys with status `-2`
- non-monotonic registration ordinals are rejected with status `-3`
- invalid descriptors are rejected with status `-1`
- `objc3_runtime_copy_registration_state_for_testing` exposes the committed
  runtime state so probes can verify failed registrations do not partially
  commit
- emitted `module.runtime-registration-manifest.json` payloads now carry the
  same duplicate/order/failure/status-code surface consumed by the runtime
  probe

`M254-C001` freezes the lowering-owned startup materialization boundary without
emitting it yet:

- the canonical lowering contract is
  `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
- the preserved constructor root remains `__objc3_runtime_register_image_ctor`
- the preserved init-stub prefix remains
  `__objc3_runtime_register_image_init_stub_`
- the reserved registration-table prefix is
  `__objc3_runtime_registration_table_`
- the future ctor list model is
  `llvm.global_ctors-single-root-priority-65535`
- emitted IR must stay explicit that no ctor root, no init stub, and no
  registration table are materialized yet

`M254-C002` then turns that frozen boundary into a live emitted startup path:

- emitted IR now contains `@llvm.global_ctors`
- the canonical ctor root remains `__objc3_runtime_register_image_ctor`
- the canonical init-stub prefix remains
  `__objc3_runtime_register_image_init_stub_`
- the canonical registration-table prefix remains
  `__objc3_runtime_registration_table_`
- startup probes link a metadata-only `.objc3` object and prove registration
  happened before `main`
