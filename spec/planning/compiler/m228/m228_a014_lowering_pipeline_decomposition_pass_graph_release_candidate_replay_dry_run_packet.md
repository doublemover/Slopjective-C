# M228-A014 Lowering Pipeline Decomposition and Pass-Graph Release-Candidate Replay Dry-Run Packet

Packet: `M228-A014`
Milestone: `M228`
Lane: `A`
Dependencies: `M228-A013`

## Purpose

Execute lane-A release-candidate replay dry-run and prove deterministic replay
stability for lowering pipeline decomposition/pass-graph readiness.

## Scope Anchors

- Contract:
  `docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_a014_expectations.md`
- Checker:
  `scripts/check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py`
- Dry-run script:
  `scripts/run_m228_a014_lowering_pipeline_decomposition_pass_graph_release_replay_dry_run.ps1`
- Tooling tests:
  `tests/tooling/test_check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py`
- Operator runbook:
  `docs/runbooks/m228_wave_execution_runbook.md`
- Shared anchors:
  - `package.json`
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Gate Commands

- `python scripts/check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m228_a014_lowering_pipeline_decomposition_pass_graph_release_replay_dry_run.ps1`
- `npm run check:objc3c:m228-a014-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Evidence Output

- `tmp/reports/m228/M228-A014/replay_dry_run_summary.json`
- `tmp/reports/m228/M228-A014/release_replay_dry_run_contract_summary.json`
