# objc3c Packaging Channels

## Scope

This runbook defines the checked-in packaging-channel surface for objc3c:

- portable archive packaging over the canonical runnable toolchain bundle
- local installer image generation and environment bootstrap scripts
- offline air-gapped bundle assembly from machine-owned release artifacts
- install smoke, rollback smoke, and channel metadata publication

This milestone does not add a system package manager, hosted update service, or
platform notarization/signing claim.

## Channel Architecture

The packaging-channel surface layers distribution channels on top of the release-foundation surface.
The canonical payload remains the staged runnable toolchain bundle produced by
`scripts/package_objc3c_runnable_toolchain.ps1` and described by the machine-
owned release manifest, SBOM, and attestation artifacts.

Packaging channels are derived views over that payload:

- `portable-archive`: a direct archive of the runnable toolchain package root
- `local-installer`: an installer image that stages the runnable payload under a
  chosen install root and emits a machine-owned install receipt
- `offline-bundle`: an air-gapped bootstrap bundle that contains the portable
  archive, installer image, and release-foundation evidence needed to install
  without network access

Do not introduce a second compiler payload or a hand-curated install tree that
can drift from the runnable package.

## Supported Platforms

The supported platform surface for this milestone is intentionally narrow and
tiered:

- `Tier 1`
  - `windows-x64`
- `Tier 2`
  - none published
- `Experimental`
  - none published

Supported channel matrix for `windows-x64`:

- portable archive
- local installer image
- offline air-gapped bootstrap bundle

Non-goals for this milestone:

- no Homebrew, apt, winget, Chocolatey, Scoop, or MSI publication claim
- no daemonized updater
- no signed/notarized installer claim
- no cross-platform parity claim beyond the checked-in `windows-x64` surface

## Trust and Ownership Boundary

The trusted packaging-channel boundary is:

- checked-in contracts under `tests/tooling/fixtures/packaging_channels/`
- checked-in generators under `scripts/`
- checked-in schemas under `schemas/`
- machine-owned outputs under `tmp/artifacts/package-channels/`,
  `tmp/reports/package-channels/`, and `tmp/pkg/`

No packaging-channel claim may depend on manual zip assembly, a hand-edited
installer manifest, or an external registry snapshot.

## Installer Behavior Policy

Installer and bootstrap flows in this milestone must follow these rules:

- installation is local-root only and must not claim a system-wide install
- install, bootstrap, and rollback logic must be machine-generated from the
  runnable package manifest and release-foundation artifacts
- installer receipts must be written as machine-owned JSON
- rollback must be explicit and deterministic over the install receipt
- offline bootstrap must not fetch tools, packages, or metadata from the
  network
- archive and installer channels must preserve the same payload digest set as
  the canonical runnable package

Compatibility rules:

- only the checked-in `windows-x64` channel matrix is supported
- installer scripts may assume `pwsh` and local filesystem access
- installer validation must prove install, bootstrap, and rollback under a
  temp-owned root

## Workflow Surface

The live packaging-channel workflow for this milestone must expose:

- a source-surface check
- a schema-surface check
- a package-channel build command
- an integrated package-channel validation command
- an end-to-end install and rollback validation command

These workflow entrypoints must remain on the shared public runner and machine-
owned reports under `tmp/reports/package-channels/`.
