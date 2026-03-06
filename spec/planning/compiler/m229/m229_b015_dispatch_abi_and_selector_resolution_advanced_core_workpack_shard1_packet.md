# M229-B015 Dispatch ABI and selector resolution Advanced Core Workpack (Shard 1) Packet

Packet: `M229-B015`
Milestone: `M229`
Lane: `B`
Issue: `#5327`
Freeze date: `2026-03-06`
Dependencies: `M229-B014`

## Purpose

Execute advanced core workpack (shard 1) governance for lane-B dispatch ABI and selector resolution so declaration-grammar boundary anchors remain deterministic and fail-closed before modular split/scaffolding expansion.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m229_dispatch_abi_and_selector_resolution_advanced_core_workpack_shard1_b015_expectations.md`
- Checker:
  `scripts/check_m229_b015_dispatch_abi_and_selector_resolution_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m229_b015_dispatch_abi_and_selector_resolution_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m229-b015-dispatch-abi-and-selector-resolution-advanced-core-workpack-shard1-contract`
  - `test:tooling:m229-b015-dispatch-abi-and-selector-resolution-advanced-core-workpack-shard1-contract`
  - `check:objc3c:m229-b015-lane-b-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m229_b015_dispatch_abi_and_selector_resolution_contract.py`
- `python -m pytest tests/tooling/test_check_m229_b015_dispatch_abi_and_selector_resolution_contract.py -q`
- `npm run check:objc3c:m229-b015-lane-b-readiness`

## Evidence Output

- `tmp/reports/m229/M229-B015/dispatch_abi_and_selector_resolution_advanced_core_workpack_shard1_summary.json`















































