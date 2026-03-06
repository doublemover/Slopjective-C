# M235 Interop Behavior for Qualified Generic APIs Core Feature Expansion Expectations (D004)

Contract ID: `objc3c-interop-behavior-for-qualified-generic-apis-core-feature-expansion/m235-d004-v1`
Status: Accepted
Scope: M235 lane-D core feature expansion continuity for interop behavior for qualified generic APIs dependency wiring.

## Objective

Fail closed unless lane-D core feature expansion dependency anchors remain
explicit, deterministic, and traceable across nullability, generics, and
qualifier completeness interop continuity surfaces. Code/spec anchors and
milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5834` defines canonical lane-D core feature expansion scope.
- Dependencies: `M235-D003`
- M235-D003 core feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m235_interop_behavior_for_qualified_generic_apis_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m235/m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_packet.md`
  - `scripts/check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m235_d003_interop_behavior_for_qualified_generic_apis_core_feature_implementation_contract.py`
- Packet/checker/test assets for D004 remain mandatory:
  - `spec/planning/compiler/m235/m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_packet.md`
  - `scripts/check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M235 lane-D D004
  interop behavior for qualified generic APIs core feature expansion anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-D interop behavior
  for qualified generic APIs core feature expansion fail-closed dependency
  wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-D
  interop behavior for qualified generic APIs core feature expansion metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-d003-interop-behavior-for-qualified-generic-apis-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m235-d003-interop-behavior-for-qualified-generic-apis-core-feature-implementation-contract`.
- `package.json` includes
  `check:objc3c:m235-d004-interop-behavior-for-qualified-generic-apis-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m235-d004-interop-behavior-for-qualified-generic-apis-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m235-d003-lane-d-readiness`.
- `package.json` includes `check:objc3c:m235-d004-lane-d-readiness`.
- Readiness dependency chain order:
  `D003 readiness -> D004 checker -> D004 pytest`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `npm run check:objc3c:m235-d003-lane-d-readiness`
- `python scripts/check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_d004_interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m235-d004-lane-d-readiness`

## Evidence Path

- `tmp/reports/m235/M235-D004/interop_behavior_for_qualified_generic_apis_core_feature_expansion_contract_summary.json`


