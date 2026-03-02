# M226 Parser Contract Snapshot Expectations (A003)

Contract ID: `objc3c-parser-contract-snapshot-contract/m226-a003-v1`
Status: Accepted
Scope: Parser core handoff metadata from parse stage into frontend pipeline artifacts.

## Objective

Implement a parser-core contract snapshot so downstream sema/lowering stages can
consume deterministic parser declaration surfaces without reparsing internals.

## Required Invariants

1. Parser contracts define a stable snapshot type:
   - `Objc3ParserContractSnapshot` in `parse/objc3_parser_contract.h`.
   - Snapshot carries declaration counters and deterministic handoff flags.
2. Parse results carry snapshot metadata:
   - `Objc3ParseResult.contract_snapshot`
   - `Objc3AstBuilderResult.contract_snapshot`
3. Pipeline preserves parser snapshot:
   - `Objc3FrontendPipelineResult.parser_contract_snapshot`
   - pipeline sets it from parse stage output.
4. Manifest output exports parser snapshot counters/flags under
   `frontend.pipeline.stages.parser`.

## Validation

- `npm run build:objc3c-native`
- `python scripts/check_m226_a003_parser_contract_snapshot_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a003_parser_contract_snapshot_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A003/parser_contract_snapshot_contract_summary.json`
