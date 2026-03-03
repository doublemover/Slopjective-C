# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A007)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-diagnostics-hardening/m243-a007-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook diagnostics hardening and deterministic fail-closed readiness progression.

## Objective

Execute lane-A diagnostics hardening so parser diagnostic grammar-hook hardening
signals remain deterministic and fail-closed before parse-recovery and
conformance readiness advances.

## Required Invariants

1. Parse/lowering readiness carries parser diagnostic hardening fields:
   - `parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent`
   - `parser_diagnostic_grammar_hooks_diagnostics_hardening_ready`
   - `parser_diagnostic_grammar_hooks_diagnostics_hardening_key`
2. `pipeline/objc3_parse_lowering_readiness_surface.h` computes hardening
   readiness deterministically from A006 robustness continuity, parse artifact
   diagnostics hardening continuity, parser diagnostic deterministic surfaces,
   and replay-key continuity.
3. Fail-closed failure reasons explicitly cover diagnostics hardening drift.
4. A006 dependency continuity is explicit in docs and packet anchors.
5. Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Validation

- `python scripts/check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m243-a007-lane-a-readiness`

## Evidence Path

- `tmp/reports/m243/M243-A007/diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract_summary.json`
