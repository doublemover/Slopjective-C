# M226 Parse-Lowering Conformance Matrix Expectations (C009)

Contract ID: `objc3c-parse-lowering-conformance-matrix-contract/m226-c009-v1`
Status: Accepted
Scope: Parse/lowering conformance-matrix readiness coverage in `native/objc3c/src/pipeline/*`.

## Objective

Require fail-closed parse-to-lowering conformance-matrix invariants that bind
parse artifacts, recovery hardening, sema handoff determinism, and lowering
readiness into one deterministic conformance matrix gate.

## Required Invariants

1. Readiness surface tracks conformance-matrix evidence:
   - `parse_lowering_conformance_matrix_consistent`
   - `parse_lowering_conformance_matrix_case_count`
   - `parse_lowering_conformance_matrix_key`
2. Readiness builder pins and computes conformance-matrix coverage:
   - `kObjc3ParseLoweringConformanceMatrixCaseCount`
   - `BuildObjc3ParseLoweringConformanceMatrixKey(...)`
3. Conformance matrix consistency is fail-closed and composes parser snapshot,
   replay, diagnostics hardening, edge robustness, recovery hardening, and
   semantic handoff determinism invariants.
4. `ready_for_lowering` requires `parse_lowering_conformance_matrix_ready`.
5. Readiness failure reason includes:
   - `parse-lowering conformance matrix is inconsistent`
6. Manifest projection includes conformance-matrix fields under
   `parse_lowering_readiness`:
   - `parse_lowering_conformance_matrix_consistent`
   - `parse_lowering_conformance_matrix_case_count`
   - `parse_lowering_conformance_matrix_key`
7. Validation entrypoints are pinned:
   - `python scripts/check_m226_c009_parse_lowering_conformance_matrix_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_c009_parse_lowering_conformance_matrix_contract.py -q`

## Validation

- `python scripts/check_m226_c009_parse_lowering_conformance_matrix_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c009_parse_lowering_conformance_matrix_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/m226_c009_parse_lowering_conformance_matrix_contract_summary.json`
