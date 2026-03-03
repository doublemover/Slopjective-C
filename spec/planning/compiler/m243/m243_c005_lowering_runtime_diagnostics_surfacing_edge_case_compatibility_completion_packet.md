# M243-C005 Lowering/Runtime Diagnostics Surfacing Edge-Case Compatibility Completion Packet

Packet: `M243-C005`
Milestone: `M243`
Lane: `C`
Dependencies: `M243-C004`

## Scope

Complete lane-C lowering/runtime diagnostics surfacing edge-case compatibility
closure so parse/lowering compatibility handoff continuity, edge-case surface
consistency, and replay-key transport remain deterministic and fail-closed.

## Anchors

- Contract: `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_c005_expectations.md`
- Checker: `scripts/check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py`
- Surface type anchor:
  `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Surface derivation anchor:
  `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h`
- Pipeline wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Artifact fail-closed wiring: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Dependency anchors from `M243-C004`:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_core_feature_expansion_c004_expectations.md`
  - `spec/planning/compiler/m243/m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_packet.md`
  - `scripts/check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m243_c004_lowering_runtime_diagnostics_surfacing_core_feature_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Required Evidence

- `tmp/reports/m243/M243-C005/lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract_summary.json`

## Determinism Criteria

- C005 readiness remains fail-closed when compatibility handoff continuity,
  parse edge-case compatibility consistency/ready state, replay-key transport,
  or lowering pipeline edge-case compatibility invariants drift.
- C004 core feature expansion remains a strict prerequisite for C005.
