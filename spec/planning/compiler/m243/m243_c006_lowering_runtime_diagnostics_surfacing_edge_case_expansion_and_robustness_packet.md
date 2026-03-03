# M243-C006 Lowering/Runtime Diagnostics Surfacing Edge-Case Expansion and Robustness Packet

Packet: `M243-C006`
Milestone: `M243`
Lane: `C`
Dependencies: `M243-C005`

## Scope

Expand lane-C lowering/runtime diagnostics surfacing closure with explicit
edge-case expansion consistency and robustness readiness guardrails so
compatibility continuity and robustness evidence remain deterministic and
fail-closed.
Code/spec anchors are mandatory scope inputs.

## Anchors

- Contract: `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_c006_expectations.md`
- Checker: `scripts/check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_c006_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract.py`
- Surface type anchor:
  `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Surface derivation anchor:
  `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h`
- Pipeline wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Artifact fail-closed wiring: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Dependency anchors from `M243-C005`:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_c005_expectations.md`
  - `spec/planning/compiler/m243/m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_packet.md`
  - `scripts/check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m243_c005_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_completion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m243-c006-lane-c-readiness`

## Required Evidence

- `tmp/reports/m243/M243-C006/lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_contract_summary.json`

## Determinism Criteria

- C006 readiness remains fail-closed when expansion consistency, robustness
  readiness, robustness replay-key transport, or lowering pipeline robustness
  prerequisites drift.
- C005 edge-case compatibility completion remains a strict prerequisite for
  C006.
