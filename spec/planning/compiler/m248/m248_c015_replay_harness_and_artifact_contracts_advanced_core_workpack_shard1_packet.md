# M248-C015 Replay Harness and Artifact Contracts Advanced Core Workpack (Shard 1) Packet

Packet: `M248-C015`
Milestone: `M248`
Lane: `C`
Issue: `#6831`
Dependencies: `M248-C014`

## Purpose

Add lane-C advanced core workpack (shard 1) contract governance on top of the
C014 release/replay dry-run closure so dependency chaining and readiness wiring
remain deterministic and fail closed against drift.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_c015_expectations.md`
- Checker:
  `scripts/check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py`
- Readiness runner:
  `scripts/run_m248_c015_lane_c_readiness.py`
- Package wiring:
  - `check:objc3c:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract`
  - `test:tooling:m248-c015-replay-harness-artifact-contracts-advanced-core-workpack-shard1-contract`
  - `check:objc3c:m248-c015-lane-c-readiness`

## Dependency Anchors (M248-C014)

- `docs/contracts/m248_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_c014_expectations.md`
- `spec/planning/compiler/m248/m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_packet.md`
- `scripts/check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py`
- `tests/tooling/test_check_m248_c014_replay_harness_and_artifact_contracts_release_candidate_and_replay_dry_run_contract.py`
- `scripts/run_m248_c014_lane_c_readiness.py`

## Gate Commands

- `python scripts/run_m248_c014_lane_c_readiness.py`
- `python scripts/check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py`
- `python scripts/check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m248_c015_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract.py -q`
- `python scripts/run_m248_c015_lane_c_readiness.py`

## Required Evidence

- `tmp/reports/m248/M248-C015/replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_contract_summary.json`
