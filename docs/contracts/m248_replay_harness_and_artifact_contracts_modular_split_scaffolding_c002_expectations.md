# M248 Replay Harness and Artifact Contracts Modular Split/Scaffolding Expectations (C002)

Contract ID: `objc3c-replay-harness-artifact-contracts-modular-split-scaffolding/m248-c002-v1`
Status: Accepted
Scope: M248 lane-C replay harness and artifact contracts modular split/scaffolding continuity with explicit `M248-C001` dependency governance.

## Objective

Fail closed unless lane-C replay harness and artifact contracts modular
split/scaffolding anchors remain explicit, deterministic, and traceable across
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Dependency Scope

- Dependencies: `M248-C001`
- Upstream C001 freeze assets remain mandatory prerequisites:
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_contract_freeze_c001_expectations.md`
  - `spec/planning/compiler/m248/m248_c001_replay_harness_and_artifact_contracts_contract_freeze_packet.md`
  - `scripts/check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
  - `tests/tooling/test_check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
- C002 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m248/m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_packet.md`
  - `scripts/check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C replay
  harness/artifact contract anchors used by `M248-C001`.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C replay harness and
  artifact contract fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C replay
  metadata anchor wording for `M248-C001` dependency continuity.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m248-c002-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m248-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m248/M248-C002/replay_harness_and_artifact_contracts_modular_split_scaffolding_contract_summary.json`

