# Objective-C 3 Runtime Tests

The live runtime test surface exists to prove the shipped runtime library and emitted native objects work together.

Authoritative runtime entrypoints:

- `objc3_runtime_lookup_selector`
- `objc3_runtime_register_selector`
- `objc3_runtime_dispatch_i32_checked`
- `objc3_runtime_dispatch_i32`
- `objc3_runtime_register_image`
- `objc3_runtime_reset_for_testing`

What the live probes should cover:

- selector lookup and dispatch over the realized object graph
- superclass, category, and class-method dispatch
- synthesized property getter/setter execution through realized per-instance storage
- property metadata reflection over emitted descriptors
- memory-management and ownership helper behavior where that surface is live
- runtime-backed storage ownership reflection over emitted property descriptors
- public ABI result, selector, and registration records exposed through the
  runtime public headers
- reset/replay behavior where deterministic runtime state matters

What does not count as proof:

- hand-authored `.ll` files or placeholder object artifacts
- sidecar-only evidence with no matching executable compile or probe path
- legacy retired-adapter, alternate-acceptance, old-surface, or retired-source
  lane naming that claims positive runtime support

Current corrective focus:

- keep dispatch proof tied to checked strict-result dispatch and the canonical
  `objc3_runtime_dispatch_i32` lowering entrypoint
- keep unsupported dispatch proof tied to strict-result status and explicit
  strict-error probes
- prove synthesized accessors through emitted objects and runtime probes
- treat storage, reflection, registration, and public ABI owner anchors as part of
  the runtime fixture metadata, not separate closeout ceremony

Representative live proof paths:

- runtime library:
  - `native/objc3c/src/runtime/dispatch/dispatch_api.cpp`
  - `native/objc3c/src/runtime/dispatch/dispatch_status.cpp`
  - `native/objc3c/src/runtime/public/objc3_runtime_result_contract.cpp`
  - `native/objc3c/src/runtime/public/objc3_runtime_result_builder.cpp`
  - `native/objc3c/src/runtime/public/objc3_runtime_dispatch_diagnostics.cpp`
  - `native/objc3c/src/runtime/storage/property_layout_realization.cpp`
  - `native/objc3c/src/runtime/storage/property_ivar_layout_index.cpp`
  - `native/objc3c/src/runtime/storage/property_lookup.cpp`
  - `native/objc3c/src/runtime/reflection/property_snapshot_api.cpp`
  - `native/objc3c/src/runtime/reflection/property_reflection_query_state.cpp`
  - `native/objc3c/src/runtime/reflection/storage_accessor_snapshot.cpp`
  - `native/objc3c/src/runtime/memory/arc.cpp`
- compile and artifact publication:
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp`
- IR emission:
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- runtime probes:
  - `tests/tooling/runtime/runtime_library_probe.cpp`
  - `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`
  - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
  - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
  - `tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp`
  - `tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp`
  - `tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp`
  - `tests/tooling/runtime/block_arc_runtime_abi_probe.cpp`
  - `tests/tooling/runtime/strict_dispatch_error_status_probe.cpp`

- integrated runtime architecture packet:
  - `npm run objc3c -- proof-runtime-architecture`
  - `tmp/reports/runtime/architecture-proof/summary.json`
- integrated runtime architecture validation:
  - `npm run objc3c -- validate-runtime-architecture`
  - `tmp/reports/runtime/architecture-integration/summary.json`

Use the runtime probes and native object fixtures as the truth source for runtime behavior. Historical milestone-by-milestone closeout notes belong under `tmp/archive/`, not here.

Runtime fixtures are canonical owner anchors. Positive probe output may prove
storage, reflection, registration, object-model, ARC/block, or public ABI
behavior; strict-dispatch probes prove rejected runtime outcomes. A probe name,
snapshot field, or README row must not turn legacy retired-adapter,
alternate-acceptance, old-surface, or retired-source lane residue into positive
support.

Fast helper-only checks:

- `npm run objc3c -- test-runtime-acceptance-fast`
- `npm run objc3c -- test-runtime-acceptance-diagnostics`
- `npm run objc3c -- test-runtime-acceptance-cross-module`

The helper-only case compiles and runs the JSON writer, snapshot stabilizer,
dispatch expectation, and representative helper-output equivalence tests. It is
the preferred first check for changes under `tests/tooling/runtime/support/`.

Direct runtime-acceptance helper scripts are implementation anchors behind the
workflow actions, not public command surface.

The runtime execution architecture published in `docs/objc3c-native.md` is the
operator-facing boundary for what these probes may claim.
