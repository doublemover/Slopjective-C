# M233 Runtime Metadata and Lookup Plumbing Contract and Architecture Freeze Expectations (D001)

Contract ID: `objc3c-installer-runtime-operations-lookup-plumbing-contract/m233-d001-v1`
Status: Accepted
Scope: M233 lane-D runtime metadata and lookup plumbing freeze for architecture-level readiness gating.

## Objective

Fail closed unless lane-D runtime metadata and lookup plumbing
anchors remain explicit, deterministic, and traceable across code/spec anchors
and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m233/m233_d001_runtime_metadata_and_lookup_plumbing_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py`
  - `tests/tooling/test_check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M233 lane-D D001
  runtime metadata and lookup plumbing fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-D installer/runtime
  operations and support tooling fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-D
  installer/runtime metadata anchor wording for M233-D001.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m233-d001-installer-runtime-operations-lookup-plumbing-contract`.
- `package.json` includes
  `test:tooling:m233-d001-installer-runtime-operations-lookup-plumbing-contract`.
- `package.json` includes `check:objc3c:m233-d001-lane-d-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py -q`
- `npm run check:objc3c:m233-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m233/M233-D001/runtime_metadata_and_lookup_plumbing_contract_summary.json`
