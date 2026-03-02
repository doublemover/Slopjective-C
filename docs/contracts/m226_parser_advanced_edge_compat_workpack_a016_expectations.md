# M226 Parser Advanced Edge Compatibility Workpack Expectations (A016)

Contract ID: `objc3c-parser-advanced-edge-compat-workpack-contract/m226-a016-v1`
Status: Accepted
Scope: Parser advanced edge compatibility workpack shard 1 for native frontend decomposition and parser completeness.

## Objective

Harden parser->sema compatibility normalization for legacy snapshots by
detecting declaration-bucket overflow and repairing bucket counts from AST so
handoff remains deterministic and fail closed.

## Required Invariants

1. Sema handoff defines overflow detector:
   - `IsObjc3ParserContractTopLevelDeclBucketOverflow`
2. Compatibility edge-case detection includes overflow:
   - `decl_bucket_overflow` participates in
     `IsObjc3ParserContractCompatibilityEdgeCaseSnapshot`.
3. Legacy normalization repairs declaration buckets when either condition holds:
   - missing top-level declaration buckets for non-empty program
   - declaration-bucket overflow
4. Normalization remains gated to `Objc3SemaCompatibilityMode::Legacy`.
5. Validation entrypoints are pinned:
   - `python scripts/check_m226_a016_parser_advanced_edge_compat_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a016_parser_advanced_edge_compat_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a016_parser_advanced_edge_compat_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a016_parser_advanced_edge_compat_workpack_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A016/parser_advanced_edge_compat_workpack_summary.json`
