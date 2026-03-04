# M247-C017 Lowering/Codegen Cost Profiling and Controls Integration Closeout and Gate Sign-Off Packet

Packet: `M247-C017`
Milestone: `M247`
Lane: `D`
Issue: `#6758`
Freeze date: `2026-03-04`
Dependencies: `M247-C016`

## Purpose

Freeze lane-C lowering/codegen cost profiling and controls integration closeout
and gate sign-off prerequisites for M247 so predecessor continuity remains
explicit, deterministic, and fail-closed, including code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_c017_expectations.md`
- Checker:
  `scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py`
- Readiness runner:
  `scripts/run_m247_c017_lane_c_readiness.py`
- Dependency anchors from `M247-C016`:
  - `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_c016_expectations.md`
  - `spec/planning/compiler/m247/m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_packet.md`
  - `scripts/check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `scripts/run_m247_c016_lane_c_readiness.py`
- Canonical readiness command names:
  - `check:objc3c:m247-c017-lowering-codegen-cost-profiling-and-controls-integration-closeout-and-gate-signoff-contract`
  - `test:tooling:m247-c017-lowering-codegen-cost-profiling-and-controls-integration-closeout-and-gate-signoff-contract`
  - `check:objc3c:m247-c017-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Readiness Chain

- `C016 readiness -> C017 checker -> C017 pytest`

## Gate Commands

- `python scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py`
- `python scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py -q`
- `python scripts/run_m247_c017_lane_c_readiness.py`
- `npm run check:objc3c:m247-c017-lane-c-readiness`

## Evidence Output

- `tmp/reports/m247/M247-C017/lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract_summary.json`


