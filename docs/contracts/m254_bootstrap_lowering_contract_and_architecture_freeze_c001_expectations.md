# M254 Bootstrap Lowering Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
Status: Accepted
Issue: `#7105`
Scope: M254 lane-C freeze of the lowering-owned startup-bootstrap materialization boundary before ctor-root, init-stub, and registration-table IR globals are emitted.

## Objective

Freeze the canonical lowering contract that later startup materialization must preserve. The project already emits the registration manifest and already enforces bootstrap semantics in the runtime library; this issue makes the lowering boundary explicit so the next implementation issue materializes startup globals from one authoritative packet instead of reconstructing the shape ad hoc in IR emission or driver code.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
   - boundary model `registration-manifest-driven-constructor-root-init-stub-and-registration-table-lowering`
   - future ctor list model `llvm.global_ctors-single-root-priority-65535`
   - preserved registration-table prefix `__objc3_runtime_registration_table_`
   - current constructor-root/init-stub/registration-table emission states `deferred-until-m254-c002`.
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic lowering boundary summary for the current state; it must be explicit that no ctor root, no init stub, and no registration table are emitted yet.
3. `ir/objc3_ir_emitter.cpp` publishes the boundary directly into emitted IR through `; runtime_bootstrap_lowering_boundary = ...`.
4. `pipeline/objc3_frontend_artifacts.cpp` publishes one canonical semantic-surface packet at `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract` and one flattened replay-stable summary so later lane-C implementation consumes a single lowering packet.
5. `io/objc3_process.cpp` and `driver/objc3_objc3_path.cpp` may publish the lowering contract into `module.runtime-registration-manifest.json`, but they may not synthesize ctor-root, init-stub, or registration-table IR globals during the freeze.
6. Happy-path native emission over `tests/tooling/fixtures/native/hello.objc3` must still produce `module.ll`, `module.obj`, and `module.runtime-registration-manifest.json` while proving the bootstrap globals are not yet materialized.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proves the emitted semantic surface carries the new bootstrap-lowering packet.
2. The same probe proves `module.runtime-registration-manifest.json` carries the lowering contract fields and `ready_for_bootstrap_lowering_materialization: true`.
3. The emitted IR must carry the lowering boundary line and must not yet contain:
   - `@llvm.global_ctors`
   - `@__objc3_runtime_register_image_ctor`
   - `__objc3_runtime_register_image_init_stub_`
   - `__objc3_runtime_registration_table_`
4. The same probe must still produce a non-empty `module.obj`.

## Non-Goals and Fail-Closed Rules

- `M254-C001` does not emit the constructor root yet.
- `M254-C001` does not emit init stubs yet.
- `M254-C001` does not emit registration-table globals yet.
- `M254-C001` does not trigger startup registration through `@llvm.global_ctors` yet.
- If the explicit lowering boundary drifts, later lane-C implementation must fail closed rather than silently materializing bootstrap globals from stale or partial manifest data.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m254-c001-bootstrap-lowering-contract-and-architecture-freeze`.
- `package.json` includes `test:tooling:m254-c001-bootstrap-lowering-contract-and-architecture-freeze`.
- `package.json` includes `check:objc3c:m254-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m254_c001_bootstrap_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m254_c001_bootstrap_lowering_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m254-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m254/M254-C001/bootstrap_lowering_contract_summary.json`
