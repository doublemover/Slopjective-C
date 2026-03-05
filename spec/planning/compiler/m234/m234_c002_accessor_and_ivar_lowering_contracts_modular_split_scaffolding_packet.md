# M234-C002 Accessor and Ivar Lowering Contracts Modular Split/Scaffolding Packet

Packet: `M234-C002`
Milestone: `M234`
Lane: `C`
Issue: `#5720`
Freeze date: `2026-03-05`
Dependencies: `M234-C001`

## Purpose

Freeze lane-C accessor/ivar lowering modular split/scaffolding continuity for
M234 so dependency wiring remains deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m234_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_c002_expectations.md`
- Checker:
  `scripts/check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`
- Dependency anchors from `M234-C001`:
  - `docs/contracts/m234_accessor_and_ivar_lowering_contracts_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m234/m234_c001_accessor_and_ivar_lowering_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
  - `tests/tooling/test_check_m234_c001_accessor_and_ivar_lowering_contracts_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m234_c002_accessor_and_ivar_lowering_contracts_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m234-c002-lane-c-readiness`

## Evidence Output

- `tmp/reports/m234/M234-C002/accessor_and_ivar_lowering_contracts_modular_split_scaffolding_summary.json`
