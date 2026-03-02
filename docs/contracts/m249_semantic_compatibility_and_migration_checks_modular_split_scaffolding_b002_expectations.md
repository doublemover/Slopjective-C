# M249 Semantic Compatibility and Migration Checks Modular Split Scaffolding Expectations (B002)

Contract ID: `objc3c-semantic-compatibility-and-migration-checks-modular-split-scaffolding/m249-b002-v1`
Status: Accepted
Scope: M249 lane-B semantic compatibility and migration checks modular split/scaffolding continuity for deterministic sema/parse compatibility handoff governance.

## Objective

Fail closed unless M249 lane-B semantic compatibility and migration checks
modular split/scaffolding anchors remain explicit, deterministic, and
traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M249-B001`
- Prerequisite frozen assets from `M249-B001` remain mandatory:
  - `docs/contracts/m249_semantic_compatibility_and_migration_checks_contract_freeze_b001_expectations.md`
  - `spec/planning/compiler/m249/m249_b001_semantic_compatibility_and_migration_checks_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
  - `tests/tooling/test_check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
- Packet/checker/test assets for `M249-B002` remain mandatory:
  - `spec/planning/compiler/m249/m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_packet.md`
  - `scripts/check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-B `M249-B002`
  semantic compatibility/migration modular split dependency anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-B semantic
  compatibility/migration modular split fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-B
  semantic compatibility/migration modular split metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-b002-semantic-compatibility-migration-checks-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m249-b002-semantic-compatibility-migration-checks-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m249-b002-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m249-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m249/M249-B002/semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract_summary.json`
