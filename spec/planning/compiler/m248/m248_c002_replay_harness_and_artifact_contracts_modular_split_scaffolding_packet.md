# M248-C002 Replay Harness and Artifact Contracts Modular Split/Scaffolding Packet

Packet: `M248-C002`
Milestone: `M248`
Lane: `C`
Freeze date: `2026-03-02`
Dependencies: `M248-C001`

## Purpose

Freeze lane-C replay harness and artifact contracts modular split/scaffolding
continuity for M248 so replay evidence routing and artifact contract boundaries
remain deterministic and fail-closed, with code/spec anchors and milestone
optimization improvements treated as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_replay_harness_and_artifact_contracts_modular_split_scaffolding_c002_expectations.md`
- Checker:
  `scripts/check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`
- Dependency anchors (`M248-C001`):
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_contract_freeze_c001_expectations.md`
  - `spec/planning/compiler/m248/m248_c001_replay_harness_and_artifact_contracts_contract_freeze_packet.md`
  - `scripts/check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
  - `tests/tooling/test_check_m248_c001_replay_harness_and_artifact_contracts_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract`
  - `test:tooling:m248-c002-replay-harness-artifact-contracts-modular-split-scaffolding-contract`
  - `check:objc3c:m248-c002-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m248-c001-lane-c-readiness`

## Evidence Output

- `tmp/reports/m248/M248-C002/replay_harness_and_artifact_contracts_modular_split_scaffolding_contract_summary.json`

