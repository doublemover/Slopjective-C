# objc3c Release Operations

## Scope

This runbook defines the checked-in release-operations surface for objc3c:

- semantic versioning claims over the published objc3c release payloads
- support windows and support-window classes for published channels
- machine-owned update manifests and upgrade-support warning payloads
- revert, deprecation, and upgrade-path publication derived from checked-in contracts
- release-operations validation over the existing release-foundation and packaging-channel outputs

This milestone does not add a hosted updater daemon, background auto-update service,
or package-manager-specific upgrade logic.

## Versioning Model

The canonical public version shape is semantic versioning:

- `major`: breaking support boundary
- `minor`: additive release within one major line
- `patch`: non-breaking repair release within one minor line

Version claims in this release-operations surface are contract-driven and
machine-published. No version claim may depend on an ad hoc changelog row or an
operator-assembled release page.

## Support Windows

Support windows are intentionally narrow and channel-scoped:

- `stable`: supported and preferred for normal users
- `candidate`: supported for release-drill and pre-publish verification
- `preview`: blocked unless generated conversion replay and revert evidence exists

Within one major line:

- `stable` must advertise a support window for the current minor line
- `candidate` may overlap the current `stable` line for upgrade rehearsal
- `preview` must carry warnings and must still publish machine-readable
  support status and revert guidance

Do not claim indefinite support, cross-major forward support, or a hosted
long-term support program in this milestone.

## Channel And Upgrade Boundary

The release-operations surface layers on top of:

- release-foundation publication artifacts
- portable archive, installer image, and offline-bundle package channels

Update and upgrade metadata are derived views over those existing outputs. The
canonical payload remains the runnable toolchain package and its release-
foundation evidence. Packaging channels remain the installable transport.

Do not introduce a second update payload, a parallel installer tree, or a
package-manager-only support owner surface.

Current platform support-tier boundary:

- `Tier 1`: `windows-x64`
- `Tier 2`: none published
- `Experimental`: none published
- release-operations metadata must not imply support on any host outside the
  checked-in `windows-x64` package-channel set

## Support And Warning Policy

Support publication for release operations must emit:

- a machine-owned update manifest with channel, version, and artifact pointers
- a machine-owned upgrade-support report with support-window, upgrade-path, and
  warning details
- explicit revert guidance tied to the published installer/offline channels
- ABI/runtime/data-format rejection diagnostics as checked-in policy contracts
- fail-closed diagnostics when required upstream release, package-channel,
  platform-support, or same-major revert artifacts are absent

Warnings must be deterministic and derived from checked-in policy classes such
as:

- out-of-window upgrade attempts
- channel downgrades from `stable` to `preview`
- cross-major upgrade requests
- deprecated channel usage
- runtime/data-format rejection requirements
- support-tier or archive support overclaim attempts outside the checked-in
  `windows-x64` package/install/update surface

## Release-Operations Workflow

The live release-operations workflow must expose:

- a source-surface check
- a schema-surface check
- an update-manifest build command
- a release-operations publication command
- an integrated release-operations validation command
- an end-to-end release-operations validation command

These entrypoints must stay on the shared `npm run objc3c -- <action>` bridge
and publish machine-owned artifacts into release-operations output families
selected by checked-in contracts.

The update manifest, upgrade-support report, and channel catalog must also
publish:

- the machine-owned platform support matrix path
- the default supported platform id
- the supported platform id set
- the support-tier table reused from the platform-hardening surface

Those fields must stay aligned with
the platform support matrix artifact selected by the checked-in
platform-hardening contract.

Release operations do not rebuild upstream owner outputs while publishing. If
the release manifest, package-channel summary, platform-support matrix, platform
support summary, package archive pointers, or upgrade-support report contract is
missing, the release-operations action fails closed with the owning public action
named in the diagnostic. Public action names stay stable; the hard cutover is in
the source and artifact contracts, not in a retired command bridge.

## Non-Goals

- no hosted update service
- no background updater or scheduler
- no package-manager upgrade semantics
- no cross-platform support claim beyond the checked-in `windows-x64` channel set
- no operator-maintained support spreadsheet or release-operation digest
- no retired adapter or evidence-log support claim
