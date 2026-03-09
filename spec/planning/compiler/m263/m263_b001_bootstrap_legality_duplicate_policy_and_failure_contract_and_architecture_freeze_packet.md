# M263-B001 Bootstrap Legality, Duplicate Policy, and Failure Contract Packet

Packet: `M263-B001`
Milestone: `M263`
Lane: `B`
Freeze date: `2026-03-09`
Dependencies: `M263-A002`, `M259-E002`
Next issue: `M263-B002`

## Purpose

Freeze the canonical bootstrap legality packet that binds semantic duplicate
policy, image-order invariants, fail-closed rejection, and restart boundaries
to the emitted `M263-A002` descriptor closure and the live `M254-B002`
bootstrap semantics surface.

## Scope Anchors

- Contract:
  `docs/contracts/m263_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze_b001_expectations.md`
- Checker:
  `scripts/check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py`
- Tooling tests:
  `tests/tooling/test_check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m263-b001-bootstrap-legality-duplicate-policy-and-failure-contract`
  - `test:tooling:m263-b001-bootstrap-legality-duplicate-policy-and-failure-contract`
  - `check:objc3c:m263-b001-lane-b-readiness`
- Code anchors:
  - `native/objc3c/src/sema/objc3_sema_contract.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Fail-Closed Boundary

- contract id
  `objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1`
- semantic/front-end surface
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_failure_contract`
- upstream contract ids:
  - `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
  - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
- frozen legality policies:
  - duplicate registration
    `fail-closed-by-translation-unit-identity-key`
  - image order invariant
    `strictly-monotonic-positive-registration-order-ordinal`
  - failure mode
    `abort-before-user-main-no-partial-registration-commit`
  - restart lifecycle
    `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
  - replay order
    `replay-re-registers-retained-images-in-original-registration-order`
  - image-local init reset
    `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
  - catalog retention
    `bootstrap-catalog-retained-across-reset-for-deterministic-replay`
- bridge-through fields preserved from `M263-A002`:
  - `registration_descriptor_identifier`
  - `image_root_identifier`
  - identity-source classification
  - translation-unit registration ordinal

## Non-Goals

- no new bootstrap execution path
- no new API/header/archive widening
- no multi-image registration/replay implementation
- no duplicate-registration runtime mutation beyond the already-live `M254-B002`
  semantics surface

## Gate Commands

- `python scripts/check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m263-b001-lane-b-readiness`

## Evidence Output

- `tmp/reports/m263/M263-B001/bootstrap_legality_duplicate_policy_and_failure_contract_summary.json`
