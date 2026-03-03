# M248-B004 Semantic/Lowering Test Architecture Core Feature Expansion Packet

Packet: `M248-B004`
Milestone: `M248`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M248-B003`

## Purpose

Freeze lane-B semantic/lowering test architecture core feature expansion
prerequisites for M248 so dependency continuity stays explicit, deterministic,
and fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_semantic_lowering_test_architecture_core_feature_expansion_b004_expectations.md`
- Checker:
  `scripts/check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py`
- Dependency anchors from `M248-B003`:
  - `docs/contracts/m248_semantic_lowering_test_architecture_core_feature_implementation_b003_expectations.md`
  - `scripts/check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m248_b003_semantic_lowering_test_architecture_core_feature_implementation_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m248-b004-semantic-lowering-test-architecture-core-feature-expansion-contract`
  - `test:tooling:m248-b004-semantic-lowering-test-architecture-core-feature-expansion-contract`
  - `check:objc3c:m248-b004-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m248-b004-lane-b-readiness`

## Evidence Output

- `tmp/reports/m248/M248-B004/semantic_lowering_test_architecture_core_feature_expansion_contract_summary.json`
