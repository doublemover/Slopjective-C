# M228 Lowering Pipeline Decomposition and Pass-Graph Core Feature Expectations (A003)

Contract ID: `objc3c-lowering-pipeline-pass-graph-core-feature/m228-a003-v1`
Status: Accepted
Scope: pass-graph core-feature readiness synthesis, artifact fail-closed gate, IR metadata propagation, and build/spec anchors.

## Objective

Implement the core pass-graph feature surface on top of A002 scaffolding so
lowering-boundary replay-key consistency, runtime dispatch declaration
consistency, and direct IR entrypoint readiness are enforced as deterministic
fail-closed prerequisites before IR emission.

## Required Invariants

1. Core-feature surface module exists:
   - `native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h`
   - `native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp`
2. Frontend pipeline wires both scaffold and core-feature surfaces:
   - `BuildObjc3LoweringPipelinePassGraphScaffold(...)`
   - `BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(...)`
3. Artifact builder fail-closes on core-feature readiness:
   - `IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady(...)`
   - deterministic diagnostic code `O3L302` on failure.
4. IR frontend metadata carries core-feature readiness + replay key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_core_feature_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_core_feature_key`
   - IR output includes `; lowering_pass_graph_core_feature = ...` and readiness line.
5. Build and documentation anchors are updated:
   - `native/objc3c/CMakeLists.txt`
   - `scripts/build_objc3c_native.ps1`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a003_lowering_pipeline_decomposition_pass_graph_core_feature_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a003_lowering_pipeline_decomposition_pass_graph_core_feature_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A003/lowering_pipeline_pass_graph_core_feature_contract_summary.json`
