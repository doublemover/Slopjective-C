# M226 Parser Release-Candidate Replay Dry-Run Expectations (A014)

Contract ID: `objc3c-parser-release-replay-dry-run-contract/m226-a014-v1`
Status: Accepted
Scope: Lane-A release-candidate replay dry-run for parser/frontend determinism.

## Objective

Run a deterministic dry-run using the native compile wrapper and prove replay
stability across two consecutive invocations on the same source.

## Required Invariants

1. Dry-run script exists:
   - `scripts/run_m226_a014_parser_release_replay_dry_run.ps1`
2. Script executes two wrapper compiles and compares deterministic artifacts:
   - `module.manifest.json`
   - `module.diagnostics.json`
   - `module.ll`
   - `module.object-backend.txt`
3. Script validates parser/frontend readiness anchors in manifest:
   - `frontend.pipeline.stages.parser.deterministic_handoff == true`
   - `frontend.pipeline.stages.parser.recovery_replay_ready == true`
   - `frontend.pipeline.parse_lowering_readiness.ready_for_lowering == true`
   - `frontend.pipeline.parse_lowering_readiness.parse_artifact_replay_key_deterministic == true`
4. Script emits summary evidence under:
   - `tmp/reports/m226/M226-A014/replay_dry_run_summary.json`

## Validation

- `python scripts/check_m226_a014_parser_release_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a014_parser_release_replay_dry_run_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a014_parser_release_replay_dry_run.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A014/replay_dry_run_summary.json`
