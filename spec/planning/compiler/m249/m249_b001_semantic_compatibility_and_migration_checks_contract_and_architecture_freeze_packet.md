# M249-B001 Semantic Compatibility and Migration Checks Contract and Architecture Freeze Packet

Packet: `M249-B001`
Milestone: `M249`
Lane: `B`
Freeze date: `2026-03-02`
Dependencies: none

## Purpose

Freeze lane-B semantic compatibility and migration check boundaries so sema
pass-flow and parse/lowering compatibility handoff behavior remains
deterministic and fail-closed before downstream modular split/scaffolding work.

## Scope Anchors

- Contract:
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_contract_freeze_b001_expectations.md`
- Checker:
  `scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
- Semantic compatibility contract surface:
  `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- Semantic compatibility pass-flow synthesis:
  `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp`
- Parse/lowering compatibility handoff surface:
  `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-b001-semantic-compatibility-migration-checks-contract`
  - `test:tooling:m249-b001-semantic-compatibility-migration-checks-contract`
  - `check:objc3c:m249-b001-lane-b-readiness`
- Shared docs:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Inputs (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b001_semantic_compatibility_and_migration_checks_contract.py -q`
- `npm run check:objc3c:m249-b001-lane-b-readiness`

## Evidence Output

- `tmp/reports/m249/M249-B001/semantic_compatibility_and_migration_checks_contract_summary.json`
