# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A006)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-edge-case-expansion-and-robustness/m243-a006-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook edge-case expansion and robustness guardrails for diagnostics UX and fix-it engine continuity.

## Objective

Extend A005 edge-case compatibility closure with explicit expansion and robustness
signals so parser diagnostic grammar-hook drift fails closed before downstream
parse/lowering recovery and conformance readiness proceeds.

## Required Invariants

1. Parse/lowering readiness carries explicit parser diagnostic edge-case expansion and robustness fields:
   - `parser_diagnostic_grammar_hooks_edge_case_expansion_consistent`
   - `parser_diagnostic_grammar_hooks_edge_case_robustness_ready`
   - `parser_diagnostic_grammar_hooks_edge_case_robustness_key`
2. `pipeline/objc3_parse_lowering_readiness_surface.h` computes these fields deterministically from:
   - A005 compatibility continuity,
   - parse artifact edge robustness continuity,
   - parser diagnostic surface determinism,
   - parser diagnostic source-precision readiness,
   - replay-key determinism continuity.
3. Readiness fails closed when parser diagnostic edge-case expansion/robustness drifts.
4. Contract anchoring preserves A005 dependency continuity:
   - `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a005_expectations.md`
   - `spec/planning/compiler/m243/m243_a005_diagnostic_grammar_hooks_and_source_precision_edge_case_compatibility_completion_packet.md`
5. Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Validation

- `python scripts/check_m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m243-a006-lane-a-readiness`

## Evidence Path

- `tmp/reports/m243/M243-A006/diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract_summary.json`
