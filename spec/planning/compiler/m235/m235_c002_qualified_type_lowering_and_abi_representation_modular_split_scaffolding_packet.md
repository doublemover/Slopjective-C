# M235-C002 Qualified Type Lowering and ABI Representation Modular Split/Scaffolding Packet

Packet: `M235-C002`
Milestone: `M235`
Lane: `C`
Issue: `#5812`
Freeze date: `2026-03-05`
Dependencies: `M235-C001`

## Purpose

Freeze lane-C qualified type lowering and ABI representation modular
split/scaffolding continuity for M235 so dependency wiring remains deterministic
and fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_c002_expectations.md`
- Checker:
  `scripts/check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`
- Dependency anchors from `M235-C001`:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m235/m235_c001_qualified_type_lowering_and_abi_representation_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m235_c001_qualified_type_lowering_and_abi_representation_contract.py`
  - `tests/tooling/test_check_m235_c001_qualified_type_lowering_and_abi_representation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Readiness Chain

- `C001 readiness -> C002 checker -> C002 pytest`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `npm run check:objc3c:m235-c001-lane-c-readiness`
- `python scripts/check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py -q`

## Evidence Output

- `tmp/reports/m235/M235-C002/qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract_summary.json`
