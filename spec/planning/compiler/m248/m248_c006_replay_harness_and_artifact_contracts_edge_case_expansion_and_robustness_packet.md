# M248-C006 Replay Harness and Artifact Contracts Edge-Case Expansion and Robustness Packet

Packet: `M248-C006`
Milestone: `M248`
Lane: `C`
Issue: `#6822`
Dependencies: `M248-C005`

## Purpose

Execute lane-C replay harness and artifact contracts edge-case expansion and
robustness governance on top of C005 edge-case/compatibility completion
assets so dependency continuity and replay-evidence readiness remain
deterministic and fail-closed.

## Scope Anchors

- Contract:
  `docs/contracts/m248_replay_harness_and_artifact_contracts_edge_case_expansion_and_robustness_c006_expectations.md`
- Checker:
  `scripts/check_m248_c006_replay_harness_and_artifact_contracts_edge_case_expansion_and_robustness_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_c006_replay_harness_and_artifact_contracts_edge_case_expansion_and_robustness_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m248-c006-replay-harness-artifact-contracts-edge-case-expansion-robustness-contract`
  - `test:tooling:m248-c006-replay-harness-artifact-contracts-edge-case-expansion-robustness-contract`
  - `check:objc3c:m248-c006-lane-c-readiness`

## Dependency Anchors (M248-C005)

- `docs/contracts/m248_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_c005_expectations.md`
- `scripts/check_m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_contract.py`
- `tests/tooling/test_check_m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_contract.py`
- `spec/planning/compiler/m248/m248_c005_replay_harness_and_artifact_contracts_edge_case_and_compatibility_completion_packet.md`

## Gate Commands

- `python scripts/check_m248_c006_replay_harness_and_artifact_contracts_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c006_replay_harness_and_artifact_contracts_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m248-c006-lane-c-readiness`

## Evidence Output

- `tmp/reports/m248/M248-C006/replay_harness_and_artifact_contracts_edge_case_expansion_and_robustness_contract_summary.json`
