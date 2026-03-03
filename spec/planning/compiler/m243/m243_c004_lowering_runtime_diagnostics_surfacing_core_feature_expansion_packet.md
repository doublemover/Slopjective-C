# M243-C004 Lowering/Runtime Diagnostics Surfacing Core Feature Expansion Packet

Packet: `M243-C004`
Milestone: `M243`
Lane: `C`
Dependencies: `M243-C003`

## Scope

Expand lane-C lowering/runtime diagnostics surfacing from core feature
implementation to core feature expansion with deterministic key-continuity,
payload-accounting, and replay-key guardrails.

## Anchors

- Contract: `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_core_feature_expansion_c004_expectations.md`
- Checker: `scripts/check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py`
- Surface type anchor:
  `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Surface derivation anchor:
  `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h`
- Pipeline surface types: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Pipeline wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Artifact fail-closed wiring: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Dependency anchors from `M243-C003`:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m243/m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_packet.md`
  - `scripts/check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Required Evidence

- `tmp/reports/m243/M243-C004/lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract_summary.json`

## Determinism Criteria

- C004 readiness remains fail-closed when diagnostics hardening key continuity,
  diagnostics payload accounting, replay-key transport, or lowering pipeline
  expansion invariants drift.
- C003 core feature implementation remains a strict prerequisite for C004.
