# M248 Replay Harness and Artifact Contracts Edge-Case and Compatibility Completion Expectations (C005)

Contract ID: `objc3c-replay-harness-artifact-contracts-edge-case-and-compatibility-completion/m248-c005-v1`
Status: Accepted
Dependencies: `M248-C004`
Scope: lane-C replay harness/artifact edge-case and compatibility completion governance with fail-closed continuity from C004.

## Objective

Complete lane-C replay harness and artifact contracts with explicit
edge-case/compatibility closure so readiness remains deterministic and fails
closed on dependency drift, readiness-chain drift, or replay evidence drift.

## Dependency Scope

- Issue `#6821` defines canonical lane-C edge-case and compatibility completion scope.
- `M248-C004` assets remain mandatory prerequisites:
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_core_feature_expansion_c004_expectations.md`
  - `scripts/check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py`
  - `spec/planning/compiler/m248/m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_packet.md`
- C005 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m248/m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_contract.py`

## Deterministic Invariants

1. lane-C edge-case/compatibility completion dependency references remain explicit
   and fail closed when dependency tokens drift.
2. Readiness command chain enforces `M248-C004` before `M248-C005`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-c005-replay-harness-artifact-contracts-edge-case-and-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m248-c005-replay-harness-artifact-contracts-edge-case-and-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m248-c005-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m248-c004-lane-c-readiness`
  - `check:objc3c:m248-c005-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m248-c005-lane-c-readiness`

## Evidence Path

- `tmp/reports/m248/M248-C005/replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_contract_summary.json`
