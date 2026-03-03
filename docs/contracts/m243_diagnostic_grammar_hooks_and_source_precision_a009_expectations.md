# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A009)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-conformance-matrix-implementation/m243-a009-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook conformance matrix implementation with deterministic fail-closed readiness and key continuity.

## Objective

Expand A008 recovery/determinism closure so parser diagnostic grammar-hook
surfaces preserve deterministic conformance matrix consistency/readiness and
conformance-matrix-key continuity before parse/lowering conformance corpus and
performance/quality guardrail progression.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-A008`
- A008 recovery/determinism anchors remain mandatory prerequisites:
  - `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a008_expectations.md`
  - `spec/planning/compiler/m243/m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m243_a008_diagnostic_grammar_hooks_and_source_precision_recovery_and_determinism_hardening_contract.py`

## Deterministic Invariants

1. Parse/lowering readiness carries parser diagnostic grammar-hook conformance matrix fields:
   - `parser_diagnostic_grammar_hooks_conformance_matrix_consistent`
   - `parser_diagnostic_grammar_hooks_conformance_matrix_ready`
   - `parser_diagnostic_grammar_hooks_conformance_matrix_key`
2. `pipeline/objc3_parse_lowering_readiness_surface.h` computes parser diagnostic
   grammar-hook conformance matrix consistency/readiness deterministically from
   A008 recovery/determinism readiness, parse replay determinism, and conformance
   case-count continuity.
3. Fail-closed failure reasons explicitly cover parser diagnostic grammar-hook
   conformance matrix consistency/readiness drift.
4. `pipeline/objc3_frontend_artifacts.cpp` projects parser diagnostic grammar-hook
   conformance matrix booleans and key in the frontend readiness artifact JSON.
5. Parse/lowering conformance matrix closure remains blocked until parser
   diagnostic grammar-hook conformance matrix consistency/readiness is satisfied.

## Architecture and Build Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-A A009
  conformance matrix implementation anchor text.
- `package.json` includes:
  - `check:objc3c:m243-a009-diagnostic-grammar-hooks-and-source-precision-conformance-matrix-implementation-contract`
  - `test:tooling:m243-a009-diagnostic-grammar-hooks-and-source-precision-conformance-matrix-implementation-contract`
  - `check:objc3c:m243-a009-lane-a-readiness`

## Validation

- `python scripts/check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m243-a009-lane-a-readiness`

## Evidence Path

- `tmp/reports/m243/M243-A009/diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract_summary.json`

