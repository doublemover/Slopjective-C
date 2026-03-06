# M231-A017 Declaration Grammar Expansion and Normalization Advanced Diagnostics Workpack (Shard 1) Packet

Packet: `M231-A017`
Milestone: `M231`
Lane: `A`
Issue: `#5509`
Freeze date: `2026-03-06`
Dependencies: `M231-A016`

## Purpose

Execute advanced diagnostics workpack (shard 1) governance for lane-A declaration grammar expansion and normalization while preserving deterministic dependency continuity from `M231-A016` and fail-closed readiness behavior.
This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m231_declaration_grammar_expansion_and_normalization_advanced_diagnostics_workpack_shard1_a017_expectations.md`
- Checker:
  `scripts/check_m231_a017_declaration_grammar_expansion_and_normalization_advanced_diagnostics_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m231_a017_declaration_grammar_expansion_and_normalization_advanced_diagnostics_workpack_shard1_contract.py`
- Prior dependency packet:
  `spec/planning/compiler/m231/m231_a016_declaration_grammar_expansion_and_normalization_advanced_edge_compatibility_workpack_shard1_packet.md`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m231-a017-declaration-grammar-expansion-and-normalization-advanced-diagnostics-workpack-shard1-contract`
  - `test:tooling:m231-a017-declaration-grammar-expansion-and-normalization-advanced-diagnostics-workpack-shard1-contract`
  - `check:objc3c:m231-a017-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m231_a017_declaration_grammar_expansion_and_normalization_advanced_diagnostics_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m231_a017_declaration_grammar_expansion_and_normalization_advanced_diagnostics_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m231-a017-lane-a-readiness`

## Evidence Output

- `tmp/reports/m231/M231-A017/declaration_grammar_expansion_and_normalization_advanced_diagnostics_workpack_shard1_summary.json`
















