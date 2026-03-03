# M227 Semantic Pass Release-Candidate Replay Dry-Run Expectations (A014)

Contract ID: `objc3c-semantic-pass-release-replay-dry-run/m227-a014-v1`
Status: Accepted
Scope: lane-A release-candidate replay dry-run closure after A013 docs/runbook synchronization.
Dependencies: `M227-A013`

Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Objective

Execute a deterministic lane-A release-candidate replay dry-run with the native
compile wrapper and prove replay stability across consecutive invocations on the
same source while preserving A013 runbook continuity.

## Deterministic Invariants

1. Dry-run script exists:
   - `scripts/run_m227_a014_semantic_pass_release_replay_dry_run.ps1`
2. Script executes two wrapper compiles and compares deterministic artifacts:
   - `module.manifest.json`
   - `module.diagnostics.json`
   - `module.ll`
   - `module.object-backend.txt`
3. Script validates parser/lowering readiness keys in manifest:
   - `frontend.pipeline.stages.parser.deterministic_handoff == true`
   - `frontend.pipeline.stages.parser.recovery_replay_ready == true`
   - `frontend.pipeline.parse_lowering_readiness.ready_for_lowering == true`
   - `frontend.pipeline.parse_lowering_readiness.parse_artifact_replay_key_deterministic == true`
4. Dry-run summary evidence is emitted under:
   - `tmp/reports/m227/M227-A014/replay_dry_run_summary.json`
5. A013 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m227_a014_semantic_pass_release_replay_dry_run.ps1`
- `npm run check:objc3c:m227-a014-lane-a-readiness`

## Evidence Path

- `tmp/reports/m227/M227-A014/replay_dry_run_summary.json`
- `tmp/reports/m227/M227-A014/release_replay_dry_run_contract_summary.json`
