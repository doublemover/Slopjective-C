# M228 Lowering Pipeline Decomposition and Pass-Graph Recovery and Determinism Hardening Expectations (A008)

Contract ID: `objc3c-lowering-pipeline-pass-graph-recovery-determinism-hardening/m228-a008-v1`
Status: Accepted
Scope: pass-graph recovery/determinism synthesis, fail-closed artifact gate hardening, IR metadata recovery projection, and architecture/spec anchors.

## Objective

Expand A007 diagnostics hardening closure with explicit recovery and determinism
consistency and readiness/key gates so pass-graph recovery drift fails closed
before IR emission.

## Required Invariants

1. Core-feature surface carries recovery/determinism fields:
   - `recovery_determinism_consistent`
   - `recovery_determinism_ready`
   - `recovery_determinism_key`
2. Recovery-determinism key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphRecoveryDeterminismKey(...)`
3. Frontend artifacts fail-close on lane-A recovery/determinism before IR emission:
   - `IsObjc3LoweringPipelinePassGraphRecoveryDeterminismReady(...)`
   - deterministic diagnostic code `O3L309`.
4. IR frontend metadata carries recovery/determinism readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_recovery_determinism_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_recovery_determinism_key`
   - IR output includes `; lowering_pass_graph_recovery_determinism = ...`
     and readiness line.
5. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a008_lowering_pipeline_decomposition_pass_graph_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a008_lowering_pipeline_decomposition_pass_graph_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m228-a008-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A008/lowering_pipeline_pass_graph_recovery_determinism_hardening_contract_summary.json`
