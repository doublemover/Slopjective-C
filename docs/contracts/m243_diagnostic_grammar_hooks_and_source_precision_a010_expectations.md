# M243 Diagnostic Grammar Hooks and Source Precision Expectations (A010)

Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-conformance-corpus-expansion/m243-a010-v1`
Status: Accepted
Scope: parser diagnostic grammar-hook conformance corpus expansion with deterministic fail-closed readiness and conformance fixture continuity.

## Objective

Expand A009 conformance-matrix closure so parser diagnostic grammar-hook
surfaces preserve deterministic conformance corpus consistency/readiness and
conformance-corpus-key continuity before parse/lowering performance-quality and
integration-closeout gates advance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-A009`
- A009 conformance matrix implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a009_expectations.md`
  - `spec/planning/compiler/m243/m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py`
- A010 conformance corpus fixture anchors remain mandatory:
  - `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/manifest.json`
  - `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/accept_deterministic_grammar_hook_surface.objc3`
  - `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_actor_nonreentrant_modifier.objc3`
  - `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_trailing_comma_parameter.objc3`
  - `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_nondeclaration_after_comma.objc3`
  - `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_non_objc_pointer_parameter.objc3`

## Deterministic Invariants

1. Parse/lowering readiness carries parser diagnostic grammar-hook conformance corpus fields:
   - `parser_diagnostic_grammar_hooks_conformance_corpus_consistent`
   - `parser_diagnostic_grammar_hooks_conformance_corpus_ready`
   - `parser_diagnostic_grammar_hooks_conformance_corpus_key`
2. `pipeline/objc3_parse_lowering_readiness_surface.h` computes parser diagnostic
   grammar-hook conformance corpus consistency/readiness deterministically from
   A009 conformance matrix readiness, parse/lowering conformance corpus
   accounting, and replay-key continuity.
3. Fail-closed failure reasons explicitly cover parser diagnostic grammar-hook
   conformance corpus consistency/readiness drift.
4. `pipeline/objc3_frontend_artifacts.cpp` projects parser diagnostic
   grammar-hook conformance corpus booleans and key in frontend readiness
   artifact JSON.
5. Conformance corpus fixture manifest preserves deterministic `A010-C001`
   through `A010-C005` case mapping and fail-closed diagnostic code anchors.

## Architecture and Build Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-A A010
  conformance corpus expansion anchor text.
- `package.json` includes:
  - `check:objc3c:m243-a010-diagnostic-grammar-hooks-and-source-precision-conformance-corpus-expansion-contract`
  - `test:tooling:m243-a010-diagnostic-grammar-hooks-and-source-precision-conformance-corpus-expansion-contract`
  - `check:objc3c:m243-a010-lane-a-readiness`

## Validation

- `python scripts/check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m243-a010-lane-a-readiness`

## Evidence Path

- `tmp/reports/m243/M243-A010/diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract_summary.json`
