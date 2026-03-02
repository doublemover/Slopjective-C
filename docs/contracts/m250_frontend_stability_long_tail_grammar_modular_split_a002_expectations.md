# M250 Frontend Stability and Long-Tail Grammar Closure Modular Split Expectations (A002)

Contract ID: `objc3c-frontend-stability-long-tail-grammar-modular-split/m250-a002-v1`
Status: Accepted
Scope: parser/support modular boundaries, parser contract transport, and lane-A scaffolding readiness commands.

## Objective

Modularize lane-A frontend stability scaffolding so long-tail grammar closure work can iterate without collapsing parser diagnostics, parser contract snapshots, and parse/lowering handoff boundaries back into monolithic parser logic.

## Required Invariants

1. Parser support and parser contract modules remain explicit:
   - `native/objc3c/src/parse/objc3_parse_support.h`
   - `native/objc3c/src/parse/objc3_parse_support.cpp`
   - `native/objc3c/src/parse/objc3_parser_contract.h`
   - `native/objc3c/src/parse/objc3_ast_builder_contract.h`
2. Parser implementation consumes support modules through stable seams:
   - `native/objc3c/src/parse/objc3_parser.cpp` includes `parse/objc3_parse_support.h`.
   - Parser uses `objc3c::parse::support::MakeDiag` and `objc3c::parse::support::ParseIntegerLiteralValue`.
   - Parser builds and stores `result.contract_snapshot = BuildObjc3ParserContractSnapshot(...)`.
3. Pipeline and readiness surfaces continue parser-contract transport:
   - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` assigns parser snapshot from parse result.
   - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h` consumes parser snapshot deterministic/replay-ready signals.
4. A001 freeze anchors remain mandatory prerequisites:
   - `docs/contracts/m250_frontend_stability_long_tail_grammar_closure_a001_expectations.md`
   - `scripts/check_m250_a001_frontend_stability_long_tail_grammar_closure_contract.py`
   - `tests/tooling/test_check_m250_a001_frontend_stability_long_tail_grammar_closure_contract.py`
   - `spec/planning/compiler/m250/m250_a001_frontend_stability_long_tail_grammar_contract_freeze.md`
5. Package command scaffolding is pinned:
   - `check:objc3c:m250-a002-frontend-stability-modular-split-contract`
   - `test:tooling:m250-a002-frontend-stability-modular-split-contract`
   - `check:objc3c:m250-a002-lane-a-readiness`

## Validation

- `python scripts/check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract.py -q`
- `npm run check:objc3c:m250-a002-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-A002/frontend_stability_long_tail_grammar_modular_split_contract_summary.json`
