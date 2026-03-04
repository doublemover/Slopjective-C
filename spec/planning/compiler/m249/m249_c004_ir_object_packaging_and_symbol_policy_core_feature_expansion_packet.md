# M249-C004 IR/Object Packaging and Symbol Policy Core Feature Expansion Packet

Packet: `M249-C004`
Milestone: `M249`
Lane: `C`
Freeze date: `2026-03-02`
Dependencies: `M249-C003`

## Purpose

Freeze lane-C IR/object packaging and symbol policy core-feature expansion
continuity for M249 so artifact packaging boundaries and symbol policy
continuity remain deterministic and fail-closed, with dependency surfaces,
code/spec anchors, and milestone optimization improvements treated as mandatory
scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m249_ir_object_packaging_and_symbol_policy_core_feature_expansion_c004_expectations.md`
- Checker:
  `scripts/check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`
- Dependency anchors (`M249-C003`):
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m249/m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_packet.md`
  - `scripts/check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m249-c004-ir-object-packaging-symbol-policy-core-feature-expansion-contract`
  - `test:tooling:m249-c004-ir-object-packaging-symbol-policy-core-feature-expansion-contract`
  - `check:objc3c:m249-c004-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c004_ir_object_packaging_and_symbol_policy_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m249-c004-lane-c-readiness`

## Evidence Output

- `tmp/reports/m249/M249-C004/ir_object_packaging_and_symbol_policy_core_feature_expansion_contract_summary.json`
