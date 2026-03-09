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

`M254-B002` extends the same runtime surface with live startup/bootstrap
semantics while preserving the canonical archive/header path:

- `objc3_runtime_register_image` now consumes a translation-unit identity key
  plus a strict positive registration ordinal
- duplicate translation-unit identity keys fail closed with status `-2`
- non-monotonic registration ordinals fail closed with status `-3`
- `objc3_runtime_copy_registration_state_for_testing` exposes committed state
  to the native runtime probe so partial commits are detectable

`M254-C001` does not extend the runtime library API. It freezes the lowering
boundary that will eventually materialize startup ctor roots, init stubs, and
registration tables from the emitted manifest:

- lowering contract `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
- preserved ctor root `__objc3_runtime_register_image_ctor`
- preserved init-stub prefix `__objc3_runtime_register_image_init_stub_`
- reserved registration-table prefix `__objc3_runtime_registration_table_`
- no runtime API additions land in the freeze

`M254-D001` freezes the runtime-owned bootstrap API surface that later
registrar/image-walk and deterministic-reset issues must preserve:

- contract id `objc3c-runtime-bootstrap-api-freeze/m254-d001-v1`
- public header `native/objc3c/src/runtime/objc3_runtime.h`
- archive `artifacts/lib/objc3_runtime.lib`
- preserved entrypoints:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- emitted startup invocation model
  `generated-init-stub-calls-runtime-register-image`
- image walk remains deferred to `M254-D002`
- deterministic reset expansion remains deferred to `M254-D003`

`M254-D002` lands the private registrar/image-walk bridge that keeps the public
runtime surface frozen while allowing emitted startup code to stage and walk the
registration table:

- private bootstrap header
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- private staging hook
  `objc3_runtime_stage_registration_table_for_bootstrap`
- private image-walk snapshot hook
  `objc3_runtime_copy_image_walk_state_for_testing`
- image-walk model
  `registration-table-roots-validated-and-staged-before-realization`
- selector-pool interning model
  `canonical-selector-pool-preinterned-during-startup-image-walk`
- realization staging model
  `registration-table-roots-retained-for-later-realization`

`M254-D003` expands that same private bootstrap surface with deterministic reset
and replay hooks for same-process runtime probes:

- private replay hook
  `objc3_runtime_replay_registered_images_for_testing`
- private reset/replay snapshot hook
  `objc3_runtime_copy_reset_replay_state_for_testing`
- reset lifecycle model
  `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
- replay order model
  `replay-re-registers-retained-images-in-original-registration-order`
- image-local init-state reset model
  `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
- bootstrap catalog retention model
  `bootstrap-catalog-retained-across-reset-for-deterministic-replay`

`M255-D001` then freezes the runtime-owned lookup/dispatch boundary that the
live lane-C call ABI now targets:

- contract id `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
- canonical selector lookup symbol `objc3_runtime_lookup_selector`
- canonical dispatch symbol `objc3_runtime_dispatch_i32`
- canonical selector handle type `objc3_runtime_selector_handle`
- selector interning model
  `process-global-selector-intern-table-stable-id-per-canonical-selector-spelling`
- metadata-backed selector lookup tables remain deferred to `M255-D002`
- method-cache and runtime slow-path lookup remain deferred to `M255-D003`
- protocol/category-aware method resolution remains deferred to `M255-D004`
- the compatibility shim remains test-only evidence and is not the
  authoritative runtime-owned lookup/dispatch implementation
