# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A002)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-modular-split/m243-a002-v1`
Status: Accepted
Scope: parser diagnostic source-precision modular split scaffold, parse-to-lowering readiness integration, and build wiring continuity.

## Objective

Execute lane-A modular split and scaffolding for parser diagnostic source
precision so diagnostics UX/fix-it expansion work can rely on deterministic
coordinate/code handoff boundaries.

## Required Invariants

1. Parser diagnostic source-precision scaffold exists and is deterministic:
   - `parse/objc3_diagnostic_source_precision_scaffold.h` defines
     `Objc3ParserDiagnosticSourcePrecisionScaffold`.
   - `parse/objc3_diagnostic_source_precision_scaffold.cpp` defines
     `BuildObjc3ParserDiagnosticSourcePrecisionScaffold(...)`,
     `BuildObjc3ParserDiagnosticSourcePrecisionScaffoldKey(...)`, and
     `IsObjc3ParserDiagnosticSourcePrecisionScaffoldReady(...)`.
2. Parse/lowering readiness integrates scaffold gating:
   - `pipeline/objc3_frontend_types.h` carries
     `parser_diagnostic_source_precision_scaffold_consistent`,
     `parser_diagnostic_source_precision_scaffold_ready`, and
     `parser_diagnostic_source_precision_scaffold_key`.
   - `pipeline/objc3_parse_lowering_readiness_surface.h` computes and enforces
     the source-precision scaffold in diagnostics hardening.
3. Build graph wiring includes the scaffold module:
   - `native/objc3c/CMakeLists.txt` includes
     `src/parse/objc3_diagnostic_source_precision_scaffold.cpp` under
     `objc3c_parse`.
   - `scripts/build_objc3c_native.ps1` includes the scaffold source in
     `frontendModules` `lex-parse` sources.
4. Architecture anchors remain authoritative in
   `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m243_a002_diagnostic_grammar_hooks_and_source_precision_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a002_diagnostic_grammar_hooks_and_source_precision_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m243-a002-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-A002/diagnostic_grammar_hooks_and_source_precision_modular_split_scaffolding_contract_summary.json`
