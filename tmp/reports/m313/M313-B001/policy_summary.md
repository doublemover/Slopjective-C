# M313-B001 Validation Consolidation Policy Summary

- policy_id: `validation-consolidation-policy-v1`
- inventory_issue: `M313-A001`
- package_scripts_total: `144`
- check_py_files: `82`
- retained_static_guard_count: `19`
- executable_validation_count: `63`

## Canonical truth order
- `shared_acceptance_harness`
- `public_workflow_validate_actions`
- `integration_validators`
- `runnable_end_to_end_validators`
- `retained_static_policy_guards`

## Retained static guard classes
- `retain:task-hygiene`
- `retain:repo-shape`
- `retain:docs-surface`
- `retain:product-surface`
- `retain:source-surface-contract`
- `retain:schema-contract`

## Legacy surface lifecycle states
- `active`
- `migration-only`
- `archival`
- `prohibited`

## Unreferenced check surfaces queued for M313-B003
- `scripts/check_activation_triggers.py`
- `scripts/check_bootstrap_readiness.py`
- `scripts/check_conformance_corpus_surface.py`
- `scripts/check_getting_started_surface.py`
- `scripts/check_objc3c_end_to_end_determinism.py`
- `scripts/check_objc3c_library_cli_parity.py`
- `scripts/check_open_blocker_audit_contract.py`

Next issues: `M313-B002`, `M313-B003`, `M313-C001`
