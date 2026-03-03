# M227-C005 Typed Sema-to-Lowering Edge-Case and Compatibility Completion Packet

Packet: `M227-C005`
Milestone: `M227`
Lane: `C`
Issue: `#5125`
Dependencies: `M227-C004`

## Scope

Complete lane-C typed sema-to-lowering edge-case compatibility by wiring
compatibility-handoff consistency, language-version pragma coordinate ordering,
parse-artifact replay determinism, and parse-artifact robustness into typed
contract and parse/lowering readiness surfaces with fail-closed gating.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_edge_case_compatibility_completion_c005_expectations.md`
- Checker:
  `scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
- Dependency anchors (`M227-C004`):
  - `docs/contracts/m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md`
  - `scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
  - `spec/planning/compiler/m227/m227_c004_typed_sema_to_lowering_core_feature_expansion_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c005-typed-sema-to-lowering-edge-case-compatibility-completion-contract`
  - `test:tooling:m227-c005-typed-sema-to-lowering-edge-case-compatibility-completion-contract`
  - `check:objc3c:m227-c005-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Required Evidence

- `tmp/reports/m227/M227-C005/typed_sema_to_lowering_edge_case_compatibility_completion_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
- `python scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m227-c005-lane-c-readiness`
