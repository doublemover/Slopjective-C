# M248 Semantic/Lowering Test Architecture Recovery and Determinism Hardening Expectations (B008)

Contract ID: `objc3c-semantic-lowering-test-architecture-recovery-and-determinism-hardening/m248-b008-v1`
Status: Accepted
Scope: M248 lane-B recovery and determinism hardening continuity for semantic/lowering test architecture dependency wiring.

## Objective

Fail closed unless lane-B recovery and determinism hardening dependency anchors
remain explicit, deterministic, and traceable across dependency surfaces, including
code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M248-B007`
- Issue `#6808` defines canonical lane-B recovery and determinism hardening scope.
- M248-B007 diagnostics hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m248_semantic_lowering_test_architecture_diagnostics_hardening_b007_expectations.md`
  - `spec/planning/compiler/m248/m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_packet.md`
  - `scripts/check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m248_b007_semantic_lowering_test_architecture_diagnostics_hardening_contract.py`
- Packet/checker/test assets for B008 remain mandatory:
  - `spec/planning/compiler/m248/m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-B semantic/lowering
  anchor continuity inherited from `M248-B001` and `M248-B002`.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-B semantic/lowering
  fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-B
  semantic/lowering metadata anchor wording for dependency continuity.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract`.
- `package.json` includes
  `test:tooling:m248-b008-semantic-lowering-test-architecture-recovery-and-determinism-hardening-contract`.
- `package.json` includes `check:objc3c:m248-b008-lane-b-readiness`.
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m248-b007-lane-b-readiness`
  - `check:objc3c:m248-b008-lane-b-readiness`

## Milestone Optimization Inputs

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Validation

- `python scripts/check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m248_b008_semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m248-b008-lane-b-readiness`

## Evidence Path

- `tmp/reports/m248/M248-B008/semantic_lowering_test_architecture_recovery_and_determinism_hardening_contract_summary.json`
