# M236-D004 Interop Behavior for Qualified Generic APIs Core Feature Expansion Packet

Packet: `M236-D004`
Milestone: `M236`
Lane: `D`
Issue: `#5831`
Freeze date: `2026-03-05`
Dependencies: `M236-C001`

## Purpose

Freeze lane-D interop behavior for qualified generic APIs contract
prerequisites for M236 so nullability, generics, and qualifier completeness
interop boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m236_runtime_ownership_abi_and_interoperability_core_feature_expansion_d004_expectations.md`
- Checker:
  `scripts/check_m236_d004_runtime_ownership_abi_and_interoperability_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m236_d004_runtime_ownership_abi_and_interoperability_contract.py`
- Dependency anchors from `M236-C001`:
  - `docs/contracts/m236_arc_style_lowering_insertion_and_cleanup_core_feature_expansion_c001_expectations.md`
  - `spec/planning/compiler/m236/m236_c001_arc_style_lowering_insertion_and_cleanup_core_feature_expansion_packet.md`
  - `scripts/check_m236_c001_arc_style_lowering_insertion_and_cleanup_contract.py`
  - `tests/tooling/test_check_m236_c001_arc_style_lowering_insertion_and_cleanup_contract.py`
- `M236-C001` dependency continuity remains fail-closed across lane-D evidence checks.
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m236-c001-arc-style-lowering-insertion-and-cleanup-contract`
  - `test:tooling:m236-c001-arc-style-lowering-insertion-and-cleanup-contract`
  - `check:objc3c:m236-c001-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m236_d004_runtime_ownership_abi_and_interoperability_contract.py`
- `python -m pytest tests/tooling/test_check_m236_d004_runtime_ownership_abi_and_interoperability_contract.py -q`
- `npm run check:objc3c:m236-c001-lane-c-readiness`

## Evidence Output

- `tmp/reports/m236/M236-D004/runtime_ownership_abi_and_interoperability_contract_summary.json`




