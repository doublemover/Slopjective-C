# M249 IR/Object Packaging and Symbol Policy Edge-Case and Compatibility Completion Expectations (C005)

Contract ID: `objc3c-ir-object-packaging-symbol-policy-edge-case-and-compatibility-completion/m249-c005-v1`
Status: Accepted
Scope: M249 lane-C IR/object packaging and symbol policy edge-case and compatibility completion continuity with explicit `M249-C004` dependency governance.

## Objective

Fail closed unless lane-C IR/object packaging and symbol policy edge-case and
compatibility completion anchors remain explicit, deterministic, and traceable
across dependency surfaces, including code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M249-C004`
- Upstream C004 assets remain mandatory prerequisites:
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_core_feature_expansion_c004_expectations.md`
  - `spec/planning/compiler/m249/m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_packet.md`
  - `scripts/check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`
- C005 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m249/m249_c005_ir_object_packaging_and_symbol_policy_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m249_c005_ir_object_packaging_and_symbol_policy_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m249_c005_ir_object_packaging_and_symbol_policy_edge_case_and_compatibility_completion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C IR/object
  packaging and symbol policy core-feature dependency anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C IR/object packaging
  and symbol policy core-feature fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  IR/object packaging core-feature metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-c005-ir-object-packaging-symbol-policy-edge-case-and-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m249-c005-ir-object-packaging-symbol-policy-edge-case-and-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m249-c005-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m249_c005_ir_object_packaging_and_symbol_policy_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c005_ir_object_packaging_and_symbol_policy_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m249-c005-lane-c-readiness`

## Evidence Path

- `tmp/reports/m249/M249-C005/ir_object_packaging_and_symbol_policy_edge_case_and_compatibility_completion_contract_summary.json`
