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

Run the policy validator before treating channel metadata as releasable:

```powershell
python scripts/check_objc3c_release_channel_operations_policy.py
```

This check is intentionally narrower than the full release operations workflow:
it proves the checked-in channel/update/rollback policy and claim boundaries,
not that a production distribution has been published.
