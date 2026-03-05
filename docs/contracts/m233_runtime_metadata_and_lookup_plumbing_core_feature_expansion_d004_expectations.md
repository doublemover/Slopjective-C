# M233 Runtime Metadata and Lookup Plumbing Core Feature Expansion Expectations (D004)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-core-feature-expansion/m233-d004-v1`
Status: Accepted
Scope: M233 lane-D runtime metadata and lookup plumbing core feature expansion continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
core feature expansion anchors remain explicit, deterministic, and traceable
across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M233-D003`
- Prerequisite core feature implementation assets from `M233-D003` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m233/m233_d003_runtime_metadata_and_lookup_plumbing_core_feature_implementation_packet.md`
  - `scripts/check_m233_d003_runtime_metadata_and_lookup_plumbing_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m233_d003_runtime_metadata_and_lookup_plumbing_core_feature_implementation_contract.py`
- Packet/checker/test/readiness assets for `M233-D004` remain mandatory:
  - `spec/planning/compiler/m233/m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_packet.md`
  - `scripts/check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
  - `scripts/run_m233_d004_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-D `M233-D003`
  installer/runtime core feature implementation anchors inherited by D004
  readiness-chain expansion.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D installer/runtime
  core feature implementation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  installer/runtime core feature metadata wording for dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d004_lane_d_readiness.py` enforces predecessor chaining
  through `check:objc3c:m233-d003-lane-d-readiness` before D004 checks execute.
- `package.json` continues to expose:
  - `check:objc3c:m233-d003-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py -q`
- `python scripts/run_m233_d004_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D004/runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract_summary.json`
