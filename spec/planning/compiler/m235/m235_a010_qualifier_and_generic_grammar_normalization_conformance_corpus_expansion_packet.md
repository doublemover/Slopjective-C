# M235-A010 Qualifier/Generic Grammar Normalization Conformance Corpus Expansion Packet

Packet: `M235-A010`
Milestone: `M235`
Lane: `A`
Issue: `#5680`
Freeze date: `2026-03-04`
Dependencies: `M235-A009`

## Purpose

Freeze lane-A conformance corpus expansion prerequisites for M235 qualifier/generic grammar normalization continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_grammar_normalization_conformance_corpus_expansion_a010_expectations.md`
- Checker:
  `scripts/check_m235_a010_qualifier_and_generic_grammar_normalization_conformance_corpus_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_a010_qualifier_and_generic_grammar_normalization_conformance_corpus_expansion_contract.py`
- Dependency anchors from `M235-A009`:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_conformance_matrix_implementation_a009_expectations.md`
  - `spec/planning/compiler/m235/m235_a009_qualifier_and_generic_grammar_normalization_conformance_matrix_implementation_packet.md`
  - `scripts/check_m235_a009_qualifier_and_generic_grammar_normalization_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m235_a009_qualifier_and_generic_grammar_normalization_conformance_matrix_implementation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m235_a010_qualifier_and_generic_grammar_normalization_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a010_qualifier_and_generic_grammar_normalization_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m235-a010-lane-a-readiness`

## Evidence Output

- `tmp/reports/m235/M235-A010/qualifier_and_generic_grammar_normalization_conformance_corpus_expansion_summary.json`




