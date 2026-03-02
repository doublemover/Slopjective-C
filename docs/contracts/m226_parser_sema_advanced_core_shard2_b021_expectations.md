# M226 Parser-Sema Advanced Core Workpack (Shard 2) Expectations (B021)

Contract ID: `objc3c-parser-sema-advanced-core-shard2-contract/m226-b021-v1`
Status: Accepted
Scope: Parser->sema advanced core shard2 synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema advanced core shard2 synchronization so advanced performance shard1 surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Advanced core shard2 sync surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaAdvancedCoreShard2`
   - `BuildObjc3ParserSemaAdvancedCoreShard2(...)`
   - `parser_sema_advanced_core_shard2`
2. Advanced core shard2 sync requires deterministic advanced performance shard1 continuity and explicit pass-manager parity handoff:
   - `advanced_performance_shard1_ready`
   - `pass_manager_contract_surface_sync`
   - `shard_surface_sync`
3. Pass-manager execution is fail-closed on advanced core shard2 sync drift:
   - `if (!result.deterministic_parser_sema_advanced_core_shard2) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 3u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces advanced core shard2 sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b021_parser_sema_advanced_core_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b021_parser_sema_advanced_core_shard2_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B021/parser_sema_advanced_core_shard2_summary.json`










