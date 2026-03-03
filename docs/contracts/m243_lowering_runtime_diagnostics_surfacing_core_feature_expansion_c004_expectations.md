# M243 Lowering/Runtime Diagnostics Surfacing Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-core-feature-expansion/m243-c004-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing core-feature expansion
closure built on C003 deterministic, fail-closed core feature foundations.

## Objective

Expand lane-C diagnostics surfacing closure with explicit key-continuity,
payload-accounting, and replay-key guardrails so readiness cannot drift into a
fail-open state after C003 core feature implementation.

## Dependency Scope

- Dependencies: `M243-C003`
- M243-C003 anchors remain mandatory prerequisites:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m243/m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_packet.md`
  - `scripts/check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py`

## Deterministic Invariants

1. `Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface`
   remains the canonical lane-C C004 expansion surface.
   - Surface type anchor:
     `native/objc3c/src/pipeline/objc3_frontend_types.h`.
   - Surface derivation/readiness helper anchor:
     `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h`.
2. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(...)`
   deterministically derives `core_feature_expansion_ready` from:
   - diagnostics hardening key continuity,
   - diagnostics payload accounting continuity,
   - replay-key completeness, and
   - lowering pipeline expansion readiness.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. `BuildObjc3FrontendArtifacts(...)` validates
   `IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(...)`
   and fails closed with deterministic `O3L322` diagnostics when C004 drift is
   detected.
5. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-c004-lowering-runtime-diagnostics-surfacing-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m243-c004-lowering-runtime-diagnostics-surfacing-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m243-c004-lane-c-readiness`.

## Validation

- `python scripts/check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m243-c004-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C004/lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract_summary.json`
