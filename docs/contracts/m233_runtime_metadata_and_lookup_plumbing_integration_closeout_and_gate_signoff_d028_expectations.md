# M233 Runtime Metadata and Lookup Plumbing Integration Closeout and Gate Sign-Off Expectations (D028)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-integration-closeout-and-gate-signoff/m233-d028-v1`
Status: Accepted
Dependencies: `M233-D027`
Issue: `#5653`
Scope: M233 lane-D runtime metadata and lookup plumbing integration closeout and gate sign-off continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
integration closeout and gate sign-off anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Prerequisite advanced core workpack (shard 3) assets from `M233-D027` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_d027_expectations.md`
  - `spec/planning/compiler/m233/m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_packet.md`
  - `scripts/check_m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_contract.py`
  - `tests/tooling/test_check_m233_d027_runtime_metadata_and_lookup_plumbing_advanced_core_workpack_shard3_contract.py`
  - `scripts/run_m233_d027_lane_d_readiness.py`
- Packet/checker/test/readiness assets for `M233-D028` remain mandatory:
  - `spec/planning/compiler/m233/m233_d028_runtime_metadata_and_lookup_plumbing_integration_closeout_and_gate_signoff_packet.md`
  - `scripts/check_m233_d028_runtime_metadata_and_lookup_plumbing_integration_closeout_and_gate_signoff_contract.py`
  - `tests/tooling/test_check_m233_d028_runtime_metadata_and_lookup_plumbing_integration_closeout_and_gate_signoff_contract.py`
  - `scripts/run_m233_d028_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves lane-D `M233-D028`
  integration closeout and gate sign-off continuity anchors tied to `M233-D027` advanced-core closure.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D runtime metadata and lookup
  integration closeout and gate sign-off fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  runtime metadata and lookup integration closeout and gate sign-off metadata wording for
  dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d028_lane_d_readiness.py` enforces predecessor chaining
  through `python scripts/run_m233_d027_lane_d_readiness.py` before D028 checks execute.
- `package.json` exposes:
  - `check:objc3c:m233-d028-installer-runtime-operations-lookup-plumbing-integration-closeout-and-gate-signoff-contract`
  - `test:tooling:m233-d028-installer-runtime-operations-lookup-plumbing-integration-closeout-and-gate-signoff-contract`
  - `check:objc3c:m233-d028-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d028_runtime_metadata_and_lookup_plumbing_integration_closeout_and_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d028_runtime_metadata_and_lookup_plumbing_integration_closeout_and_gate_signoff_contract.py -q`
- `python scripts/run_m233_d028_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D028/runtime_metadata_and_lookup_plumbing_integration_closeout_and_gate_signoff_contract_summary.json`

