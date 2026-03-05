# M234 Accessor and Ivar Lowering Contracts Modular Split/Scaffolding Expectations (C002)

Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-modular-split-scaffolding/m234-c002-v1`
Status: Accepted
Dependencies: `M234-C001`
Scope: M234 lane-C modular split/scaffolding continuity for accessor/ivar lowering dependency wiring.

## Objective

Fail closed unless lane-C accessor/ivar lowering modular split/scaffolding
anchors remain explicit, deterministic, and traceable across code/spec anchors
and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5720` defines canonical lane-C modular split and scaffolding scope.
- Dependencies: `M234-C001`
- M234-C001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m234_accessor_and_ivar_lowering_contracts_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m234/m234_c001_accessor_and_ivar_lowering_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
  - `tests/tooling/test_check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
- Packet/checker/test assets for C002 remain mandatory:
  - `spec/planning/compiler/m234/m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_packet.md`
  - `scripts/check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M234 lane-C C002
  accessor/ivar lowering modular split/scaffolding anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C accessor/ivar
  lowering modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  accessor/ivar lowering modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-c002-accessor-and-ivar-lowering-contracts-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m234-c002-accessor-and-ivar-lowering-contracts-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m234-c002-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m234-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m234/M234-C002/accessor_and_ivar_lowering_contracts_modular_split_scaffolding_summary.json`
