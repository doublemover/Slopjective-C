# M226 Parser-Sema Advanced Diagnostics Workpack (Shard 2) Expectations (B023)

Contract ID: `objc3c-parser-sema-advanced-diagnostics-shard2-contract/m226-b023-v1`
Status: Accepted
Scope: Parser->sema advanced diagnostics shard2 synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema advanced diagnostics shard2 synchronization so advanced edge-compatibility shard2 surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Advanced diagnostics shard2 sync surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaAdvancedDiagnosticsShard2`
   - `BuildObjc3ParserSemaAdvancedDiagnosticsShard2(...)`
   - `parser_sema_advanced_diagnostics_shard2`
2. Advanced diagnostics shard2 sync requires deterministic advanced edge-compatibility shard2 continuity and explicit pass-manager parity handoff:
   - `advanced_edge_compatibility_shard2_ready`
   - `pass_manager_contract_surface_sync`
   - `shard_surface_sync`
3. Pass-manager execution is fail-closed on advanced diagnostics shard2 sync drift:
   - `if (!result.deterministic_parser_sema_advanced_diagnostics_shard2) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 3u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces advanced diagnostics shard2 sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b023_parser_sema_advanced_diagnostics_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b023_parser_sema_advanced_diagnostics_shard2_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B023/parser_sema_advanced_diagnostics_shard2_summary.json`












