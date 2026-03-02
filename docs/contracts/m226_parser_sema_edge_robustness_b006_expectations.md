# M226 Parser-to-Sema Edge Robustness Expectations (B006)

Contract ID: `objc3c-parser-sema-edge-robustness-contract/m226-b006-v1`
Status: Accepted
Scope: Parser->sema handoff robustness invariants for parser contract snapshots.

## Objective

Harden parser->sema handoff so malformed or drifted parser snapshots fail closed
before sema execution, including budget checks and subset-bound checks for
parser-derived declaration counters.

## Required Invariants

1. Parser snapshot consistency enforces parser diagnostic budget:
   - `parser_diagnostic_count <= token_count` when `token_count > 0`
2. Parser snapshot consistency enforces parser token budget for top-level
   declarations:
   - `top_level_declaration_count <= token_count` when `token_count > 0`
3. Parser snapshot consistency enforces subset-bound counters:
   - `interface_category_decl_count <= interface_decl_count`
   - `implementation_category_decl_count <= implementation_decl_count`
   - `function_prototype_count <= function_decl_count`
   - `function_pure_count <= function_decl_count`
4. These robustness checks are part of
   `IsObjc3ParserContractSnapshotConsistentWithProgram(...)` and are required in
   the final deterministic pass/fail decision for sema handoff.
5. Checker and tests are fail-closed and write summary output under `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_b006_parser_sema_edge_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b006_parser_sema_edge_robustness_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B006/parser_sema_edge_robustness_summary.json`
