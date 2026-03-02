# M226 Parse-Lowering Edge Compatibility Completion Expectations (C005)

Contract ID: `objc3c-parse-lowering-edge-compatibility-completion-contract/m226-c005-v1`
Status: Accepted
Scope: Parse artifact edge-case and compatibility readiness completion in `native/objc3c/src/pipeline/*`.

## Objective

Complete parse-to-lowering readiness with explicit edge-case compatibility
coverage by gating on AST top-level layout fingerprint parity and
compatibility handoff consistency before lowering proceeds.

## Required Invariants

1. Readiness surface tracks edge-case parse artifact layout evidence:
   - `parser_ast_top_level_layout_fingerprint`
   - `ast_top_level_layout_fingerprint`
   - `parse_artifact_layout_fingerprint_consistent`
2. Readiness surface tracks compatibility handoff evidence:
   - `compatibility_handoff_consistent`
   - `compatibility_handoff_key`
3. `BuildObjc3ParseLoweringReadinessSurface(...)` computes C005 compatibility
   readiness by:
   - comparing parser snapshot and parsed-program top-level layout fingerprints,
   - validating migration-hint and language-version pragma contract consistency,
   - requiring compatibility handoff consistency for parse artifact replay-key determinism.
4. Parse snapshot readiness fails closed on edge/compat drift:
   - deterministic failure reasons include layout fingerprint inconsistency and
     compatibility handoff inconsistency.
5. Artifact manifest projects C005 evidence under `parse_lowering_readiness`:
   - `parse_artifact_layout_fingerprint_consistent`
   - `compatibility_handoff_consistent`
   - `parser_ast_top_level_layout_fingerprint`
   - `ast_top_level_layout_fingerprint`
   - `compatibility_handoff_key`
6. Checker and tests remain fail-closed and emit summaries under
   `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_c005_parse_lowering_edge_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c005_parse_lowering_edge_compatibility_completion_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_c005_parse_lowering_edge_compatibility_completion_contract_summary.json`
