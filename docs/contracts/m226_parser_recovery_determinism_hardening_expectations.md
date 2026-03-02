# M226 Parser Recovery and Determinism Hardening Expectations (A008)

Contract ID: `objc3c-parser-recovery-determinism-hardening-contract/m226-a008-v1`
Status: Accepted
Scope: C-style compatibility parameter-list recovery and deterministic diagnostics.

## Objective

Harden parser recovery in C-style compatibility function declarations so malformed
parameter lists fail with deterministic diagnostics instead of ambiguous fallback
errors.

## Required Invariants

1. Trailing comma in a compatibility parameter list is rejected explicitly:
   - `trailing ',' in C-style compatibility parameter list is not allowed`
2. Parameter-list continuation after comma is validated deterministically:
   - `expected parameter declaration after ',' in C-style compatibility parameter list, found ...`
3. Token-context suffixes are derived from parser token descriptions.
4. Diagnostic codes remain stable for these paths:
   - `O3P104` for trailing comma rejection
   - `O3P100` for invalid post-comma declaration starts

## Validation

- `python scripts/check_m226_a008_parser_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a008_parser_recovery_determinism_hardening_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-A008/parser_recovery_determinism_hardening_summary.json`
