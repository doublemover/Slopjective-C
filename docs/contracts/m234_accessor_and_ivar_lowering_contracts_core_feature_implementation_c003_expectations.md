# M234 Accessor and Ivar Lowering Contracts Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-core-feature-implementation/m234-c003-v1`
Status: Accepted
Scope: M234 lane-C accessor/ivar lowering contracts core feature implementation continuity with explicit `M234-C001` and `M234-C002` dependency governance.

## Objective

Fail closed unless lane-C accessor/ivar lowering contracts core-feature
implementation anchors remain explicit, deterministic, and traceable across
dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5721` defines canonical lane-C core feature implementation scope.
- Dependencies: `M234-C001`, `M234-C002`
- Upstream C001/C002 assets remain mandatory prerequisites:
  - `docs/contracts/m234_accessor_and_ivar_lowering_contracts_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m234/m234_c001_accessor_and_ivar_lowering_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
  - `tests/tooling/test_check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
  - `docs/contracts/m234_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m234/m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_packet.md`
  - `scripts/check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`
- C003 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m234/m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_packet.md`
  - `scripts/check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C accessor/ivar
  lowering contracts core-feature dependency anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C accessor/ivar
  lowering contracts core-feature fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  accessor/ivar lowering core-feature metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-c003-accessor-and-ivar-lowering-contracts-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m234-c003-accessor-and-ivar-lowering-contracts-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m234-c003-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m234-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m234/M234-C003/accessor_and_ivar_lowering_contracts_core_feature_implementation_summary.json`
