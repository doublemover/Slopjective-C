# M226 Parser Snapshot Surface Expansion Expectations (A004)

Contract ID: `objc3c-parser-snapshot-surface-expansion-contract/m226-a004-v1`
Status: Accepted
Scope: Expansion of parser snapshot surface for declaration/token accounting.

## Objective

Expand parser snapshot metadata so conformance and lowering lanes can reason
about parser topology with deterministic counts.

## Required Invariants

1. `Objc3ParserContractSnapshot` includes:
   - `token_count`
   - `top_level_declaration_count`
2. Snapshot builder computes declaration totals from parser contract fields.
3. Parse entrypoint passes token stream size to snapshot builder.
4. Frontend artifact manifest emits:
   - `frontend.pipeline.stages.parser.token_count`
   - `frontend.pipeline.stages.parser.top_level_declarations`

## Validation

- `npm run build:objc3c-native`
- `python scripts/check_m226_a004_parser_snapshot_surface_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a004_parser_snapshot_surface_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A004/parser_snapshot_surface_expansion_summary.json`
