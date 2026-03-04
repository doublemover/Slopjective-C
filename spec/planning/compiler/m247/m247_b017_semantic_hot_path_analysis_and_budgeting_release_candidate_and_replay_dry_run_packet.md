# M247-B017 Semantic Hot-Path Analysis and Budgeting Release-Candidate and Replay Dry-Run Packet

Packet: `M247-B017`
Milestone: `M247`
Lane: `B`
Freeze date: `2026-03-04`
Issue: `#6740`
Dependencies: `M247-B016`

## Purpose

Freeze lane-B semantic hot-path analysis and budgeting release-candidate/replay dry-run prerequisites so `M247-B016` dependency continuity stays explicit, deterministic, and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m247_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_b017_expectations.md`
- Checker:
  `scripts/check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`
- Readiness runner:
  `scripts/run_m247_b017_lane_b_readiness.py`
- Dependency anchors from `M247-B016`:
  - `docs/contracts/m247_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_b016_expectations.md`
  - `spec/planning/compiler/m247/m247_b016_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_packet.md`
  - `scripts/check_m247_b016_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m247_b016_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `scripts/run_m247_b016_lane_b_readiness.py`
- Canonical readiness command names:
  - `check:objc3c:m247-b017-semantic-hot-path-analysis-and-budgeting-release-candidate-and-replay-dry-run-contract`
  - `test:tooling:m247-b017-semantic-hot-path-analysis-and-budgeting-release-candidate-and-replay-dry-run-contract`
  - `check:objc3c:m247-b017-lane-b-readiness`
  - `check:objc3c:m247-b016-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `compile:objc3c`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py -q`
- `python scripts/run_m247_b017_lane_b_readiness.py`
- `npm run check:objc3c:m247-b017-lane-b-readiness`

## Evidence Output

- `tmp/reports/m247/M247-B017/semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract_summary.json`
