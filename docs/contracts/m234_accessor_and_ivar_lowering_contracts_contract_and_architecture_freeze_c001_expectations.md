# M234 Accessor and Ivar Lowering Contracts Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-contract/m234-c001-v1`
Status: Accepted
Dependencies: none
Scope: M234 lane-C accessor and ivar lowering contracts contract and architecture freeze for deterministic lowering/governance continuity.

## Objective

Fail closed unless lane-C accessor and ivar lowering contract anchors remain
explicit, deterministic, and traceable across code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5719` defines canonical lane-C contract and architecture freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m234/m234_c001_accessor_and_ivar_lowering_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
  - `tests/tooling/test_check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M234 lane-C C001
  accessor/ivar lowering fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C accessor/ivar
  lowering fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  accessor/ivar lowering metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-c001-accessor-and-ivar-lowering-contracts-contract-and-architecture-freeze-contract`.
- `package.json` includes
  `test:tooling:m234-c001-accessor-and-ivar-lowering-contracts-contract-and-architecture-freeze-contract`.
- `package.json` includes `check:objc3c:m234-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
- `python -m pytest tests/tooling/test_check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py -q`
- `npm run check:objc3c:m234-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m234/M234-C001/accessor_and_ivar_lowering_contracts_contract_summary.json`
