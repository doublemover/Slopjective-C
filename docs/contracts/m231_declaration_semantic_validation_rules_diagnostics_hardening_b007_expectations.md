# M231 Declaration Grammar Expansion and Normalization Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-declaration-semantic-validation-rules/m231-b007-v1`
Status: Accepted
Owner: Objective-C 3 native lane-B
Issue: `#5521`
Dependencies: `M231-B006`

## Objective

Execute diagnostics hardening governance for lane-B Declaration semantic validation rules, locking deterministic declaration-grammar boundaries before modular split/scaffolding work begins.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- `docs/contracts/m231_declaration_semantic_validation_rules_diagnostics_hardening_b007_expectations.md`
- `spec/planning/compiler/m231/m231_b007_declaration_semantic_validation_rules_diagnostics_hardening_packet.md`
- `scripts/check_m231_b007_declaration_semantic_validation_rules_contract.py`
- `tests/tooling/test_check_m231_b007_declaration_semantic_validation_rules_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m231-b007-lane-b-readiness`)

## Deterministic Invariants

1. Contract-freeze artifacts and packet anchors must remain synchronized and fail closed on drift.
2. Lane-A architecture/spec/package anchors must remain explicit and deterministic for `M231-B007`.
3. Readiness checks must preserve lane-B freeze validation and emit evidence under `tmp/reports`.

## Required Commands

- `check:objc3c:m231-b007-declaration-semantic-validation-rules-diagnostics-hardening-contract`
- `test:tooling:m231-b007-declaration-semantic-validation-rules-diagnostics-hardening-contract`
- `check:objc3c:m231-b007-lane-b-readiness`
- `python scripts/check_m231_b007_declaration_semantic_validation_rules_contract.py`
- `python -m pytest tests/tooling/test_check_m231_b007_declaration_semantic_validation_rules_contract.py -q`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m231/M231-B007/declaration_semantic_validation_rules_diagnostics_hardening_summary.json`














