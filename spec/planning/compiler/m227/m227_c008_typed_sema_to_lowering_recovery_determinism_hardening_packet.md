# M227-C008 Typed Sema-to-Lowering Recovery and Determinism Hardening Packet

Packet: `M227-C008`
Milestone: `M227`
Lane: `C`
Issue: `#5128`
Dependencies: `M227-C007`

## Scope

Harden lane-C typed sema-to-lowering recovery/determinism by wiring
recovery/determinism consistency and readiness through typed contract and
parse/lowering readiness surfaces with deterministic fail-closed alignment.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_recovery_determinism_hardening_c008_expectations.md`
- Checker:
  `scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
- Dependency anchors (`M227-C007`):
  - `docs/contracts/m227_typed_sema_to_lowering_diagnostics_hardening_c007_expectations.md`
  - `scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m227/m227_c007_typed_sema_to_lowering_diagnostics_hardening_packet.md`
- Typed/pipeline anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c008-typed-sema-to-lowering-recovery-determinism-hardening-contract`
  - `test:tooling:m227-c008-typed-sema-to-lowering-recovery-determinism-hardening-contract`
  - `check:objc3c:m227-c008-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Required Evidence

- `tmp/reports/m227/M227-C008/typed_sema_to_lowering_recovery_determinism_hardening_contract_summary.json`

## Gate Commands

- `python scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
- `python scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m227-c008-lane-c-readiness`
