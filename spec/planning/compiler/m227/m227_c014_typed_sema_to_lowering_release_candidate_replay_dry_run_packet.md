# M227-C014 Typed Sema-to-Lowering Release-Candidate and Replay Dry-Run Packet

Packet: `M227-C014`
Milestone: `M227`
Lane: `C`
Issue: `#5134`
Dependencies: `M227-C013`

## Scope

Implement lane-C typed sema-to-lowering release-candidate/replay-dry-run
consistency and readiness by wiring dry-run invariants through typed contract
and parse/lowering readiness surfaces with deterministic fail-closed alignment.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_release_candidate_replay_dry_run_c014_expectations.md`
- Checker:
  `scripts/check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py`
- Dependency anchors (`M227-C013`):
  - `docs/contracts/m227_typed_sema_to_lowering_docs_runbook_sync_c013_expectations.md`
  - `scripts/check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py`
  - `tests/tooling/test_check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py`
  - `spec/planning/compiler/m227/m227_c013_typed_sema_to_lowering_docs_runbook_sync_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c014-typed-sema-to-lowering-release-candidate-replay-dry-run-contract`
  - `test:tooling:m227-c014-typed-sema-to-lowering-release-candidate-replay-dry-run-contract`
  - `check:objc3c:m227-c014-lane-c-readiness`

## Required Evidence

- `tmp/reports/m227/M227-C014/typed_sema_to_lowering_release_candidate_replay_dry_run_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py`
- `python scripts/check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py -q`
- `npm run check:objc3c:m227-c014-lane-c-readiness`
