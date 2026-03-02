# M248 Replay Harness and Artifact Contracts Contract Freeze Expectations (C001)

Contract ID: `objc3c-replay-harness-artifact-contracts-contract/m248-c001-v1`
Status: Accepted
Scope: M248 lane-C replay harness and artifact contract freeze for CI replay governance continuity.

## Objective

Fail closed unless lane-C replay harness and artifact contract anchors remain
explicit, deterministic, and traceable across code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m248/m248_c001_replay_harness_and_artifact_contracts_contract_freeze_packet.md`
  - `scripts/check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
  - `tests/tooling/test_check_m248_c001_replay_harness_and_artifact_contracts_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M248 lane-C C001
  replay harness/artifact fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C replay harness and
  artifact contract fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C replay
  metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-c001-replay-harness-artifact-contracts-contract`.
- `package.json` includes
  `test:tooling:m248-c001-replay-harness-artifact-contracts-contract`.
- `package.json` includes `check:objc3c:m248-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c001_replay_harness_and_artifact_contracts_contract.py -q`
- `npm run check:objc3c:m248-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m248/M248-C001/replay_harness_and_artifact_contracts_contract_summary.json`
