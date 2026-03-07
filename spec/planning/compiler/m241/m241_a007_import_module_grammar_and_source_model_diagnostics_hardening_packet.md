# M241-A007 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Packet

Packet: `M241-A007`
Milestone: `M241`
Lane: `A`
Issue: `#5764`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-A qualifier/generic grammar normalization contract prerequisites for M241 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m241_import_module_grammar_and_source_model_diagnostics_hardening_a007_expectations.md`
- Checker:
  `scripts/check_m241_a007_import_module_grammar_and_source_model_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m241_a007_import_module_grammar_and_source_model_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m241-a007-import-module-grammar-and-source-model-contract`
  - `test:tooling:m241-a007-import-module-grammar-and-source-model-contract`
  - `check:objc3c:m241-a007-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m241_a007_import_module_grammar_and_source_model_contract.py`
- `python -m pytest tests/tooling/test_check_m241_a007_import_module_grammar_and_source_model_contract.py -q`
- `npm run check:objc3c:m241-a007-lane-a-readiness`

## Evidence Output

- `tmp/reports/m241/M241-A007/import_module_grammar_and_source_model_contract_summary.json`







