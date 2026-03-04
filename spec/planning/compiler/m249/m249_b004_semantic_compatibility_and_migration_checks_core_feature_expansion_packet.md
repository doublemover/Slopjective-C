# M249-B004 Semantic Compatibility and Migration Checks Core Feature Expansion Packet

Packet: `M249-B004`
Milestone: `M249`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M249-B003`

## Purpose

Freeze lane-B semantic compatibility and migration checks core feature expansion
prerequisites for M249 so dependency continuity stays explicit, deterministic,
and fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_core_feature_expansion_b004_expectations.md`
- Checker:
  `scripts/check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py`
- Readiness runner:
  `scripts/run_m249_b004_lane_b_readiness.py`
- Dependency anchors from `M249-B003`:
  - `docs/contracts/m249_semantic_compatibility_and_migration_checks_core_feature_implementation_b003_expectations.md`
  - `spec/planning/compiler/m249/m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_packet.md`
  - `scripts/check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-b004-semantic-compatibility-migration-checks-core-feature-expansion-contract`
  - `test:tooling:m249-b004-semantic-compatibility-migration-checks-core-feature-expansion-contract`
  - `check:objc3c:m249-b004-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py -q`
- `python scripts/run_m249_b004_lane_b_readiness.py`
- `npm run check:objc3c:m249-b004-lane-b-readiness`

## Evidence Output

- `tmp/reports/m249/M249-B004/semantic_compatibility_and_migration_checks_core_feature_expansion_summary.json`
