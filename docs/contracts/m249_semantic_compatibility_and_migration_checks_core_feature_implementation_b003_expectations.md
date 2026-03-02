# M249 Semantic Compatibility and Migration Checks Core Feature Implementation Expectations (B003)

Contract ID: `objc3c-semantic-compatibility-and-migration-checks-core-feature-implementation/m249-b003-v1`
Status: Accepted
Scope: M249 lane-B core feature implementation continuity for semantic compatibility and migration checks dependency wiring.

## Objective

Fail closed unless lane-B core feature implementation dependency anchors remain
explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M249-B002`
- M249-B002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m249_semantic_compatibility_and_migration_checks_modular_split_scaffolding_b002_expectations.md`
  - `spec/planning/compiler/m249/m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_packet.md`
  - `scripts/check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_contract.py`
- Packet/checker/test assets for B003 remain mandatory:
  - `spec/planning/compiler/m249/m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_packet.md`
  - `scripts/check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M249 lane-B B003 semantic compatibility/migration core feature implementation anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic compatibility/migration core feature implementation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B semantic compatibility/migration core feature metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-b003-semantic-compatibility-migration-checks-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m249-b003-semantic-compatibility-migration-checks-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m249-b003-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m249-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m249/M249-B003/semantic_compatibility_and_migration_checks_core_feature_implementation_summary.json`
