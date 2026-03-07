# M240 Interop Behavior for Qualified Generic APIs Modular Split and Scaffolding Expectations (D002)

Contract ID: `objc3c-runtime-registration-and-startup-integration/m240-d002-v1`
Status: Accepted
Dependencies: `M240-C001`
Scope: M240 lane-D interop behavior for qualified generic APIs modular split and scaffolding for nullability, generics, and qualifier completeness interop continuity.

## Objective

Fail closed unless lane-D interop behavior for qualified generic APIs anchors
remain explicit, deterministic, and traceable across code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6191` defines canonical lane-D modular split and scaffolding scope.
- Dependencies: `M240-C001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m240/m240_d002_runtime_registration_and_startup_integration_modular_split_and_scaffolding_packet.md`
  - `scripts/check_m240_d002_runtime_registration_and_startup_integration_contract.py`
  - `tests/tooling/test_check_m240_d002_runtime_registration_and_startup_integration_contract.py`
- Lane-D D002 freeze remains issue-local and fail-closed against `M240-C001` drift.
- Dependency anchor assets from `M240-C001` remain mandatory:
  - `docs/contracts/m240_metadata_lowering_and_section_emission_modular_split_and_scaffolding_c001_expectations.md`
  - `scripts/check_m240_c001_metadata_lowering_and_section_emission_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit lane-C `M240-C001`
  qualified type lowering and ABI representation anchors that lane-D interop
  freeze checks consume as fail-closed dependency continuity.
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

- `python scripts/check_m240_d002_runtime_registration_and_startup_integration_contract.py`
- `python -m pytest tests/tooling/test_check_m240_d002_runtime_registration_and_startup_integration_contract.py -q`

## Evidence Path

- `tmp/reports/m240/M240-D002/runtime_registration_and_startup_integration_contract_summary.json`


