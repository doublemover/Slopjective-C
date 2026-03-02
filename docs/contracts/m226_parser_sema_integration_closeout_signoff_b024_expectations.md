# M226 Parser-Sema Integration Closeout and Gate Signoff Expectations (B024)

Contract ID: `objc3c-parser-sema-integration-closeout-signoff-contract/m226-b024-v1`
Status: Accepted
Scope: Parser->sema integration closeout signoff synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema integration closeout signoff synchronization so advanced diagnostics shard2 surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Integration closeout signoff surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaIntegrationCloseoutSignoff`
   - `BuildObjc3ParserSemaIntegrationCloseoutSignoff(...)`
   - `parser_sema_integration_closeout_signoff`
2. Integration closeout signoff requires deterministic advanced diagnostics shard2 continuity and explicit pass-manager parity handoff:
   - `advanced_diagnostics_shard2_ready`
   - `pass_manager_contract_surface_sync`
   - `gate_signoff_surface_sync`
3. Pass-manager execution is fail-closed on integration closeout signoff sync drift:
   - `if (!result.deterministic_parser_sema_integration_closeout_signoff) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 3u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces integration closeout signoff sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b024_parser_sema_integration_closeout_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b024_parser_sema_integration_closeout_signoff_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B024/parser_sema_integration_closeout_signoff_summary.json`













