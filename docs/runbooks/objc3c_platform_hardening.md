# objc3c Platform Hardening Boundary

## Working Boundary

This runbook defines the live platform, toolchain, packaging, install, and
support claim surface for objc3c.

Use it when changing host support claims, toolchain version expectations,
packaged install behavior, or archive/update support rules.

Downstream platform work must stay on the existing implementation paths below
instead of inventing a second portability harness or publishing broader support
claims than the packaged evidence can prove.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/platform_hardening/boundary_inventory.json`
- `tests/tooling/fixtures/packaging_channels/supported_platforms.json`
- `tests/tooling/fixtures/packaging_channels/installer_policy.json`
- release operations upgrade-claim policy:
  `tests/tooling/fixtures/release_operations/upgrade_support_claim_policy.json`
  (live contract fields are upgrade/support-scoped)

Replayable public workflow actions:

- `npm run objc3c -- build-platform-support-matrix`
- `npm run objc3c -- validate-platform-hardening`
- `npm run objc3c -- validate-platform-hardening-end-to-end`
- `npm run objc3c -- build-package-channels`
- `npm run objc3c -- validate-packaging-channels`
- `npm run objc3c -- validate-packaging-channels-end-to-end`
- `npm run objc3c -- build-update-manifest`
- `npm run objc3c -- publish-release-operations`
- `npm run objc3c -- validate-release-operations`
- `npm run objc3c -- validate-release-operations-end-to-end`

Helper implementations are registry anchors for the workflow
actions above, not separate current-facing commands.

## Exact Live Implementation Paths

- native build and toolchain probing:
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- inspect-capability-explorer`
- runnable package assembly:
  - `npm run objc3c -- package-runnable-toolchain`
  - `npm run objc3c -- build-release-manifest`
- package-channel and installer flow:
  - `npm run objc3c -- build-package-channels`
  - `npm run objc3c -- validate-packaging-channels`
  - `npm run objc3c -- validate-packaging-channels-end-to-end`
- release/update support flow:
  - `npm run objc3c -- build-update-manifest`
  - `npm run objc3c -- publish-release-operations`
  - `npm run objc3c -- validate-release-operations`
  - `npm run objc3c -- validate-release-operations-end-to-end`
- public command and workflow surface:
  - package bridge: `npm run objc3c -- <action>`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`

## Current Support Matrix

The current checked-in support matrix is intentionally narrow.

- `Tier 1`:
  - `windows-x64`
  - public package/install channels:
    - `portable-archive`
    - `local-installer`
    - `offline-bundle`
  - required local tools:
    - `pwsh`
    - `python`
    - `clang++`
- `Tier 2`:
  - none published
- `Experimental`:
  - none published
- `Unsupported`:
  - every non-`windows-x64` host shape
  - every package-manager or system-install claim outside the checked-in
    portable archive, local installer, and offline bundle
  - every signed/notarized installer or cross-platform parity claim

The supported host/toolchain matrix is narrow but real, verified, tiered, and
rooted in the same package and release workflows users run.

## Host And Toolchain Claim Boundary

Current support claims must stay narrower than the evidence:

- only the checked-in `windows-x64` host family is supported
- only the checked-in archive/install channel set is supported
- no cross-platform parity claim exists today
- no system package manager publication claim exists today
- no notarization or signed-installer claim exists today

Toolchain claims must also stay narrow:

- `clang++`, `python`, and `pwsh` presence are part of the live support surface
- unsupported hosts and unsupported toolchain shapes must fail closed with
  explicit diagnostics
- packaged install behavior, archive behavior, and update/revert publication
  must all agree on the same support boundary

## Platform Support Tier Policy

Platform support must be expressed with explicit tiers instead of flat
supported/unsupported shorthand.

- `Tier 1`:
  - checked-in, replayed, and package/install validated on the canonical public
    workflow path
  - currently only `windows-x64`
- `Tier 2`:
  - reserved for future hosts that prove replayable compile, package, install,
    and update coverage but are not yet the default operator recommendation
  - no current entries
- `Experimental`:
  - reserved for host or toolchain shapes that may gain limited probes later
    without becoming installable support claims
  - no current entries
- `Unsupported`:
  - any host, channel, or toolchain shape outside the checked-in matrix

Tier publication rules:

- no tier may be published without machine-owned support-matrix evidence
- package/install/update claims must agree with the same tier assignment
- `windows-x64` is the only host family that may currently be described as
  supported on the public workflow surface

## Install And Archive Support Boundary

Packaging and install support is part of platform support, not a separate
story.

- the canonical payload remains the runnable toolchain package
- package channels are transport views over that payload
- install receipts, bootstrap scripts, revert guidance, and update metadata must all
  resolve back to the same packaged payload family
- archive and installer support claims remain `windows-x64` only until
  another host is proved on the same public workflow surface

## Toolchain-Range And Archive Support Policy

Toolchain-range and archive support claims must also stay narrower than
the evidence.

- the current live claim boundary is the checked-in `windows-x64` package and
  install surface produced from the local runnable toolchain bundle
- packaged archive reuse, installer replay, revert guidance, and update publication
  are only claimable for the same checked-in host family
- toolchain presence by itself does not imply archive or install support
- a new LLVM or Clang major line is not automatically supported just because the
  current host can launch it

Archive support rules:

- portable archive, installer archive, and offline bundle must all resolve to
  the same runnable payload family
- package-channel publication and release-operations metadata must describe the
  same host and channel boundary
- archive support claims remain fail-closed outside the checked-in
  `windows-x64` package/install/update path

## Explicit Unsupported-Host Behavior

Unsupported hosts and unsupported toolchain shapes must not degrade into vague
best-effort language.

- unsupported host claims stay fail-closed
- unsupported channel claims stay fail-closed
- unsupported toolchain-range claims stay fail-closed
- widening support later must happen by expanding checked-in contracts,
  generated matrix artifacts, and public workflow validation

## Unsupported-Host Fail-Closed Policy

Unsupported-host behavior must be deterministic and machine-describable.

Hard-fail classes:

- host OS or host architecture outside the checked-in support matrix
- missing required local tools for the claimed host tier
- installer or package-channel invocation outside the published host/channel set
- update or support publication that implies support outside the checked-in
  matrix

Allowed fail-closed behavior:

- capability inspection and docs-only policy checks may still run on an
  unsupported host when they do not widen support claims
- public package, install, revert, and support-tier publication must fail
  closed instead of silently degrading into unsupported behavior

No unsupported host may be described as:

- `best effort supported`
- `probably compatible`
- `supported if LLVM is installed`

## Working Rules For Downstream Issues

- treat the `package.json` bridge, `npm run objc3c -- <action>`, as the
  only public command routing surface
- keep support-tier and support-window publication machine-owned
- keep transient package/install reports and matrix captures under `tmp/`
- keep checked-in platform policy under `docs/runbooks/`,
  `tests/tooling/fixtures/`, and `schemas/`
- prove new support claims through the package/install/update path, not just a
  compile-only probe

## Machine-Owned Artifact Contract

The canonical generated support-matrix surface for this milestone is:

- `tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json`

It must be generated by:

- `npm run objc3c -- build-platform-support-matrix`

The checked-in schema and contract surfaces for that artifact are:

- `schemas/objc3c-platform-support-matrix-v1.schema.json`
- `tests/tooling/fixtures/platform_hardening/platform_matrix_artifact_contract.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

The generated summary/report family for this milestone lives under:

- `tmp/reports/platform-hardening/`

Downstream validation and publication work must extend this artifact instead of
inventing a second matrix format.

## Build And Package Validation Surface

The live build/package validation path for this milestone must stay on the same
public build/package surfaces users run:

- `npm run objc3c -- build-platform-support-matrix`
- `npm run objc3c -- build-native-binaries`
- `npm run objc3c -- package-runnable-toolchain`
- `npm run objc3c -- validate-platform-hardening`
- `npm run objc3c -- validate-platform-hardening-end-to-end`
- `npm run objc3c -- build-package-channels`
- `npm run objc3c -- validate-packaging-channels`
- `npm run objc3c -- validate-packaging-channels-end-to-end`

The matrix validator for this slice is:

- `npm run objc3c -- validate-platform-hardening`

It must fail closed if the current host is outside the checked-in support
matrix.

## Closeout Gate Replay

Use this command before widening any host, toolchain, package, install, or
release claim:

- `npm run objc3c -- validate-platform-hardening`

The closeout gate reruns the milestone summary builders and validators, then
verifies that the published support boundary still resolves to exactly:

- default platform: `windows-x64`
- supported platform ids: `windows-x64`
- unpublished tiers: `tier-2`, `experimental`
- package, release, and support artifacts all reference the same support
  matrix

## Toolchain Replay And Support Evidence

Toolchain-range evidence for this milestone must come from replayable host
probes plus the checked-in release/update support outputs.

The replay surface for this slice is:

- `npm run objc3c -- inspect-capability-explorer`
- `npm run objc3c -- validate-release-operations`
- `npm run objc3c -- validate-release-operations-end-to-end`
- `npm run objc3c -- validate-platform-hardening`

This surface proves the current host/toolchain shape that the checked-in matrix
can actually claim and keeps broader toolchain-range rhetoric fail-closed.

## Runnable Install-Matrix Integration

The runnable install-matrix proof for this milestone is the composition of:

- support-matrix generation
- build/package validation on the checked-in host tier
- toolchain/release support replay
- installer and offline-bundle smoke under temp-owned roots

The integrator for this slice is:

- `npm run objc3c -- validate-platform-hardening`

It must prove the current supported host tier through the same package/install
artifacts that operators would actually consume.

## Public Publication Surface

The tiered support matrix must now be visible on the public workflow and
release/update metadata surfaces:

- public commands:
  - `npm run objc3c -- build-platform-support-matrix`
  - `npm run objc3c -- validate-platform-hardening`
  - `npm run objc3c -- validate-platform-hardening-end-to-end`
- update metadata:
  - `tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json`
- support publication:
  - `tmp/artifacts/release-operations/publication/objc3c-upgrade-support-report.json`
  - `tmp/artifacts/release-operations/publication/objc3c-release-channel-catalog.json`

These surfaces must publish the same support tiers and supported platform ids as
`tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json`.

## Explicit Non-Goals

- no non-`windows-x64` support claim in the current milestone slice
- no package-manager-specific install or upgrade semantics
- no signed or notarized installer claim
- no system-wide installer claim
- no parallel portability harness outside the existing package, release, and
  public workflow paths
