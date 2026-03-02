# M228 Lowering Pipeline Decomposition and Pass-Graph Conformance Matrix Implementation Expectations (A009)

Contract ID: `objc3c-lowering-pipeline-pass-graph-conformance-matrix-implementation/m228-a009-v1`
Status: Accepted
Scope: pass-graph conformance-matrix synthesis, fail-closed artifact gate hardening, IR metadata conformance projection, and architecture/spec anchors.

## Objective

Expand A008 recovery/determinism closure with explicit conformance-matrix
consistency and readiness/key gates so pass-graph conformance drift fails
closed before IR emission.

## Required Invariants

1. Core-feature surface carries conformance-matrix fields:
   - `conformance_matrix_consistent`
   - `conformance_matrix_ready`
   - `conformance_matrix_key`
2. Conformance-matrix key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphConformanceMatrixKey(...)`
3. Frontend artifacts fail-close on lane-A conformance matrix before IR emission:
   - `IsObjc3LoweringPipelinePassGraphConformanceMatrixReady(...)`
   - deterministic diagnostic code `O3L311`.
4. IR frontend metadata carries conformance-matrix readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_conformance_matrix_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_conformance_matrix_key`
   - IR output includes `; lowering_pass_graph_conformance_matrix = ...`
     and readiness line.
5. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a009_lowering_pipeline_decomposition_pass_graph_conformance_matrix_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a009_lowering_pipeline_decomposition_pass_graph_conformance_matrix_contract.py -q`
- `npm run check:objc3c:m228-a009-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A009/lowering_pipeline_pass_graph_conformance_matrix_contract_summary.json`
