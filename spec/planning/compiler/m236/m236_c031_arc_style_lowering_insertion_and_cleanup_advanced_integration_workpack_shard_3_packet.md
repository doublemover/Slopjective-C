# M236-C031 Qualified Type Lowering and ABI Representation Advanced Integration Workpack (shard 3) Packet

Packet: `M236-C031`
Milestone: `M236`
Lane: `C`
Issue: `#5811`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-C qualified type lowering and ABI representation contract
prerequisites for M236 so nullability, generics, and qualifier completeness
lowering/ABI boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m236_arc_style_lowering_insertion_and_cleanup_advanced_integration_workpack_shard_3_c031_expectations.md`
- Checker:
  `scripts/check_m236_c031_arc_style_lowering_insertion_and_cleanup_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m236_c031_arc_style_lowering_insertion_and_cleanup_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m236-c031-arc-style-lowering-insertion-and-cleanup-contract`
  - `test:tooling:m236-c031-arc-style-lowering-insertion-and-cleanup-contract`
  - `check:objc3c:m236-c031-lane-c-readiness`
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

- `python scripts/check_m236_c031_arc_style_lowering_insertion_and_cleanup_contract.py`
- `python -m pytest tests/tooling/test_check_m236_c031_arc_style_lowering_insertion_and_cleanup_contract.py -q`
- `npm run check:objc3c:m236-c031-lane-c-readiness`

## Evidence Output

- `tmp/reports/m236/M236-C031/arc_style_lowering_insertion_and_cleanup_contract_summary.json`
































