# M245 Lowering/IR Portability Contracts Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-lowering-ir-portability-contracts-contract/m245-c001-v1`
Status: Accepted
Scope: M245 lane-C lowering/IR portability contracts contract and architecture freeze for deterministic toolchain portability governance continuity.

## Objective

Fail closed unless lane-C lowering/IR portability contract anchors remain
explicit, deterministic, and traceable across code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c001_lowering_ir_portability_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m245_c001_lowering_ir_portability_contracts_contract.py`
  - `tests/tooling/test_check_m245_c001_lowering_ir_portability_contracts_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M245 lane-C C001
  lowering/IR portability contracts fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C lowering/IR
  portability contracts fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  lowering/IR portability metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m245-c001-lowering-ir-portability-contracts-contract`.
- `package.json` includes
  `test:tooling:m245-c001-lowering-ir-portability-contracts-contract`.
- `package.json` includes `check:objc3c:m245-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c001_lowering_ir_portability_contracts_contract.py`
- `python -m pytest tests/tooling/test_check_m245_c001_lowering_ir_portability_contracts_contract.py -q`
- `npm run check:objc3c:m245-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m245/M245-C001/lowering_ir_portability_contracts_contract_summary.json`


