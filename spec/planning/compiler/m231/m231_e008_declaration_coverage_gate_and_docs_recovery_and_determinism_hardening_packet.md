# M231-E008 Declaration Grammar Expansion and Normalization Contract and Architecture Freeze Packet

Packet: `M231-E008`
Milestone: `M231`
Lane: `A`
Issue: `#5515`
Freeze date: `2026-03-06`
Dependencies: `M231-E007`

## Purpose

Execute recovery and determinism hardening governance for lane-E Declaration coverage gate and docs so declaration-grammar boundary anchors remain deterministic and fail-closed before modular split/scaffolding expansion.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m231_declaration_coverage_gate_and_docs_recovery_and_determinism_hardening_e008_expectations.md`
- Checker:
  `scripts/check_m231_e008_declaration_coverage_gate_and_docs_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m231_e008_declaration_coverage_gate_and_docs_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m231-e008-declaration-coverage-gate-and-docs-recovery-and-determinism-hardening-contract`
  - `test:tooling:m231-e008-declaration-coverage-gate-and-docs-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m231-e008-lane-e-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m231_e008_declaration_coverage_gate_and_docs_contract.py`
- `python -m pytest tests/tooling/test_check_m231_e008_declaration_coverage_gate_and_docs_contract.py -q`
- `npm run check:objc3c:m231-e008-lane-e-readiness`

## Evidence Output

- `tmp/reports/m231/M231-E008/declaration_coverage_gate_and_docs_recovery_and_determinism_hardening_summary.json`

















