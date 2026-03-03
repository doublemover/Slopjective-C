# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A005)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-edge-case-compatibility-completion/m243-a005-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook edge-case compatibility completion, compatibility-handoff hardening, and parse artifact edge robustness fail-closed integration.

## Objective

Complete lane-A edge-case compatibility behavior for parser diagnostics so
compatibility mode transitions, pragma coordinate ordering, and parser token
budget constraints are explicitly enforced before lowering-readiness advances.

## Required Invariants

1. Edge-case compatibility surface exists:
   - `parse/objc3_diagnostic_grammar_hooks_edge_case_compatibility_surface.h`
     defines `Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface`.
   - The surface exposes deterministic compatibility guardrails:
     `parser_snapshot_accounting_consistent`,
     `parser_diagnostic_token_budget_consistent`,
     `language_version_pragma_coordinate_order_consistent`,
     `compatibility_handoff_consistent`, and
     `edge_case_compatibility_ready`.
2. Parse/lowering readiness enforces edge-case compatibility completion:
   - `pipeline/objc3_frontend_types.h` carries
     `parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent`,
     `parser_diagnostic_grammar_hooks_edge_case_compatibility_ready`, and
     `parser_diagnostic_grammar_hooks_edge_case_compatibility_key`.
   - `pipeline/objc3_parse_lowering_readiness_surface.h` integrates the
     edge-case surface and fails closed when compatibility invariants drift.
3. Readiness command wiring remains deterministic:
   - `package.json` defines
     `check:objc3c:m243-a005-diagnostic-grammar-hooks-and-source-precision-edge-case-compatibility-completion-contract`,
     `test:tooling:m243-a005-diagnostic-grammar-hooks-and-source-precision-edge-case-compatibility-completion-contract`,
     and `check:objc3c:m243-a005-lane-a-readiness`.
4. Architecture anchors remain authoritative in
   `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m243_a005_diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a005_diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m243-a005-lane-a-readiness`

## Evidence Path

- `tmp/reports/m243/M243-A005/diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_contract_summary.json`
