# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A003)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-core-feature/m243-a003-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook core feature implementation, source-precision handoff consistency, and parse-to-lowering fail-closed readiness integration.

## Objective

Implement lane-A core feature behavior for parser diagnostic grammar hooks and
source precision so diagnostics UX/fix-it work can rely on deterministic parser
hook namespace coverage and coordinate ordering.

## Required Invariants

1. Diagnostic grammar hooks core feature surface exists:
   - `parse/objc3_diagnostic_grammar_hooks_core_feature.h` defines
     `Objc3DiagnosticGrammarHooksCoreFeatureSurface`.
   - `parse/objc3_diagnostic_grammar_hooks_core_feature.cpp` defines
     `BuildObjc3DiagnosticGrammarHooksCoreFeatureSurface(...)`,
     `BuildObjc3DiagnosticGrammarHooksCoreFeatureKey(...)`, and
     `IsObjc3DiagnosticGrammarHooksCoreFeatureReady(...)`.
2. Parse/lowering readiness integrates core feature gating:
   - `pipeline/objc3_frontend_types.h` carries
     `parser_diagnostic_grammar_hooks_core_feature_consistent`,
     `parser_diagnostic_grammar_hooks_core_feature_ready`, and
     `parser_diagnostic_grammar_hooks_core_feature_key`.
   - `pipeline/objc3_parse_lowering_readiness_surface.h` enforces grammar-hook
     core feature consistency inside diagnostics hardening and failure reasons.
3. Build graph includes parser grammar-hook core feature module:
   - `native/objc3c/CMakeLists.txt` includes
     `src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp`.
   - `scripts/build_objc3c_native.ps1` includes the same source under the
     `lex-parse` module list.
4. Architecture anchors remain authoritative in
   `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a003_diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m243-a003-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-A003/diagnostic_grammar_hooks_and_source_precision_core_feature_implementation_contract_summary.json`
