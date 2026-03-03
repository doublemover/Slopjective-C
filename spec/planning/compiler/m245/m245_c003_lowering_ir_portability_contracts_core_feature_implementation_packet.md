# M245-C003 Lowering/IR Portability Contracts Core Feature Implementation Packet

Packet: `M245-C003`
Milestone: `M245`
Lane: `C`
Freeze date: `2026-03-02`
Dependencies: `M245-C001`, `M245-C002`

## Purpose

Freeze lane-C lowering/IR portability contracts core-feature implementation
continuity for M245 so lowering portability boundaries and IR portability
continuity remain deterministic and fail-closed, with dependency surfaces,
code/spec anchors, and milestone optimization improvements treated as mandatory
scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_lowering_ir_portability_contracts_core_feature_implementation_c003_expectations.md`
- Checker:
  `scripts/check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
- Dependency anchors (`M245-C001`):
  - `docs/contracts/m245_lowering_ir_portability_contracts_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m245/m245_c001_lowering_ir_portability_contracts_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m245_c001_lowering_ir_portability_contracts_contract.py`
  - `tests/tooling/test_check_m245_c001_lowering_ir_portability_contracts_contract.py`
- Dependency anchors (`M245-C002`):
  - `docs/contracts/m245_lowering_ir_portability_contracts_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m245/m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_packet.md`
  - `scripts/check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m245_c002_lowering_ir_portability_contracts_modular_split_scaffolding_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m245-c003-lowering-ir-portability-contracts-core-feature-implementation-contract`
  - `test:tooling:m245-c003-lowering-ir-portability-contracts-core-feature-implementation-contract`
  - `check:objc3c:m245-c003-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m245-c003-lane-c-readiness`

## Evidence Output

- `tmp/reports/m245/M245-C003/lowering_ir_portability_contracts_core_feature_implementation_summary.json`


