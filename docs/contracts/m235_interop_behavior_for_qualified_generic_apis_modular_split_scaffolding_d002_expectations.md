# M235 Interop Behavior for Qualified Generic APIs Modular Split/Scaffolding Expectations (D002)

Contract ID: `objc3c-interop-behavior-for-qualified-generic-apis-modular-split-scaffolding/m235-d002-v1`
Status: Accepted
Scope: M235 lane-D modular split/scaffolding continuity for interop behavior for qualified generic APIs dependency wiring.

## Objective

Fail closed unless lane-D modular split/scaffolding dependency anchors remain
explicit, deterministic, and traceable across nullability, generics, and
qualifier completeness interop continuity surfaces. Code/spec anchors and
milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5832` defines canonical lane-D modular split/scaffolding scope.
- Dependencies: `M235-D001`
- M235-D001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m235_interop_behavior_for_qualified_generic_apis_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m235/m235_d001_interop_behavior_for_qualified_generic_apis_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py`
  - `tests/tooling/test_check_m235_d001_interop_behavior_for_qualified_generic_apis_contract.py`
- Packet/checker/test assets for D002 remain mandatory:
  - `spec/planning/compiler/m235/m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_packet.md`
  - `scripts/check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M235 lane-D D001
  interop behavior for qualified generic APIs contract-freeze anchors consumed
  as fail-closed dependency continuity by D002.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D interop behavior
  for qualified generic APIs fail-closed dependency wording inherited by D002.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  interop behavior for qualified generic APIs metadata wording inherited by
  D002.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-d001-interop-behavior-for-qualified-generic-apis-contract`.
- `package.json` includes
  `test:tooling:m235-d001-interop-behavior-for-qualified-generic-apis-contract`.
- `package.json` includes `check:objc3c:m235-d001-lane-d-readiness`.
- Readiness dependency chain order:
  `C001 readiness -> D001 readiness -> D002 checker -> D002 pytest`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `npm run check:objc3c:m235-d001-lane-d-readiness`
- `python scripts/check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py -q`

## Evidence Path

- `tmp/reports/m235/M235-D002/interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract_summary.json`
