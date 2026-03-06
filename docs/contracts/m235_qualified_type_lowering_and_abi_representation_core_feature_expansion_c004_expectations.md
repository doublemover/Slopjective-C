# M235 Qualified Type Lowering and ABI Representation Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-qualified-type-lowering-and-abi-representation-core-feature-expansion/m235-c004-v1`
Status: Accepted
Dependencies: `M235-C003`
Scope: M235 lane-C qualified type lowering and ABI representation core feature expansion continuity for deterministic dependency-chain governance.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
core feature expansion anchors remain explicit, deterministic, and traceable
across dependency-chain surfaces. Code/spec anchors and milestone
optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5814` defines canonical lane-C core feature expansion scope.
- Dependencies: `M235-C003`
- M235-C003 core feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m235/m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_packet.md`
  - `scripts/check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py`
- Packet/checker/test assets for C004 remain mandatory:
  - `spec/planning/compiler/m235/m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_packet.md`
  - `scripts/check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M235 lane-C C004
  qualified type lowering and ABI representation core feature expansion anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C qualified type
  lowering and ABI representation core feature expansion fail-closed dependency wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  qualified type lowering and ABI representation core feature expansion metadata wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m235-c003-lane-c-readiness`.
- `package.json` includes
  `check:objc3c:m235-c004-qualified-type-lowering-and-abi-representation-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m235-c004-qualified-type-lowering-and-abi-representation-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m235-c004-lane-c-readiness`.
- Readiness dependency chain order: `C003 readiness -> C004 checker -> C004 pytest`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m235-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m235/M235-C004/qualified_type_lowering_and_abi_representation_core_feature_expansion_contract_summary.json`
