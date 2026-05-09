# objc3c Long-Horizon Operations

## Working Boundary

This runbook defines release support-window maintenance, canonical conversion,
revert, soak, aging-regression, and support-window boundaries for objc3c.

Use it when changing:

- release support windows, channel aging, and deprecation policy
- conversion replay and revert drills
- long-running soak or aging-regression evidence
- operator-facing support-window claims
- package and canonical-application workflows that feed support evidence

Canonical checked-in boundary surfaces:

- `tests/tooling/fixtures/long_horizon_operations/boundary_inventory.json`
- `docs/runbooks/objc3c_release_operations.md`
- `docs/runbooks/objc3c_distribution_credibility.md`
- `docs/runbooks/objc3c_performance_governance.md`
- `docs/runbooks/objc3c_package_ecosystem.md`
- `docs/runbooks/objc3c_application_architecture_testing.md`
- package bridge: `npm run objc3c -- <action>`
- action catalog: checked-in workflow action catalog

Replayable boundary inventory:

- `npm run objc3c -- validate-long-horizon-operations`

Helper implementations behind the workflow actions are not separate operator
commands.

## Current Boundary

Long-horizon operations start after release, package, and canonical application
workflows are executable. This milestone does not invent a new package manager,
registry, release transport, or compiler runtime. It composes existing live
surfaces and adds durable maintenance evidence around them.

The current usable substrate is:

- release operations: update manifests, support-window reports, release-channel
  catalogs, revert guidance, and support windows
- package ecosystem: deterministic local package locks, package authoring, and
  offline mirror validation
- application architecture: project template and canonical application workspace
  replay through the `npm run objc3c -- <action>` bridge
- performance governance: generated performance dashboards and budget evidence
- distribution credibility: release trust reports, provenance, and generated
  publication metadata

## Claim Boundary

Supported in this boundary:

- same-major support-window maintenance tied to generated release metadata
- explicit deprecation and support-window policy
- conversion replay drills over checked-in package and application surfaces
- revert drills that consume generated update and release-operation metadata
- soak and aging evidence that can be replayed under `tmp/reports/`
- operator-visible support-window publication through the `npm run objc3c -- <action>` bridge

Not supported in this boundary:

- evergreen support or forever-supported release claims
- cross-major support without a generated conversion proof
- hosted registry availability or network-backed dependency resolution
- background auto-update behavior
- manual waiver-only support status
- soak evidence that cannot be regenerated from checked-in contracts

## Deprecation And Support-Window Maintenance Policy

The canonical deprecation support policy contract is checked in at this
historically named path:

- `tests/tooling/fixtures/long_horizon_operations/deprecation_compatibility_policy.json`

The file path is retained as a checked-in fixture address only; the live
contract ID and fields own support-window truth and do not publish compatibility
support.

Replay it with:

- `npm run objc3c -- validate-long-horizon-operations`

Support-window maintenance is a generated release metadata promise, not a
forever-supported language/runtime claim. Deprecations must:

- name the affected public surface, warning channel, support window, and
  successor behavior
- stay tied to existing release-operation warning classes from
  `tests/tooling/fixtures/release_operations/update_channel_policy.json`
- fail closed when a public claim uses forbidden upgrade/support phrases from
  `tests/tooling/fixtures/release_operations/compatibility_claim_policy.json`
- remain demoted until conversion replay and revert evidence exists for the
  affected package/application path

## Conversion, Revert, And Support Windows

The canonical conversion/revert semantics contract is checked in at this
historically named path:

- `tests/tooling/fixtures/long_horizon_operations/migration_rollback_support_window_semantics.json`

The filename is historical. The live contract fields use conversion replay and
revert readiness terminology, and they reject compatibility or fallback support
claims.

Replay it with:

- `npm run objc3c -- validate-long-horizon-operations`

Conversion and revert are operator-visible behaviors. The replay path uses:

- `npm run objc3c -- build-update-manifest`
- `npm run objc3c -- publish-release-operations`

Generated evidence is valid only when the update manifest and support-window
report agree on the current version, supported platform ids, support windows,
upgrade paths, and revert guidance. Cross-major conversion claims remain
blocked unless a generated long-horizon conversion replay artifact names the
source version, target version, package lock, canonical application workspace,
and revert target.

## Aging Regression And Release Cadence

The canonical criteria contract is checked in at:

- `tests/tooling/fixtures/long_horizon_operations/aging_regression_release_cadence_criteria.json`

Replay it with:

- `npm run objc3c -- validate-long-horizon-operations`

A release cadence is supportable only when aging evidence is fresh enough to
trust and broad enough to cover the public claim. This boundary consumes existing
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
- a release trains forward while revert evidence is stale or absent

## Artifact Contract

The canonical long-horizon operations artifact contract is checked in at:

- `tests/tooling/fixtures/long_horizon_operations/artifact_contract.json`

Schema surface:

- `schemas/objc3c-long-horizon-operations-evidence-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

Replay it with:

- `npm run objc3c -- validate-long-horizon-operations`

Generated machine-owned outputs stay under:

- `tmp/artifacts/long-horizon-operations/`
- `tmp/reports/long-horizon-operations/`

No long-horizon claim is supportable unless it can be regenerated from the
checked-in policy contracts and validated through this artifact contract.

## Evidence Generation

The canonical evidence generator is:

- `npm run objc3c -- validate-long-horizon-operations`

It generates:

- `tmp/artifacts/long-horizon-operations/long-horizon-operations-evidence.json`
- `tmp/reports/long-horizon-operations/evidence-summary.json`

The generator replays the long-horizon policy summaries and live package,
canonical-application, performance-governance, conformance, stress,
external-validation, and public-conformance integration checks before writing
the artifact. Generated evidence is temporary output; the checked-in contracts
and scripts remain the source of truth.

## Public Workflow Integration

The repo-scope long-horizon workflow is:

- `npm run objc3c -- validate-long-horizon-operations`
- `npm run objc3c -- publish-long-horizon-operations`

It maps to:

- `npm run objc3c -- validate-long-horizon-operations`
- `npm run objc3c -- publish-long-horizon-operations`

The public workflow validates the generated evidence artifact shape, claim
audit, conversion evidence, revert channel coverage, and soak evidence family
coverage.

Support-window publication emits:

- `tmp/artifacts/long-horizon-operations/support-window-publication.json`
- `tmp/reports/long-horizon-operations/publication-summary.json`

## Closeout Gate

The closeout gate is:

- `npm run objc3c -- validate-long-horizon-operations`

It replays all long-horizon summaries, integration, support-window publication, public
command rendering, documentation/repository surface checks, and source hygiene.
The gate rejects widened support-window claims, missing public runner actions,
missing operator publication metadata, stale package manifest fields, and any
claim audit release blocker.

## Successor Pressure

This milestone feeds production-readiness and governance closeout. Later
claims about release cadence, support windows, or stability must consume generated
long-horizon evidence rather than manually restating release intent.
