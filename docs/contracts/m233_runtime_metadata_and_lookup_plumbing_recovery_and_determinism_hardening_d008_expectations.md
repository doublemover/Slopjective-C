# M233 Runtime Metadata and Lookup Plumbing Recovery and Determinism Hardening Expectations (D008)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-recovery-and-determinism-hardening/m233-d008-v1`
Status: Accepted
Scope: M233 lane-D runtime metadata and lookup plumbing recovery and determinism hardening continuity for deterministic readiness-chain and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
recovery and determinism hardening anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M233-D007`
- Prerequisite diagnostics hardening assets from `M233-D007` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_diagnostics_hardening_d007_expectations.md`
  - `spec/planning/compiler/m233/m233_d007_runtime_metadata_and_lookup_plumbing_diagnostics_hardening_packet.md`
  - `scripts/check_m233_d007_runtime_metadata_and_lookup_plumbing_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m233_d007_runtime_metadata_and_lookup_plumbing_diagnostics_hardening_contract.py`
  - `scripts/run_m233_d007_lane_d_readiness.py`
- Packet/checker/test/readiness assets for `M233-D008` remain mandatory:
  - `spec/planning/compiler/m233/m233_d008_runtime_metadata_and_lookup_plumbing_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m233_d008_runtime_metadata_and_lookup_plumbing_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m233_d008_runtime_metadata_and_lookup_plumbing_recovery_and_determinism_hardening_contract.py`
  - `scripts/run_m233_d008_lane_d_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-D `M233-D004`
  runtime metadata and lookup core feature expansion anchors inherited by D005 through
  D008 readiness-chain recovery and determinism closure.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D runtime metadata and lookup
  recovery and determinism hardening fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  runtime metadata and lookup recovery and determinism hardening metadata wording for
  dependency continuity.

## Build and Readiness Integration

- `scripts/run_m233_d008_lane_d_readiness.py` enforces predecessor chaining
  through `python scripts/run_m233_d007_lane_d_readiness.py` before D008 checks execute.
- `package.json` continues to expose:
  - `check:objc3c:m233-d004-lane-d-readiness`
  - `compile:objc3c`
  - `proof:objc3c`
  - `test:objc3c:execution-replay-proof`
  - `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m233_d008_runtime_metadata_and_lookup_plumbing_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d008_runtime_metadata_and_lookup_plumbing_recovery_and_determinism_hardening_contract.py -q`
- `python scripts/run_m233_d008_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m233/M233-D008/runtime_metadata_and_lookup_plumbing_recovery_and_determinism_hardening_contract_summary.json`
