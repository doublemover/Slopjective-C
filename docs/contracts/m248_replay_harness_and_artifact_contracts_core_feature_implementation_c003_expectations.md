# M248 Replay Harness and Artifact Contracts Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-replay-harness-artifact-contracts-core-feature-implementation/m248-c003-v1`
Status: Accepted
Scope: M248 lane-C replay harness and artifact contracts core-feature implementation continuity with explicit `M248-C001` and `M248-C002` dependency governance.

## Objective

Fail closed unless lane-C replay harness and artifact contracts core-feature
implementation anchors remain explicit, deterministic, and traceable across
dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M248-C001`, `M248-C002`
- Upstream C001/C002 assets remain mandatory prerequisites:
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_contract_freeze_c001_expectations.md`
  - `scripts/check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
  - `tests/tooling/test_check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_modular_split_scaffolding_c002_expectations.md`
  - `scripts/check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`
- C003 checker/test assets remain mandatory:
  - `scripts/check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C replay
  harness/artifact dependency anchors from `M248-C001` and `M248-C002`.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C replay
  harness/artifact fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  replay metadata anchor wording for dependency continuity.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-c003-replay-harness-artifact-contracts-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m248-c003-replay-harness-artifact-contracts-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m248-c003-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m248-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m248/M248-C003/replay_harness_and_artifact_contracts_core_feature_implementation_contract_summary.json`
