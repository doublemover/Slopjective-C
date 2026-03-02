# M226 Parser Architecture Freeze Expectations (A001)

Contract ID: `objc3c-parser-architecture-freeze-contract/m226-a001-v1`
Status: Accepted
Scope: `native/objc3c/src/parse/*`, parser-facing pipeline boundaries, and parser grammar/spec anchors.

## Objective

Freeze parser architecture boundaries for M226 so grammar expansion can proceed
without regressing parser/sema contracts or deterministic diagnostics behavior.

## Required Invariants

1. Parser API surface remains contract-wrapped:
   - `ParseObjc3Program(const Objc3LexTokenStream &tokens)` returns `Objc3ParseResult`.
   - `Objc3ParseResult` exposes `Objc3ParsedProgram` from `parse/objc3_parser_contract.h`.
2. Pipeline consumes parser output via AST-builder contract only:
   - `pipeline/objc3_frontend_pipeline.cpp` includes `parse/objc3_ast_builder_contract.h`.
   - The pipeline must not include parser implementation headers directly.
3. Parser implementation remains grammar-owned and deterministic:
   - `parse/objc3_parser.cpp` owns parse diagnostics collection and returns it through
     `Objc3ParseResult`.
   - Deterministic parser recovery remains a hard requirement for lane-E replay-proof gates.
4. Architecture policy is aligned with extracted module topology:
   - `native/objc3c/src/ARCHITECTURE.md` must declare parser/sema contract boundaries
     and driver-only `main.cpp` responsibilities.
5. Spec anchors remain present and authoritative:
   - `spec/FORMAL_GRAMMAR_AND_PRECEDENCE.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Validation

- `python scripts/check_m226_a001_parser_architecture_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a001_parser_architecture_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A001/parser_architecture_contract_summary.json`
