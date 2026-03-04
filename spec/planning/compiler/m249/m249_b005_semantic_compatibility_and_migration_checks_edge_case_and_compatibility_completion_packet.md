# M249-B005 Semantic Compatibility and Migration Checks Edge-Case and Compatibility Completion Packet

Packet: `M249-B005`
Milestone: `M249`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M249-B004`

## Purpose

Freeze lane-B semantic compatibility and migration checks edge-case and
compatibility completion prerequisites for M249 so dependency continuity stays
explicit, deterministic, and fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_edge_case_and_compatibility_completion_b005_expectations.md`
- Checker:
  `scripts/check_m249_b005_semantic_compatibility_and_migration_checks_edge_case_and_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_b005_semantic_compatibility_and_migration_checks_edge_case_and_compatibility_completion_contract.py`
- Readiness runner:
  `scripts/run_m249_b005_lane_b_readiness.py`
- Dependency anchors from `M249-B004`:
  - `docs/contracts/m249_semantic_compatibility_and_migration_checks_core_feature_expansion_b004_expectations.md`
  - `spec/planning/compiler/m249/m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_packet.md`
  - `scripts/check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m249_b004_semantic_compatibility_and_migration_checks_core_feature_expansion_contract.py`
  - `scripts/run_m249_b004_lane_b_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-b005-semantic-compatibility-migration-checks-edge-case-and-compatibility-completion-contract`
  - `test:tooling:m249-b005-semantic-compatibility-migration-checks-edge-case-and-compatibility-completion-contract`
  - `check:objc3c:m249-b005-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m249_b005_semantic_compatibility_and_migration_checks_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b005_semantic_compatibility_and_migration_checks_edge_case_and_compatibility_completion_contract.py -q`
- `python scripts/run_m249_b005_lane_b_readiness.py`
- `npm run check:objc3c:m249-b005-lane-b-readiness`

## Evidence Output

- `tmp/reports/m249/M249-B005/semantic_compatibility_and_migration_checks_edge_case_and_compatibility_completion_summary.json`
