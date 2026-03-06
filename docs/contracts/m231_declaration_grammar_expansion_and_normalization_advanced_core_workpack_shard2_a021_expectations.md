# M231 Declaration Grammar Expansion and Normalization Advanced Core Workpack (Shard 2) Expectations (A021)

Contract ID: `objc3c-declaration-grammar-expansion-and-normalization-advanced-core-workpack-shard2/m231-a021-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5513`
Dependencies: `M231-A020`

## Objective

Execute advanced core workpack (shard 2) governance for lane-A declaration grammar expansion and normalization so advanced performance workpack (shard 1) outputs from `M231-A020` are consumed deterministically and fail-closed before edge-case and compatibility workpacks begin.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Inputs (M231-A020)

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_advanced_performance_workpack_shard1_a020_expectations.md`
- `spec/planning/compiler/m231/m231_a020_declaration_grammar_expansion_and_normalization_advanced_performance_workpack_shard1_packet.md`
- `scripts/check_m231_a020_declaration_grammar_expansion_and_normalization_advanced_performance_workpack_shard1_contract.py`
- `tests/tooling/test_check_m231_a020_declaration_grammar_expansion_and_normalization_advanced_performance_workpack_shard1_contract.py`

## Scope Anchors

- `docs/contracts/m231_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_a021_expectations.md`
- `spec/planning/compiler/m231/m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_packet.md`
- `scripts/check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py`
- `tests/tooling/test_check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m231-a021-lane-a-readiness`)

## Deterministic Invariants

1. A021 readiness must chain from `M231-A020` readiness and fail closed when dependency continuity drifts.
2. Core-feature implementation docs and packet anchors remain synchronized with architecture/spec coverage.
3. Parser replay and execution-smoke optimization commands stay present as required lane-A optimization inputs.

## Required Commands

- `check:objc3c:m231-a021-declaration-grammar-expansion-and-normalization-advanced-core-workpack-shard2-contract`
- `check:objc3c:m231-a021-lane-a-readiness`
- `python scripts/check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m231-a021-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m231/M231-A021/declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_summary.json`




















