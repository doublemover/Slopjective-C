# M245 Lowering/IR Portability Contracts Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-lowering-ir-portability-contracts-core-feature-implementation/m245-c003-v1`
Status: Accepted
Scope: M245 lane-C lowering/IR portability contracts core feature implementation continuity with explicit `M245-C001` and `M245-C002` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts core-feature
implementation anchors remain explicit, deterministic, and traceable across
dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M245-C001`, `M245-C002`
- Upstream C001/C002 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m245/m245_c001_lowering_ir_portability_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m245_c001_lowering_ir_portability_contracts_contract.py`
  - `tests/tooling/test_check_m245_c001_lowering_ir_portability_contracts_contract.py`
  - `docs/contracts/m245_lowering_ir_portability_contracts_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m245/m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_packet.md`
  - `scripts/check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
- C003 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c003_lowering_ir_portability_contracts_core_feature_implementation_packet.md`
  - `scripts/check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C lowering/IR
  portability contracts core-feature dependency anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C lowering/IR
  portability contracts core-feature fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  lowering/IR portability core-feature metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m245-c003-lowering-ir-portability-contracts-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m245-c003-lowering-ir-portability-contracts-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m245-c003-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m245-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m245/M245-C003/lowering_ir_portability_contracts_core_feature_implementation_summary.json`


