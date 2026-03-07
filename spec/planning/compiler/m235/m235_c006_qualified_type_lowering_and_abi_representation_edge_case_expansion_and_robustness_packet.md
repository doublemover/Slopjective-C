# M235-C006 Qualified Type Lowering and ABI Representation Edge-case Expansion and Robustness Packet

Packet: `M235-C006`
Milestone: `M235`
Lane: `C`
Issue: `#5816`
Freeze date: `2026-03-05`
Dependencies: `M235-C005`

## Purpose

Freeze lane-C qualified type lowering and ABI representation edge-case and
compatibility completion continuity for M235 so dependency wiring remains deterministic and
fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualified_type_lowering_and_abi_representation_edge_case_expansion_and_robustness_c006_expectations.md`
- Checker:
  `scripts/check_m235_c006_qualified_type_lowering_and_abi_representation_edge_case_expansion_and_robustness_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_c006_qualified_type_lowering_and_abi_representation_edge_case_expansion_and_robustness_contract.py`
- Dependency anchors from `M235-C005`:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_edge_case_and_compatibility_completion_c005_expectations.md`
  - `spec/planning/compiler/m235/m235_c005_qualified_type_lowering_and_abi_representation_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m235_c005_qualified_type_lowering_and_abi_representation_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m235_c005_qualified_type_lowering_and_abi_representation_edge_case_and_compatibility_completion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Readiness Chain

- `C005 readiness -> C006 checker -> C006 pytest`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `npm run check:objc3c:m235-c005-lane-c-readiness`
- `python scripts/check_m235_c006_qualified_type_lowering_and_abi_representation_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c006_qualified_type_lowering_and_abi_representation_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m235-c006-lane-c-readiness`

## Evidence Output

- `tmp/reports/m235/M235-C006/qualified_type_lowering_and_abi_representation_edge_case_expansion_and_robustness_contract_summary.json`


