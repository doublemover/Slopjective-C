# M226 Parser Conformance Corpus Expansion Expectations (A010)

Contract ID: `objc3c-parser-conformance-corpus-contract/m226-a010-v1`
Status: Accepted
Scope: Lane-A parser conformance corpus expansion for C-style compatibility diagnostics.

## Objective

Expand parser conformance corpus coverage for compatibility declaration forms
added/hardened in `A005` through `A009`, with deterministic case indexing.

## Required Invariants

1. Corpus manifest exists at:
   - `tests/tooling/fixtures/m226_a010_parser_conformance_corpus/manifest.json`
2. Manifest contract id is:
   - `objc3c-parser-conformance-corpus-contract/m226-a010-v1`
3. Corpus includes at least these deterministic cases:
   - Accept `void*` compatibility parameter.
   - Reject `i32*` compatibility parameter (`O3P114`).
   - Reject `bool*` compatibility parameter (`O3P114`).
   - Reject trailing comma in compatibility parameter list (`O3P104`).
   - Reject non-declaration token after comma (`O3P100`).
4. All listed case files are present and non-empty.

## Validation

- `python scripts/check_m226_a010_parser_conformance_corpus_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a010_parser_conformance_corpus_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A010/parser_conformance_corpus_summary.json`
