# M235-A005 Qualifier/Generic Grammar Normalization Edge-Case and Compatibility Completion Packet

Packet: `M235-A005`
Milestone: `M235`
Lane: `A`
Issue: `#5768`
Freeze date: `2026-03-04`
Dependencies: `M235-A004`

## Purpose

Freeze lane-A edge-case and compatibility completion prerequisites for M235 qualifier/generic grammar normalization continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_a005_expectations.md`
- Checker:
  `scripts/check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py`
- Dependency anchors from `M235-A004`:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_core_feature_expansion_a004_expectations.md`
  - `spec/planning/compiler/m235/m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_packet.md`
  - `scripts/check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m235-a005-lane-a-readiness`

## Evidence Output

- `tmp/reports/m235/M235-A005/qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_summary.json`

