# M226 Parser-to-Sema Edge/Compatibility Completion Expectations (B005)

Contract ID: `objc3c-parser-sema-edge-compat-handoff-contract/m226-b005-v1`
Status: Accepted
Scope: Parser->sema handoff edge-case completion for legacy-compat snapshots.

## Objective

Complete parser->sema handoff compatibility by accepting legacy parser snapshots
that omit newer fingerprint/count fields, while preserving canonical fail-closed
behavior for true drift.

## Required Invariants

1. Handoff scaffold detects legacy compatibility edge-cases:
   - missing `ast_shape_fingerprint`
   - missing `ast_top_level_layout_fingerprint`
   - missing aggregate `top_level_declaration_count` when declaration buckets are present
2. Normalization is gated to `Objc3SemaCompatibilityMode::Legacy`.
3. Canonical mode remains strict fail-closed; compatibility normalization does not
   mask non-zero mismatches.
4. Normalized snapshots still flow through deterministic fingerprint and consistency
   checks before sema pass execution.

## Validation

- `python scripts/check_m226_b005_parser_sema_edge_compat_handoff_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b005_parser_sema_edge_compat_handoff_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B005/parser_sema_edge_compat_handoff_summary.json`
