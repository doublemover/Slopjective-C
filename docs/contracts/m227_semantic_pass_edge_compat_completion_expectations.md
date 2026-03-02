# M227 Semantic Pass Edge-Case and Compatibility Completion Expectations (A005)

Contract ID: `objc3c-sema-pass-edge-compat-completion/m227-a005-v1`
Status: Accepted
Scope: pass-flow compatibility-mode handoff semantics and migration-assist edge-case accounting.

## Objective

Complete edge-case compatibility handling for semantic pass-flow contracts so canonical vs legacy compatibility behavior and migration-assist semantics are deterministic and inspectable.

## Required Invariants

1. Pass-flow summary carries compatibility handoff fields:
   - `compatibility_mode`
   - `migration_assist_enabled`
   - `migration_legacy_literal_total`
   - `compatibility_handoff_consistent`
2. Sema pass manager seeds compatibility inputs into pass-flow summary from `Objc3SemaPassManagerInput`.
3. Pass-flow scaffold validates compatibility consistency and folds compatibility state into fingerprint/handoff-key generation.
4. Frontend artifacts project compatibility completion fields under `sema_pass_manager`:
   - `pass_flow_compatibility_mode`
   - `pass_flow_migration_assist_enabled`
   - `pass_flow_migration_legacy_literal_total`
   - `pass_flow_compatibility_handoff_consistent`

## Validation

- `python scripts/check_m227_a005_semantic_pass_edge_compat_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a005_semantic_pass_edge_compat_completion_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-A005/semantic_pass_edge_compat_completion_contract_summary.json`
