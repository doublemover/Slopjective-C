# M226 Parser-Sema Cross-Lane Integration Sync Expectations (B012)

Contract ID: `objc3c-parser-sema-cross-lane-integration-sync-contract/m226-b012-v1`
Status: Accepted
Scope: Parser->sema cross-lane integration synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema cross-lane synchronization so matrix/corpus/guardrail surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Cross-lane sync surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaCrossLaneIntegrationSync`
   - `BuildObjc3ParserSemaCrossLaneIntegrationSync(...)`
   - `parser_sema_cross_lane_integration_sync`
2. Cross-lane sync requires deterministic matrix/corpus/guardrail continuity and explicit pass-manager parity handoff:
   - `matrix_consistent`
   - `corpus_consistent`
   - `performance_quality_guardrails_consistent`
   - `pass_manager_contract_surface_sync`
3. Pass-manager execution is fail-closed on cross-lane sync drift:
   - `if (!result.deterministic_parser_sema_cross_lane_integration_sync) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 4u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces cross-lane sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b012_parser_sema_cross_lane_integration_sync_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b012_parser_sema_cross_lane_integration_sync_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B012/parser_sema_cross_lane_integration_sync_summary.json`

