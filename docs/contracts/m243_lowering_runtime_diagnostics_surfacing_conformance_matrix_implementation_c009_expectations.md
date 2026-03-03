# M243 Lowering/Runtime Diagnostics Surfacing Conformance Matrix Implementation Expectations (C009)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-conformance-matrix-implementation/m243-c009-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing conformance matrix
implementation closure built on C008 recovery/determinism hardening
foundations.

## Objective

Expand lane-C diagnostics surfacing closure with explicit conformance-matrix
consistency, readiness, and replay-key guardrails so conformance evidence cannot
drift fail-open after C008 recovery/determinism closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-C008`
- M243-C008 recovery and determinism hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_c008_expectations.md`
  - `spec/planning/compiler/m243/m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_packet.md`
  - `scripts/check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py`
- Packet/checker/test assets for C009 remain mandatory:
  - `spec/planning/compiler/m243/m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. `Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface`
   remains the canonical lane-C C009 conformance matrix implementation surface.
   - Surface type anchor:
     `native/objc3c/src/pipeline/objc3_frontend_types.h`.
   - Surface derivation/readiness helper anchor:
     `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h`.
2. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(...)`
   deterministically derives `conformance_matrix_ready` from:
   - C008 recovery/determinism continuity,
   - parse conformance-matrix consistency and readiness surfaces,
   - semantic conformance-matrix consistency and readiness surfaces,
   - lowering pass-graph conformance-matrix readiness, and
   - replay-key completeness for parse/semantic/lowering conformance closure.
3. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationKey(...)`
   remains deterministic and includes parse, semantic, and lowering replay-key
   readiness evidence continuity.
4. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
5. `BuildObjc3FrontendArtifacts(...)` validates
   `IsObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurfaceReady(...)`
   and fails closed with deterministic `O3L327` diagnostics when C009 drift is
   detected.
6. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-c009-lowering-runtime-diagnostics-surfacing-conformance-matrix-implementation-contract`.
- `package.json` includes
  `test:tooling:m243-c009-lowering-runtime-diagnostics-surfacing-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m243-c009-lane-c-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m243-c009-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C009/lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract_summary.json`
