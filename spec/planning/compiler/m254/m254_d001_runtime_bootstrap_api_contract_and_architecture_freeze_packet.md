# M254-D001 Runtime Bootstrap API Contract and Architecture Freeze Packet

Packet: `M254-D001`
Milestone: `M254`
Lane: `D`
Freeze date: `2026-03-08`
Dependencies: none

## Purpose

Freeze one canonical runtime-owned bootstrap API packet so later registrar,
image-walk, and deterministic-reset work extends a stable ABI and manifest
handoff boundary instead of creating new runtime entrypoints ad hoc.

## Scope Anchors

- Contract:
  `docs/contracts/m254_runtime_bootstrap_api_contract_and_architecture_freeze_d001_expectations.md`
- Checker:
  `scripts/check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py`
- Tooling tests:
  `tests/tooling/test_check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py`
- Runtime probe:
  `tests/tooling/runtime/m254_d001_runtime_bootstrap_api_probe.cpp`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze`
  - `test:tooling:m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze`
  - `check:objc3c:m254-d001-lane-d-readiness`
- Code anchors:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/io/objc3_process.h`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `native/objc3c/src/runtime/README.md`
  - `tests/tooling/runtime/README.md`

## Fail-Closed Boundary

- Contract id `objc3c-runtime-bootstrap-api-freeze/m254-d001-v1`
- Semantic surface
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract`
- Frozen runtime-owned API surface:
  - public header `native/objc3c/src/runtime/objc3_runtime.h`
  - archive `artifacts/lib/objc3_runtime.lib`
  - status enum `objc3_runtime_registration_status_code`
  - image descriptor `objc3_runtime_image_descriptor`
  - selector handle `objc3_runtime_selector_handle`
  - registration snapshot `objc3_runtime_registration_state_snapshot`
  - entrypoints:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
    - `objc3_runtime_copy_registration_state_for_testing`
    - `objc3_runtime_reset_for_testing`
- Frozen semantic policies:
  - registration result model `zero-success-negative-fail-closed`
  - registration ordinal model
    `strictly-monotonic-positive-registration-order-ordinal`
  - runtime-owned locking model `process-global-mutex-serialized-runtime-state`
  - startup invocation model
    `generated-init-stub-calls-runtime-register-image`
  - image walk lifecycle `deferred-until-m254-d002`
  - deterministic reset lifecycle `deferred-until-m254-d003`
- Emitted registration manifests must expose the same handoff fields under
  `bootstrap_runtime_api_*`.

## Non-Goals

- no runtime image walk yet
- no image realization traversal yet
- no deterministic reset expansion beyond the current testing hook
- no new runtime API entrypoints beyond the frozen surface

## Acceptance Checklist

- semantic surface packet exists and is ready/fail-closed
- flattened `runtime_bootstrap_api_*` handoff keys exist and match the packet
- `module.runtime-registration-manifest.json` publishes the bootstrap runtime
  API handoff fields
- runtime probe compiles against the public header and archive
- runtime probe proves register/lookup/dispatch/snapshot/reset behavior
- evidence is written to
  `tmp/reports/m254/M254-D001/runtime_bootstrap_api_contract_summary.json`

## Gate Commands

- `python scripts/check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m254-d001-lane-d-readiness`
