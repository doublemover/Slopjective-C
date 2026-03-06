# M228-B014 Ownership-Aware Lowering Behavior Release-Candidate and Replay Dry-Run Packet

Packet: `M228-B014`
Milestone: `M228`
Lane: `B`
Freeze date: `2026-03-06`
Issue: `#5208`
Dependencies: `M228-B013`

## Scope

Freeze lane-B ownership-aware lowering release/replay governance so dependency,
command, and evidence continuity remains deterministic and fail-closed on top
of B013 docs/runbook synchronization.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m228_ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_b014_expectations.md`
- Operator runbook:
  `docs/runbooks/m228_wave_execution_runbook.md`
- Checker:
  `scripts/check_m228_b014_ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_b014_ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_contract.py`
- Dependency anchors (`M228-B013`):
  - `docs/contracts/m228_ownership_aware_lowering_behavior_docs_operator_runbook_sync_b013_expectations.md`
  - `scripts/check_m228_b013_ownership_aware_lowering_behavior_docs_operator_runbook_sync_contract.py`
  - `tests/tooling/test_check_m228_b013_ownership_aware_lowering_behavior_docs_operator_runbook_sync_contract.py`
  - `spec/planning/compiler/m228/m228_b013_ownership_aware_lowering_behavior_docs_operator_runbook_sync_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m228-b013-lane-b-readiness`
  - `check:objc3c:m228-b014-ownership-aware-lowering-behavior-release-candidate-and-replay-dry-run-contract`
  - `test:tooling:m228-b014-ownership-aware-lowering-behavior-release-candidate-and-replay-dry-run-contract`
  - `run:objc3c:m228-b014-ownership-aware-lowering-behavior-release-replay-dry-run`
  - `check:objc3c:m228-b014-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-smoke`
- `test:objc3c:lowering-replay-proof`

## Required Evidence

- `tmp/reports/m228/M228-B014/ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_contract_summary.json`

## Gate Commands

- `python scripts/check_m228_b013_ownership_aware_lowering_behavior_docs_operator_runbook_sync_contract.py`
- `python scripts/check_m228_b014_ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b014_ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_contract.py -q`
- `npm run run:objc3c:m228-b014-ownership-aware-lowering-behavior-release-replay-dry-run`
- `npm run check:objc3c:m228-b014-lane-b-readiness`
