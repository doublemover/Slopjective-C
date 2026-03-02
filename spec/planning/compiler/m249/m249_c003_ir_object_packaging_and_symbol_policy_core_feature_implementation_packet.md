# M249-C003 IR/Object Packaging and Symbol Policy Core Feature Implementation Packet

Packet: `M249-C003`
Milestone: `M249`
Lane: `C`
Freeze date: `2026-03-02`
Dependencies: `M249-C001`, `M249-C002`

## Purpose

Freeze lane-C IR/object packaging and symbol policy core-feature implementation
continuity for M249 so artifact packaging boundaries and symbol policy
continuity remain deterministic and fail-closed, with dependency surfaces,
code/spec anchors, and milestone optimization improvements treated as mandatory
scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_ir_object_packaging_and_symbol_policy_core_feature_implementation_c003_expectations.md`
- Checker:
  `scripts/check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
- Dependency anchors (`M249-C001`):
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_contract_freeze_c001_expectations.md`
  - `spec/planning/compiler/m249/m249_c001_ir_object_packaging_and_symbol_policy_contract_freeze_packet.md`
  - `scripts/check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
  - `tests/tooling/test_check_m249_c001_ir_object_packaging_and_symbol_policy_contract.py`
- Dependency anchors (`M249-C002`):
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m249/m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_packet.md`
  - `scripts/check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-c003-ir-object-packaging-symbol-policy-core-feature-implementation-contract`
  - `test:tooling:m249-c003-ir-object-packaging-symbol-policy-core-feature-implementation-contract`
  - `check:objc3c:m249-c003-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m249-c003-lane-c-readiness`

## Evidence Output

- `tmp/reports/m249/M249-C003/ir_object_packaging_and_symbol_policy_core_feature_implementation_contract_summary.json`

