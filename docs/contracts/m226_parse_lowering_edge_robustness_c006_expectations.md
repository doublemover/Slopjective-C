# M226 Parse-Lowering Edge Robustness Expectations (C006)

Contract ID: `objc3c-parse-lowering-edge-robustness-contract/m226-c006-v1`
Status: Accepted
Scope: Parse artifact edge-robustness fail-closed readiness coverage in `native/objc3c/src/pipeline/*`.

## Objective

Harden parse-to-lowering readiness by requiring explicit edge-robustness
evidence (token budget, pragma coordinate ordering, and deterministic
edge-robustness replay key materialization) before lowering proceeds.

## Required Invariants

1. Readiness surface tracks edge-robustness evidence:
   - `parser_token_count_budget_consistent`
   - `language_version_pragma_coordinate_order_consistent`
   - `parse_artifact_edge_case_robustness_consistent`
   - `parse_artifact_edge_robustness_key`
2. `BuildObjc3ParseLoweringReadinessSurface(...)` computes edge robustness by:
   - enforcing parser token budget coverage across parser snapshot and AST top-level counts,
   - enforcing language-version pragma coordinate ordering consistency,
   - requiring non-empty parse artifact handoff/replay compatibility keys,
   - deriving `parse_artifact_edge_robustness_key` from edge robustness inputs.
3. Parse snapshot replay readiness fails closed on edge-robustness drift:
   - readiness requires `parse_artifact_edge_case_robustness_consistent`,
   - deterministic failure reasons include token budget inconsistency,
     pragma coordinate ordering inconsistency, and edge-case robustness inconsistency.
4. Artifact manifest projects C006 evidence under `parse_lowering_readiness`:
   - `parser_token_count_budget_consistent`
   - `language_version_pragma_coordinate_order_consistent`
   - `parse_artifact_edge_case_robustness_consistent`
   - `parse_artifact_edge_robustness_key`
5. Checker and tests remain fail-closed and emit summaries under `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_c006_parse_lowering_edge_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c006_parse_lowering_edge_robustness_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_c006_parse_lowering_edge_robustness_contract_summary.json`
