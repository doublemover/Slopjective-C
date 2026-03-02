# M226 C-Style `void*` Compatibility Expectations (A005)

Contract ID: `objc3c-cstyle-void-pointer-compat-contract/m226-a005-v1`
Status: Accepted
Scope: Parser compatibility edge-case handling for C-style top-level declarations.

## Objective

Close a parser compatibility gap by accepting `void*` in C-style compatibility
function signatures as an opaque object-pointer boundary type.

## Required Invariants

1. In C-style compatibility type parsing, `void*` must be accepted.
2. `void*` must be mapped to object-pointer compatibility metadata
   (`ValueType::ObjCObjectPtr`, object-pointer spelling enabled).
3. Existing fail-closed behavior for unsupported primitive pointer forms
   (`i32*`, `bool*`) remains intact.

## Validation

- `python scripts/check_m226_a005_cstyle_void_pointer_compat_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a005_cstyle_void_pointer_compat_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-A005/cstyle_void_pointer_compat_summary.json`
