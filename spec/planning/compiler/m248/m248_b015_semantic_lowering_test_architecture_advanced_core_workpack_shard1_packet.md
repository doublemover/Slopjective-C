# M248-B015 Semantic/Lowering Test Architecture Advanced Core Workpack (Shard 1) Packet

Packet: `M248-B015`
Milestone: `M248`
Lane: `B`
Freeze date: `2026-03-03`
Issue: `#6815`
Dependencies: `M248-B014`

## Purpose

Freeze lane-B semantic/lowering test architecture advanced core workpack
(shard 1) prerequisites so B014 dependency continuity and advanced core
workpack closure stay explicit, deterministic, and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.
This packet keeps code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_semantic_lowering_test_architecture_advanced_core_workpack_shard1_b015_expectations.md`
- Checker:
  `scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`
- Readiness runner:
  `scripts/run_m248_b015_lane_b_readiness.py`
- Dependency anchors from `M248-B014`:
  - `docs/contracts/m248_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_b014_expectations.md`
  - `spec/planning/compiler/m248/m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
  - `scripts/run_m248_b014_lane_b_readiness.py`
  - `scripts/run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1`
- Canonical readiness command names:
  - `check:objc3c:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract`
  - `test:tooling:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract`
  - `check:objc3c:m248-b015-lane-b-readiness`
  - `check:objc3c:m248-b014-lane-b-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
- `python scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py -q`
- `python scripts/run_m248_b015_lane_b_readiness.py`
- `npm run check:objc3c:m248-b015-lane-b-readiness`

## Evidence Output

- `tmp/reports/m248/M248-B015/semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract_summary.json`
