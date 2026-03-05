# M233 Runtime Metadata and Lookup Plumbing Advanced Edge Compatibility Workpack (Shard 2) Expectations (D022)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-advanced-edge-compatibility-workpack-shard2/m233-d022-v1`
Status: Accepted
Dependencies: `M233-D021`
Issue: `#5647`
Scope: M233 lane-D runtime metadata and lookup plumbing advanced edge compatibility workpack (shard 2) continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
advanced edge compatibility workpack (shard 2) anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Prerequisite advanced core workpack (shard 2) assets from `M233-D021` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard2_d021_expectations.md`
  - `spec/planning/compiler/m233/m233_d021_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard2_packet.md`
  - `scripts/check_m233_d021_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard2_contract.py`
  - `tests/tooling/test_check_m233_d021_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard2_contract.py`
  - `scripts/run_m233_d021_lane_d_readiness.py`
- Packet/checker/test/readiness assets for `M233-D022` remain mandatory:
  - `spec/planning/compiler/m233/m233_d022_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard2_packet.md`
  - `scripts/check_m233_d022_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard2_contract.py`
  - `tests/tooling/test_check_m233_d022_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard2_contract.py`
  - `scripts/run_m233_d022_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves lane-D `M233-D022`
  advanced edge compatibility workpack (shard 2) continuity anchors tied to `M233-D021` advanced-core closure.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D runtime metadata and lookup
  advanced edge compatibility workpack (shard 2) fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  runtime metadata and lookup advanced edge compatibility workpack (shard 2) metadata wording for
  dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d022_lane_d_readiness.py` enforces predecessor chaining
  through `python scripts/run_m233_d021_lane_d_readiness.py` before D022 checks execute.
- `package.json` exposes:
  - `check:objc3c:m233-d022-installer-runtime-operations-lookup-plumbing-advanced-edge-compatibility-workpack-shard2-contract`
  - `test:tooling:m233-d022-installer-runtime-operations-lookup-plumbing-advanced-edge-compatibility-workpack-shard2-contract`
  - `check:objc3c:m233-d022-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d022_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d022_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard2_contract.py -q`
- `python scripts/run_m233_d022_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D022/runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard2_contract_summary.json`

