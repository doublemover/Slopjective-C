# M247-C007 Lowering/Codegen Cost Profiling and Controls Diagnostics Hardening Packet

Packet: `M247-C007`
Milestone: `M247`
Lane: `C`
Issue: `#6748`
Freeze date: `2026-03-04`
Dependencies: `M247-C006`

## Objective

Freeze lane-C lowering/codegen cost profiling and controls diagnostics hardening
prerequisites for M247 so predecessor continuity remains explicit,
deterministic, and fail-closed. Code/spec anchors and milestone optimization
improvements are mandatory scope inputs.

## Required Inputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_edge_case_expansion_and_robustness_c006_expectations.md`
- `spec/planning/compiler/m247/m247_c006_lowering_codegen_cost_profiling_and_controls_edge_case_expansion_and_robustness_packet.md`
- `scripts/check_m247_c006_lowering_codegen_cost_profiling_and_controls_edge_case_expansion_and_robustness_contract.py`
- `tests/tooling/test_check_m247_c006_lowering_codegen_cost_profiling_and_controls_edge_case_expansion_and_robustness_contract.py`

## Outputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_c007_expectations.md`
- `scripts/check_m247_c007_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_contract.py`
- `tests/tooling/test_check_m247_c007_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_contract.py`
- `scripts/run_m247_c007_lane_c_readiness.py`
- `package.json` (`check:objc3c:m247-c007-lane-c-readiness`)

## Readiness Chain

- `C006 readiness -> C007 checker -> C007 pytest`

## Validation Commands

- `python scripts/check_m247_c007_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m247_c007_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_contract.py -q`
- `python scripts/run_m247_c007_lane_c_readiness.py`
- `npm run check:objc3c:m247-c007-lane-c-readiness`

## Evidence

- `tmp/reports/m247/M247-C007/lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_contract_summary.json`

