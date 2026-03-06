# M235 Interop Behavior for Qualified Generic APIs Core Feature Implementation Expectations (D003)

Contract ID: `objc3c-interop-behavior-for-qualified-generic-apis-core-feature-implementation/m235-d003-v1`
Status: Accepted
Scope: M235 lane-D core feature implementation continuity for interop behavior for qualified generic APIs dependency wiring.

## Objective

Fail closed unless lane-D core feature implementation dependency anchors remain
explicit, deterministic, and traceable across nullability, generics, and
qualifier completeness interop continuity surfaces. Code/spec anchors and
milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5833` defines canonical lane-D core feature implementation scope.
- Dependencies: `M235-D002`
- M235-D002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m235_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_d002_expectations.md`
  - `spec/planning/compiler/m235/m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_packet.md`
  - `scripts/check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m235_d002_interop_behavior_for_qualified_generic_apis_modular_split_scaffolding_contract.py`
- Packet/checker/test assets for D003 remain mandatory:
  - `spec/planning/compiler/m235/m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_packet.md`
  - `scripts/check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M235 lane-D D002
  interop behavior for qualified generic APIs modular split/scaffolding anchors
  consumed as fail-closed dependency continuity by D003.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D interop behavior
  for qualified generic APIs modular split/scaffolding fail-closed dependency
  wording inherited by D003.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  interop behavior for qualified generic APIs modular split/scaffolding
  metadata wording inherited by D003.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-d002-interop-behavior-for-qualified-generic-apis-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m235-d002-interop-behavior-for-qualified-generic-apis-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m235-d002-lane-d-readiness`.
- Readiness dependency chain order:
  `C001 readiness -> D001 readiness -> D002 readiness -> D003 checker -> D003 pytest`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `npm run check:objc3c:m235-d002-lane-d-readiness`
- `python scripts/check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py -q`

## Evidence Path

- `tmp/reports/m235/M235-D003/interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract_summary.json`
