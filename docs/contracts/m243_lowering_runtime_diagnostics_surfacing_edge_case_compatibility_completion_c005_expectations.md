# M243 Lowering/Runtime Diagnostics Surfacing Edge-Case Compatibility Completion Expectations (C005)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-edge-case-compatibility-completion/m243-c005-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing edge-case compatibility
completion closure built on C004 deterministic core-feature expansion
foundations.

## Objective

Complete lane-C diagnostics surfacing compatibility closure with explicit
parse/lowering handoff continuity, edge-case surface consistency, and replay-key
transport guardrails so readiness cannot drift fail-open after C004.

## Dependency Scope

- Dependencies: `M243-C004`
- M243-C004 anchors remain mandatory prerequisites:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_core_feature_expansion_c004_expectations.md`
  - `spec/planning/compiler/m243/m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_packet.md`
  - `scripts/check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py`

## Deterministic Invariants

1. `Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface`
   remains the canonical lane-C C005 edge-case compatibility surface.
   - Surface type anchor:
     `native/objc3c/src/pipeline/objc3_frontend_types.h`.
   - Surface derivation/readiness helper anchor:
     `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h`.
2. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface(...)`
   deterministically derives `edge_case_compatibility_ready` from:
   - C004 expansion readiness continuity,
   - parse compatibility handoff and coordinate-order continuity,
   - parse edge-case compatibility surface consistency/ready state,
   - lowering pipeline edge-case compatibility readiness, and
   - replay-key completeness for compatibility and recovery anchors.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. `BuildObjc3FrontendArtifacts(...)` validates
   `IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurfaceReady(...)`
   and fails closed with deterministic `O3L323` diagnostics when C005 drift is
   detected.
5. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-c005-lowering-runtime-diagnostics-surfacing-edge-case-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m243-c005-lowering-runtime-diagnostics-surfacing-edge-case-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m243-c005-lane-c-readiness`.

## Validation

- `python scripts/check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m243-c005-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C005/lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract_summary.json`
