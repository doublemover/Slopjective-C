# M226 Parser Diagnostics Hardening Expectations (A007)

Contract ID: `objc3c-parser-diagnostics-hardening-contract/m226-a007-v1`
Status: Accepted
Scope: Parser diagnostics precision for C-style compatibility function declarations.

## Objective

Harden parser diagnostics so C-style compatibility function declarations report
deterministic, context-rich failures with explicit expected-vs-found wording.

## Required Invariants

1. A dedicated token-description helper exists for compatibility diagnostics.
2. Function declaration identifier diagnostics use:
   - `expected function identifier, found ...`
3. Parameter identifier diagnostics use:
   - `expected parameter identifier, found ...`
4. Delimiter diagnostics include found-token context:
   - `missing '(' after function name; found ...`
   - `missing ')' after parameters; found ...`
5. Diagnostic codes remain stable:
   - `O3P101`
   - `O3P106`
   - `O3P109`

## Validation

- `python scripts/check_m226_a007_parser_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a007_parser_diagnostics_hardening_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-A007/parser_diagnostics_hardening_summary.json`
