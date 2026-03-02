# M228 Lowering Pipeline Decomposition and Pass-Graph Diagnostics Hardening Expectations (A007)

Contract ID: `objc3c-lowering-pipeline-pass-graph-diagnostics-hardening/m228-a007-v1`
Status: Accepted
Scope: pass-graph diagnostics-hardening synthesis, fail-closed artifact gate hardening, IR metadata diagnostics projection, and architecture/spec anchors.

## Objective

Expand A006 edge-case robustness closure with explicit diagnostics-hardening
consistency and readiness/key gates so pass-graph diagnostic drift fails closed
before IR emission.

## Required Invariants

1. Core-feature surface carries diagnostics-hardening fields:
   - `diagnostics_hardening_consistent`
   - `diagnostics_hardening_ready`
   - `diagnostics_hardening_key`
2. Diagnostics-hardening key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphDiagnosticsHardeningKey(...)`
3. Frontend artifacts fail-close on lane-A diagnostics hardening before IR emission:
   - `IsObjc3LoweringPipelinePassGraphDiagnosticsHardeningReady(...)`
   - deterministic diagnostic code `O3L308`.
4. IR frontend metadata carries diagnostics-hardening readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_diagnostics_hardening_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_diagnostics_hardening_key`
   - IR output includes `; lowering_pass_graph_diagnostics_hardening = ...`
     and readiness line.
5. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a007_lowering_pipeline_decomposition_pass_graph_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a007_lowering_pipeline_decomposition_pass_graph_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m228-a007-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A007/lowering_pipeline_pass_graph_diagnostics_hardening_contract_summary.json`
