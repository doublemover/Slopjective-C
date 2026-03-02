# M226 Parser-Sema Release-Candidate and Replay Dry-Run Expectations (B014)

Contract ID: `objc3c-parser-sema-release-candidate-replay-dry-run-contract/m226-b014-v1`
Status: Accepted
Scope: Parser->sema release-candidate replay dry-run synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema release-candidate replay dry-run synchronization so docs/runbook surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Release-candidate replay dry-run sync surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaReleaseCandidateReplayDryRun`
   - `BuildObjc3ParserSemaReleaseCandidateReplayDryRun(...)`
   - `parser_sema_release_candidate_replay_dry_run`
2. Release-candidate replay dry-run sync requires deterministic docs/runbook continuity and explicit pass-manager parity handoff:
   - `docs_runbook_sync_ready`
   - `pass_manager_contract_surface_sync`
   - `replay_surface_sync`
3. Pass-manager execution is fail-closed on release-candidate replay dry-run sync drift:
   - `if (!result.deterministic_parser_sema_release_candidate_replay_dry_run) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 3u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces release-candidate replay dry-run sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b014_parser_sema_release_candidate_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b014_parser_sema_release_candidate_replay_dry_run_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B014/parser_sema_release_candidate_replay_dry_run_summary.json`



