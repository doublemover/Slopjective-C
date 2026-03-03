# M227 Typed Sema-to-Lowering Docs/Runbook Synchronization Expectations (C013)

Contract ID: `objc3c-typed-sema-to-lowering-docs-runbook-sync/m227-c013-v1`
Status: Accepted
Scope: typed sema-to-lowering docs/runbook synchronization on top of C012 cross-lane integration sync.

## Objective

Execute issue `#5133` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with docs/runbook synchronization consistency/readiness
invariants, with deterministic fail-closed alignment checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C012`
- `M227-C012` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_cross_lane_integration_sync_c012_expectations.md`
  - `scripts/check_m227_c012_typed_sema_to_lowering_cross_lane_integration_sync_contract.py`
  - `tests/tooling/test_check_m227_c012_typed_sema_to_lowering_cross_lane_integration_sync_contract.py`
  - `spec/planning/compiler/m227/m227_c012_typed_sema_to_lowering_cross_lane_integration_sync_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed docs/runbook fields:
   - `typed_docs_runbook_sync_consistent`
   - `typed_docs_runbook_sync_ready`
   - `typed_docs_runbook_sync_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed docs/runbook fields:
   - `typed_sema_docs_runbook_sync_consistent`
   - `typed_sema_docs_runbook_sync_ready`
   - `typed_sema_docs_runbook_sync_key`
3. Parse/lowering readiness fails closed when typed docs/runbook synchronization
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c013-typed-sema-to-lowering-docs-runbook-sync-contract`
  - `test:tooling:m227-c013-typed-sema-to-lowering-docs-runbook-sync-contract`
  - `check:objc3c:m227-c013-lane-c-readiness`

## Validation

- `python scripts/check_m227_c012_typed_sema_to_lowering_cross_lane_integration_sync_contract.py`
- `python scripts/check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c013_typed_sema_to_lowering_docs_runbook_sync_contract.py -q`
- `npm run check:objc3c:m227-c013-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C013/typed_sema_to_lowering_docs_runbook_sync_contract_summary.json`
