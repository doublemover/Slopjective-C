# M248 Replay Harness and Artifact Contracts Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-replay-harness-artifact-contracts-core-feature-expansion/m248-c004-v1`
Status: Accepted
Dependencies: `M248-C003`
Scope: lane-C replay harness/artifact core-feature expansion governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Extend lane-C replay harness and artifact contracts governance from C003
core-feature implementation with explicit expansion dependency anchors so
readiness fails closed on dependency, readiness-chain, or replay-evidence drift.

## Dependency Scope

- Issue `#6820` defines canonical lane-C core-feature expansion scope.
- `M248-C003` assets remain mandatory prerequisites:
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_core_feature_implementation_c003_expectations.md`
  - `scripts/check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py`

## Deterministic Invariants

1. lane-C core-feature expansion dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M248-C003` before `M248-C004`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-c004-replay-harness-artifact-contracts-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m248-c004-replay-harness-artifact-contracts-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m248-c004-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m248-c003-lane-c-readiness`
  - `check:objc3c:m248-c004-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m248-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m248/M248-C004/replay_harness_and_artifact_contracts_core_feature_expansion_contract_summary.json`
