# M226 Parser-Sema Docs and Operator Runbook Synchronization Expectations (B013)

Contract ID: `objc3c-parser-sema-docs-runbook-sync-contract/m226-b013-v1`
Status: Accepted
Scope: Parser->sema docs/runbook synchronization with fail-closed pass-manager gating.

## Objective

Freeze parser->sema docs/runbook synchronization so cross-lane surfaces stay coherent and sema execution fails closed when sync determinism drifts.

## Required Invariants

1. Docs/runbook sync surface is explicit and versioned in the handoff scaffold:
   - `Objc3ParserSemaDocsRunbookSync`
   - `BuildObjc3ParserSemaDocsRunbookSync(...)`
   - `parser_sema_docs_runbook_sync`
2. Docs/runbook sync requires deterministic cross-lane continuity and explicit pass-manager parity handoff:
   - `cross_lane_integration_sync_ready`
   - `pass_manager_contract_surface_sync`
   - `parity_surface_sync`
3. Pass-manager execution is fail-closed on docs/runbook sync drift:
   - `if (!result.deterministic_parser_sema_docs_runbook_sync) { return result; }`
4. Parity/readiness requires sync determinism and complete sync accounting:
   - `required_sync_count == 3u`
   - `passed_sync_count == required_sync_count`
   - `failed_sync_count == 0u`
5. Tooling checker enforces docs/runbook sync contract anchors and fails closed on drift.

## Validation

- `python scripts/check_m226_b013_parser_sema_docs_runbook_sync_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b013_parser_sema_docs_runbook_sync_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B013/parser_sema_docs_runbook_sync_summary.json`


