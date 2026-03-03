# M245 Lowering/IR Portability Contracts Modular Split/Scaffolding Expectations (C002)

Contract ID: `objc3c-lowering-ir-portability-contracts-modular-split-scaffolding/m245-c002-v1`
Status: Accepted
Scope: M245 lane-C modular split/scaffolding continuity for lowering/IR portability contracts dependency wiring.

## Objective

Fail closed unless lane-C lowering/IR portability modular split/scaffolding
anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M245-C001`
- M245-C001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m245/m245_c001_lowering_ir_portability_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m245_c001_lowering_ir_portability_contracts_contract.py`
  - `tests/tooling/test_check_m245_c001_lowering_ir_portability_contracts_contract.py`
- Packet/checker/test assets for C002 remain mandatory:
  - `spec/planning/compiler/m245/m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_packet.md`
  - `scripts/check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M245 lane-C C002
  lowering/IR portability modular split/scaffolding anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C lowering/IR
  portability modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  lowering/IR portability modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m245-c002-lowering-ir-portability-contracts-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m245-c002-lowering-ir-portability-contracts-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m245-c002-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m245-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m245/M245-C002/lowering_ir_portability_contracts_modular_split_scaffolding_summary.json`
