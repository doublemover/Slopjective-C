# M243-C008 Lowering/Runtime Diagnostics Surfacing Recovery and Determinism Hardening Packet

Packet: `M243-C008`
Milestone: `M243`
Lane: `C`
Dependencies: `M243-C007`

## Scope

Expand lane-C lowering/runtime diagnostics surfacing closure with explicit
recovery/determinism consistency/readiness and replay-key continuity guardrails
so recovery evidence remains deterministic and fail-closed.
Code/spec anchors are mandatory scope inputs.

## Anchors

- Contract: `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_c008_expectations.md`
- Checker: `scripts/check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_c008_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract.py`
- Surface type anchor:
  `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Surface derivation anchor:
  `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h`
- Pipeline wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Artifact fail-closed wiring: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Dependency anchors from `M243-C007`:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_c007_expectations.md`
  - `spec/planning/compiler/m243/m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_packet.md`
  - `scripts/check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m243-c008-lane-c-readiness`

## Required Evidence

- `tmp/reports/m243/M243-C008/lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_contract_summary.json`

## Determinism Criteria

- C008 readiness remains fail-closed when recovery/determinism consistency,
  readiness, replay-key continuity, or lowering pipeline recovery prerequisites
  drift.
- C007 diagnostics hardening closure remains a strict prerequisite for C008.
