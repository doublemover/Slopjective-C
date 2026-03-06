# M231-A021 Declaration Grammar Expansion and Normalization Advanced Core Workpack (Shard 2) Packet

Packet: `M231-A021`
Milestone: `M231`
Lane: `A`
Issue: `#5513`
Freeze date: `2026-03-06`
Dependencies: `M231-A020`

## Purpose

Execute advanced core workpack (shard 2) governance for lane-A declaration grammar expansion and normalization while preserving deterministic dependency continuity from `M231-A020` and fail-closed readiness behavior.
This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m231_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_a021_expectations.md`
- Checker:
  `scripts/check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py`
- Prior dependency packet:
  `spec/planning/compiler/m231/m231_a020_declaration_grammar_expansion_and_normalization_advanced_performance_workpack_shard1_packet.md`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m231-a021-declaration-grammar-expansion-and-normalization-advanced-core-workpack-shard2-contract`
  - `test:tooling:m231-a021-declaration-grammar-expansion-and-normalization-advanced-core-workpack-shard2-contract`
  - `check:objc3c:m231-a021-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m231_a021_declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m231-a021-lane-a-readiness`

## Evidence Output

- `tmp/reports/m231/M231-A021/declaration_grammar_expansion_and_normalization_advanced_core_workpack_shard2_summary.json`




















