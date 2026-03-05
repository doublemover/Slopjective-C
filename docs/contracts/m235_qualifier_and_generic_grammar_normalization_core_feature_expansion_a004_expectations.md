# M235 Qualifier/Generic Grammar Normalization Core Feature Expansion Expectations (A004)

Contract ID: `objc3c-qualifier-and-generic-grammar-normalization-core-feature-expansion/m235-a004-v1`
Status: Accepted
Scope: M235 lane-A core feature expansion continuity for qualifier/generic grammar normalization dependency wiring.

## Objective

Fail closed unless lane-A core feature expansion dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue: `#5767`
- Dependencies: `M235-A003`
- M235-A003 core feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_core_feature_implementation_a003_expectations.md`
  - `spec/planning/compiler/m235/m235_a003_qualifier_and_generic_grammar_normalization_core_feature_implementation_packet.md`
  - `scripts/check_m235_a003_qualifier_and_generic_grammar_normalization_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m235_a003_qualifier_and_generic_grammar_normalization_core_feature_implementation_contract.py`
- Packet/checker/test assets for A004 remain mandatory:
  - `spec/planning/compiler/m235/m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_packet.md`
  - `scripts/check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M235 lane-A A004 qualifier/generic grammar normalization core feature expansion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization core feature expansion fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization core feature expansion metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-a004-qualifier-and-generic-grammar-normalization-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m235-a004-qualifier-and-generic-grammar-normalization-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m235-a004-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a004_qualifier_and_generic_grammar_normalization_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m235-a004-lane-a-readiness`

## Evidence Path

- `tmp/reports/m235/M235-A004/qualifier_and_generic_grammar_normalization_core_feature_expansion_summary.json`




