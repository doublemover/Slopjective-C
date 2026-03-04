# M248 Replay Harness and Artifact Contracts Advanced Conformance Workpack (Shard 1) Expectations (C018)

Contract ID: `objc3c-replay-harness-and-artifact-contracts-advanced-conformance-workpack-shard1/m248-c018-v1`
Status: Accepted
Dependencies: `M248-C017`
Scope: lane-C replay harness/artifact advanced conformance workpack (shard 1) closure with fail-closed dependency chaining from C017.

## Objective

Execute issue `#6834` by extending lane-C replay harness/artifact contract
governance with an advanced conformance workpack shard that keeps
dependency anchors explicit, keeps lane-C readiness chaining deterministic, and
fails closed when contract or wiring drift is detected.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6834` defines canonical lane-C advanced conformance workpack (shard 1) scope.
- `M248-C017` assets remain mandatory prerequisites:
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_advanced_diagnostics_workpack_shard1_c017_expectations.md`
  - `spec/planning/compiler/m248/m248_c017_replay_harness_and_artifact_contracts_advanced_diagnostics_workpack_shard1_packet.md`
  - `scripts/check_m248_c017_replay_harness_and_artifact_contracts_advanced_diagnostics_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m248_c017_replay_harness_and_artifact_contracts_advanced_diagnostics_workpack_shard1_contract.py`
  - `scripts/run_m248_c017_lane_c_readiness.py`

## Deterministic Invariants

1. C018 validation is fail-closed for dependency anchors, packet/expectations
   continuity, package wiring, and lane-C readiness runner chaining.
2. lane-C readiness chain is deterministic and ordered:
   - `python scripts/run_m248_c017_lane_c_readiness.py`
   - `python scripts/check_m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract.py -q`
3. package wiring remains explicit for C018:
   - `check:objc3c:m248-c018-replay-harness-artifact-contracts-advanced-conformance-workpack-shard1-contract`
   - `test:tooling:m248-c018-replay-harness-artifact-contracts-advanced-conformance-workpack-shard1-contract`
   - `check:objc3c:m248-c018-lane-c-readiness`
4. Findings are stable across repeated runs with deterministic failure ordering.
5. Evidence output is persisted under `tmp/reports/m248/M248-C018/`.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m248-c018-replay-harness-artifact-contracts-advanced-conformance-workpack-shard1-contract`
  - `test:tooling:m248-c018-replay-harness-artifact-contracts-advanced-conformance-workpack-shard1-contract`
  - `check:objc3c:m248-c018-lane-c-readiness`
- Readiness runner script:
  - `python scripts/run_m248_c018_lane_c_readiness.py`

## Validation

- `python scripts/check_m248_c017_replay_harness_and_artifact_contracts_advanced_diagnostics_workpack_shard1_contract.py`
- `python scripts/check_m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract.py`
- `python scripts/check_m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m248_c018_replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m248-c018-lane-c-readiness`

## Evidence Path

- `tmp/reports/m248/M248-C018/replay_harness_and_artifact_contracts_advanced_conformance_workpack_shard1_contract_summary.json`


