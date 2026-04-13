# objc3c Long-Horizon Operations

## Working Boundary

This runbook defines the compatibility-maintenance, migration, rollback, soak,
aging-regression, and support-window boundary for objc3c.

Use it when changing:

- release support windows, channel aging, and deprecation policy
- migration replay and rollback drills
- long-running soak or aging-regression evidence
- operator-facing compatibility and support claims
- package and canonical-application workflows that feed compatibility evidence

Canonical checked-in boundary surfaces:

- `tests/tooling/fixtures/long_horizon_operations/boundary_inventory.json`
- `docs/runbooks/objc3c_release_operations.md`
- `docs/runbooks/objc3c_distribution_credibility.md`
- `docs/runbooks/objc3c_performance_governance.md`
- `docs/runbooks/objc3c_package_ecosystem.md`
- `docs/runbooks/objc3c_application_architecture_testing.md`
- `scripts/build_objc3c_update_manifest.py`
- `scripts/publish_objc3c_release_operations_metadata.py`
- `scripts/check_objc3c_release_operations_integration.py`
- `scripts/check_objc3c_release_operations_end_to_end.py`
- `scripts/check_objc3c_package_ecosystem_integration.py`
- `scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py`

Replayable boundary inventory:

- `python scripts/build_long_horizon_operations_boundary_inventory_summary.py`

## Current Boundary

Long-horizon operations start after release, package, and canonical application
workflows are executable. This milestone does not invent a new package manager,
registry, release transport, or compiler runtime. It composes existing live
surfaces and adds durable maintenance evidence around them.

The current usable substrate is:

- release operations: update manifests, compatibility reports, release-channel
  catalogs, rollback guidance, and support windows
- package ecosystem: deterministic local package locks, package authoring, and
  offline mirror validation
- application architecture: project template and canonical application workspace
  replay through the public workflow runner
- performance governance: generated performance dashboards and budget evidence
- distribution credibility: release trust reports, provenance, and generated
  publication metadata

## Claim Boundary

Supported in this boundary:

- same-major compatibility maintenance tied to generated release metadata
- explicit deprecation and support-window policy
- migration replay drills over checked-in package and application surfaces
- rollback drills that consume generated update and release-operation metadata
- soak and aging evidence that can be replayed under `tmp/reports/`
- operator-visible support-window publication through the public workflow runner

Not supported in this boundary:

- evergreen or forever-compatible release claims
- cross-major compatibility without a generated migration proof
- hosted registry availability or network-backed dependency resolution
- background auto-update behavior
- manual waiver-only compatibility status
- soak evidence that cannot be regenerated from checked-in contracts

## Deprecation And Compatibility Maintenance Policy

The canonical policy contract is checked in at:

- `tests/tooling/fixtures/long_horizon_operations/deprecation_compatibility_policy.json`

Replay it with:

- `python scripts/build_long_horizon_operations_deprecation_policy_summary.py`

Compatibility maintenance is a support-window promise over generated release
metadata, not a forever-compatible language/runtime claim. Deprecations must:

- name the affected public surface, warning channel, support window, and
  successor behavior
- stay tied to existing release-operation warning classes from
  `tests/tooling/fixtures/release_operations/update_channel_policy.json`
- fail closed when a public claim uses forbidden release-operation phrases from
  `tests/tooling/fixtures/release_operations/compatibility_claim_policy.json`
- remain demoted until migration replay and rollback evidence exists for the
  affected package/application path

## Migration, Rollback, And Support Windows

The canonical semantics contract is checked in at:

- `tests/tooling/fixtures/long_horizon_operations/migration_rollback_support_window_semantics.json`

Replay it with:

- `python scripts/build_long_horizon_operations_migration_rollback_summary.py`

Migration and rollback are operator-visible behaviors. The replay path uses:

- `scripts/build_objc3c_update_manifest.py`
- `scripts/publish_objc3c_release_operations_metadata.py`

Generated evidence is valid only when the update manifest and compatibility
report agree on the current version, supported platform ids, support windows,
upgrade paths, and rollback guidance. Cross-major migration claims remain
blocked unless a generated long-horizon migration replay artifact names the
source version, target version, package lock, canonical application workspace,
and rollback target.

## Aging Regression And Release Cadence

The canonical criteria contract is checked in at:

- `tests/tooling/fixtures/long_horizon_operations/aging_regression_release_cadence_criteria.json`

Replay it with:

- `python scripts/build_long_horizon_operations_aging_cadence_summary.py`

A release cadence is supportable only when aging evidence is fresh enough to
trust and broad enough to cover the public claim. M328 consumes existing
performance-governance freshness budgets from
`tests/tooling/fixtures/performance_governance/budget_model.json` and
full-envelope soak inputs from
`tests/tooling/fixtures/full_envelope_claimability/soak_external_validation_contract.json`.

Cadence claims must block when:

- performance, compiler-throughput, or runtime-performance freshness exceeds a
  blocking budget
- conformance, stress, external-validation, public-conformance, package, or
  canonical-application evidence is missing
- soak evidence is hand-written or cannot be regenerated
- a release trains forward while rollback evidence is stale or absent

## Artifact Contract

The canonical long-horizon operations artifact contract is checked in at:

- `tests/tooling/fixtures/long_horizon_operations/artifact_contract.json`

Schema surface:

- `schemas/objc3c-long-horizon-operations-evidence-v1.schema.json`

Replay it with:

- `python scripts/build_long_horizon_operations_artifact_contract_summary.py`

Generated machine-owned outputs stay under:

- `tmp/artifacts/long-horizon-operations/`
- `tmp/reports/long-horizon-operations/`

No long-horizon claim is supportable unless it can be regenerated from the
checked-in policy contracts and validated through this artifact contract.

## Evidence Generation

The canonical evidence generator is:

- `python scripts/build_objc3c_long_horizon_operations_evidence.py`

It generates:

- `tmp/artifacts/long-horizon-operations/long-horizon-operations-evidence.json`
- `tmp/reports/long-horizon-operations/evidence-summary.json`

The generator replays the M328 policy summaries and live package,
canonical-application, performance-governance, conformance, stress,
external-validation, and public-conformance integration checks before writing
the artifact. Generated evidence is temporary output; the checked-in contracts
and scripts remain the source of truth.

## Successor Pressure

This milestone feeds production-readiness and governance closeout. Later
claims about release cadence, compatibility, or stability must consume generated
long-horizon evidence rather than manually restating release intent.
