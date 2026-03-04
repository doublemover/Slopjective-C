# M247-C002 Lowering/Codegen Cost Profiling and Controls Modular Split/Scaffolding Packet

Packet: `M247-C002`
Milestone: `M247`
Lane: `C`
Issue: `#6743`
Freeze date: `2026-03-04`
Dependencies: `M247-C001`

## Objective

Freeze lane-C lowering/codegen cost profiling and controls modular split and
scaffolding prerequisites for M247 so predecessor continuity remains explicit,
deterministic, and fail-closed. Code/spec anchors and milestone optimization
improvements are mandatory scope inputs.

## Required Inputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_c001_expectations.md`
- `spec/planning/compiler/m247/m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_packet.md`
- `scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py`
- `tests/tooling/test_check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py`

## Outputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_c002_expectations.md`
- `scripts/check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py`
- `tests/tooling/test_check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py`
- `scripts/run_m247_c002_lane_c_readiness.py`
- `package.json` (`check:objc3c:m247-c002-lane-c-readiness`)

## Readiness Chain

- `C001 readiness -> C002 checker -> C002 pytest`

## Validation Commands

- `python scripts/check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py -q`
- `python scripts/run_m247_c002_lane_c_readiness.py`
- `npm run check:objc3c:m247-c002-lane-c-readiness`

## Evidence

- `tmp/reports/m247/M247-C002/lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract_summary.json`

