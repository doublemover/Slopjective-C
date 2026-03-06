# M229-B008 Dispatch ABI and selector resolution Recovery and Determinism Hardening Packet

Packet: `M229-B008`
Milestone: `M229`
Lane: `B`
Issue: `#5319`
Freeze date: `2026-03-06`
Dependencies: `M229-B007`

## Purpose

Execute recovery and determinism hardening governance for lane-B dispatch ABI and selector resolution so declaration-grammar boundary anchors remain deterministic and fail-closed before modular split/scaffolding expansion.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m229_dispatch_abi_and_selector_resolution_recovery_and_determinism_hardening_b008_expectations.md`
- Checker:
  `scripts/check_m229_b008_dispatch_abi_and_selector_resolution_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m229_b008_dispatch_abi_and_selector_resolution_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m229-b008-dispatch-abi-and-selector-resolution-recovery-and-determinism-hardening-contract`
  - `test:tooling:m229-b008-dispatch-abi-and-selector-resolution-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m229-b008-lane-b-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m229_b008_dispatch_abi_and_selector_resolution_contract.py`
- `python -m pytest tests/tooling/test_check_m229_b008_dispatch_abi_and_selector_resolution_contract.py -q`
- `npm run check:objc3c:m229-b008-lane-b-readiness`

## Evidence Output

- `tmp/reports/m229/M229-B008/dispatch_abi_and_selector_resolution_recovery_and_determinism_hardening_summary.json`

































