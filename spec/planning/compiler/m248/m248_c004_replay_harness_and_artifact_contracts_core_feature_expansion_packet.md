# M248-C004 Replay Harness and Artifact Contracts Core Feature Expansion Packet

Packet: `M248-C004`
Milestone: `M248`
Lane: `C`
Issue: `#6820`
Dependencies: `M248-C003`

## Purpose

Execute lane-C replay harness and artifact contracts core-feature expansion
governance on top of C003 core-feature implementation assets so downstream
expansion and closeout integration remain deterministic and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m248_replay_harness_and_artifact_contracts_core_feature_expansion_c004_expectations.md`
- Checker:
  `scripts/check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m248-c004-replay-harness-artifact-contracts-core-feature-expansion-contract`
  - `test:tooling:m248-c004-replay-harness-artifact-contracts-core-feature-expansion-contract`
  - `check:objc3c:m248-c004-lane-c-readiness`

## Dependency Anchors (M248-C003)

- `docs/contracts/m248_replay_harness_and_artifact_contracts_core_feature_implementation_c003_expectations.md`
- `scripts/check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py`
- `tests/tooling/test_check_m248_c003_replay_harness_and_artifact_contracts_core_feature_implementation_contract.py`

## Gate Commands

- `python scripts/check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m248-c004-lane-c-readiness`

## Evidence Output

- `tmp/reports/m248/M248-C004/replay_harness_and_artifact_contracts_core_feature_expansion_contract_summary.json`
