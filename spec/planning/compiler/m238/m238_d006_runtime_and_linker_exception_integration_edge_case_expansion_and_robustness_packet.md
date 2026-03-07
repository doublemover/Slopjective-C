# M238-D006 Interop Behavior for Qualified Generic APIs Edge-case Expansion and Robustness Packet

Packet: `M238-D006`
Milestone: `M238`
Lane: `D`
Issue: `#5831`
Freeze date: `2026-03-05`
Dependencies: `M238-C001`

## Purpose

Freeze lane-D interop behavior for qualified generic APIs contract
prerequisites for M238 so nullability, generics, and qualifier completeness
interop boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m238_runtime_and_linker_exception_integration_edge_case_expansion_and_robustness_d006_expectations.md`
- Checker:
  `scripts/check_m238_d006_runtime_and_linker_exception_integration_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m238_d006_runtime_and_linker_exception_integration_contract.py`
- Dependency anchors from `M238-C001`:
  - `docs/contracts/m238_cleanup_lowering_and_unwind_control_flow_edge_case_expansion_and_robustness_c001_expectations.md`
  - `spec/planning/compiler/m238/m238_c001_cleanup_lowering_and_unwind_control_flow_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m238_c001_cleanup_lowering_and_unwind_control_flow_contract.py`
  - `tests/tooling/test_check_m238_c001_cleanup_lowering_and_unwind_control_flow_contract.py`
- `M238-C001` dependency continuity remains fail-closed across lane-D evidence checks.
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m238-c001-cleanup-lowering-and-unwind-control-flow-contract`
  - `test:tooling:m238-c001-cleanup-lowering-and-unwind-control-flow-contract`
  - `check:objc3c:m238-c001-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m238_d006_runtime_and_linker_exception_integration_contract.py`
- `python -m pytest tests/tooling/test_check_m238_d006_runtime_and_linker_exception_integration_contract.py -q`
- `npm run check:objc3c:m238-c001-lane-c-readiness`

## Evidence Output

- `tmp/reports/m238/M238-D006/runtime_and_linker_exception_integration_contract_summary.json`






