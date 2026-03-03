# M228 Lowering Pipeline Decomposition and Pass-Graph Release-Candidate Replay Dry-Run Expectations (A014)

Contract ID: `objc3c-lowering-pipeline-pass-graph-release-replay-dry-run/m228-a014-v1`
Status: Accepted
Scope: lane-A release-candidate replay dry-run closure after A013 docs/runbook synchronization.

## Objective

Execute a deterministic lane-A release-candidate replay dry-run with the native
compile wrapper and prove replay stability across two consecutive invocations on
the same source while preserving A013 runbook continuity.

## Deterministic Invariants

- Dependencies: `M228-A013`

1. Dry-run script exists:
   - `scripts/run_m228_a014_lowering_pipeline_decomposition_pass_graph_release_replay_dry_run.ps1`
2. Script executes two wrapper compiles and compares deterministic artifacts:
   - `module.manifest.json`
   - `module.diagnostics.json`
   - `module.ll`
   - `module.object-backend.txt`
3. Script validates lane-A readiness anchors in manifest:
   - `frontend.pipeline.stages.parser.deterministic_handoff == true`
   - `frontend.pipeline.stages.parser.recovery_replay_ready == true`
   - `frontend.pipeline.parse_lowering_readiness.ready_for_lowering == true`
   - `frontend.pipeline.parse_lowering_readiness.parse_artifact_replay_key_deterministic == true`
   - `frontend.pipeline.parse_lowering_readiness.parse_lowering_performance_quality_guardrails_consistent == true`
   - `frontend.pipeline.parse_lowering_readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent == true`
   - `frontend.pipeline.parse_lowering_readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready == true`
4. `parse_lowering_performance_quality_guardrails_key` includes
   `toolchain_runtime_ga_operations_docs_runbook_sync_key=` as fail-closed
   continuity evidence.
5. Dry-run summary evidence is emitted under:
   - `tmp/reports/m228/M228-A014/replay_dry_run_summary.json`
6. A013 remains a mandatory prerequisite.

## Build and Readiness Integration

Shared-file deltas required for full lane-A readiness:

- `package.json`
  - add `check:objc3c:m228-a014-lowering-pipeline-pass-graph-release-candidate-replay-dry-run-contract`
  - add `test:tooling:m228-a014-lowering-pipeline-pass-graph-release-candidate-replay-dry-run-contract`
  - add `run:objc3c:m228-a014-lowering-pipeline-pass-graph-release-replay-dry-run`
  - add `check:objc3c:m228-a014-lane-a-readiness`
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add A014 contract/command sequence anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-A A014 replay dry-run anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add fail-closed release-replay wiring language
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic release-replay metadata anchor language

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Validation

- `python scripts/check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m228_a014_lowering_pipeline_decomposition_pass_graph_release_replay_dry_run.ps1`
- `npm run check:objc3c:m228-a014-lane-a-readiness`

## Evidence Path

- `tmp/reports/m228/M228-A014/replay_dry_run_summary.json`
- `tmp/reports/m228/M228-A014/release_replay_dry_run_contract_summary.json`
