# M226 Parse-Lowering Conformance Corpus Expectations (C010)

Contract ID: `objc3c-parse-lowering-conformance-corpus-contract/m226-c010-v1`
Status: Accepted
Scope: Parse/lowering conformance corpus expansion coverage in `native/objc3c/src/pipeline/*`.

## Objective

Expand C009 conformance-matrix readiness into deterministic corpus coverage with
explicit required/passed/failed case accounting and fail-closed lowerability
gating.

## Required Invariants

1. Readiness surface tracks conformance-corpus evidence:
   - `parse_lowering_conformance_corpus_consistent`
   - `parse_lowering_conformance_corpus_case_count`
   - `parse_lowering_conformance_corpus_passed_case_count`
   - `parse_lowering_conformance_corpus_failed_case_count`
   - `parse_lowering_conformance_corpus_key`
2. Readiness builder pins and computes corpus coverage:
   - `kObjc3ParseLoweringConformanceCorpusCaseCount`
   - `BuildObjc3ParseLoweringConformanceCorpusKey(...)`
3. Conformance-corpus consistency is fail-closed and requires full pass across
   parser snapshot, handoff, replay, diagnostics hardening, edge robustness,
   recovery hardening, semantic handoff, and lowering boundary cases.
4. `ready_for_lowering` requires `parse_lowering_conformance_corpus_ready`.
5. Readiness failure reason includes:
   - `parse-lowering conformance corpus is inconsistent`
6. Manifest projection includes conformance-corpus fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c010_parse_lowering_conformance_corpus_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c010_parse_lowering_conformance_corpus_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/m226_c010_parse_lowering_conformance_corpus_contract_summary.json`
