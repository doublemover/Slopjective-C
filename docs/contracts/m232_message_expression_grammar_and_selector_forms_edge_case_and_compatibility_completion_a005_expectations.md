# M232 Message Expression Grammar and Selector Forms Contract and Architecture Freeze Expectations (A005)

Contract ID: `objc3c-message-expression-grammar-and-selector-forms/m232-a005-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5569`
Dependencies: none

## Objective

Execute edge-case and compatibility completion governance for lane-A message expression grammar and selector forms, locking deterministic declaration-grammar boundaries before modular split/scaffolding work begins.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- `docs/contracts/m232_message_expression_grammar_and_selector_forms_edge_case_and_compatibility_completion_a005_expectations.md`
- `spec/planning/compiler/m232/m232_a005_message_expression_grammar_and_selector_forms_edge_case_and_compatibility_completion_packet.md`
- `scripts/check_m232_a005_message_expression_grammar_and_selector_forms_contract.py`
- `tests/tooling/test_check_m232_a005_message_expression_grammar_and_selector_forms_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m232-a005-lane-a-readiness`)

## Deterministic Invariants

1. Contract-freeze artifacts and packet anchors must remain synchronized and fail closed on drift.
2. Lane-A architecture/spec/package anchors must remain explicit and deterministic for `M232-A005`.
3. Readiness checks must preserve lane-A freeze validation and emit evidence under `tmp/reports`.

## Required Commands

- `check:objc3c:m232-a005-message-expression-grammar-and-selector-forms-contract`
- `test:tooling:m232-a005-message-expression-grammar-and-selector-forms-contract`
- `check:objc3c:m232-a005-lane-a-readiness`
- `python scripts/check_m232_a005_message_expression_grammar_and_selector_forms_contract.py`
- `python -m pytest tests/tooling/test_check_m232_a005_message_expression_grammar_and_selector_forms_contract.py -q`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m232/M232-A005/message_expression_grammar_and_selector_forms_contract_summary.json`






