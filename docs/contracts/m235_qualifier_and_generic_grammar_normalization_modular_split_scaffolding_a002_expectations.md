# M235 Qualifier/Generic Grammar Normalization Modular Split/Scaffolding Expectations (A002)

Contract ID: `objc3c-qualifier-and-generic-grammar-normalization-modular-split-scaffolding/m235-a002-v1`
Status: Accepted
Scope: M235 lane-A modular split/scaffolding continuity for qualifier/generic grammar normalization dependency wiring.

## Objective

Fail closed unless lane-A modular split/scaffolding dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5765` defines canonical lane-A modular split/scaffolding scope.
- Dependencies: `M235-A001`
- M235-A001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m235/m235_a001_qualifier_and_generic_grammar_normalization_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py`
  - `tests/tooling/test_check_m235_a001_qualifier_and_generic_grammar_normalization_contract.py`
- Packet/checker/test assets for A002 remain mandatory:
  - `spec/planning/compiler/m235/m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_packet.md`
  - `scripts/check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M235 lane-A A002 qualifier/generic grammar normalization modular split/scaffolding anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract`.
- `package.json` includes `test:tooling:m235-a002-qualifier-and-generic-grammar-normalization-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m235-a002-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a002_qualifier_and_generic_grammar_normalization_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m235-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m235/M235-A002/qualifier_and_generic_grammar_normalization_modular_split_scaffolding_summary.json`
