# M235 Interop Behavior for Qualified Generic APIs Contract and Architecture Freeze Expectations (D001)

Contract ID: `objc3c-interop-behavior-for-qualified-generic-apis/m235-d001-v1`
Status: Accepted
Dependencies: `M235-C001`
Scope: M235 lane-D interop behavior for qualified generic APIs contract and architecture freeze for nullability, generics, and qualifier completeness interop continuity.

## Objective

Fail closed unless lane-D interop behavior for qualified generic APIs anchors
remain explicit, deterministic, and traceable across code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5831` defines canonical lane-D contract and architecture freeze scope.
- Dependencies: `M235-C001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m235/m235_d001_interop_behavior_for_qualified_generic_apis_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py`
  - `tests/tooling/test_check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py`
- Dependency anchor assets from `M235-C001` remain mandatory:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_contract_and_architecture_freeze_c001_expectations.md`
  - `scripts/check_m235_c001_qualified_type_lowering_and_abi_representation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit lane-C `M235-C001`
  qualified type lowering and ABI representation anchors that lane-D interop
  freeze checks consume as fail-closed dependency continuity.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-c001-qualified-type-lowering-and-abi-representation-contract`.
- `package.json` includes
  `test:tooling:m235-c001-qualified-type-lowering-and-abi-representation-contract`.
- `package.json` includes `check:objc3c:m235-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py`
- `python -m pytest tests/tooling/test_check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py -q`

## Evidence Path

- `tmp/reports/m235/M235-D001/interop_behavior_for_qualified_generic_apis_contract_summary.json`
