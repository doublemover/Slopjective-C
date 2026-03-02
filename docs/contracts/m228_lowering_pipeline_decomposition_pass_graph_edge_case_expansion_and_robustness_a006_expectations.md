# M228 Lowering Pipeline Decomposition and Pass-Graph Edge-Case Expansion and Robustness Expectations (A006)

Contract ID: `objc3c-lowering-pipeline-pass-graph-edge-case-expansion-and-robustness/m228-a006-v1`
Status: Accepted
Scope: pass-graph edge-case expansion/robustness synthesis, fail-closed artifact gate hardening, IR metadata robustness projection, and architecture/spec anchors.

## Objective

Expand A005 edge-case compatibility closure with explicit edge-case expansion
consistency and robustness readiness/key gates so lowering pass-graph drift
fails closed before IR emission.

## Required Invariants

1. Core-feature surface carries edge-case expansion/robustness fields:
   - `edge_case_expansion_consistent`
   - `edge_case_robustness_ready`
   - `edge_case_robustness_key`
2. Robustness key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphEdgeCaseRobustnessKey(...)`
3. Frontend artifacts fail-close on lane-A edge-case robustness before IR emission:
   - `IsObjc3LoweringPipelinePassGraphEdgeCaseRobustnessReady(...)`
   - deterministic diagnostic code `O3L307`.
4. IR frontend metadata carries edge-case robustness readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_edge_case_robustness_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_edge_case_robustness_key`
   - IR output includes `; lowering_pass_graph_edge_case_robustness = ...`
     and readiness line.
5. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a006_lowering_pipeline_decomposition_pass_graph_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a006_lowering_pipeline_decomposition_pass_graph_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m228-a006-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A006/lowering_pipeline_pass_graph_edge_case_expansion_and_robustness_contract_summary.json`
