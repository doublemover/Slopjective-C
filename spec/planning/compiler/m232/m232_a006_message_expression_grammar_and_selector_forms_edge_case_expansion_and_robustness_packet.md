# M232-A006 Message Expression Grammar and Selector Forms Contract and Architecture Freeze Packet

Packet: `M232-A006`
Milestone: `M232`
Lane: `A`
Issue: `#5570`
Freeze date: `2026-03-06`
Dependencies: none

## Purpose

Execute edge-case expansion and robustness governance for lane-A message expression grammar and selector forms so declaration-grammar boundary anchors remain deterministic and fail-closed before modular split/scaffolding expansion.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m232_message_expression_grammar_and_selector_forms_edge_case_expansion_and_robustness_a006_expectations.md`
- Checker:
  `scripts/check_m232_a006_message_expression_grammar_and_selector_forms_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m232_a006_message_expression_grammar_and_selector_forms_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m232-a006-message-expression-grammar-and-selector-forms-contract`
  - `test:tooling:m232-a006-message-expression-grammar-and-selector-forms-contract`
  - `check:objc3c:m232-a006-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m232_a006_message_expression_grammar_and_selector_forms_contract.py`
- `python -m pytest tests/tooling/test_check_m232_a006_message_expression_grammar_and_selector_forms_contract.py -q`
- `npm run check:objc3c:m232-a006-lane-a-readiness`

## Evidence Output

- `tmp/reports/m232/M232-A006/message_expression_grammar_and_selector_forms_contract_summary.json`







