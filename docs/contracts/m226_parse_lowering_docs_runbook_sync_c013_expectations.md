# M226 Parse-Lowering Docs/Runbook Sync Expectations (C013)

Contract ID: `objc3c-parse-lowering-docs-runbook-sync-contract/m226-c013-v1`
Status: Accepted
Scope: Parse/lowering docs and operator runbook synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C012 cross-lane integration closure with explicit docs/runbook
synchronization consistency/readiness evidence so parse lowering fails closed if
operator readiness, integration-closeout continuity, or docs synchronization key
determinism drifts.

## Required Invariants

1. Readiness surface tracks docs/runbook synchronization evidence:
   - `toolchain_runtime_ga_operations_docs_runbook_sync_consistent`
   - `toolchain_runtime_ga_operations_docs_runbook_sync_ready`
   - `toolchain_runtime_ga_operations_docs_runbook_sync_key`
2. Readiness builder computes docs/runbook sync deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncKey(...)`
3. Docs/runbook key evidence is folded back into integration-closeout and
   parse-lowering guardrail key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   docs/runbook sync consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations docs and runbook synchronization is inconsistent`
   - `toolchain/runtime GA operations docs and runbook synchronization is not ready`
6. Manifest projection includes docs/runbook sync fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c013_parse_lowering_docs_runbook_sync_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c013_parse_lowering_docs_runbook_sync_contract.py -q`
- `python scripts/check_m226_c012_parse_lowering_cross_lane_integration_sync_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c013_parse_lowering_docs_runbook_sync_contract_summary.json`
