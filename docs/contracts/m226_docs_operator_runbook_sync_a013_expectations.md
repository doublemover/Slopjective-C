# M226 Docs and Operator Runbook Sync Expectations (A013)

Contract ID: `objc3c-docs-operator-runbook-sync-contract/m226-a013-v1`
Status: Accepted
Scope: Lane-A runbook synchronization for currently active M226 packets.

## Objective

Provide an operator-facing runbook that reflects the current M226 lane contract
set and the canonical validation command sequence used in this wave.

## Required Invariants

1. Runbook exists at:
   - `docs/runbooks/m226_wave_execution_runbook.md`
2. Runbook includes contract IDs for:
   - A011, B003, C003, D003, E003, A012
3. Runbook includes canonical commands for:
   - `npm run build:objc3c-native`
   - `scripts/objc3c_native_compile.ps1` smoke invocation
   - A012 cross-lane checker
4. Runbook references evidence location under `tmp/reports/m226`.

## Validation

- `python scripts/check_m226_a013_docs_operator_runbook_sync_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a013_docs_operator_runbook_sync_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A013/docs_operator_runbook_sync_summary.json`
