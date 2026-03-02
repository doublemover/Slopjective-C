# M226 AST Contract Parser-to-Sema Conformance Matrix Expectations (B009)

Contract ID: `objc3c-parser-sema-conformance-matrix-contract/m226-b009-v1`
Status: Accepted
Scope: AST contract/parser->sema conformance matrix hardening and fail-closed execution gate.

## Objective

Enforce a concrete parser->sema conformance matrix that binds parser contract
snapshot rows to AST-derived expectations and blocks sema execution whenever any
matrix row drifts.

## Required Invariants

1. Parser->sema conformance matrix is explicit and row-based:
   - `Objc3ParserSemaConformanceMatrix`
   - `BuildObjc3ParserSemaConformanceMatrix(...)`
2. Matrix rows cover declaration-count and fingerprint parity across parser
   snapshot vs AST-derived expected snapshot:
   - top-level/global/protocol/interface/implementation/function buckets
   - property/method/category/prototype/pure derivative buckets
   - `ast_shape_fingerprint` and `ast_top_level_layout_fingerprint`
   - snapshot fingerprint parity
3. Matrix row consistency is fail-closed and includes budget/subset bounds:
   - `parser_diagnostic_budget_consistent`
   - `parser_token_top_level_budget_consistent`
   - `parser_subset_count_consistent`
4. Sema pass manager fail-closes before pass execution if matrix determinism
   fails:
   - `if (!result.deterministic_parser_sema_conformance_matrix) { return result; }`
5. Parity surface/readiness requires matrix determinism and matrix row pass
   flags before reporting ready.
6. Checker and tests are fail-closed and write summary output under
   `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_b009_parser_sema_conformance_matrix_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b009_parser_sema_conformance_matrix_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B009/parser_sema_conformance_matrix_summary.json`
