# M249 IR/Object Packaging and Symbol Policy Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-ir-object-packaging-symbol-policy-core-feature-expansion/m249-c004-v1`
Status: Accepted
Scope: M249 lane-C IR/object packaging and symbol policy core feature expansion continuity with explicit `M249-C003` dependency governance.

## Objective

Fail closed unless lane-C IR/object packaging and symbol policy core-feature
implementation anchors remain explicit, deterministic, and traceable across
dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M249-C003`
- Upstream C003 assets remain mandatory prerequisites:
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m249/m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_packet.md`
  - `scripts/check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
- C004 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m249/m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_packet.md`
  - `scripts/check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C IR/object
  packaging and symbol policy core-feature dependency anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C IR/object packaging
  and symbol policy core-feature fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  IR/object packaging core-feature metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-c004-ir-object-packaging-symbol-policy-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m249-c004-ir-object-packaging-symbol-policy-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m249-c004-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m249-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m249/M249-C004/ir_object_packaging_and_symbol_policy_core_feature_expansion_contract_summary.json`
