# objc3c Distribution Credibility

## Scope

This runbook defines the checked-in distribution-credibility surface for objc3c:

- release-operation trust signals derived from the live shipped artifacts
- clean local package install credibility generated from the package ecosystem root
- operator-facing release drill and recovery expectations
- machine-owned dashboards and trust reports summarizing publish readiness
- credibility claims that terminate in existing release-foundation, packaging-channel,
  package-ecosystem, release-operations, and release-evidence outputs

This distribution-credibility surface does not add a second release pipeline, a
hosted trust service, or release-status bookkeeping outside the checked-in public
workflow surface.

## Architecture

Distribution credibility is a derived reporting layer over existing release work.
The canonical upstream surfaces are:

- release-foundation manifests, SBOMs, and provenance attestations
- packaging-channel payloads, install receipts, and rollback proofs
- package-ecosystem clean install evidence from
  `npm run objc3c -- validate-package-install-distribution --from-nothing`
- release-operations update manifests, support-window reports, and rollback guidance
- the existing release-evidence index from `npm run objc3c -- check-release-evidence`

Helper implementations are action-registry anchors, not a
second credibility command surface.

No credibility claim may bypass those live outputs. If a trust signal cannot be
derived from a checked-in contract and executable artifact, it is out of scope.

## Trust Signals

The machine-owned distribution trust story is limited to:

- release payload provenance and reproducibility
- install and rollback smoke over the packaged channels
- clean local package install evidence with no network access and hosted registry
  support explicitly fail-closed
- a package-ecosystem `from_nothing_probe` proving that
  `tmp/artifacts/package-ecosystem` and `tmp/reports/package-ecosystem` were
  removed before the install evidence was regenerated
- update-manifest and support-window publication coherence
- release-evidence gate coverage over the published conformance artifacts
- explicit recovery and operator drill guidance for the live package surfaces

Trust signals are additive summaries, not a new owner surface. The canonical
artifact lineage remains the shipped runnable package, its package channels, and
their attached release-operation metadata.

## Install Docs And Trust Report Inputs

The user-facing install and release-document inputs for distribution credibility are:

- `README.md` for the top-level product description
- `docs/tutorials/getting_started.md` for first-run operator expectations
- `docs/tutorials/build_run_verify.md` for build and validation expectations
- `docs/runbooks/objc3c_packaging_channels.md` for installable channel behavior
- `docs/runbooks/objc3c_package_ecosystem.md` for local package lock, mirror,
  registry, and clean-install behavior
- `docs/runbooks/objc3c_release_operations.md` for versioning, update, and rollback metadata

The trust report must be derived from those checked-in docs plus the live release
artifacts. It must not become a separate narrative program that drifts away from
the shipped install and recovery surfaces.

## Publication Surface

This milestone must leave behind:

- a checked-in source-surface contract
- checked-in policy contracts for operator release behavior and incident handling
- checked-in schema and artifact-surface contracts for dashboard/report outputs
- a machine-owned dashboard summary under `tmp/reports/distribution-credibility/`
- a machine-owned trust report under `tmp/artifacts/distribution-credibility/`
- integrated and end-to-end validation entrypoints on the shared `npm run objc3c -- <action>` bridge

The canonical publication artifacts are:

- a machine-owned dashboard summary JSON
- a machine-owned trust report JSON
- a human-readable markdown trust report derived from the JSON publication

These outputs must live under `tmp/reports/distribution-credibility/` and
`tmp/artifacts/distribution-credibility/` and stay validated by checked-in schema
contracts.

The checked-in artifact surface is exhaustive for this owner. Source-surface,
schema-surface, dashboard, publication, integration, and end-to-end summaries
all resolve under `tmp/reports/distribution-credibility/`; the copied dashboard
artifact and trust report outputs resolve under `tmp/artifacts/distribution-credibility/`.
The publication script is only an entrypoint. Report construction, evidence path
selection, markdown rendering, and artifact publication live in the distribution
credibility owner package.

## Operator Release Policy

Credibility publication is operator-gated:

- `ready`: all required upstream trust signals passed on the live release surface
- `degraded`: one or more non-fatal trust signals regressed and require explicit caution
- `blocked`: a release drill, install smoke, clean package install, rollback proof,
  or release-operation proof failed

Incidents for distribution credibility are limited to:

- install failure on a published package channel
- clean package install failure, stale install root dependence, or hosted-registry
  claim widening in the package ecosystem evidence
- rollback failure on the live installer or offline bundle path
- trust-report drift against the published release-operation metadata
- missing or invalid release-evidence index for the shipped release payload

Do not publish a trust-positive summary when the state is `blocked`.

## Release Drill And Adoption Smoke

This milestone uses the existing package and metadata surfaces for release drills:

- package-channel install and rollback smoke from the packaging-channel surface
- clean package install validation from the package-ecosystem surface
- update-manifest and support-window publication from the release-operations surface
- release-evidence index generation from the existing evidence gate

The drill model is intentionally narrow:

- stage the live packaged channels under a temp-owned root
- verify install, clean package install, metadata publication, and rollback coherence
- summarize the result as a machine-owned trust signal set
- require a reproducibility audit over the released payload metadata before claiming `ready`

No drill may depend on ad hoc screenshots, operator-maintained notes, or a second
package assembly path.

## Workflow Surface

The live distribution-credibility workflow must expose:

- a source-surface check
- a schema-surface check
- a dashboard build command
- a trust-report publication command
- an integrated distribution-credibility validation command
- an end-to-end distribution-credibility validation command

These entrypoints must stay on the shared `npm run objc3c -- <action>` bridge and reuse the
existing release-foundation, packaging-channel, package-ecosystem,
release-operations, and release-evidence surfaces instead of inventing a
milestone-only drill lane.

The integrated workflow steps are fixed:

- `validate-release-operations`
- `validate-package-install-distribution`
- `check-distribution-credibility-surface`
- `check-distribution-credibility-schema-surface`
- `build-distribution-credibility-dashboard`
- `publish-distribution-credibility`

Integration and end-to-end summaries must prove that the dashboard artifact,
trust report JSON, markdown report, trust signals, release drill steps, and
operator actions all match the checked-in contract surfaces. They must also fail
closed when the package-install summary was generated without the from-nothing
probe; preexisting `tmp` package-ecosystem artifacts are replay outputs only, not
distribution-credibility source truth.

## Non-Goals

- no hosted status page
- no new update service or release transport
- no operator-maintained trust badges or release summaries
- no package-manager-specific release-credibility path
