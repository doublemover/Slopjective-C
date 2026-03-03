# M243-A010 Diagnostic Grammar Hooks and Source Precision Conformance Corpus Expansion Packet

Packet: `M243-A010`
Milestone: `M243`
Lane: `A`
Freeze date: `2026-03-03`
Dependencies: `M243-A009`

## Purpose

Freeze lane-A parser diagnostic grammar-hook conformance corpus expansion
prerequisites so conformance corpus consistency/readiness and deterministic key
continuity remain fail-closed before parse/lowering performance-quality and
integration-closeout gates advance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Required Code Anchors

- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Required Contract and Tooling Artifacts

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a010_expectations.md`
- `scripts/check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py`
- `tests/tooling/test_check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py`

## Conformance Fixture Anchors

- `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/manifest.json`
- `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/accept_deterministic_grammar_hook_surface.objc3`
- `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_actor_nonreentrant_modifier.objc3`
- `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_trailing_comma_parameter.objc3`
- `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_nondeclaration_after_comma.objc3`
- `tests/tooling/fixtures/m243_a010_diagnostic_grammar_hooks_conformance_corpus/reject_non_objc_pointer_parameter.objc3`

## Dependency Anchors (M243-A009)

- `docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_a009_expectations.md`
- `spec/planning/compiler/m243/m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_packet.md`
- `scripts/check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py`
- `tests/tooling/test_check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py`

## Validation Commands

- `python scripts/check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m243-a010-lane-a-readiness`

## Evidence Output

- `tmp/reports/m243/M243-A010/diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract_summary.json`

## Notes

- All temporary validation artifacts must be written under `tmp/` and retained.
