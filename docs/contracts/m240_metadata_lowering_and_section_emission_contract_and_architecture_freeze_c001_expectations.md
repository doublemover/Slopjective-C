# M240 Qualified Type Lowering and ABI Representation Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-metadata-lowering-and-section-emission-contract/m240-c001-v1`
Status: Accepted
Dependencies: none
Scope: M240 lane-C qualified type lowering and ABI representation contract and architecture freeze for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
anchors remain explicit, deterministic, and traceable across code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6170` defines canonical lane-C contract and architecture freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m240/m240_c001_metadata_lowering_and_section_emission_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m240_c001_metadata_lowering_and_section_emission_contract.py`
  - `tests/tooling/test_check_m240_c001_metadata_lowering_and_section_emission_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M240 lane-C C001
  qualified type lowering and ABI representation fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m240-c001-metadata-lowering-and-section-emission-contract`.
- `package.json` includes
  `test:tooling:m240-c001-metadata-lowering-and-section-emission-contract`.
- `package.json` includes `check:objc3c:m240-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m240_c001_metadata_lowering_and_section_emission_contract.py`
- `python -m pytest tests/tooling/test_check_m240_c001_metadata_lowering_and_section_emission_contract.py -q`
- `npm run check:objc3c:m240-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m240/M240-C001/metadata_lowering_and_section_emission_contract_summary.json`


