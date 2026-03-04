# M247-B018 Semantic Hot-Path Analysis and Budgeting Integration Closeout and Gate Signoff Packet

Packet: `M247-B018`
Milestone: `M247`
Lane: `B`
Freeze date: `2026-03-04`
Issue: `#6741`
Dependencies: `M247-B017`

## Purpose

Freeze lane-B semantic hot-path analysis and budgeting integration closeout/gate signoff prerequisites so `M247-B017` dependency continuity stays explicit, deterministic, and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m247_semantic_hot_path_analysis_and_budgeting_integration_closeout_and_gate_signoff_b018_expectations.md`
- Checker:
  `scripts/check_m247_b018_semantic_hot_path_analysis_and_budgeting_integration_closeout_and_gate_signoff_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m247_b018_semantic_hot_path_analysis_and_budgeting_integration_closeout_and_gate_signoff_contract.py`
- Readiness runner:
  `scripts/run_m247_b018_lane_b_readiness.py`
- Dependency anchors from `M247-B017`:
  - `docs/contracts/m247_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_b017_expectations.md`
  - `spec/planning/compiler/m247/m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`
  - `scripts/run_m247_b017_lane_b_readiness.py`
- Canonical readiness command names:
  - `check:objc3c:m247-b018-semantic-hot-path-analysis-and-budgeting-integration-closeout-and-gate-signoff-contract`
  - `test:tooling:m247-b018-semantic-hot-path-analysis-and-budgeting-integration-closeout-and-gate-signoff-contract`
  - `check:objc3c:m247-b018-lane-b-readiness`
  - `check:objc3c:m247-b017-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `compile:objc3c`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m247_b018_semantic_hot_path_analysis_and_budgeting_integration_closeout_and_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m247_b018_semantic_hot_path_analysis_and_budgeting_integration_closeout_and_gate_signoff_contract.py -q`
- `python scripts/run_m247_b018_lane_b_readiness.py`
- `npm run check:objc3c:m247-b018-lane-b-readiness`

## Evidence Output

- `tmp/reports/m247/M247-B018/semantic_hot_path_analysis_and_budgeting_integration_closeout_and_gate_signoff_contract_summary.json`
