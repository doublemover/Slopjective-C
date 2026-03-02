# M249 IR/Object Packaging and Symbol Policy Contract Freeze Expectations (C001)

Contract ID: `objc3c-ir-object-packaging-symbol-policy-contract/m249-c001-v1`
Status: Accepted
Scope: M249 lane-C IR/object packaging and symbol policy contract freeze for deterministic artifact packaging governance continuity.

## Objective

Fail closed unless lane-C IR/object packaging and symbol policy anchors remain
explicit, deterministic, and traceable across code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m249/m249_c001_ir_object_packaging_and_symbol_policy_contract_freeze_packet.md`
  - `scripts/check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
  - `tests/tooling/test_check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M249 lane-C C001
  IR/object packaging and symbol policy fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C IR/object packaging
  and symbol policy fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  IR/object packaging metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-c001-ir-object-packaging-symbol-policy-contract`.
- `package.json` includes
  `test:tooling:m249-c001-ir-object-packaging-symbol-policy-contract`.
- `package.json` includes `check:objc3c:m249-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py -q`
- `npm run check:objc3c:m249-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m249/M249-C001/ir_object_packaging_and_symbol_policy_contract_summary.json`

