# M233 Runtime Metadata and Lookup Plumbing Advanced Diagnostics Workpack (Shard 1) Expectations (D017)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-advanced-diagnostics-workpack-shard1/m233-d017-v1`
Status: Accepted
Dependencies: `M233-D016`
Issue: `#6944`
Scope: M233 lane-D runtime metadata and lookup plumbing advanced diagnostics workpack (shard 1) continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
advanced diagnostics workpack (shard 1) anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Prerequisite advanced edge compatibility workpack (shard 1) assets from `M233-D016` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard1_d016_expectations.md`
  - `spec/planning/compiler/m233/m233_d016_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard1_packet.md`
  - `scripts/check_m233_d016_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m233_d016_runtime_metadata_and_lookup_plumbing_advanced_edge_compatibility_workpack_shard1_contract.py`
  - `scripts/run_m233_d016_lane_d_readiness.py`
- Packet/checker/test/readiness assets for `M233-D017` remain mandatory:
  - `spec/planning/compiler/m233/m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_packet.md`
  - `scripts/check_m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_contract.py`
  - `scripts/run_m233_d017_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves lane-D `M233-D017`
  advanced diagnostics workpack (shard 1) continuity anchors tied to `M233-D016` advanced-edge compatibility closure.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D runtime metadata and lookup
  advanced diagnostics workpack (shard 1) fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  runtime metadata and lookup advanced diagnostics workpack (shard 1) metadata wording for
  dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d017_lane_d_readiness.py` enforces predecessor chaining
  through `python scripts/run_m233_d016_lane_d_readiness.py` before D017 checks execute.
- `package.json` exposes:
  - `check:objc3c:m233-d017-installer-runtime-operations-lookup-plumbing-advanced-diagnostics-workpack-shard1-contract`
  - `test:tooling:m233-d017-installer-runtime-operations-lookup-plumbing-advanced-diagnostics-workpack-shard1-contract`
  - `check:objc3c:m233-d017-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d017_runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_contract.py -q`
- `python scripts/run_m233_d017_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D017/runtime_metadata_and_lookup_plumbing_advanced_diagnostics_workpack_shard1_contract_summary.json`


