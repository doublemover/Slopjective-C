# M241-D004 Interop Behavior for Qualified Generic APIs Core Feature Expansion Packet

Packet: `M241-D004`
Milestone: `M241`
Lane: `D`
Issue: `#5831`
Freeze date: `2026-03-05`
Dependencies: `M241-C001`

## Purpose

Freeze lane-D interop behavior for qualified generic APIs contract
prerequisites for M241 so nullability, generics, and qualifier completeness
interop boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m241_build_system_and_artifact_orchestration_core_feature_expansion_d004_expectations.md`
- Checker:
  `scripts/check_m241_d004_build_system_and_artifact_orchestration_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m241_d004_build_system_and_artifact_orchestration_contract.py`
- Dependency anchors from `M241-C001`:
  - `docs/contracts/m241_incremental_lowering_and_artifact_reuse_core_feature_expansion_c001_expectations.md`
  - `spec/planning/compiler/m241/m241_c001_incremental_lowering_and_artifact_reuse_core_feature_expansion_packet.md`
  - `scripts/check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py`
  - `tests/tooling/test_check_m241_c001_incremental_lowering_and_artifact_reuse_contract.py`
- `M241-C001` dependency continuity remains fail-closed across lane-D evidence checks.
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m241-c001-incremental-lowering-and-artifact-reuse-contract`
  - `test:tooling:m241-c001-incremental-lowering-and-artifact-reuse-contract`
  - `check:objc3c:m241-c001-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m241_d004_build_system_and_artifact_orchestration_contract.py`
- `python -m pytest tests/tooling/test_check_m241_d004_build_system_and_artifact_orchestration_contract.py -q`
- `npm run check:objc3c:m241-c001-lane-c-readiness`

## Evidence Output

- `tmp/reports/m241/M241-D004/build_system_and_artifact_orchestration_contract_summary.json`




