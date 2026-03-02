# M228 Lowering Pipeline Decomposition and Pass-Graph Core Feature Expansion Expectations (A004)

Contract ID: `objc3c-lowering-pipeline-pass-graph-core-feature-expansion/m228-a004-v1`
Status: Accepted
Scope: pass-graph core-feature expansion readiness synthesis, fail-closed artifact gate hardening, IR metadata expansion projection, and architecture/spec anchors.

## Objective

Expand A003 pass-graph core-feature closure with explicit expansion-accounting
and replay-proof guardrails so IR emission remains fail-closed when expansion
readiness drifts.

## Required Invariants

1. Core-feature surface carries expansion fields:
   - `edge_case_dispatch_shape_coverage_ready`
   - `replay_proof_expansion_ready`
   - `expansion_ready`
   - `expansion_key`
2. Expansion key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphCoreFeatureExpansionKey(...)`
3. Frontend artifacts fail-close on expansion readiness before IR emission:
   - `IsObjc3LoweringPipelinePassGraphCoreFeatureExpansionReady(...)`
   - deterministic diagnostic code `O3L303`.
4. IR frontend metadata carries expansion readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_core_feature_expansion_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_core_feature_expansion_key`
   - IR output includes `; lowering_pass_graph_core_feature_expansion = ...`
     and readiness line.
5. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a004_lowering_pipeline_decomposition_pass_graph_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a004_lowering_pipeline_decomposition_pass_graph_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m228-a004-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A004/lowering_pipeline_pass_graph_core_feature_expansion_contract_summary.json`
