# M226 Parse-Lowering Release-Candidate Replay Dry-Run Expectations (C014)

Contract ID: `objc3c-parse-lowering-release-replay-dry-run-contract/m226-c014-v1`
Status: Accepted
Scope: Parse/lowering release-candidate replay dry-run for native frontend readiness.

## Objective

Run a deterministic release-candidate dry-run over native compile wrapper output
and prove replay stability across two consecutive invocations on the same source
while validating C013 docs/runbook sync readiness anchors.

## Required Invariants

1. Dry-run script exists:
   - `scripts/run_m226_c014_parse_lowering_release_replay_dry_run.ps1`
2. Script executes two wrapper compiles and compares deterministic artifacts:
   - `module.manifest.json`
   - `module.diagnostics.json`
   - `module.ll`
   - `module.object-backend.txt`
3. Script validates parse-lowering readiness anchors in manifest:
   - `ready_for_lowering == true`
   - `parse_artifact_replay_key_deterministic == true`
   - `parse_recovery_determinism_hardening_consistent == true`
   - `parse_lowering_conformance_matrix_consistent == true`
   - `parse_lowering_conformance_corpus_consistent == true`
   - `parse_lowering_performance_quality_guardrails_consistent == true`
   - `toolchain_runtime_ga_operations_cross_lane_integration_consistent == true`
   - `toolchain_runtime_ga_operations_cross_lane_integration_ready == true`
   - `toolchain_runtime_ga_operations_docs_runbook_sync_consistent == true`
   - `toolchain_runtime_ga_operations_docs_runbook_sync_ready == true`
4. Script verifies deterministic key evidence:
   - `parse_lowering_performance_quality_guardrails_key` includes
     `toolchain_runtime_ga_operations_cross_lane_integration_key=`
   - `parse_lowering_performance_quality_guardrails_key` includes
     `toolchain_runtime_ga_operations_docs_runbook_sync_key=`
   - `long_tail_grammar_integration_closeout_key` includes
     `toolchain_runtime_ga_operations_docs_runbook_sync_key=`
5. Script emits summary evidence under:
   - `tmp/reports/m226/M226-C014/replay_dry_run_summary.json`
6. C013 remains a mandatory prerequisite.

## Validation

- `python scripts/check_m226_c014_parse_lowering_release_candidate_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c014_parse_lowering_release_candidate_replay_dry_run_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_c014_parse_lowering_release_replay_dry_run.ps1`

## Evidence Path

- `tmp/reports/m226/M226-C014/replay_dry_run_summary.json`
