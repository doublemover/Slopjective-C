# M243-A001 Diagnostic Grammar Hooks and Source Precision Contract and Architecture Freeze Packet

Packet: `M243-A001`
Milestone: `M243`
Lane: `A`
Dependencies: none

## Scope

Freeze lane-A parser diagnostic hooks and source precision contracts for M243
before grammar expansion and fix-it synthesis phases.

## Anchors

- Contract: `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a001_expectations.md`
- Checker: `scripts/check_m243_a001_diagnostic_grammar_hooks_and_source_precision_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_a001_diagnostic_grammar_hooks_and_source_precision_contract.py`
- Parser support diagnostics formatter: `native/objc3c/src/parse/objc3_parse_support.cpp`
- Parser contract snapshot/fingerprints: `native/objc3c/src/parse/objc3_parser_contract.h`
- Parser diagnostics emission: `native/objc3c/src/parse/objc3_parser.cpp`
- Parse/lowering diagnostic readiness: `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m243/M243-A001/diagnostic_grammar_hooks_and_source_precision_contract_summary.json`

## Determinism Criteria

- Diagnostic formatting must retain line/column/code payload shape.
- Parser contract fingerprinting must include source-coordinate dimensions.
- Parse/lowering readiness must keep parser diagnostic surface consistency and
  deterministic diagnostic code coverage gating.
