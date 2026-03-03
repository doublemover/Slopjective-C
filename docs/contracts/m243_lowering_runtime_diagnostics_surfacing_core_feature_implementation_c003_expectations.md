# M243 Lowering/Runtime Diagnostics Surfacing Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-core-feature-implementation/m243-c003-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing core-feature
implementation closure built on C001/C002 fail-closed foundations.

## Objective

Implement lane-C core feature closure so lowering/runtime diagnostics surfacing
readiness is derived from deterministic hardening/replay-key invariants instead
of ad hoc readiness flags.

## Dependency Scope

- Dependencies: `M243-C001`, `M243-C002`
- M243-C001/M243-C002 contract and modular split anchors remain mandatory:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_c001_expectations.md`
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_c002_expectations.md`
  - `scripts/check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py`
  - `scripts/check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py`
  - `tests/tooling/test_check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py`

## Deterministic Invariants

1. `Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface`
   remains the canonical lane-C C003 core feature readiness surface.
2. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(...)`
   remains the canonical fail-closed closure builder.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(...)`
   and stores the surface in `Objc3FrontendPipelineResult`.
4. `BuildObjc3FrontendArtifacts(...)` validates
   `IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurfaceReady(...)`
   and fails closed with deterministic `O3L321` diagnostics when C003 drift is
   detected.
5. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m243-c003-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C003/lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract_summary.json`
