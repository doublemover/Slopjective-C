# M248 Replay Harness and Artifact Contracts Recovery and Determinism Hardening Expectations (C008)

Contract ID: `objc3c-replay-harness-artifact-contracts-recovery-and-determinism-hardening/m248-c008-v1`
Status: Accepted
Dependencies: `M248-C007`
Scope: lane-C replay harness/artifact recovery and determinism hardening governance with fail-closed continuity from C007.

## Objective

Expand lane-C replay harness and artifact contracts on top of C007 diagnostics
hardening assets so readiness continuity remains deterministic and fails closed
on dependency drift, readiness-chain drift, or replay evidence drift.

## Dependency Scope

- Issue `#6824` defines canonical lane-C recovery and determinism hardening scope.
- `M248-C007` assets remain mandatory prerequisites:
  - `docs/contracts/m248_replay_harness_and_artifact_contracts_diagnostics_hardening_c007_expectations.md`
  - `scripts/check_m248_c007_replay_harness_and_artifact_contracts_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m248_c007_replay_harness_and_artifact_contracts_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m248/m248_c007_replay_harness_and_artifact_contracts_diagnostics_hardening_packet.md`
- C008 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m248/m248_c008_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m248_c008_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m248_c008_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_contract.py`

## Deterministic Invariants

1. lane-C recovery and determinism hardening dependency references remain
   explicit and fail closed when dependency tokens drift.
2. Readiness command chain enforces `M248-C007` before `M248-C008`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-c008-replay-harness-artifact-contracts-recovery-and-determinism-hardening-contract`.
- `package.json` includes
  `test:tooling:m248-c008-replay-harness-artifact-contracts-recovery-and-determinism-hardening-contract`.
- `package.json` includes `check:objc3c:m248-c008-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m248-c007-lane-c-readiness`
  - `check:objc3c:m248-c008-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m248_c008_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m248_c008_replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m248-c008-lane-c-readiness`

## Evidence Path

- `tmp/reports/m248/M248-C008/replay_harness_and_artifact_contracts_recovery_and_determinism_hardening_contract_summary.json`
