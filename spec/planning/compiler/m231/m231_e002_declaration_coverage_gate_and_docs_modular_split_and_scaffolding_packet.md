# M231-E002 Declaration Grammar Expansion and Normalization Contract and Architecture Freeze Packet

Packet: `M231-E002`
Milestone: `M231`
Lane: `A`
Issue: `#5515`
Freeze date: `2026-03-06`
Dependencies: `M231-E001`

## Purpose

Execute modular split and scaffolding governance for lane-E Declaration coverage gate and docs so declaration-grammar boundary anchors remain deterministic and fail-closed before modular split/scaffolding expansion.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m231_declaration_coverage_gate_and_docs_modular_split_and_scaffolding_e002_expectations.md`
- Checker:
  `scripts/check_m231_e002_declaration_coverage_gate_and_docs_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m231_e002_declaration_coverage_gate_and_docs_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m231-e002-declaration-coverage-gate-and-docs-modular-split-and-scaffolding-contract`
  - `test:tooling:m231-e002-declaration-coverage-gate-and-docs-modular-split-and-scaffolding-contract`
  - `check:objc3c:m231-e002-lane-e-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m231_e002_declaration_coverage_gate_and_docs_contract.py`
- `python -m pytest tests/tooling/test_check_m231_e002_declaration_coverage_gate_and_docs_contract.py -q`
- `npm run check:objc3c:m231-e002-lane-e-readiness`

## Evidence Output

- `tmp/reports/m231/M231-E002/declaration_coverage_gate_and_docs_modular_split_and_scaffolding_summary.json`





