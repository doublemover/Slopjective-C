# M248 Semantic/Lowering Test Architecture Advanced Core Workpack (Shard 1) Expectations (B015)

Contract ID: `objc3c-semantic-lowering-test-architecture-advanced-core-workpack-shard1/m248-b015-v1`
Status: Accepted
Scope: M248 lane-B advanced core workpack (shard 1) continuity for semantic/lowering test architecture dependency wiring.

## Objective

Fail closed unless lane-B advanced core workpack (shard 1) dependency anchors
remain explicit, deterministic, and traceable across dependency surfaces,
including code/spec anchors and milestone optimization improvements as
mandatory scope inputs.

## Dependency Scope

- Dependencies: `M248-B014`
- Issue `#6815` defines canonical lane-B advanced core workpack (shard 1) scope.
- M248-B014 release-candidate and replay dry-run anchors remain mandatory prerequisites:
  - `docs/contracts/m248_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_b014_expectations.md`
  - `spec/planning/compiler/m248/m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
  - `scripts/run_m248_b014_lane_b_readiness.py`
  - `scripts/run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1`
- Packet/checker/test/readiness assets for B015 remain mandatory:
  - `spec/planning/compiler/m248/m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_packet.md`
  - `scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`
  - `scripts/run_m248_b015_lane_b_readiness.py`

## Deterministic Invariants

1. Lane-B advanced core workpack (shard 1) dependency references remain explicit
   and fail closed when dependency tokens drift.
2. Lane-B readiness chaining remains deterministic and fail-closed:
   - `python scripts/run_m248_b014_lane_b_readiness.py`
   - `python scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py -q`
3. `package.json` lane-B command wiring remains explicit and deterministic for:
   - `check:objc3c:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract`
   - `test:tooling:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract`
   - `check:objc3c:m248-b015-lane-b-readiness`
4. Readiness execution for B015 remains Python-driven (no nested npm chaining inside
   the readiness runner implementation).
5. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- Canonical command names for this contract:
  - `check:objc3c:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract`
  - `test:tooling:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract`
  - `check:objc3c:m248-b015-lane-b-readiness`
- lane-B readiness chaining expected by this contract remains deterministic and fail-closed:
  - `check:objc3c:m248-b014-lane-b-readiness`
  - `check:objc3c:m248-b015-lane-b-readiness`

## Milestone Optimization Inputs

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Validation

- `python scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
- `python scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py -q`
- `python scripts/run_m248_b015_lane_b_readiness.py`
- `npm run check:objc3c:m248-b015-lane-b-readiness`

## Evidence Path

- `tmp/reports/m248/M248-B015/semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract_summary.json`
