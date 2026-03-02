# M228 Lowering Pipeline Decomposition and Pass-Graph Edge-Case Compatibility Completion Expectations (A005)

Contract ID: `objc3c-lowering-pipeline-pass-graph-edge-case-compatibility/m228-a005-v1`
Status: Accepted
Scope: pass-graph edge-case compatibility completion readiness synthesis, fail-closed artifact gate hardening, IR metadata compatibility projection, and architecture/spec anchors.

## Objective

Expand A004 pass-graph closure with explicit edge-case compatibility gates so
compatibility handoff and language-version/pragma coordinate ordering drift
fail closed before IR emission.

## Required Invariants

1. Core-feature surface carries edge-case compatibility fields:
   - `compatibility_handoff_consistent`
   - `language_version_pragma_coordinate_order_consistent`
   - `edge_case_compatibility_ready`
   - `edge_case_compatibility_key`
2. Compatibility key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityKey(...)`
3. Frontend artifacts fail-close on edge-case compatibility readiness before IR emission:
   - `IsObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityReady(...)`
   - deterministic diagnostic code `O3L305`.
4. IR frontend metadata carries edge-case compatibility readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_edge_case_compatibility_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_edge_case_compatibility_key`
   - IR output includes `; lowering_pass_graph_edge_case_compatibility = ...`
     and readiness line.
5. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a005_lowering_pipeline_decomposition_pass_graph_edge_case_compatibility_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a005_lowering_pipeline_decomposition_pass_graph_edge_case_compatibility_contract.py -q`
- `npm run check:objc3c:m228-a005-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A005/lowering_pipeline_pass_graph_edge_case_compatibility_contract_summary.json`
