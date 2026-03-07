# M242-C017 Qualified Type Lowering and ABI Representation Integration Closeout and Gate Sign-off Packet

Packet: `M242-C017`
Milestone: `M242`
Lane: `C`
Issue: `#5811`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-C qualified type lowering and ABI representation contract
prerequisites for M242 so nullability, generics, and qualifier completeness
lowering/ABI boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m242_expanded_source_lowering_traceability_integration_closeout_and_gate_sign_off_c017_expectations.md`
- Checker:
  `scripts/check_m242_c017_expanded_source_lowering_traceability_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m242_c017_expanded_source_lowering_traceability_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m242-c017-expanded-source-lowering-traceability-contract`
  - `test:tooling:m242-c017-expanded-source-lowering-traceability-contract`
  - `check:objc3c:m242-c017-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m242_c017_expanded_source_lowering_traceability_contract.py`
- `python -m pytest tests/tooling/test_check_m242_c017_expanded_source_lowering_traceability_contract.py -q`
- `npm run check:objc3c:m242-c017-lane-c-readiness`

## Evidence Output

- `tmp/reports/m242/M242-C017/expanded_source_lowering_traceability_contract_summary.json`


















