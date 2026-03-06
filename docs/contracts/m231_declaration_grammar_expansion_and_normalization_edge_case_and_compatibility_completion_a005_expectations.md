# M231 Declaration Grammar Expansion and Normalization Edge-case and Compatibility Completion Expectations (A005)

Contract ID: `objc3c-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion/m231-a005-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5497`
Dependencies: `M231-A004`

## Objective

Execute edge-case and compatibility completion governance for lane-A declaration grammar expansion and normalization so core feature expansion outputs from `M231-A004` are consumed deterministically and fail-closed before edge-case and compatibility workpacks begin.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Inputs (M231-A004)

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_core_feature_expansion_a004_expectations.md`
- `spec/planning/compiler/m231/m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_packet.md`
- `scripts/check_m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_contract.py`
- `tests/tooling/test_check_m231_a004_declaration_grammar_expansion_and_normalization_core_feature_expansion_contract.py`

## Scope Anchors

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_a005_expectations.md`
- `spec/planning/compiler/m231/m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_packet.md`
- `scripts/check_m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_contract.py`
- `tests/tooling/test_check_m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m231-a005-lane-a-readiness`)

## Deterministic Invariants

1. A005 readiness must chain from `M231-A004` readiness and fail closed when dependency continuity drifts.
2. Core-feature implementation docs and packet anchors remain synchronized with architecture/spec coverage.
3. Parser replay and execution-smoke optimization commands stay present as required lane-A optimization inputs.

## Required Commands

- `check:objc3c:m231-a005-declaration-grammar-expansion-and-normalization-edge-case-and-compatibility-completion-contract`
- `check:objc3c:m231-a005-lane-a-readiness`
- `python scripts/check_m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m231_a005_declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m231-a005-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m231/M231-A005/declaration_grammar_expansion_and_normalization_edge_case_and_compatibility_completion_summary.json`




