# M227-C009 Typed Sema-to-Lowering Conformance Matrix Implementation Packet

Packet: `M227-C009`
Milestone: `M227`
Lane: `C`
Issue: `#5129`
Dependencies: `M227-C008`

## Scope

Implement lane-C typed sema-to-lowering conformance-matrix consistency and
readiness by wiring conformance matrix invariants through typed contract and
parse/lowering readiness surfaces with deterministic fail-closed alignment.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_conformance_matrix_implementation_c009_expectations.md`
- Checker:
  `scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
- Dependency anchors (`M227-C008`):
  - `docs/contracts/m227_typed_sema_to_lowering_recovery_determinism_hardening_c008_expectations.md`
  - `scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
  - `spec/planning/compiler/m227/m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c009-typed-sema-to-lowering-conformance-matrix-implementation-contract`
  - `test:tooling:m227-c009-typed-sema-to-lowering-conformance-matrix-implementation-contract`
  - `check:objc3c:m227-c009-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Required Evidence

- `tmp/reports/m227/M227-C009/typed_sema_to_lowering_conformance_matrix_implementation_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
- `python scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m227-c009-lane-c-readiness`
