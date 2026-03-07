# M238 Interop Behavior for Qualified Generic APIs Conformance Corpus Expansion Expectations (D010)

Contract ID: `objc3c-runtime-and-linker-exception-integration/m238-d010-v1`
Status: Accepted
Dependencies: `M238-C001`
Scope: M238 lane-D interop behavior for qualified generic APIs conformance corpus expansion for nullability, generics, and qualifier completeness interop continuity.

## Objective

Fail closed unless lane-D interop behavior for qualified generic APIs anchors
remain explicit, deterministic, and traceable across code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6113` defines canonical lane-D conformance corpus expansion scope.
- Dependencies: `M238-C001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m238/m238_d010_runtime_and_linker_exception_integration_conformance_corpus_expansion_packet.md`
  - `scripts/check_m238_d010_runtime_and_linker_exception_integration_contract.py`
  - `tests/tooling/test_check_m238_d010_runtime_and_linker_exception_integration_contract.py`
- Lane-D D010 freeze remains issue-local and fail-closed against `M238-C001` drift.
- Dependency anchor assets from `M238-C001` remain mandatory:
  - `docs/contracts/m238_cleanup_lowering_and_unwind_control_flow_conformance_corpus_expansion_c001_expectations.md`
  - `scripts/check_m238_c001_cleanup_lowering_and_unwind_control_flow_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit lane-C `M238-C001`
  qualified type lowering and ABI representation anchors that lane-D interop
  freeze checks consume as fail-closed dependency continuity.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m238-c001-cleanup-lowering-and-unwind-control-flow-contract`.
- `package.json` includes
  `test:tooling:m238-c001-cleanup-lowering-and-unwind-control-flow-contract`.
- `package.json` includes `check:objc3c:m238-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m238_d010_runtime_and_linker_exception_integration_contract.py`
- `python -m pytest tests/tooling/test_check_m238_d010_runtime_and_linker_exception_integration_contract.py -q`

## Evidence Path

- `tmp/reports/m238/M238-D010/runtime_and_linker_exception_integration_contract_summary.json`










