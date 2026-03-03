# M227-C006 Typed Sema-to-Lowering Edge-Case Expansion and Robustness Packet

Packet: `M227-C006`
Milestone: `M227`
Lane: `C`
Issue: `#5126`
Dependencies: `M227-C005`

## Scope

Expand lane-C typed sema-to-lowering edge-case robustness by wiring
edge-case expansion consistency and robustness readiness through typed contract
and parse/lowering readiness surfaces with deterministic fail-closed gating.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_edge_case_expansion_and_robustness_c006_expectations.md`
- Checker:
  `scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
- Dependency anchors (`M227-C005`):
  - `docs/contracts/m227_typed_sema_to_lowering_edge_case_compatibility_completion_c005_expectations.md`
  - `scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
  - `spec/planning/compiler/m227/m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c006-typed-sema-to-lowering-edge-case-expansion-and-robustness-contract`
  - `test:tooling:m227-c006-typed-sema-to-lowering-edge-case-expansion-and-robustness-contract`
  - `check:objc3c:m227-c006-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Required Evidence

- `tmp/reports/m227/M227-C006/typed_sema_to_lowering_edge_case_expansion_and_robustness_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
- `python scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m227-c006-lane-c-readiness`
