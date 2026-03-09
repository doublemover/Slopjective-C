# M263 Bootstrap Legality, Duplicate Policy, and Failure Contract Expectations (B001)

Contract ID: `objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1`
Status: Accepted
Issue: `#7222`
Scope: `M263` lane-B contract and architecture freeze for bootstrap legality,
duplicate policy, image-order invariants, fail-closed rejection, and restart
boundaries.

## Objective

Freeze one fail-closed semantic legality packet that bridges:

- the emitted `M263-A002` registration-descriptor frontend closure
- the live `M254-B002` bootstrap semantics surface

so later lowering/runtime implementation extends one deterministic bootstrap
legality contract instead of re-deriving duplicate, ordering, or restart rules.

## Required Invariants

1. `native/objc3c/src/sema/objc3_sema_contract.h` remains the canonical
   declaration point for `Objc3BootstrapLegalityFailureContractSummary`.
2. `native/objc3c/src/sema/objc3_semantic_passes.cpp` remains the canonical
   builder point for the sema-owned legality packet and its replay key.
3. `native/objc3c/src/pipeline/objc3_frontend_types.h` remains the canonical
   declaration point for
   `Objc3RuntimeBootstrapLegalityFailureContractSummary`.
4. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` remains the
   canonical manifest publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_failure_contract`
   - flattened `runtime_bootstrap_legality_failure_*` summary keys
5. The frozen frontend packet stays explicitly tied to:
   - `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
   - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
6. The frozen legality model preserves:
   - duplicate-registration policy
     `fail-closed-by-translation-unit-identity-key`
   - image-order invariant
     `strictly-monotonic-positive-registration-order-ordinal`
   - failure mode
     `abort-before-user-main-no-partial-registration-commit`
   - restart lifecycle model
     `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
   - replay order model
     `replay-re-registers-retained-images-in-original-registration-order`
   - image-local init reset model
     `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
   - catalog retention model
     `bootstrap-catalog-retained-across-reset-for-deterministic-replay`
   - runtime state snapshot symbol
     `objc3_runtime_copy_registration_state_for_testing`
7. Registration descriptor identifier, image-root identifier, identity-source
   classification, and translation-unit registration ordinal pass through from
   the emitted `M263-A002` frontend closure without reconstruction.
8. `M263-B001` remains fail-closed and semantic-diagnostics-required.

## Non-Goals and Fail-Closed Rules

- `M263-B001` does not land new runtime bootstrap execution.
- `M263-B001` does not widen the bootstrap API surface.
- `M263-B001` does not materialize multi-image registration.
- `M263-B002` must preserve this freeze while implementing live duplicate and
  image-order semantics.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m263-b001-bootstrap-legality-duplicate-policy-and-failure-contract`.
- `package.json` includes
  `test:tooling:m263-b001-bootstrap-legality-duplicate-policy-and-failure-contract`.
- `package.json` includes `check:objc3c:m263-b001-lane-b-readiness`.

## Validation

- `python scripts/check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m263-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m263/M263-B001/bootstrap_legality_duplicate_policy_and_failure_contract_summary.json`
