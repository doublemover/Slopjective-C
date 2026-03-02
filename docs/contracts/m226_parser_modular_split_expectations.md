# M226 Parser Modular Split Expectations (A002)

Contract ID: `objc3c-parser-modular-split-contract/m226-a002-v1`
Status: Accepted
Scope: Parser modularization scaffolding for `native/objc3c/src/parse/*` and build wiring.

## Objective

Reduce parser monolith pressure by extracting reusable parse utilities into
dedicated compilation units while preserving deterministic parser behavior.

## Required Invariants

1. Shared parse support module exists:
   - `native/objc3c/src/parse/objc3_parse_support.h`
   - `native/objc3c/src/parse/objc3_parse_support.cpp`
2. Parser implementation depends on support module for core literal/diagnostic helpers:
   - `parse/objc3_parser.cpp` includes `parse/objc3_parse_support.h`.
   - Extracted helpers are not redefined as `static` locals in `objc3_parser.cpp`.
3. Build graphs include the new module in both native and wrapper paths:
   - `native/objc3c/CMakeLists.txt` includes `src/parse/objc3_parse_support.cpp` in `objc3c_parse`.
   - `scripts/build_objc3c_native.ps1` source manifest includes
     `native/objc3c/src/parse/objc3_parse_support.cpp`.
4. Determinism contract remains intact:
   - Parser diagnostics still flow through `MakeDiag`.
   - Integer literal parsing semantics remain parser-contract stable.

## Validation

- `npm run build:objc3c-native`
- `python scripts/check_m226_a002_parser_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a002_parser_modular_split_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A002/parser_modular_split_contract_summary.json`
