# M226 C-Style Pointer Diagnostic Precision Expectations (A006)

Contract ID: `objc3c-cstyle-pointer-diagnostic-precision-contract/m226-a006-v1`
Status: Accepted
Scope: Parser edge-case diagnostic precision for unsupported primitive pointers.

## Objective

Improve parser robustness by emitting deterministic, type-specific diagnostics
for unsupported primitive pointer forms in C-style compatibility declarations.

## Required Invariants

1. Unsupported primitive pointer diagnostics identify the rejected form:
   - `i32*`
   - `bool*`
2. Diagnostic code remains `O3P114`.
3. `void*` compatibility handling remains accepted (A005 behavior).

## Validation

- `python scripts/check_m226_a006_cstyle_pointer_diagnostic_precision_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a006_cstyle_pointer_diagnostic_precision_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-A006/cstyle_pointer_diagnostic_precision_summary.json`
