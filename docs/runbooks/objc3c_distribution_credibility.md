# objc3c Distribution Credibility

## Scope

This runbook defines the checked-in distribution-credibility surface for objc3c:

- release-operation trust signals derived from the live shipped artifacts
- operator-facing release drill and recovery expectations
- machine-owned dashboards and trust reports summarizing publish readiness
- credibility claims that terminate in existing release-foundation, packaging-channel,
  release-operations, and release-evidence outputs

This milestone does not add a second release pipeline, a hosted trust service, or
manual release-status bookkeeping outside the checked-in public workflow surface.

## Architecture

Distribution credibility is a derived reporting layer over existing release work.
The canonical upstream surfaces are:

- `M297` release-foundation manifests, SBOMs, and provenance attestations
- `M298` packaging-channel payloads, install receipts, and rollback proofs
- `M299` update manifests, compatibility reports, and rollback guidance
- the existing release-evidence index from `scripts/check_release_evidence.py`

No credibility claim may bypass those live outputs. If a trust signal cannot be
derived from a checked-in contract and executable artifact, it is out of scope.

## Trust Signals

The machine-owned trust story for this milestone is limited to:

- release payload provenance and reproducibility
- install and rollback smoke over the packaged channels
- update-manifest and compatibility publication coherence
- release-evidence gate coverage over the published conformance artifacts
- explicit recovery and operator drill guidance for the live package surfaces

Trust signals are additive summaries, not a new source of truth. The canonical
artifact lineage remains the shipped runnable package, its package channels, and
their attached release-operation metadata.

## Install Docs And Trust Report Inputs

The user-facing install and release-document inputs for this milestone are:

- `README.md` for the top-level product description
- `docs/tutorials/getting_started.md` for first-run operator expectations
- `docs/tutorials/build_run_verify.md` for build and validation expectations
- `docs/runbooks/objc3c_packaging_channels.md` for installable channel behavior
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
- integrated and end-to-end validation entrypoints on the shared public workflow runner

## Operator Release Policy

Credibility publication is operator-gated:

- `ready`: all required upstream trust signals passed on the live release surface
- `degraded`: one or more non-fatal trust signals regressed and require explicit caution
- `blocked`: a release drill, install smoke, rollback proof, or release-operation proof failed

Incidents for this milestone are limited to:

- install failure on a published package channel
- rollback failure on the live installer or offline bundle path
- trust-report drift against the published release-operation metadata
- missing or invalid release-evidence index for the shipped release payload

Do not publish a trust-positive summary when the state is `blocked`.

## Release Drill And Adoption Smoke

This milestone uses the existing package and metadata surfaces for release drills:

- package-channel install and rollback smoke from `M298`
- update-manifest and compatibility publication from `M299`
- release-evidence index generation from the existing evidence gate

The drill model is intentionally narrow:

- stage the live packaged channels under a temp-owned root
- verify install, metadata publication, and rollback coherence
- summarize the result as a machine-owned trust signal set
- require a reproducibility audit over the released payload metadata before claiming `ready`

No drill may depend on manual screenshots, hand-edited operator notes, or a second
package assembly path.

## Non-Goals

- no hosted status page
- no new update service or release transport
- no hand-written trust badges or manually edited release summaries
- no package-manager-specific release-credibility path
