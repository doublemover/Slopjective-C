# M243 Lowering/Runtime Diagnostics Surfacing Diagnostics Hardening Expectations (C007)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-diagnostics-hardening/m243-c007-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing diagnostics hardening
closure built on C006 deterministic edge-case expansion and robustness
foundations.

## Objective

Expand lane-C diagnostics surfacing closure with explicit diagnostics hardening
consistency, readiness, and replay-key guardrails so readiness cannot drift
fail-open after C006 robustness closure.
Code/spec anchors are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-C006`
- M243-C006 anchors remain mandatory prerequisites:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_c006_expectations.md`
  - `spec/planning/compiler/m243/m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py`
- Packet/checker/test assets for C007 remain mandatory:
  - `spec/planning/compiler/m243/m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_packet.md`
  - `scripts/check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. `Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface`
   remains the canonical lane-C C007 diagnostics hardening surface.
   - Surface type anchor:
     `native/objc3c/src/pipeline/objc3_frontend_types.h`.
   - Surface derivation/readiness helper anchor:
     `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h`.
2. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(...)`
   deterministically derives `diagnostics_hardening_ready` from:
   - C006 edge-case robustness continuity,
   - parse diagnostics hardening consistency/readiness surfaces,
   - semantic diagnostics hardening consistency/readiness surfaces,
   - lowering pass-graph diagnostics hardening readiness, and
   - diagnostics hardening replay-key completeness.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. `BuildObjc3FrontendArtifacts(...)` validates
   `IsObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurfaceReady(...)`
   and fails closed with deterministic `O3L325` diagnostics when C007 drift is
   detected.
5. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract`.
- `package.json` includes
  `test:tooling:m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract`.
- `package.json` includes `check:objc3c:m243-c007-lane-c-readiness`.

## Validation

- `python scripts/check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m243-c007-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C007/lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract_summary.json`
