# Validation Consolidation Policy

Policy id: `validation-consolidation-policy-v1`
Effective from: `M313`

## Purpose
Define one acceptance-first validation model for the repo, limit retained static guards to the few classes that still add unique value, and give every legacy validation surface a lifecycle state instead of letting it masquerade as primary truth.

## Canonical truth order
- `shared_acceptance_harness`
  - Reusable subsystem acceptance suites and compile/runtime drill entrypoints such as `scripts/shared_compiler_runtime_acceptance_harness.py`.
- `public_workflow_validate_actions`
  - Named `validate-*` and `test-*` actions composed by `scripts/objc3c_public_workflow_runner.py` and exposed through `package.json`.
- `integration_validators`
  - Live in-repo executable proofs such as `scripts/check_objc3c_*_integration.py`.
- `runnable_end_to_end_validators`
  - Staged or packaged executable proofs such as `scripts/check_objc3c_runnable_*_end_to_end.py` and other explicit e2e flows.
- `retained_static_policy_guards`
  - Narrow static checks retained only for repo-shape, docs, product layout, source-surface, schema, and task-hygiene contracts.

## Retained static guard classes
- `retain:task-hygiene`
- `retain:repo-shape`
- `retain:docs-surface`
- `retain:product-surface`
- `retain:source-surface-contract`
- `retain:schema-contract`

## Legacy surface lifecycle
- `active`
  - canonical validation path; named by a shared harness or public workflow action
- `migration-only`
  - still runnable, but only as a bridge to a shared acceptance-first successor
- `archival`
  - historical replay/reference only; not part of default CI or public docs
- `prohibited`
  - must not return as current truth; closeout gates should fail if it does

## Implementation rules
- Extend `scripts/objc3c_public_workflow_runner.py` instead of adding milestone-local wrapper commands.
- Extend `scripts/shared_compiler_runtime_acceptance_harness.py` or another shared executable harness instead of creating one-off validators.
- Keep retained static guards narrow; they do not prove runtime or integration behavior.
- If an executable validator remains necessary, wire it through a public `validate-*` or `test-*` action.
- Quote validation counts only from replayable snapshots such as `tmp/reports/m313/M313-A001/validation_surface_inventory.json`.

## Immediate follow-on work
- `M313-B002`: consolidate subsystem execution into shared harnesses and public workflow composition
- `M313-B003`: classify legacy `check_*` surfaces into `active`, `migration-only`, `archival`, or `prohibited`
- `M313-C001`: define one replayable artifact schema for acceptance evidence, migration state, and retained-guard reporting