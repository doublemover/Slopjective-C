# M235 Qualifier/Generic Grammar Normalization Edge-Case and Compatibility Completion Expectations (A005)

Contract ID: `objc3c-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion/m235-a005-v1`
Status: Accepted
Scope: M235 lane-A edge-case and compatibility completion continuity for qualifier/generic grammar normalization dependency wiring.

## Objective

Fail closed unless lane-A edge-case and compatibility completion dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue: `#5768`
- Dependencies: `M235-A004`
- M235-A004 core feature expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_core_feature_expansion_a004_expectations.md`
  - `spec/planning/compiler/m235/m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_packet.md`
  - `scripts/check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py`
- Packet/checker/test assets for A005 remain mandatory:
  - `spec/planning/compiler/m235/m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M235 lane-A A005 qualifier/generic grammar normalization edge-case and compatibility completion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization edge-case and compatibility completion fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization edge-case and compatibility completion metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-a005-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m235-a005-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m235-a005-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m235-a005-lane-a-readiness`

## Evidence Path

- `tmp/reports/m235/M235-A005/qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_summary.json`

