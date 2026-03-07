# M238-C024 Qualified Type Lowering and ABI Representation Advanced Conformance Workpack (shard 2) Packet

Packet: `M238-C024`
Milestone: `M238`
Lane: `C`
Issue: `#5811`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-C qualified type lowering and ABI representation contract
prerequisites for M238 so nullability, generics, and qualifier completeness
lowering/ABI boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m238_cleanup_lowering_and_unwind_control_flow_advanced_conformance_workpack_shard_2_c024_expectations.md`
- Checker:
  `scripts/check_m238_c024_cleanup_lowering_and_unwind_control_flow_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m238_c024_cleanup_lowering_and_unwind_control_flow_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m238-c024-cleanup-lowering-and-unwind-control-flow-contract`
  - `test:tooling:m238-c024-cleanup-lowering-and-unwind-control-flow-contract`
  - `check:objc3c:m238-c024-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m238_c024_cleanup_lowering_and_unwind_control_flow_contract.py`
- `python -m pytest tests/tooling/test_check_m238_c024_cleanup_lowering_and_unwind_control_flow_contract.py -q`
- `npm run check:objc3c:m238-c024-lane-c-readiness`

## Evidence Output

- `tmp/reports/m238/M238-C024/cleanup_lowering_and_unwind_control_flow_contract_summary.json`

























