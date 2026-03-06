# M231 Declaration Grammar Expansion and Normalization Modular Split/Scaffolding Expectations (A002)

Contract ID: `objc3c-declaration-grammar-expansion-and-normalization-modular-split-scaffolding/m231-a002-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5494`
Dependencies: `M231-A001`

## Objective

Establish modular split/scaffolding governance for lane-A declaration grammar expansion and normalization so the A001 freeze artifacts are consumed deterministically and fail-closed while preparing expansion workpacks.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Inputs (M231-A001)

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_contract_and_architecture_freeze_a001_expectations.md`
- `spec/planning/compiler/m231/m231_a001_declaration_grammar_expansion_and_normalization_contract_and_architecture_freeze_packet.md`
- `scripts/check_m231_a001_declaration_grammar_expansion_and_normalization_contract.py`
- `tests/tooling/test_check_m231_a001_declaration_grammar_expansion_and_normalization_contract.py`

## Scope Anchors

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_a002_expectations.md`
- `spec/planning/compiler/m231/m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_packet.md`
- `scripts/check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py`
- `tests/tooling/test_check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m231-a002-lane-a-readiness`)

## Deterministic Invariants

1. A002 readiness must chain from `M231-A001` readiness and fail closed when dependency continuity drifts.
2. Modular split/scaffolding docs and packet anchors remain synchronized with architecture/spec coverage.
3. Parser replay and execution-smoke optimization commands stay present as required lane-A optimization inputs.

## Required Commands

- `check:objc3c:m231-a002-declaration-grammar-expansion-and-normalization-modular-split-scaffolding-contract`
- `check:objc3c:m231-a002-lane-a-readiness`
- `python scripts/check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m231-a002-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m231/M231-A002/declaration_grammar_expansion_and_normalization_modular_split_scaffolding_summary.json`


