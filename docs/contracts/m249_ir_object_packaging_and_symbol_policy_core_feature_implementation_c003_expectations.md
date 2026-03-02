# M249 IR/Object Packaging and Symbol Policy Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-ir-object-packaging-symbol-policy-core-feature-implementation/m249-c003-v1`
Status: Accepted
Scope: M249 lane-C IR/object packaging and symbol policy core feature implementation continuity with explicit `M249-C001` and `M249-C002` dependency governance.

## Objective

Fail closed unless lane-C IR/object packaging and symbol policy core-feature
implementation anchors remain explicit, deterministic, and traceable across
dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M249-C001`, `M249-C002`
- Upstream C001/C002 assets remain mandatory prerequisites:
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_contract_freeze_c001_expectations.md`
  - `spec/planning/compiler/m249/m249_c001_ir_object_packaging_and_symbol_policy_contract_freeze_packet.md`
  - `scripts/check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
  - `tests/tooling/test_check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m249/m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_packet.md`
  - `scripts/check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py`
- C003 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m249/m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_packet.md`
  - `scripts/check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C IR/object
  packaging and symbol policy core-feature dependency anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C IR/object packaging
  and symbol policy core-feature fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  IR/object packaging core-feature metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-c003-ir-object-packaging-symbol-policy-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m249-c003-ir-object-packaging-symbol-policy-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m249-c003-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m249-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m249/M249-C003/ir_object_packaging_and_symbol_policy_core_feature_implementation_contract_summary.json`

