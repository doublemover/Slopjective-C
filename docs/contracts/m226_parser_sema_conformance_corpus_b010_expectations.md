# M226 Parser-Sema Conformance Corpus Expectations (B010)

Contract ID: `objc3c-parser-sema-conformance-corpus/m226-b010-v1`
Status: Accepted
Scope: Parser-to-sema conformance corpus expansion and deterministic gate wiring.

## Objective

Expand parser-to-sema conformance from matrix-only checks into a deterministic
conformance corpus with explicit required/passed/failed case accounting and
fail-closed pass-manager gating.

## Required Invariants

1. Handoff scaffold exposes a corpus builder:
   - `BuildObjc3ParserSemaConformanceCorpus(...)`
2. Corpus surface encodes deterministic case accounting:
   - required/passed/failed case counts
   - deterministic flag gated on full pass
3. Pass-manager contract surface carries corpus state:
   - `parser_sema_conformance_corpus`
   - `deterministic_parser_sema_conformance_corpus`
4. Pass-manager execution fail-closes when corpus determinism fails.
5. Integration tests assert corpus wiring across handoff, contract, and manager.

## Validation

- `python scripts/check_m226_b010_parser_sema_conformance_corpus_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b010_parser_sema_conformance_corpus_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B010/parser_sema_conformance_corpus_summary.json`
