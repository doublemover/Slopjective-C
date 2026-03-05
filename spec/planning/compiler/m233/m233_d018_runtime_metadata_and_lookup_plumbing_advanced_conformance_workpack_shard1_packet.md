# M233-D018 Runtime Metadata and Lookup Plumbing Advanced Conformance Workpack (Shard 1) Packet

Packet: `M233-D018`
Issue: `#6945`
Milestone: `M233`
Lane: `D`
Freeze date: `2026-03-04`
Dependencies: `M233-D017`

## Purpose

Freeze lane-D runtime metadata and lookup plumbing advanced conformance
workpack (shard 1) prerequisites so predecessor continuity remains explicit,
deterministic, and fail-closed, including code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_d018_expectations.md`
- Checker:
  `scripts/check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py`
- Readiness runner:
  `scripts/run_m233_d018_lane_d_readiness.py`
  - Chains through `python scripts/run_m233_d017_lane_d_readiness.py` before D018 checks.
- Dependency anchors from `M233-D017`:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_d017_expectations.md`
  - `spec/planning/compiler/m233/m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_packet.md`
  - `scripts/check_m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_contract.py`
  - `scripts/run_m233_d017_lane_d_readiness.py`
- Existing build/readiness anchors (`package.json`):
  - `check:objc3c:m233-d018-installer-runtime-operations-lookup-plumbing-advanced-conformance-workpack-shard1-contract`
  - `test:tooling:m233-d018-installer-runtime-operations-lookup-plumbing-advanced-conformance-workpack-shard1-contract`
  - `check:objc3c:m233-d018-lane-d-readiness`
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

- `python scripts/check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d018_runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract.py -q`
- `python scripts/run_m233_d018_lane_d_readiness.py`

## Evidence Output

- `tmp/reports/m233/M233-D018/runtime_metadata_and_lookup_plumbing_advanced_conformance_workpack_shard1_contract_summary.json`

