# M233-D004 Runtime Metadata and Lookup Plumbing Core Feature Expansion Packet

Packet: `M233-D004`
Milestone: `M233`
Lane: `D`
Freeze date: `2026-03-03`
Dependencies: `M233-D003`

## Purpose

Freeze lane-D runtime metadata and lookup plumbing core feature
expansion prerequisites for M233 so predecessor continuity remains explicit,
deterministic, and fail-closed, including code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_core_feature_expansion_d004_expectations.md`
- Checker:
  `scripts/check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
- Readiness runner:
  `scripts/run_m233_d004_lane_d_readiness.py`
- Dependency anchors from `M233-D003`:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m233/m233_d003_runtime_metadata_and_lookup_plumbing_core_feature_implementation_packet.md`
  - `scripts/check_m233_d003_runtime_metadata_and_lookup_plumbing_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m233_d003_runtime_metadata_and_lookup_plumbing_core_feature_implementation_contract.py`
- Existing build/readiness anchors (`package.json`):
  - `check:objc3c:m233-d003-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`
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

- `python scripts/check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py -q`
- `python scripts/run_m233_d004_lane_d_readiness.py`

## Evidence Output

- `tmp/reports/m233/M233-D004/runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract_summary.json`
