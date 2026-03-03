# M243 Lowering/Runtime Diagnostics Surfacing Recovery and Determinism Hardening Expectations (C008)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening/m243-c008-v1`
Status: Accepted
Scope: lane-C lowering/runtime diagnostics surfacing recovery and determinism
hardening closure built on C007 diagnostics hardening foundations.

## Objective

Expand lane-C diagnostics surfacing closure with explicit recovery/determinism
consistency, readiness, and replay-key guardrails so readiness cannot drift
fail-open after C007 diagnostics hardening closure.
Code/spec anchors are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-C007`
- M243-C007 anchors remain mandatory prerequisites:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_c007_expectations.md`
  - `spec/planning/compiler/m243/m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_packet.md`
  - `scripts/check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py`
- Packet/checker/test assets for C008 remain mandatory:
  - `spec/planning/compiler/m243/m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_packet.md`
  - `scripts/check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py`

## Deterministic Invariants

1. `Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface`
   remains the canonical lane-C C008 recovery/determinism hardening surface.
   - Surface type anchor:
     `native/objc3c/src/pipeline/objc3_frontend_types.h`.
   - Surface derivation/readiness helper anchor:
     `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h`.
2. `BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(...)`
   deterministically derives `recovery_determinism_ready` from:
   - C007 diagnostics hardening continuity,
   - parse recovery/determinism consistency and readiness surfaces,
   - semantic recovery/determinism consistency and readiness surfaces,
   - lowering pass-graph recovery/determinism readiness, and
   - replay-key completeness for parse/semantic/lowering recovery closure.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. `BuildObjc3FrontendArtifacts(...)` validates
   `IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(...)`
   and fails closed with deterministic `O3L326` diagnostics when C008 drift is
   detected.
5. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-c008-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening-contract`.
- `package.json` includes
  `test:tooling:m243-c008-lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening-contract`.
- `package.json` includes `check:objc3c:m243-c008-lane-c-readiness`.

## Validation

- `python scripts/check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m243-c008-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C008/lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract_summary.json`
