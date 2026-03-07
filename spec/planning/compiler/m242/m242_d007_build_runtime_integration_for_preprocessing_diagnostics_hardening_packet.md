# M242-D007 Interop Behavior for Qualified Generic APIs Diagnostics Hardening Packet

Packet: `M242-D007`
Milestone: `M242`
Lane: `D`
Issue: `#5831`
Freeze date: `2026-03-05`
Dependencies: `M242-C001`

## Purpose

Freeze lane-D interop behavior for qualified generic APIs contract
prerequisites for M242 so nullability, generics, and qualifier completeness
interop boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m242_build_runtime_integration_for_preprocessing_diagnostics_hardening_d007_expectations.md`
- Checker:
  `scripts/check_m242_d007_build_runtime_integration_for_preprocessing_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m242_d007_build_runtime_integration_for_preprocessing_contract.py`
- Dependency anchors from `M242-C001`:
  - `docs/contracts/m242_expanded_source_lowering_traceability_diagnostics_hardening_c001_expectations.md`
  - `spec/planning/compiler/m242/m242_c001_expanded_source_lowering_traceability_diagnostics_hardening_packet.md`
  - `scripts/check_m242_c001_expanded_source_lowering_traceability_contract.py`
  - `tests/tooling/test_check_m242_c001_expanded_source_lowering_traceability_contract.py`
- `M242-C001` dependency continuity remains fail-closed across lane-D evidence checks.
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m242-c001-expanded-source-lowering-traceability-contract`
  - `test:tooling:m242-c001-expanded-source-lowering-traceability-contract`
  - `check:objc3c:m242-c001-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m242_d007_build_runtime_integration_for_preprocessing_contract.py`
- `python -m pytest tests/tooling/test_check_m242_d007_build_runtime_integration_for_preprocessing_contract.py -q`
- `npm run check:objc3c:m242-c001-lane-c-readiness`

## Evidence Output

- `tmp/reports/m242/M242-D007/build_runtime_integration_for_preprocessing_contract_summary.json`







