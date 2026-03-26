# Objective-C 3 Runtime Tests

The live runtime test surface exists to prove the shipped runtime library and emitted native objects work together.

Authoritative runtime entrypoints:

- `objc3_runtime_lookup_selector`
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
- reset/replay behavior where deterministic runtime state matters

What does not count as proof:

- hand-authored `.ll` files or placeholder object artifacts
- sidecar-only evidence with no matching executable compile or probe path

Current corrective focus:

- keep dispatch proof tied to `objc3_runtime_dispatch_i32`
- prove synthesized accessors through emitted objects and runtime probes
- treat native-output provenance as part of the test surface, not separate ceremony

Representative live proof paths:

- runtime library:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- compile and artifact publication:
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- IR emission:
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- runtime probes:
  - `tests/tooling/runtime/runtime_library_probe.cpp`
  - `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`
  - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
  - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
  - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

- integrated runtime architecture packet:
  - `scripts/check_objc3c_runtime_architecture_proof_packet.py`
  - `tmp/reports/runtime/architecture-proof/summary.json`
- integrated runtime architecture validation:
  - `scripts/check_objc3c_runtime_architecture_integration.py`
  - `tmp/reports/runtime/architecture-integration/summary.json`

Use the runtime probes and native object fixtures as the truth source for runtime behavior. Historical milestone-by-milestone closeout notes belong under `tmp/archive/`, not here.

The runtime execution architecture published in `docs/objc3c-native.md` is the
operator-facing boundary for what these probes may claim.
