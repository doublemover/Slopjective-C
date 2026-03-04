# M247-C005 Lowering/Codegen Cost Profiling and Controls Edge-Case and Compatibility Completion Packet

Packet: `M247-C005`
Milestone: `M247`
Lane: `C`
Issue: `#6746`
Freeze date: `2026-03-04`
Dependencies: `M247-C004`

## Objective

Freeze lane-C lowering/codegen cost profiling and controls edge-case and compatibility completion
prerequisites for M247 so predecessor continuity remains explicit,
deterministic, and fail-closed. Code/spec anchors and milestone optimization
improvements are mandatory scope inputs.

## Required Inputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_core_feature_expansion_c004_expectations.md`
- `spec/planning/compiler/m247/m247_c004_lowering_codegen_cost_profiling_and_controls_core_feature_expansion_packet.md`
- `scripts/check_m247_c004_lowering_codegen_cost_profiling_and_controls_core_feature_expansion_contract.py`
- `tests/tooling/test_check_m247_c004_lowering_codegen_cost_profiling_and_controls_core_feature_expansion_contract.py`

## Outputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_c005_expectations.md`
- `scripts/check_m247_c005_lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_contract.py`
- `tests/tooling/test_check_m247_c005_lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_contract.py`
- `scripts/run_m247_c005_lane_c_readiness.py`
- `package.json` (`check:objc3c:m247-c005-lane-c-readiness`)

## Readiness Chain

- `C004 readiness -> C005 checker -> C005 pytest`

## Validation Commands

- `python scripts/check_m247_c005_lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m247_c005_lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_contract.py -q`
- `python scripts/run_m247_c005_lane_c_readiness.py`
- `npm run check:objc3c:m247-c005-lane-c-readiness`

## Evidence

- `tmp/reports/m247/M247-C005/lowering_codegen_cost_profiling_and_controls_edge_case_and_compatibility_completion_contract_summary.json`
