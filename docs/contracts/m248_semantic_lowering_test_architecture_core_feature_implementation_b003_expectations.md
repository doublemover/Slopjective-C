# M248 Semantic/Lowering Test Architecture Core Feature Implementation Expectations (B003)

Contract ID: `objc3c-semantic-lowering-test-architecture-core-feature-implementation/m248-b003-v1`
Status: Accepted
Scope: M248 lane-B core feature implementation continuity for semantic/lowering test architecture dependency wiring.

## Objective

Fail closed unless lane-B core feature implementation dependency anchors remain
explicit, deterministic, and traceable across code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M248-B002`
- M248-B002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m248_semantic_lowering_test_architecture_modular_split_scaffolding_b002_expectations.md`
  - `spec/planning/compiler/m248/m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_packet.md`
  - `scripts/check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py`
- Checker/test assets for B003 remain mandatory:
  - `scripts/check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-b003-semantic-lowering-test-architecture-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m248-b003-semantic-lowering-test-architecture-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m248-b003-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m248-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m248/M248-B003/semantic_lowering_test_architecture_core_feature_implementation_contract_summary.json`
