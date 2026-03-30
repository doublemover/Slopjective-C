# objc3c Release Foundation

## Scope

This runbook defines the checked-in release foundation for objc3c:

- canonical release artifact taxonomy
- distribution trust model
- audited payload and channel boundary
- reproducibility, provenance, and hermetic-toolchain policy
- SBOM, attestation, and source-stamping publication flow

This milestone does not introduce installers, operating-system package managers,
or auto-updating channels. Those belong to later distribution milestones.

## Canonical Release Shape

The canonical release payload is the existing staged runnable toolchain bundle
produced by `scripts/package_objc3c_runnable_toolchain.ps1`.

The release surface is composed of:

- the staged runnable package root under `tmp/pkg/`
- the runnable package manifest emitted inside that package root
- the checked-in repo-superclean source-of-truth artifact emitted by
  `scripts/build_objc3c_native.ps1`
- the release-evidence index produced by `scripts/check_release_evidence.py`
- the machine-generated release manifest, SBOM, and attestation artifacts

Do not create a second release payload layout or a sidecar installer-shaped
bundle in this milestone.

## Trust Boundary

The trusted release boundary is repo-relative and machine-generated:

- build and packaging scripts in `scripts/`
- checked-in contracts under `tests/tooling/fixtures/release_foundation/`
- schemas under `schemas/`
- machine-owned outputs under `tmp/reports/release-foundation/`,
  `tmp/artifacts/release-foundation/`, and `tmp/pkg/`

No claim may depend on hand-edited release notes, spreadsheet-only digests, or
manual checksum tables.

## Reproducibility Boundary

Reproducibility for this milestone means:

- the runnable package can be staged repeatedly from the same repo state
- the selected release payload produces the same sorted file-digest set across
  repeated package assembly runs
- provenance publication is derived from the package manifest and checked-in
  contracts, not from hand-maintained metadata

This is a local reproducible-assembly contract, not a cross-platform binary
reproducibility claim.

## Provenance Publication

Release provenance publication must emit:

- a release manifest with the selected payload entries and normalized digests
- an SBOM-style component inventory over the selected payload
- an attestation document that binds the release manifest, SBOM, package
  manifest, repo-superclean source-of-truth artifact, and release-evidence
  index together

These artifacts must remain machine-owned outputs.

## Required Surfaces

Checked-in contract roots:

- `tests/tooling/fixtures/release_foundation/artifact_taxonomy.json`
- `tests/tooling/fixtures/release_foundation/distribution_trust_model.json`
- `tests/tooling/fixtures/release_foundation/distribution_audit.json`
- `tests/tooling/fixtures/release_foundation/reproducibility_policy.json`
- `tests/tooling/fixtures/release_foundation/release_payload_policy.json`
- `tests/tooling/fixtures/release_foundation/provenance_policy.json`
- `tests/tooling/fixtures/release_foundation/source_surface.json`
- `tests/tooling/fixtures/release_foundation/schema_surface.json`
- `tests/tooling/fixtures/release_foundation/workflow_surface.json`

Machine-owned outputs:

- `tmp/reports/release-foundation/`
- `tmp/artifacts/release-foundation/`

## Non-Goals

- no installer UX
- no package-manager metadata
- no code-signing or notarization claim
- no hosted artifact registry requirement
- no second packaging flow outside the runnable toolchain bundle
