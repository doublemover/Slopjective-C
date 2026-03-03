# M248 Semantic/Lowering Test Architecture Core Feature Expansion Expectations (B004)

Contract ID: `objc3c-semantic-lowering-test-architecture-core-feature-expansion/m248-b004-v1`
Status: Accepted
Scope: M248 lane-B core feature expansion continuity for semantic/lowering test architecture dependency wiring.

## Objective

Fail closed unless lane-B core feature expansion dependency anchors remain
explicit, deterministic, and traceable across dependency surfaces, including
code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M248-B003`
- M248-B003 core feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m248_semantic_lowering_test_architecture_core_feature_implementation_b003_expectations.md`
  - `scripts/check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py`
- Packet/checker/test assets for B004 remain mandatory:
  - `spec/planning/compiler/m248/m248_b004_semantic_lowering_test_architecture_core_feature_expansion_packet.md`
  - `scripts/check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-B semantic/lowering
  anchor continuity inherited from `M248-B001` and `M248-B002`.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-B semantic/lowering
  fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-B
  semantic/lowering metadata anchor wording for dependency continuity.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-b004-semantic-lowering-test-architecture-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m248-b004-semantic-lowering-test-architecture-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m248-b004-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m248-b004-lane-b-readiness`

## Evidence Path

- `tmp/reports/m248/M248-B004/semantic_lowering_test_architecture_core_feature_expansion_contract_summary.json`
