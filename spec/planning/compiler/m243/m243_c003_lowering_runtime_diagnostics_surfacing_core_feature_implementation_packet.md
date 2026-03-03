# M243-C003 Lowering/Runtime Diagnostics Surfacing Core Feature Implementation Packet

Packet: `M243-C003`
Milestone: `M243`
Lane: `C`
Dependencies: `M243-C001`, `M243-C002`

## Scope

Implement lane-C core feature closure for lowering/runtime diagnostics
surfacing so hardening and replay-key invariants remain deterministic and
fail-closed before lowering pipeline emission proceeds.

## Anchors

- Contract: `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_core_feature_implementation_c003_expectations.md`
- Checker: `scripts/check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py`
- Core feature surface: `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h`
- Pipeline surface types: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Pipeline wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Artifact fail-closed wiring: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Required Evidence

- `tmp/reports/m243/M243-C003/lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract_summary.json`

## Determinism Criteria

- C003 readiness remains fail-closed when diagnostics hardening/replay-key
  invariants drift.
- C002 diagnostics surfacing scaffold signals remain strict prerequisites for
  C003 readiness closure.
