# M241 Qualified Type Lowering and ABI Representation Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-incremental-lowering-and-artifact-reuse-contract/m241-c001-v1`
Status: Accepted
Dependencies: none
Scope: M241 lane-C qualified type lowering and ABI representation contract and architecture freeze for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
anchors remain explicit, deterministic, and traceable across code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6257` defines canonical lane-C contract and architecture freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m241/m241_c001_incremental_lowering_and_artifact_reuse_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py`
  - `tests/tooling/test_check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M241 lane-C C001
  qualified type lowering and ABI representation fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m241-c001-incremental-lowering-and-artifact-reuse-contract`.
- `package.json` includes
  `test:tooling:m241-c001-incremental-lowering-and-artifact-reuse-contract`.
- `package.json` includes `check:objc3c:m241-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py`
- `python -m pytest tests/tooling/test_check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py -q`
- `npm run check:objc3c:m241-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m241/M241-C001/incremental_lowering_and_artifact_reuse_contract_summary.json`


