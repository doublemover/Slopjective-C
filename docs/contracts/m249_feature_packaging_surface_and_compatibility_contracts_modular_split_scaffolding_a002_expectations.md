# M249 Feature Packaging Surface and Compatibility Contracts Modular Split/Scaffolding Expectations (A002)

Contract ID: `objc3c-feature-packaging-surface-compatibility-contracts-modular-split-scaffolding/m249-a002-v1`
Status: Accepted
Scope: M249 lane-A modular split/scaffolding continuity for feature packaging surface and compatibility contracts dependency wiring.

## Objective

Fail closed unless lane-A modular split/scaffolding dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M249-A001`
- M249-A001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_contract_freeze_a001_expectations.md`
  - `spec/planning/compiler/m249/m249_a001_feature_packaging_surface_and_compatibility_contracts_contract_freeze_packet.md`
  - `scripts/check_m249_a001_feature_packaging_surface_and_compatibility_contracts_contract.py`
  - `tests/tooling/test_check_m249_a001_feature_packaging_surface_and_compatibility_contracts_contract.py`
- Packet/checker/test assets for A002 remain mandatory:
  - `spec/planning/compiler/m249/m249_a002_feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_packet.md`
  - `scripts/check_m249_a002_feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m249_a002_feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M249 lane-A A002 feature packaging modular split/scaffolding anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A feature packaging modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A feature packaging modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-a002-feature-packaging-surface-compatibility-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m249-a002-feature-packaging-surface-compatibility-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m249-a002-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m249_a002_feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m249_a002_feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m249-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m249/M249-A002/feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_summary.json`
