# objc3c Release Channel Operations

## Source Policy

The release-channel source of truth is
`tests/tooling/fixtures/release_channel_operations_policy.json`, validated by
`scripts/check_objc3c_release_channel_operations_policy.py` against
`schemas/objc3c-release-channel-operations-policy-v1.schema.json`.

Generated release manifests, update manifests, channel manifests, upgrade
reports, package-channel summaries, and tmp reports are replayable evidence.
They do not create support claims by themselves. A channel may be called
releasable only after the policy-required release-foundation, update-manifest,
rollback-proof, and signed-artifact evidence is verified.

Release-note and public-changelog source lists are checked-source lists. They
must point at repo files such as runbooks, schemas, and fixture manifests, never
at `tmp/` release manifests or reports. Generated update, channel, rollback,
and package artifacts stay in generated-evidence lists only.

The docs/index evidence slice for #8179 is recorded in
`docs/issues/objc3_next_8153_8179_evidence.md`. That page is limited to
source-derived release notes, public changelog, and runbook references; release
publication scripts and manifests stay owned by the release-operations lane.

## Channel Mechanics

`stable` is the only channel allowed to carry a stable support claim, and only
after evidence is verified. Its gates include release foundation,
release-candidate conformance, packaging channels, release operations
end-to-end validation, and distribution credibility.

`nightly` is evidence-only. It requires the nightly workflow plus release
foundation, packaging channels, and release operations validation. Nightly does
not satisfy the stable gate and must not be described as stable support or a
warning-free promotion path.

## Update And Rollback Mechanics

Update publication is manifest selected and fail-closed. Missing channel rows,
missing update manifests, silent cross-major upgrades, hosted background
updates, and package-registry upgrade semantics block release-channel
publication for this slice.

Rollback publication is also fail-closed. Stable rollback uses the local
installer primitive; nightly rollback uses the offline-bundle primitive. Both
require the packaging-channel end-to-end rollback drill command and proof
metadata before publication can proceed.

## Operator Check

Run the public release-operations validator before treating channel metadata as
releasable:

```powershell
npm run objc3c -- validate-release-operations
```

This check proves the checked-in channel/update/rollback policy and claim
boundaries through the public command surface. It does not mean a production
distribution has been published.
