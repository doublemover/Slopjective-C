# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A001)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-freeze/m243-a001-v1`
Status: Accepted
Scope: parser diagnostic hook coordinates, source-precision fingerprinting, and parser-to-lowering diagnostic determinism gates.

## Objective

Freeze lane-A diagnostic grammar hook and source-precision boundaries so later
M243 diagnostics/fix-it expansion cannot regress parser coordinate fidelity or
deterministic diagnostic handoff evidence.

## Required Invariants

1. Parser diagnostic hooks preserve source coordinates:
   - `parse/objc3_parse_support.cpp` formats diagnostics as
     `error:<line>:<column>: <message> [<code>]`.
   - `parse/objc3_parser.cpp` emits diagnostics through `MakeDiag(...)` using
     parser token/derived line and column values.
2. Parser contract snapshots preserve source-precision fingerprints:
   - `parse/objc3_parser_contract.h` mixes line/column into AST shape and
     top-level layout fingerprints.
   - Top-level layout ordering remains line/column deterministic before mixing.
3. Parse/lowering readiness keeps diagnostic determinism explicit:
   - `pipeline/objc3_parse_lowering_readiness_surface.h` computes
     `parser_diagnostic_surface_consistent`.
   - Diagnostic code surfaces remain deterministic through
     `parser_diagnostic_code_surface_deterministic` and
     `BuildObjc3ParseArtifactDiagnosticsHardeningKey(...)`.
4. Architecture anchors remain authoritative for this contract freeze in
   `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m243_a001_diagnostic_grammar_hooks_and_source_precision_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a001_diagnostic_grammar_hooks_and_source_precision_contract.py -q`
- `npm run check:objc3c:m243-a001-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-A001/diagnostic_grammar_hooks_and_source_precision_contract_summary.json`
