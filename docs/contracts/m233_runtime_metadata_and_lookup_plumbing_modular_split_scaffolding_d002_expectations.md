# M233 Runtime Metadata and Lookup Plumbing Modular Split Scaffolding Expectations (D002)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-modular-split-scaffolding/m233-d002-v1`
Status: Accepted
Scope: M233 lane-D runtime metadata and lookup plumbing modular split/scaffolding continuity for deterministic runtime-route and lookup-plumbing governance.

## Objective

Fail closed unless M233 lane-D runtime metadata and lookup plumbing
modular split/scaffolding anchors remain explicit, deterministic, and
traceable across dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M233-D001`
- Prerequisite frozen assets from `M233-D001` remain mandatory:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m233/m233_d001_runtime_metadata_and_lookup_plumbing_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py`
  - `tests/tooling/test_check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py`
- Packet/checker/test assets for `M233-D002` remain mandatory:
  - `spec/planning/compiler/m233/m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_packet.md`
  - `scripts/check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-D `M233-D002`
  installer/runtime modular split dependency anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D
  installer/runtime modular split fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  installer/runtime modular split metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m233-d002-installer-runtime-operations-lookup-plumbing-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m233-d002-installer-runtime-operations-lookup-plumbing-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m233-d002-lane-d-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m233-d002-lane-d-readiness`

## Evidence Path

- `tmp/reports/m233/M233-D002/runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract_summary.json`
