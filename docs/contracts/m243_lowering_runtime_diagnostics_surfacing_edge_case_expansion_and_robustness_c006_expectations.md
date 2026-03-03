# M243 Lowering/Runtime Diagnostics Surfacing Edge-Case Expansion and Robustness Expectations (C006)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness/m243-c006-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing edge-case expansion and
robustness closure built on C005 deterministic edge-case compatibility
foundations.

## Objective

Expand lane-C diagnostics surfacing closure with explicit expansion consistency,
robustness readiness, and robustness replay-key guardrails so readiness cannot
drift fail-open after C005 compatibility completion.
Code/spec anchors are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-C005`
- M243-C005 anchors remain mandatory prerequisites:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_c005_expectations.md`
  - `spec/planning/compiler/m243/m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_packet.md`
  - `scripts/check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py`
- Packet/checker/test assets for C006 remain mandatory:
  - `spec/planning/compiler/m243/m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py`

## Deterministic Invariants

1. `Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface`
   remains the canonical lane-C C006 edge-case robustness surface.
   - Surface type anchor:
     `native/objc3c/src/pipeline/objc3_frontend_types.h`.
   - Surface derivation/readiness helper anchor:
     `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h`.
2. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(...)`
   deterministically derives `edge_case_robustness_ready` from:
   - C005 compatibility closure continuity,
   - parse edge-case expansion consistency and robustness readiness,
   - lowering pipeline edge-case robustness readiness, and
   - robustness replay-key completeness.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. `BuildObjc3FrontendArtifacts(...)` validates
   `IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(...)`
   and fails closed with deterministic `O3L324` diagnostics when C006 drift is
   detected.
5. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-c006-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness-contract`.
- `package.json` includes
  `test:tooling:m243-c006-lowering-runtime-diagnostics-surfacing-edge-case-expansion-and-robustness-contract`.
- `package.json` includes `check:objc3c:m243-c006-lane-c-readiness`.

## Validation

- `python scripts/check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m243-c006-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C006/lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract_summary.json`
