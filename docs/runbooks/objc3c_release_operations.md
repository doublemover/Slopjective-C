# objc3c Release Operations

## Scope

This runbook defines the checked-in release-operations surface for objc3c:

- semantic versioning claims over the published objc3c release payloads
- support windows and support-window classes for published channels
- distinct stable and nightly channel operation gates
- machine-owned update manifests and upgrade-support warning payloads
- machine-owned release-channel manifests with local provenance and rollback safety
- revert, deprecation, and upgrade-path publication derived from checked-in contracts
- release-operations validation over the existing release-foundation and packaging-channel outputs

The post-cutover issue evidence page for #8153/#8179 is
`docs/issues/objc3_next_8153_8179_evidence.md`. It records docs and release-note
references only; generated release artifacts and release operation scripts remain
owned by the release proof lane.

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
- package-ecosystem clean install evidence from
  `npm run objc3c -- validate-package-install-distribution --from-nothing`

Update and upgrade metadata are derived views over those existing outputs. The
canonical payload remains the runnable toolchain package and its release-
foundation evidence. Packaging channels remain the installable transport.

Do not introduce a second update payload, a parallel installer tree, or a
package-manager-only support owner surface.

`stable` and `nightly` are release-operation channels with different gates:

- `stable` publication requires release-foundation proof, release-candidate
  conformance, package-channel proof, release-operations end-to-end proof, and
  distribution-credibility proof before it can be treated as a public stable
  channel.
- `nightly` publication is evidence-only. It requires the nightly workflow plus
  release-foundation, packaging-channel, and release-operations proof, but it is
  not a stable support claim and cannot satisfy the stable gate.

The checked-in source for that split is
`tests/tooling/fixtures/release_operations/channel_operations_model.json`. The
machine-owned projection is
`tmp/artifacts/release-operations/channel-manifest/objc3c-release-channel-manifest.json`.
Publication fails closed when either required channel is absent.

Current platform support-tier boundary:

- `Tier 1`: `windows-x64`
- `Tier 2`: none published
- `Experimental`: none published
- release-operations metadata must not imply support on any host outside the
  checked-in `windows-x64` package-channel set

## Support And Warning Policy

Support publication for release operations must emit:

- a machine-owned update manifest with channel, version, and artifact pointers
- a machine-owned release-channel manifest with stable/nightly gate actions,
  source-derived notes policy, local provenance, release evidence, and rollback
  safety
- a machine-owned upgrade-support report with support-window, upgrade-path, and
  warning details
- machine-owned release notes and a public changelog derived from the update
  manifest, release-channel manifest, checked channel model, and checked
  release-operations policy sources
- signed local-installer digest validation copied from the package-channel
  manifest into each update-channel artifact set
- explicit revert guidance tied to the published installer/offline channels
- user-facing rollback diagnostics with the rollback channel and operator
  command needed to recover from blocked update publication
- ABI/runtime/data-format rejection diagnostics as checked-in policy contracts
- fail-closed diagnostics when required upstream release, package-channel,
  platform-support, or same-major revert artifacts are absent
- release evidence derived from checked manifests, channel policy, and
  release-foundation/package-channel outputs; generated reports may summarize
  that evidence but cannot become source truth
- a clean package-install prerequisite for every channel; the prerequisite must
  delete package-ecosystem owned temp roots first and publish
  `tmp/reports/package-ecosystem/install-distribution-credibility-summary.json`
  with a passing `from_nothing_probe`
- package-channel freshness provenance for every channel; the release-channel
  manifest must record `generated_at_utc` from the package-channel summary,
  package-channel manifest, and platform support matrix, and publication must
  fail closed if those machine-owned artifacts drift beyond the checked
  `max_artifact_skew_hours` policy

Warnings must be deterministic and derived from checked-in policy classes such
as:

- out-of-window upgrade attempts
- channel downgrades from `stable` to `preview`
- cross-major upgrade requests
- deprecated channel usage
- runtime/data-format rejection requirements
- support-tier or archive compatibility overclaim attempts outside the checked-in
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

The update manifest, upgrade-support report, channel catalog, release notes, and
public changelog must also publish:

- the release-channel manifest path
- the release notes and public changelog artifact paths when publishing
  release-operation metadata
- the machine-owned platform support matrix path
- the default supported platform id
- the supported platform id set
- the support-tier table reused from the platform-hardening surface
- the local installer digest signature selected by the package-channel surface
- rollback diagnostics derived from the checked-in fail-closed diagnostics
  policy, not from prose release notes
- release-note and changelog source paths derived from checked source
  manifests, never from manual changelog rows or tmp-only support claims
- generated release, update, channel, rollback, and package artifacts listed as
  replay evidence only; they must not appear in `release_note_sources` or
  `public_changelog_sources`
- local provenance for the release manifest digest, package-channel manifest
  digest, release payload digest, installer signature, archive digests, and git
  source stamp

Those fields must stay aligned with
the platform support matrix artifact selected by the checked-in
platform-hardening contract.

Release operations do not rebuild upstream owner outputs while publishing. If
the release manifest, package-channel summary, platform-support matrix, platform
support summary, package archive pointers, or upgrade-support report contract is
missing, the release-operations action fails closed with the owning public action
named in the diagnostic. Public action names stay stable; the hard cutover is in
the source and artifact contracts, not in a retired command bridge.

Release operations also fail closed when package-channel freshness cannot be
proved. The checked-in channel model requires the package-channel summary,
package-channel manifest, and platform support matrix timestamps to stay within
six hours of each other. If they do not, operators must refresh the package
channels through `npm run objc3c -- build-package-channels` before publishing
update, rollback, or release-channel metadata.

Release operations may reference package-ecosystem install credibility only when
the checked-in channel model requires `validate-package-install-distribution`
with the `--from-nothing` flag. A preexisting package-ecosystem `tmp` summary is
not source truth for channel closure unless its `from_nothing_probe` proves that
the package-ecosystem owned temp roots were absent after cleanup and regenerated
by the replay.

## Non-Goals

- no hosted update service
- no background updater or scheduler
- no package-manager upgrade semantics
- no cross-platform support claim beyond the checked-in `windows-x64` channel set
- no operator-maintained support spreadsheet or release-operation digest
- no retired adapter or evidence-log support claim
