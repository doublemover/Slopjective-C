# objc3c Packaging Channels

## Scope

This runbook defines the checked-in packaging-channel surface for objc3c:

- portable archive packaging over the canonical runnable toolchain bundle
- local installer image generation and environment bootstrap scripts
- offline air-gapped bundle assembly from machine-owned release artifacts
- install smoke, rollback smoke, and channel metadata publication

This milestone does not add a system package manager, hosted update service, or
platform notarization claim. Installer trust is represented by the
machine-owned local installer digest signature in the package-channel manifest.

## Channel Architecture

The packaging-channel surface layers distribution channels on top of the release-foundation surface.
The canonical payload remains the staged runnable toolchain bundle produced by
`npm run objc3c -- package-runnable-toolchain` and described by the machine-
owned release manifest, SBOM, and attestation artifacts.

Packaging-channel commands route through `npm run objc3c -- <action>`; helper
implementations are action-registry anchors only.

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

The supported packaging-channel platform surface is intentionally narrow and
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

Packaging-channel non-goals:

- no Homebrew, apt, winget, Chocolatey, Scoop, or MSI publication claim
- no daemonized updater
- no OS notarization or external certificate-signing claim
- no cross-platform parity claim beyond the checked-in `windows-x64` surface

## Trust and Ownership Boundary

The trusted packaging-channel boundary is:

- checked-in contracts under `tests/tooling/fixtures/packaging_channels/`
- action-catalog-owned generators
- checked-in schemas under `schemas/`
- machine-owned outputs under `tmp/artifacts/package-channels/`,
  `tmp/reports/package-channels/`, and `tmp/pkg/`

No packaging-channel claim may depend on ad hoc zip assembly, a checked-in
installer manifest rewrite, or an external registry snapshot.

## Installer Behavior Policy

Installer and bootstrap flows in this packaging-channel surface must follow these rules:

- installation is local-root only and must not claim a system-wide install
- install, bootstrap, and rollback logic must be machine-generated from the
  runnable package manifest and release-foundation artifacts
- installer receipts must be written as machine-owned JSON
- rollback must be explicit and deterministic over the install receipt
- offline bootstrap must not fetch tools, packages, or metadata from the
  network
- archive and installer channels must preserve the same payload digest set as
  the canonical runnable package
- the local installer archive must publish an `objc3c-local-sha256-v1`
  signature payload whose artifact path, digest, and public verification command
  are copied into release-update metadata

Compatibility rules:

- only the checked-in `windows-x64` channel matrix is supported
- installer scripts may assume `pwsh` and local filesystem access
- installer validation must prove install, bootstrap, and rollback under a
  temp-owned root
- archive compatibility claims must remain tied to the same `windows-x64`
  runnable payload family; publishing a package does not imply cross-host reuse

## Workflow Surface

The live packaging-channel workflow must expose:

- a source-surface check
- a schema-surface check
- a package-channel build command
- an integrated package-channel validation command
- an end-to-end install and rollback validation command

These workflow entrypoints must remain on the shared public runner and machine-
owned reports under `tmp/reports/package-channels/`.

Current public platform-support entrypoints layered onto this surface:

- `npm run objc3c -- build-platform-support-matrix`
- `npm run objc3c -- validate-platform-hardening`
- `npm run objc3c -- validate-platform-hardening-end-to-end`

The package-channel manifest and summary must publish the same support-tier
boundary as the machine-owned platform support matrix.

The package-channel manifest and summary must also publish
`installer_signature`. The end-to-end package-channel validator recomputes the
installer archive digest and fails closed when the signature payload, artifact
path, or public verification command drifts.

The package-channel manifest must publish `payload_contract` and
`receipt_contracts` from the checked-in packaging-channel metadata surface. The
payload contract binds the runnable toolchain manifest path, manifest digest,
required compiler/runtime/stdlib/docs entries, and per-entry SHA-256 digests.
The receipt contracts bind the local-installer and offline-bundle receipts to
the channel id, bootstrap entrypoint, package bridge, runnable package manifest,
manifest digest, required payload entries, rollback requirement, and no-network
offline policy.

Package-channel builders must initialize fresh owner-controlled roots under
`tmp/pkg/objc3c-package-channels/` and `tmp/artifacts/package-channels/` before
staging channel outputs. A pre-existing `tmp/` artifact may be removed as owned
scratch, but it must never be treated as source evidence for a package,
installer, receipt, or archive digest claim.

The package-channel manifest also publishes the package-ecosystem interop loader
summary from `tests/tooling/fixtures/package_ecosystem/mixed_image_interop_loader_metadata.json`.
That keeps C and Objective-C header import/export support tied to the same
fixture-backed header import/export counts, ABI alignment counts, foreign type
counts, mixed-image package ids, negative diagnostics, and fail-closed tamper
diagnostic used by the lock and mirror workflows.
