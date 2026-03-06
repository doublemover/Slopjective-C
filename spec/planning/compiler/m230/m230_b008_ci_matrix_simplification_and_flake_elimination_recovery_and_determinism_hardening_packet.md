# M230-B008 Conformance Corpus Governance and Sharding Contract and Architecture Freeze Packet

Packet: `M230-B008`
Milestone: `M230`
Lane: `A`
Issue: `#5395`
Freeze date: `2026-03-06`
Dependencies: `M230-B007`

## Purpose

Execute recovery and determinism hardening governance for lane-B CI matrix simplification and flake elimination so corpus boundary anchors remain deterministic and fail-closed before modular split/scaffolding expansion.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m230_ci_matrix_simplification_and_flake_elimination_recovery_and_determinism_hardening_b008_expectations.md`
- Checker:
  `scripts/check_m230_b008_ci_matrix_simplification_and_flake_elimination_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m230_b008_ci_matrix_simplification_and_flake_elimination_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m230-b008-ci-matrix-simplification-and-flake-elimination-recovery-and-determinism-hardening-contract`
  - `test:tooling:m230-b008-ci-matrix-simplification-and-flake-elimination-recovery-and-determinism-hardening-contract`
  - `check:objc3c:m230-b008-lane-b-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m230_b008_ci_matrix_simplification_and_flake_elimination_contract.py`
- `python -m pytest tests/tooling/test_check_m230_b008_ci_matrix_simplification_and_flake_elimination_contract.py -q`
- `npm run check:objc3c:m230-b008-lane-b-readiness`

## Evidence Output

- `tmp/reports/m230/M230-B008/ci_matrix_simplification_and_flake_elimination_recovery_and_determinism_hardening_summary.json`















