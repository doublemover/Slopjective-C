# objc3c Release Operations

## Scope

This runbook defines the checked-in release-operations surface for objc3c:

- semantic versioning claims over the published objc3c release payloads
- compatibility windows and support-window classes for published channels
- machine-owned update manifests and compatibility warning payloads
- rollback, deprecation, and upgrade-path publication derived from checked-in contracts
- release-operations validation over the existing release-foundation and packaging-channel outputs

This milestone does not add a hosted updater daemon, background auto-update service,
or package-manager-specific upgrade logic.

## Versioning Model

The canonical public version shape is semantic versioning:

- `major`: breaking compatibility boundary
- `minor`: additive compatible release within one major line
- `patch`: non-breaking repair release within one minor line

Version claims in this milestone are contract-driven and machine-published. No
version claim may depend on a hand-edited changelog row or a manually assembled
release page.

## Support Windows

Support windows are intentionally narrow and channel-scoped:

- `stable`: supported and preferred for normal users
- `candidate`: supported for release-drill and pre-publish verification
- `preview`: best-effort for short-lived compatibility probes only

Within one major line:

- `stable` must advertise a compatibility window for the current minor line
- `candidate` may overlap the current `stable` line for upgrade rehearsal
- `preview` may carry warnings, but must still publish machine-readable
  compatibility status and rollback guidance

Do not claim indefinite support, cross-major forward compatibility, or a hosted
long-term support program in this milestone.

## Channel And Upgrade Boundary

`M299` layers on top of:

- `M297` release-foundation publication artifacts
- `M298` portable archive, installer image, and offline-bundle package channels

Update and upgrade metadata are derived views over those existing outputs. The
canonical payload remains the runnable toolchain package and its release-
foundation evidence. Packaging channels remain the installable transport.

Do not introduce a second update payload, a parallel installer tree, or a
package-manager-only compatibility source of truth.

## Compatibility And Warning Policy

Compatibility publication for this milestone must emit:

- a machine-owned update manifest with channel, version, and artifact pointers
- a machine-owned compatibility report with support-window, upgrade-path, and
  warning details
- explicit rollback guidance tied to the published installer/offline channels
- ABI/runtime/data-format fallback diagnostics as checked-in policy contracts

Warnings must be deterministic and derived from checked-in policy classes such
as:

- out-of-window upgrade attempts
- channel downgrades from `stable` to `preview`
- cross-major upgrade requests
- deprecated channel usage
- runtime/data-format fallback requirements

## Release-Operations Workflow

The live workflow for this milestone must expose:

- a source-surface check
- a schema-surface check
- an update-manifest build command
- a release-operations publication command
- an integrated release-operations validation command
- an end-to-end release-operations validation command

These entrypoints must stay on the shared public workflow runner and publish
machine-owned artifacts under `tmp/reports/release-operations/` and
`tmp/artifacts/release-operations/`.

## Non-Goals

- no hosted update service
- no background updater or scheduler
- no package-manager upgrade semantics
- no cross-platform support claim beyond the checked-in `windows-x64` channel set
- no manual compatibility spreadsheet or hand-authored release-operation digest
