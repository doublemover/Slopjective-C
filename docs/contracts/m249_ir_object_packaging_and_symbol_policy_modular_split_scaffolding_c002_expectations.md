# M249 IR/Object Packaging and Symbol Policy Modular Split/Scaffolding Expectations (C002)

Contract ID: `objc3c-ir-object-packaging-symbol-policy-modular-split-scaffolding/m249-c002-v1`
Status: Accepted
Scope: M249 lane-C IR/object packaging and symbol policy modular split/scaffolding continuity with explicit `M249-C001` dependency governance.

## Objective

Fail closed unless lane-C IR/object packaging and symbol policy modular
split/scaffolding anchors remain explicit, deterministic, and traceable across
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Dependency Scope

- Dependencies: `M249-C001`
- Upstream C001 freeze assets remain mandatory prerequisites:
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_contract_freeze_c001_expectations.md`
  - `spec/planning/compiler/m249/m249_c001_ir_object_packaging_and_symbol_policy_contract_freeze_packet.md`
  - `scripts/check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
  - `tests/tooling/test_check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
- C002 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m249/m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_packet.md`
  - `scripts/check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C IR/object
  packaging and symbol policy contract anchors used by `M249-C001`.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C IR/object packaging
  and symbol policy fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  IR/object packaging metadata anchor wording for `M249-C001` dependency
  continuity.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-c002-ir-object-packaging-symbol-policy-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m249-c002-ir-object-packaging-symbol-policy-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m249-c002-lane-c-readiness`.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m249-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m249/M249-C002/ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract_summary.json`

