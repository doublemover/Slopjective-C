# M249-B002 Semantic Compatibility and Migration Checks Modular Split and Scaffolding Packet

Packet: `M249-B002`
Milestone: `M249`
Lane: `B`
Freeze date: `2026-03-02`
Dependencies: `M249-B001`

## Purpose

Freeze lane-B semantic compatibility/migration modular split/scaffolding
prerequisites for M249 so dependency continuity stays deterministic and
fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_modular_split_scaffolding_b002_expectations.md`
- Checker:
  `scripts/check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`
- Prerequisite B001 assets:
  - `docs/contracts/m249_semantic_compatibility_and_migration_checks_contract_freeze_b001_expectations.md`
  - `spec/planning/compiler/m249/m249_b001_semantic_compatibility_and_migration_checks_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
  - `tests/tooling/test_check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-b002-semantic-compatibility-migration-checks-modular-split-scaffolding-contract`
  - `test:tooling:m249-b002-semantic-compatibility-migration-checks-modular-split-scaffolding-contract`
  - `check:objc3c:m249-b002-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m249-b002-lane-b-readiness`

## Evidence Output

- `tmp/reports/m249/M249-B002/semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract_summary.json`
