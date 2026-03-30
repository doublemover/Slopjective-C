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

## Publication Surface

This milestone must leave behind:

- a checked-in source-surface contract
- checked-in policy contracts for operator release behavior and incident handling
- checked-in schema and artifact-surface contracts for dashboard/report outputs
- a machine-owned dashboard summary under `tmp/reports/distribution-credibility/`
- a machine-owned trust report under `tmp/artifacts/distribution-credibility/`
- integrated and end-to-end validation entrypoints on the shared public workflow runner

## Non-Goals

- no hosted status page
- no new update service or release transport
- no hand-written trust badges or manually edited release summaries
- no package-manager-specific release-credibility path
