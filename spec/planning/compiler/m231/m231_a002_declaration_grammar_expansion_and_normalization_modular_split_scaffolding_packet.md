# M231-A002 Declaration Grammar Expansion and Normalization Modular Split/Scaffolding Packet

Packet: `M231-A002`
Milestone: `M231`
Lane: `A`
Issue: `#5494`
Freeze date: `2026-03-06`
Dependencies: `M231-A001`

## Purpose

Execute modular split/scaffolding governance for lane-A declaration grammar expansion and normalization while preserving deterministic dependency continuity from `M231-A001` and fail-closed readiness behavior.
This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m231_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_a002_expectations.md`
- Checker:
  `scripts/check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py`
- Prior dependency packet:
  `spec/planning/compiler/m231/m231_a001_declaration_grammar_expansion_and_normalization_contract_and_architecture_freeze_packet.md`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m231-a002-declaration-grammar-expansion-and-normalization-modular-split-scaffolding-contract`
  - `test:tooling:m231-a002-declaration-grammar-expansion-and-normalization-modular-split-scaffolding-contract`
  - `check:objc3c:m231-a002-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m231_a002_declaration_grammar_expansion_and_normalization_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m231-a002-lane-a-readiness`

## Evidence Output

- `tmp/reports/m231/M231-A002/declaration_grammar_expansion_and_normalization_modular_split_scaffolding_summary.json`


