# M254 Runtime Bootstrap API Contract and Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-bootstrap-api-freeze/m254-d001-v1`
Status: Accepted
Issue: `#7108`
Scope: M254 lane-D contract and architecture freeze for the runtime-owned
bootstrap API surface.

## Objective

Freeze the runtime-owned bootstrap API surface that emitted startup lowering and
registration manifests target, so later registrar/image-walk and reset work
extends one deterministic ABI boundary instead of drifting across ad hoc entry
points.

## Required Outcomes

1. `native/objc3c/src/pipeline/objc3_frontend_types.h` remains the canonical
   declaration point for `Objc3RuntimeBootstrapApiSummary`.
2. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` remains the
   canonical publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract`
   - flattened `runtime_bootstrap_api_*` summary keys
3. `module.runtime-registration-manifest.json` publishes the emitted bootstrap
   API handoff fields under `bootstrap_runtime_api_*`.
4. The frozen runtime-owned bootstrap surface preserves:
   - public header `native/objc3c/src/runtime/objc3_runtime.h`
   - archive `artifacts/lib/objc3_runtime.lib`
   - status enum type `objc3_runtime_registration_status_code`
   - image descriptor type `objc3_runtime_image_descriptor`
   - selector handle type `objc3_runtime_selector_handle`
   - registration snapshot type
     `objc3_runtime_registration_state_snapshot`
   - registration entrypoint `objc3_runtime_register_image`
   - selector lookup symbol `objc3_runtime_lookup_selector`
   - dispatch symbol `objc3_runtime_dispatch_i32`
   - state snapshot symbol `objc3_runtime_copy_registration_state_for_testing`
   - reset symbol `objc3_runtime_reset_for_testing`
5. The frozen semantic policy surface preserves:
   - registration result model `zero-success-negative-fail-closed`
   - registration ordinal model
     `strictly-monotonic-positive-registration-order-ordinal`
   - runtime-owned locking model `process-global-mutex-serialized-runtime-state`
   - startup invocation model
     `generated-init-stub-calls-runtime-register-image`
   - image walk lifecycle model `deferred-until-m254-d002`
   - deterministic reset lifecycle model `deferred-until-m254-d003`
6. `tests/tooling/runtime/m254_d001_runtime_bootstrap_api_probe.cpp` compiles
   against the frozen header/archive surface and proves registration, selector
   lookup, dispatch, snapshot, and reset behavior on the live runtime library.

## Non-Goals and Fail-Closed Rules

- `M254-D001` does not land image walk or realization traversal.
- `M254-D001` does not expand deterministic reset beyond the currently frozen
  testing hook.
- `M254-D002` must preserve the frozen API while implementing runtime registrar
  and image-walk behavior.
- `M254-D003` must preserve the frozen API while expanding deterministic reset
  semantics.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/runtime/README.md`
- `tests/tooling/runtime/README.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze`.
- `package.json` includes
  `test:tooling:m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze`.
- `package.json` includes `check:objc3c:m254-d001-lane-d-readiness`.

## Validation

- `python scripts/check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m254-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m254/M254-D001/runtime_bootstrap_api_contract_summary.json`
