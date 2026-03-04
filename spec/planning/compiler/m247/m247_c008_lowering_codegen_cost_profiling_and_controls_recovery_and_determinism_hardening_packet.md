# M247-C008 Lowering/Codegen Cost Profiling and Controls Recovery and Determinism Hardening Packet

Packet: `M247-C008`
Milestone: `M247`
Lane: `C`
Issue: `#6749`
Freeze date: `2026-03-04`
Dependencies: `M247-C007`

## Objective

Freeze lane-C lowering/codegen cost profiling and controls recovery and determinism hardening
prerequisites for M247 so predecessor continuity remains explicit,
deterministic, and fail-closed. Code/spec anchors and milestone optimization
improvements are mandatory scope inputs.

## Required Inputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_c007_expectations.md`
- `spec/planning/compiler/m247/m247_c007_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_packet.md`
- `scripts/check_m247_c007_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_contract.py`
- `tests/tooling/test_check_m247_c007_lowering_codegen_cost_profiling_and_controls_diagnostics_hardening_contract.py`

## Outputs

- `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_recovery_and_determinism_hardening_c008_expectations.md`
- `scripts/check_m247_c008_lowering_codegen_cost_profiling_and_controls_recovery_and_determinism_hardening_contract.py`
- `tests/tooling/test_check_m247_c008_lowering_codegen_cost_profiling_and_controls_recovery_and_determinism_hardening_contract.py`
- `scripts/run_m247_c008_lane_c_readiness.py`
- `package.json` (`check:objc3c:m247-c008-lane-c-readiness`)

## Readiness Chain

- `C007 readiness -> C008 checker -> C008 pytest`

## Validation Commands

- `python scripts/check_m247_c008_lowering_codegen_cost_profiling_and_controls_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m247_c008_lowering_codegen_cost_profiling_and_controls_recovery_and_determinism_hardening_contract.py -q`
- `python scripts/run_m247_c008_lane_c_readiness.py`
- `npm run check:objc3c:m247-c008-lane-c-readiness`

## Evidence

- `tmp/reports/m247/M247-C008/lowering_codegen_cost_profiling_and_controls_recovery_and_determinism_hardening_contract_summary.json`


