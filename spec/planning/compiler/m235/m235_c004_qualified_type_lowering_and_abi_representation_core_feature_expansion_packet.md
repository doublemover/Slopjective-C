# M235-C004 Qualified Type Lowering and ABI Representation Core Feature Expansion Packet

Packet: `M235-C004`
Milestone: `M235`
Lane: `C`
Issue: `#5814`
Freeze date: `2026-03-05`
Dependencies: `M235-C003`

## Purpose

Freeze lane-C qualified type lowering and ABI representation core feature
expansion continuity for M235 so dependency wiring remains deterministic and
fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualified_type_lowering_and_abi_representation_core_feature_expansion_c004_expectations.md`
- Checker:
  `scripts/check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py`
- Dependency anchors from `M235-C003`:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m235/m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_packet.md`
  - `scripts/check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Readiness Chain

- `C003 readiness -> C004 checker -> C004 pytest`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `npm run check:objc3c:m235-c003-lane-c-readiness`
- `python scripts/check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c004_qualified_type_lowering_and_abi_representation_core_feature_expansion_contract.py -q`

## Evidence Output

- `tmp/reports/m235/M235-C004/qualified_type_lowering_and_abi_representation_core_feature_expansion_contract_summary.json`
