# M231 Declaration Grammar Expansion and Normalization Diagnostics Hardening Expectations (A007)

Contract ID: `objc3c-declaration-grammar-expansion-and-normalization-diagnostics-hardening/m231-a007-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5499`
Dependencies: `M231-A006`

## Objective

Execute diagnostics hardening governance for lane-A declaration grammar expansion and normalization so edge-case expansion and robustness outputs from `M231-A006` are consumed deterministically and fail-closed before edge-case and compatibility workpacks begin.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Inputs (M231-A006)

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_edge_case_expansion_and_robustness_a006_expectations.md`
- `spec/planning/compiler/m231/m231_a006_declaration_grammar_expansion_and_normalization_edge_case_expansion_and_robustness_packet.md`
- `scripts/check_m231_a006_declaration_grammar_expansion_and_normalization_edge_case_expansion_and_robustness_contract.py`
- `tests/tooling/test_check_m231_a006_declaration_grammar_expansion_and_normalization_edge_case_expansion_and_robustness_contract.py`

## Scope Anchors

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_diagnostics_hardening_a007_expectations.md`
- `spec/planning/compiler/m231/m231_a007_declaration_grammar_expansion_and_normalization_diagnostics_hardening_packet.md`
- `scripts/check_m231_a007_declaration_grammar_expansion_and_normalization_diagnostics_hardening_contract.py`
- `tests/tooling/test_check_m231_a007_declaration_grammar_expansion_and_normalization_diagnostics_hardening_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m231-a007-lane-a-readiness`)

## Deterministic Invariants

1. A007 readiness must chain from `M231-A006` readiness and fail closed when dependency continuity drifts.
2. Core-feature implementation docs and packet anchors remain synchronized with architecture/spec coverage.
3. Parser replay and execution-smoke optimization commands stay present as required lane-A optimization inputs.

## Required Commands

- `check:objc3c:m231-a007-declaration-grammar-expansion-and-normalization-diagnostics-hardening-contract`
- `check:objc3c:m231-a007-lane-a-readiness`
- `python scripts/check_m231_a007_declaration_grammar_expansion_and_normalization_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m231_a007_declaration_grammar_expansion_and_normalization_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m231-a007-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m231/M231-A007/declaration_grammar_expansion_and_normalization_diagnostics_hardening_summary.json`






