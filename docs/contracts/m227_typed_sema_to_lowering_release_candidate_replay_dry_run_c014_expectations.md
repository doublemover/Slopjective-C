# M227 Typed Sema-to-Lowering Release-Candidate and Replay Dry-Run Expectations (C014)

Contract ID: `objc3c-typed-sema-to-lowering-release-candidate-replay-dry-run/m227-c014-v1`
Status: Accepted
Scope: typed sema-to-lowering release-candidate and replay dry-run synchronization on top of C013 docs/runbook synchronization.

## Objective

Execute issue `#5134` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with release-candidate/replay-dry-run
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C013`
- `M227-C013` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_docs_runbook_sync_c013_expectations.md`
  - `scripts/check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py`
  - `tests/tooling/test_check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py`
  - `spec/planning/compiler/m227/m227_c013_typed_sema_to_lowering_docs_runbook_sync_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed release-candidate/replay-dry-run fields:
   - `typed_release_candidate_replay_dry_run_consistent`
   - `typed_release_candidate_replay_dry_run_ready`
   - `typed_release_candidate_replay_dry_run_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed release-candidate/replay-dry-run fields:
   - `typed_sema_release_candidate_replay_dry_run_consistent`
   - `typed_sema_release_candidate_replay_dry_run_ready`
   - `typed_sema_release_candidate_replay_dry_run_key`
3. Parse/lowering readiness fails closed when typed release-candidate/replay
   dry-run alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c014-typed-sema-to-lowering-release-candidate-replay-dry-run-contract`
  - `test:tooling:m227-c014-typed-sema-to-lowering-release-candidate-replay-dry-run-contract`
  - `check:objc3c:m227-c014-lane-c-readiness`

## Validation

- `python scripts/check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py`
- `python scripts/check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py -q`
- `npm run check:objc3c:m227-c014-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C014/typed_sema_to_lowering_release_candidate_replay_dry_run_contract_summary.json`
