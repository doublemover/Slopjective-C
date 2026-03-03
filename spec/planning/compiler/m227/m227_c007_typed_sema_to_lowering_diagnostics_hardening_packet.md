# M227-C007 Typed Sema-to-Lowering Diagnostics Hardening Packet

Packet: `M227-C007`
Milestone: `M227`
Lane: `C`
Issue: `#5127`
Dependencies: `M227-C006`

## Scope

Harden lane-C typed sema-to-lowering diagnostics by wiring diagnostics
hardening consistency/readiness through typed contract and parse/lowering
readiness surfaces with deterministic fail-closed alignment.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_diagnostics_hardening_c007_expectations.md`
- Checker:
  `scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
- Dependency anchors (`M227-C006`):
  - `docs/contracts/m227_typed_sema_to_lowering_edge_case_expansion_and_robustness_c006_expectations.md`
  - `scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
  - `spec/planning/compiler/m227/m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c007-typed-sema-to-lowering-diagnostics-hardening-contract`
  - `test:tooling:m227-c007-typed-sema-to-lowering-diagnostics-hardening-contract`
  - `check:objc3c:m227-c007-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Required Evidence

- `tmp/reports/m227/M227-C007/typed_sema_to_lowering_diagnostics_hardening_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
- `python scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m227-c007-lane-c-readiness`
