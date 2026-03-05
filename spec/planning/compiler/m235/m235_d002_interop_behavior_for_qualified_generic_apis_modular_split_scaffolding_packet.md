# M235-D002 Interop Behavior for Qualified Generic APIs Modular Split/Scaffolding Packet

Packet: `M235-D002`
Milestone: `M235`
Lane: `D`
Issue: `#5832`
Freeze date: `2026-03-05`
Dependencies: `M235-D001`

## Purpose

Freeze lane-D modular split/scaffolding prerequisites for M235 interop behavior
for qualified generic APIs continuity so dependency wiring remains deterministic
and fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_d002_expectations.md`
- Checker:
  `scripts/check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`
- Dependency anchors from `M235-D001`:
  - `docs/contracts/m235_interop_behavior_for_qualified_generic_apis_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m235/m235_d001_interop_behavior_for_qualified_generic_apis_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py`
  - `tests/tooling/test_check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m235-d001-interop-behavior-for-qualified-generic-apis-contract`
  - `test:tooling:m235-d001-interop-behavior-for-qualified-generic-apis-contract`
  - `check:objc3c:m235-d001-lane-d-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `npm run check:objc3c:m235-d001-lane-d-readiness`
- `python scripts/check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py -q`

## Evidence Output

- `tmp/reports/m235/M235-D002/interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract_summary.json`
