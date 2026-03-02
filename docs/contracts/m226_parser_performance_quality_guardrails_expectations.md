# M226 Parser Performance and Quality Guardrails Expectations (A011)

Contract ID: `objc3c-parser-performance-quality-guardrails-contract/m226-a011-v1`
Status: Accepted
Scope: Parser quality/performance guardrails for compatibility declaration paths.

## Objective

Freeze lightweight structural guardrails that reduce regression risk from parser
monolith growth and preserve maintainable boundaries for hot-path compatibility
declaration parsing.

## Required Invariants

1. Guardrail checker enforces bounded line counts for core compatibility parse
   functions in `native/objc3c/src/parse/objc3_parser.cpp`.
2. Guardrail checker fails closed if any guarded function exceeds configured
   thresholds.
3. Guardrails are versioned and validated via tooling tests.

## Guarded Functions and Max Lines

| Function | Max Lines |
|---|---|
| `ParseCStyleCompatType` | `180` |
| `ParseCStyleCompatFunctionParameters` | `90` |
| `ParseTopLevelCompatFunctionDecl` | `220` |

## Validation

- `python scripts/check_m226_a011_parser_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a011_parser_performance_quality_guardrails_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A011/parser_performance_quality_guardrails_summary.json`
