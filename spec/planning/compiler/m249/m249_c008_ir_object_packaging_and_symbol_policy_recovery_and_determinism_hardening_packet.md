# M249-C008 IR/Object Packaging and Symbol Policy Recovery and Determinism Hardening Packet

Packet: `M249-C008`
Milestone: `M249`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: `M249-C007`

## Purpose

Freeze lane-C IR/object packaging and symbol policy recovery and determinism
hardening continuity for M249 so artifact packaging boundaries and symbol
policy continuity remain deterministic and fail-closed, with dependency
surfaces, code/spec anchors, and milestone optimization improvements treated as
mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_c008_expectations.md`
- Checker:
  `scripts/check_m249_c008_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_c008_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_contract.py`
- Dependency anchors (`M249-C007`):
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_diagnostics_hardening_c007_expectations.md`
  - `spec/planning/compiler/m249/m249_c007_ir_object_packaging_and_symbol_policy_diagnostics_hardening_packet.md`
  - `scripts/check_m249_c007_ir_object_packaging_and_symbol_policy_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m249_c007_ir_object_packaging_and_symbol_policy_diagnostics_hardening_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-c008-ir-object-packaging-symbol-policy-recovery-and-determinism-hardening-contract`
  - `test:tooling:m249-c008-ir-object-packaging-symbol-policy-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m249-c008-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m249_c008_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c008_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m249-c008-lane-c-readiness`

## Evidence Output

- `tmp/reports/m249/M249-C008/ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_contract_summary.json`
