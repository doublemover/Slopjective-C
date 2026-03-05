# M233 Runtime Metadata and Lookup Plumbing Advanced Core Workpack (Shard 3) Expectations (D027)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-advanced-core-workpack-shard3/m233-d027-v1`
Status: Accepted
Dependencies: `M233-D026`
Issue: `#5652`
Scope: M233 lane-D runtime metadata and lookup plumbing advanced core workpack (shard 3) continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
advanced core workpack (shard 3) anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Prerequisite advanced performance workpack (shard 2) assets from `M233-D026` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_advanced_performance_workpack_shard2_d026_expectations.md`
  - `spec/planning/compiler/m233/m233_d026_runtime_metadata_and_lookup_plumbing_advanced_performance_workpack_shard2_packet.md`
  - `scripts/check_m233_d026_runtime_metadata_and_lookup_plumbing_advanced_performance_workpack_shard2_contract.py`
  - `tests/tooling/test_check_m233_d026_runtime_metadata_and_lookup_plumbing_advanced_performance_workpack_shard2_contract.py`
  - `scripts/run_m233_d026_lane_d_readiness.py`
- Packet/checker/test/readiness assets for `M233-D027` remain mandatory:
  - `spec/planning/compiler/m233/m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_packet.md`
  - `scripts/check_m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_contract.py`
  - `tests/tooling/test_check_m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_contract.py`
  - `scripts/run_m233_d027_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves lane-D `M233-D027`
  advanced core workpack (shard 3) continuity anchors tied to `M233-D026` advanced-performance closure.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D runtime metadata and lookup
  advanced core workpack (shard 3) fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  runtime metadata and lookup advanced core workpack (shard 3) metadata wording for
  dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d027_lane_d_readiness.py` enforces predecessor chaining
  through `python scripts/run_m233_d026_lane_d_readiness.py` before D027 checks execute.
- `package.json` exposes:
  - `check:objc3c:m233-d027-installer-runtime-operations-lookup-plumbing-advanced-core-workpack-shard3-contract`
  - `test:tooling:m233-d027-installer-runtime-operations-lookup-plumbing-advanced-core-workpack-shard3-contract`
  - `check:objc3c:m233-d027-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_contract.py -q`
- `python scripts/run_m233_d027_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D027/runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_contract_summary.json`

