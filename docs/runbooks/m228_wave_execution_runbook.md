# M228 Wave Execution Runbook

## Scope

This runbook tracks current M228 lane sync coverage for:

- `objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1`
- `objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`
- `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`
- `objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1`
- `objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1`
- `objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1`
- `objc3c-lowering-pipeline-pass-graph-docs-operator-runbook-sync/m228-a013-v1`

## Canonical Validation Sequence

1. Build native binaries:
   - `npm run build:objc3c-native`
2. Run smoke compile through invocation wrapper:
   - `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m228-wave-smoke`
3. Validate cross-lane integration sync gate:
   - `python scripts/check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py`
4. Validate docs/operator runbook synchronization gate:
   - `python scripts/check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py -q`
5. Run lane-A readiness chain:
   - `npm run check:objc3c:m228-a013-lane-a-readiness`

## Evidence

Persist wave evidence under:

- `tmp/reports/m228/`
