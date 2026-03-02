# M226 Parser-Sema Advanced Edge Compatibility Workpack (Shard 1) Expectations (B016)

Contract ID: `objc3c-parser-sema-advanced-edge-compatibility-shard1-contract/m226-b016-v1`
Status: Accepted
Scope: Parser->sema advanced edge compatibility shard1 synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema advanced edge compatibility shard1 synchronization so advanced core shard1 surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Advanced edge compatibility shard1 sync surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaAdvancedEdgeCompatibilityShard1`
   - `BuildObjc3ParserSemaAdvancedEdgeCompatibilityShard1(...)`
   - `parser_sema_advanced_edge_compatibility_shard1`
2. Advanced edge compatibility shard1 sync requires deterministic advanced core shard1 continuity and explicit pass-manager parity handoff:
   - `advanced_core_shard1_ready`
   - `pass_manager_contract_surface_sync`
   - `shard_surface_sync`
3. Pass-manager execution is fail-closed on advanced edge compatibility shard1 sync drift:
   - `if (!result.deterministic_parser_sema_advanced_edge_compatibility_shard1) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 3u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces advanced edge compatibility shard1 sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b016_parser_sema_advanced_edge_compatibility_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b016_parser_sema_advanced_edge_compatibility_shard1_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B016/parser_sema_advanced_edge_compatibility_shard1_summary.json`





