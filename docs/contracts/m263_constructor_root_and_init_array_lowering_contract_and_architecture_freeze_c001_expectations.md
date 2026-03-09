# M263 Constructor-Root and Init-Array Lowering Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-runtime-constructor-root-init-array-lowering/m263-c001-v1`
Status: Accepted
Issue: `#7225`
Scope: M263 lane-C freeze of the live constructor-root/init-array lowering boundary above the emitted registration-descriptor artifact and emitted registration manifest.

## Objective

Freeze the real lowering contract that is already materializing startup bootstrap globals into native object artifacts. The compiler now emits constructor roots, derived init stubs, registration tables, image-local init-state cells, and `@llvm.global_ctors`; this issue makes that live boundary explicit and ties it to the emitted `M263-A002` registration-descriptor artifact so later multi-image lowering extends one authoritative packet.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-constructor-root-init-array-lowering/m263-c001-v1`
   - boundary model `registration-descriptor-and-registration-manifest-drive-constructor-root-init-stub-registration-table-and-platform-init-array-lowering`
   - descriptor handoff contract id `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
   - descriptor artifact `module.runtime-registration-descriptor.json`
   - descriptor handoff model `registration-descriptor-artifact-and-registration-manifest-are-authoritative-lowering-inputs`
   - constructor-root/init-stub/registration-table emission states that reflect live materialization rather than the historical deferred placeholder.
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic replay-stable lowering boundary summary for the live path.
3. `ir/objc3_ir_emitter.cpp` publishes the live lowering boundary directly into emitted IR through `; runtime_bootstrap_lowering_boundary = ...` and preserves the current ctor/init emission path.
4. `pipeline/objc3_frontend_types.h` and `pipeline/objc3_frontend_artifacts.cpp` keep `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract` as the canonical frontend packet for the live lowering boundary.
5. The frontend summary must explicitly carry:
   - registration-manifest contract continuity
   - bootstrap-semantics contract continuity
   - registration-descriptor frontend-closure continuity
   - registration-descriptor artifact name
   - constructor-root/init-stub/registration-table/image-local-init symbol families
6. Happy-path native emission over both explicit and module-derived `M263` bootstrap fixtures must prove the emitted registration manifest, emitted registration-descriptor artifact, IR, and object artifact all agree on the live lowering boundary.

## Dynamic Coverage

1. Native compile probes over:
   - `tests/tooling/fixtures/native/m263_bootstrap_failure_restart_default.objc3`
   - `tests/tooling/fixtures/native/m263_bootstrap_failure_restart_explicit.objc3`
   must each emit:
   - `module.runtime-registration-manifest.json`
   - `module.runtime-registration-descriptor.json`
   - `module.ll`
   - `module.obj`
2. The emitted frontend semantic surface at `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract` must carry the new contract id, descriptor handoff fields, live emission states, and a non-empty replay key.
3. `module.runtime-registration-manifest.json` and `module.runtime-registration-descriptor.json` must agree on:
   - `constructor_root_symbol`
   - derived init-stub symbol
   - derived registration-table symbol
   - derived image-local-init-state symbol
   - `registration_entrypoint_symbol`
4. The emitted IR must contain:
   - the live `runtime_bootstrap_lowering_boundary` line
   - the live `runtime_bootstrap_ctor_init_emission` line
   - `@llvm.global_ctors`
   - `@__objc3_runtime_register_image_ctor`
   - `@__objc3_runtime_register_image_init_stub_...`
   - `@__objc3_runtime_registration_table_...`
   - `@__objc3_runtime_image_local_init_state_...`
   - a call to `@objc3_runtime_stage_registration_table_for_bootstrap`
   - a call to `@objc3_runtime_register_image`
5. `module.object-backend.txt` must remain `llvm-direct` and `module.obj` must be non-empty.

## Non-Goals and Fail-Closed Rules

- `M263-C001` does not add multi-image root fanout yet.
- `M263-C001` does not add replay partitioning or teardown policy; that stays in lane-D.
- `M263-C001` does not relax fail-closed runtime bootstrap semantics.
- Later lane-C implementation must consume the emitted descriptor/manifest handoff rather than synthesizing startup globals from unrelated IR-local heuristics.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m263-c001-constructor-root-and-init-array-lowering-contract`.
- `package.json` includes `test:tooling:m263-c001-constructor-root-and-init-array-lowering-contract`.
- `package.json` includes `check:objc3c:m263-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m263_c001_constructor_root_and_init_array_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m263_c001_constructor_root_and_init_array_lowering_contract_and_architecture_freeze.py -q`
- `python scripts/run_m263_c001_lane_c_readiness.py`

## Evidence Path

- `tmp/reports/m263/M263-C001/constructor_root_and_init_array_lowering_contract_summary.json`
