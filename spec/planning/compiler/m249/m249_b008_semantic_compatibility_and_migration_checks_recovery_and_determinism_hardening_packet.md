# M249-B008 Semantic Compatibility and Migration Checks Recovery and Determinism Hardening Packet

Packet: `M249-B008`
Milestone: `M249`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M249-B007`

## Purpose

Freeze lane-B semantic compatibility and migration checks recovery and determinism hardening
prerequisites for M249 so dependency continuity stays explicit, deterministic,
and fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_recovery_and_determinism_hardening_b008_expectations.md`
- Checker:
  `scripts/check_m249_b008_semantic_compatibility_and_migration_checks_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_b008_semantic_compatibility_and_migration_checks_recovery_and_determinism_hardening_contract.py`
- Readiness runner:
  `scripts/run_m249_b008_lane_b_readiness.py`
- Dependency anchors from `M249-B007`:
  - `docs/contracts/m249_semantic_compatibility_and_migration_checks_diagnostics_hardening_b007_expectations.md`
  - `spec/planning/compiler/m249/m249_b007_semantic_compatibility_and_migration_checks_diagnostics_hardening_packet.md`
  - `scripts/check_m249_b007_semantic_compatibility_and_migration_checks_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m249_b007_semantic_compatibility_and_migration_checks_diagnostics_hardening_contract.py`
  - `scripts/run_m249_b007_lane_b_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-b008-semantic-compatibility-migration-checks-recovery-and-determinism-hardening-contract`
  - `test:tooling:m249-b008-semantic-compatibility-migration-checks-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m249-b008-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m249_b008_semantic_compatibility_and_migration_checks_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b008_semantic_compatibility_and_migration_checks_recovery_and_determinism_hardening_contract.py -q`
- `python scripts/run_m249_b008_lane_b_readiness.py`
- `npm run check:objc3c:m249-b008-lane-b-readiness`

## Evidence Output

- `tmp/reports/m249/M249-B008/semantic_compatibility_and_migration_checks_recovery_and_determinism_hardening_summary.json`
