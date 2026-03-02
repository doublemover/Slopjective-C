# M226 Parser Conformance Matrix Expectations (A009)

Contract ID: `objc3c-parser-conformance-matrix-contract/m226-a009-v1`
Status: Accepted
Scope: Deterministic parser conformance matrix for C-style compatibility declarations.

## Objective

Freeze an explicit parser conformance matrix for C-style compatibility declaration
paths so accepted/rejected forms and expected diagnostics remain stable.

## Conformance Matrix

| Case ID | Form | Expected Result | Diagnostic Code |
|---|---|---|---|
| `A009-C001` | `void *opaque` parameter | Accept | n/a |
| `A009-C002` | `i32 *bad` parameter | Reject | `O3P114` |
| `A009-C003` | `bool *bad` parameter | Reject | `O3P114` |
| `A009-C004` | Trailing comma before `)` | Reject | `O3P104` |
| `A009-C005` | Non-declaration token after comma | Reject | `O3P100` |
| `A009-C006` | Missing function identifier | Reject | `O3P101` |

## Required Invariants

1. Matrix rows above are present and versioned in this contract.
2. Parser source still contains the corresponding diagnostic anchors.
3. `A005`, `A006`, `A007`, and `A008` diagnostics remain represented in the matrix.

## Validation

- `python scripts/check_m226_a009_parser_conformance_matrix_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a009_parser_conformance_matrix_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A009/parser_conformance_matrix_summary.json`
