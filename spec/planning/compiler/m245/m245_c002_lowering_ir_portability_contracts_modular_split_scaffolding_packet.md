# M245-C002 Lowering/IR Portability Contracts Modular Split/Scaffolding Packet

Packet: `M245-C002`
Milestone: `M245`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: `M245-C001`

## Purpose

Freeze lane-C lowering/IR portability modular split/scaffolding continuity for
M245 so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_lowering_ir_portability_contracts_modular_split_scaffolding_c002_expectations.md`
- Checker:
  `scripts/check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
- Dependency anchors from `M245-C001`:
  - `docs/contracts/m245_lowering_ir_portability_contracts_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m245/m245_c001_lowering_ir_portability_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m245_c001_lowering_ir_portability_contracts_contract.py`
  - `tests/tooling/test_check_m245_c001_lowering_ir_portability_contracts_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m245-c002-lane-c-readiness`

## Evidence Output

- `tmp/reports/m245/M245-C002/lowering_ir_portability_contracts_modular_split_scaffolding_summary.json`
