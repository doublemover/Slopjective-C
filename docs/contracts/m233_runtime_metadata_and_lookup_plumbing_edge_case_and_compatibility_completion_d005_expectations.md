# M233 Runtime Metadata and Lookup Plumbing Edge-Case and Compatibility Completion Expectations (D005)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-edge-case-and-compatibility-completion/m233-d005-v1`
Status: Accepted
Scope: M233 lane-D runtime metadata and lookup plumbing edge-case and compatibility completion continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
edge-case and compatibility completion anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M233-D004`
- Prerequisite core feature expansion assets from `M233-D004` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_core_feature_expansion_d004_expectations.md`
  - `spec/planning/compiler/m233/m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_packet.md`
  - `scripts/check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m233_d004_runtime_metadata_and_lookup_plumbing_core_feature_expansion_contract.py`
  - `scripts/run_m233_d004_lane_d_readiness.py`
- Packet/checker/test/readiness assets for `M233-D005` remain mandatory:
  - `spec/planning/compiler/m233/m233_d005_runtime_metadata_and_lookup_plumbing_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m233_d005_runtime_metadata_and_lookup_plumbing_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m233_d005_runtime_metadata_and_lookup_plumbing_edge_case_and_compatibility_completion_contract.py`
  - `scripts/run_m233_d005_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-D `M233-D004`
  installer/runtime core feature expansion anchors inherited by D005
  readiness-chain completion.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D installer/runtime
  edge-case and compatibility completion fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  installer/runtime edge-case and compatibility completion metadata wording for
  dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d005_lane_d_readiness.py` enforces predecessor chaining
  through `check:objc3c:m233-d004-lane-d-readiness` before D005 checks execute.
- `package.json` continues to expose:
  - `check:objc3c:m233-d004-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d005_runtime_metadata_and_lookup_plumbing_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d005_runtime_metadata_and_lookup_plumbing_edge_case_and_compatibility_completion_contract.py -q`
- `python scripts/run_m233_d005_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D005/runtime_metadata_and_lookup_plumbing_edge_case_and_compatibility_completion_contract_summary.json`
