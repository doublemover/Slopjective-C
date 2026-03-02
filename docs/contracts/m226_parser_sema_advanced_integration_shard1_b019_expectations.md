# M226 Parser-Sema Advanced Integration Workpack (Shard 1) Expectations (B019)

Contract ID: `objc3c-parser-sema-advanced-integration-shard1-contract/m226-b019-v1`
Status: Accepted
Scope: Parser->sema advanced integration shard1 synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema advanced integration shard1 synchronization so advanced diagnostics shard1 surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Advanced integration shard1 sync surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaAdvancedIntegrationShard1`
   - `BuildObjc3ParserSemaAdvancedIntegrationShard1(...)`
   - `parser_sema_advanced_integration_shard1`
2. Advanced integration shard1 sync requires deterministic advanced conformance shard1 continuity and explicit pass-manager parity handoff:
   - `advanced_conformance_shard1_ready`
   - `pass_manager_contract_surface_sync`
   - `shard_surface_sync`
3. Pass-manager execution is fail-closed on advanced integration shard1 sync drift:
   - `if (!result.deterministic_parser_sema_advanced_integration_shard1) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 3u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces advanced integration shard1 sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b019_parser_sema_advanced_integration_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b019_parser_sema_advanced_integration_shard1_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B019/parser_sema_advanced_integration_shard1_summary.json`








