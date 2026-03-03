# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A004)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-core-feature-expansion/m243-a004-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook core feature expansion, case-accounting/replay-key hardening, and parse-to-lowering fail-closed readiness integration.

## Objective

Expand lane-A parser diagnostic grammar-hook readiness beyond core feature
implementation so downstream diagnostics UX/fix-it work depends on explicit
accounting and replay-key guardrails, not only namespace/order checks.

## Required Invariants

1. Core feature expansion surface exists:
   - `parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h`
     defines `Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface`.
   - The expansion surface exposes deterministic expansion state:
     `accounting_consistent`, `replay_keys_ready`,
     `core_feature_expansion_ready`, and `expansion_key`.
2. Parse/lowering readiness enforces expansion guardrails:
   - `pipeline/objc3_frontend_types.h` carries
     `parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent`,
     `parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready`,
     `parser_diagnostic_grammar_hooks_core_feature_expansion_ready`, and
     `parser_diagnostic_grammar_hooks_core_feature_expansion_key`.
   - `pipeline/objc3_parse_lowering_readiness_surface.h` integrates expansion
     accounting/replay checks into diagnostics hardening and fail-closed failure
     reasons.
3. Readiness command wiring remains deterministic:
   - `package.json` defines
     `check:objc3c:m243-a004-diagnostic-grammar-hooks-and-source-precision-core-feature-expansion-contract`,
     `test:tooling:m243-a004-diagnostic-grammar-hooks-and-source-precision-core-feature-expansion-contract`,
     and `check:objc3c:m243-a004-lane-a-readiness`.
4. Architecture anchors remain authoritative in
   `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a004_diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m243-a004-lane-a-readiness`

## Evidence Path

- `tmp/reports/m243/M243-A004/diagnostic_grammar_hooks_and_source_precision_core_feature_expansion_contract_summary.json`
