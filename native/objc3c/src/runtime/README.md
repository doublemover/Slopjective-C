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
- the runtime owns installed registration, lookup, dispatch, allocation, property access, and execution state once payloads are ingested

Current dispatch path:

1. IR lowers supported message sends to `objc3_runtime_dispatch_i32`
2. the runtime interns or resolves the selector through `objc3_runtime_lookup_selector`
3. dispatch probes the method cache and then the realized class/category/protocol slow path
4. resolved methods execute either live emitted method bodies or runtime builtins such as `alloc`, `init`, and synthesized property accessors
5. unresolved sends still fall back to the deterministic arithmetic path in `ComputeDispatchResult`

Installation lifecycle:

1. the compile path emits a coupled object, manifest, runtime registration manifest, and compile provenance set
2. the loader or linked probe retains the emitted registration table roots
3. `objc3_runtime_register_image` installs the emitted image descriptor and staged registration table into runtime-owned state
4. lookup and dispatch consume only that installed runtime-owned state plus runtime builtins
5. `objc3_runtime_reset_for_testing` is the deterministic lifecycle reset hook for acceptance and replay paths

Installation ABI and loader lifecycle surface:

- public installation ABI:
  - `objc3_runtime_register_image`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- private loader lifecycle testing boundary:
  - `objc3_runtime_stage_registration_table_for_bootstrap`
  - `objc3_runtime_copy_image_walk_state_for_testing`
  - `objc3_runtime_replay_registered_images_for_testing`
  - `objc3_runtime_copy_reset_replay_state_for_testing`
- authoritative executable probe:
  - `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`
- emitted compile-manifest keys:
  - `runtime_installation_abi_surface`
  - `runtime_loader_lifecycle_surface`

Current synthesized-property path:

1. frontend metadata carries effective getter/setter selectors, binding symbols, and ivar layout records
2. IR emission synthesizes missing getter/setter bodies in `Objc3IREmitter::EmitSyntheticMethod`
3. property descriptors carry runtime implementation pointers for those accessors
4. runtime realization builds `RealizedPropertyAccessor` records from emitted descriptors and ivar layout
5. getter/setter dispatch executes against realized per-instance slot storage through the current-property helper surface

Current corrective focus:

- keep dispatch on the canonical `objc3_runtime_lookup_selector` / `objc3_runtime_dispatch_i32` path
- tighten the remaining gap between emitted dispatch IR and runtime selector/materialization behavior
- remove stale transitional lowering artifacts that no longer participate in live execution
- prove native output only from real compiler invocations and executable probes

Authoritative code paths for the current tranche:

- runtime registration and dispatch:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime.h`
- message-send lowering:
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- compile and artifact publication:
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- runtime/lowering boundary comments:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
- published architecture map:
  - `docs/objc3c-native.md`
  - `native/objc3c/src/runtime/ARCHITECTURE.md`

Non-goals for the current corrective tranche:

- no public runtime ABI widening beyond the existing header
- no new compatibility-only dispatch path
- no alternate loader lifecycle outside the existing register-image plus testing-reset surface
- no milestone-specific proof sidecars or bookkeeping surfaces in the live product tree
- no synthetic `.ll` or hand-authored artifact used as authoritative runtime proof

The live runtime docs describe the current executable surface only. Historical milestone freeze text is archived under `tmp/archive/`.

Integrated runtime architecture proof:

- runner:
  - `scripts/check_objc3c_runtime_architecture_proof_packet.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py proof-runtime-architecture`
- integrated packet:
  - `tmp/reports/runtime/architecture-proof/summary.json`

Integrated runtime architecture validation:

- runner:
  - `scripts/check_objc3c_runtime_architecture_integration.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture`
- integrated summary:
  - `tmp/reports/runtime/architecture-integration/summary.json`
