# M247-E015 Performance SLO Gate and Reporting Advanced Core Workpack (Shard 1) Packet

Packet: `M247-E015`
Milestone: `M247`
Lane: `E`
Issue: `#6786`
Freeze date: `2026-03-04`
Dependencies: `M247-E014`, `M247-A015`, `M247-B017`, `M247-C016`, `M247-D012`

## Purpose

Freeze lane-E advanced core workpack (shard 1) prerequisites for M247
performance SLO gate and reporting continuity so dependency wiring remains
deterministic and fail-closed, including lane readiness-chain continuity and
issue-local evidence. Code/spec anchors and milestone optimization improvements
are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_e015_expectations.md`
- Checker:
  `scripts/check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py`
- Lane-E readiness runner:
  `scripts/run_m247_e015_lane_e_readiness.py`
- Tooling tests:
  `tests/tooling/test_check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py`
- Dependency anchors from completed lane-E stage:
  - `docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_e014_expectations.md`
  - `spec/planning/compiler/m247/m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py`
  - `scripts/run_m247_e014_lane_e_readiness.py`
- Dependency anchors from lane-A advanced core workpack (shard 1):
  - `docs/contracts/m247_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_a015_expectations.md`
  - `spec/planning/compiler/m247/m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_packet.md`
  - `scripts/check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py`
  - `scripts/run_m247_a015_lane_a_readiness.py`
- Dependency anchors from lane-B release-candidate and replay dry-run:
  - `docs/contracts/m247_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_b017_expectations.md`
  - `spec/planning/compiler/m247/m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`
  - `scripts/run_m247_b017_lane_b_readiness.py`
- Dependency anchors from lane-C advanced edge compatibility workpack (shard 1):
  - `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_c016_expectations.md`
  - `spec/planning/compiler/m247/m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_packet.md`
  - `scripts/check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `scripts/run_m247_c016_lane_c_readiness.py`
- Dependency anchors from lane-D cross-lane integration sync:
  - `docs/contracts/m247_runtime_link_build_throughput_optimization_cross_lane_integration_sync_d012_expectations.md`
  - `spec/planning/compiler/m247/m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_packet.md`
  - `scripts/check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py`
  - `tests/tooling/test_check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py`
  - `scripts/run_m247_d012_lane_d_readiness.py`
## Gate Sequence

- `E014 readiness -> E015 checker -> E015 pytest`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py`
- `python scripts/check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py -q`
- `python scripts/run_m247_e015_lane_e_readiness.py`

## Evidence Output

- `tmp/reports/m247/M247-E015/performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract_summary.json`
