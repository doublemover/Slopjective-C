# M227-C024 Typed Sema-to-Lowering Advanced Conformance Workpack (Shard 2) Packet

Packet: `M227-C024`
Milestone: `M227`
Lane: `C`
Issue: `#5144`
Dependencies: `M227-C023`

## Scope

Implement lane-C typed sema-to-lowering advanced-conformance-shard2
consistency and readiness by wiring shard invariants through typed contract and
parse/lowering readiness surfaces with deterministic fail-closed alignment.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_advanced_conformance_shard2_c024_expectations.md`
- Checker:
  `scripts/check_m227_c024_typed_sema_to_lowering_advanced_conformance_shard2_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c024_typed_sema_to_lowering_advanced_conformance_shard2_contract.py`
- Dependency anchors (`M227-C023`):
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_diagnostics_shard2_c023_expectations.md`
  - `scripts/check_m227_c023_typed_sema_to_lowering_advanced_diagnostics_shard2_contract.py`
  - `tests/tooling/test_check_m227_c023_typed_sema_to_lowering_advanced_diagnostics_shard2_contract.py`
  - `spec/planning/compiler/m227/m227_c023_typed_sema_to_lowering_advanced_diagnostics_shard2_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c024-typed-sema-to-lowering-advanced-conformance-shard2-contract`
  - `test:tooling:m227-c024-typed-sema-to-lowering-advanced-conformance-shard2-contract`
  - `check:objc3c:m227-c024-lane-c-readiness`

## Required Evidence

- `tmp/reports/m227/M227-C024/typed_sema_to_lowering_advanced_conformance_shard2_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c023_typed_sema_to_lowering_advanced_diagnostics_shard2_contract.py`
- `python scripts/check_m227_c024_typed_sema_to_lowering_advanced_conformance_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c024_typed_sema_to_lowering_advanced_conformance_shard2_contract.py -q`
- `npm run check:objc3c:m227-c024-lane-c-readiness`





