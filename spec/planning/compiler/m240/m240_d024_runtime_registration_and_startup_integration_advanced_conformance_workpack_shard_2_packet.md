# M240-D024 Interop Behavior for Qualified Generic APIs Advanced Conformance Workpack (shard 2) Packet

Packet: `M240-D024`
Milestone: `M240`
Lane: `D`
Issue: `#5831`
Freeze date: `2026-03-05`
Dependencies: `M240-C001`

## Purpose

Freeze lane-D interop behavior for qualified generic APIs contract
prerequisites for M240 so nullability, generics, and qualifier completeness
interop boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m240_runtime_registration_and_startup_integration_advanced_conformance_workpack_shard_2_d024_expectations.md`
- Checker:
  `scripts/check_m240_d024_runtime_registration_and_startup_integration_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m240_d024_runtime_registration_and_startup_integration_contract.py`
- Dependency anchors from `M240-C001`:
  - `docs/contracts/m240_metadata_lowering_and_section_emission_advanced_conformance_workpack_shard_2_c001_expectations.md`
  - `spec/planning/compiler/m240/m240_c001_metadata_lowering_and_section_emission_advanced_conformance_workpack_shard_2_packet.md`
  - `scripts/check_m240_c001_metadata_lowering_and_section_emission_contract.py`
  - `tests/tooling/test_check_m240_c001_metadata_lowering_and_section_emission_contract.py`
- `M240-C001` dependency continuity remains fail-closed across lane-D evidence checks.
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m240-c001-metadata-lowering-and-section-emission-contract`
  - `test:tooling:m240-c001-metadata-lowering-and-section-emission-contract`
  - `check:objc3c:m240-c001-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m240_d024_runtime_registration_and_startup_integration_contract.py`
- `python -m pytest tests/tooling/test_check_m240_d024_runtime_registration_and_startup_integration_contract.py -q`
- `npm run check:objc3c:m240-c001-lane-c-readiness`

## Evidence Output

- `tmp/reports/m240/M240-D024/runtime_registration_and_startup_integration_contract_summary.json`
























