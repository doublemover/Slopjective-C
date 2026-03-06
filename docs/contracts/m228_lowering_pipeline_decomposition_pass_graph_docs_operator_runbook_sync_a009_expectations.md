# M228 Lowering Pipeline Decomposition and Pass-Graph Docs and Operator Runbook Synchronization Expectations (A013)

Contract ID: `objc3c-lowering-pipeline-pass-graph-docs-operator-runbook-sync/m228-a013-v1`
Status: Accepted
Scope: lane-A docs and operator runbook synchronization after A012 cross-lane integration sync closure.

## Objective

Freeze lane-A operator-facing documentation and command sequencing so M228
cross-lane readiness evidence remains synchronized and fail-closed for
day-to-day wave execution.

## Required Invariants

1. Operator runbook exists at:
   - `docs/runbooks/m228_wave_execution_runbook.md`
2. Runbook includes canonical contract anchors for:
   - `objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1`
   - `objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`
   - `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`
   - `objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1`
   - `objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1`
   - `objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1`
   - `objc3c-lowering-pipeline-pass-graph-docs-operator-runbook-sync/m228-a013-v1`
3. Runbook includes canonical command sequence:
   - `npm run build:objc3c-native`
   - `scripts/objc3c_native_compile.ps1` smoke invocation into `tmp/artifacts/...`
   - `python scripts/check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py`
   - `python scripts/check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py -q`
   - `npm run check:objc3c:m228-a013-lane-a-readiness`
4. Runbook references evidence storage under `tmp/reports/m228/`.
5. Shared anchors are synchronized:
   - `spec/planning/compiler/m228/m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_packet.md`
   - `package.json` contains A013 check/test/readiness scripts.
   - `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-A A013 anchor text.
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes fail-closed docs/runbook wiring language.
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes docs/runbook metadata-anchor language.

## Validation

- `python scripts/check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py -q`
- `npm run check:objc3c:m228-a013-lane-a-readiness`

## Evidence Path

- `tmp/reports/m228/M228-A013/docs_operator_runbook_sync_contract_summary.json`
