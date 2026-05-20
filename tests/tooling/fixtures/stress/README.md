# objc3c Stress Input Families

This directory owns the checked-in source surface for `M302`.

It does not own machine-generated reducer or triage outputs. Those stay under
`tmp/artifacts/stress/` and `tmp/reports/stress/`.

Canonical checked-in inputs stay split into these families:

- parser/sema malformed-input corpus
- lowering/runtime stress fixtures
- mixed-module differential fixtures
- replay-backed contract fixtures

The authoritative inventory lives in `tests/tooling/fixtures/stress/source_surface.json`.
Behavior ownership for this mixed stress surface is split under
`tests/tooling/fixtures/stress/behavior_owner_splits/` before any stress input
can be cited as parser, sema, lowering, IR, runtime, or e2e support.

The checked-in safety policy lives in
`tests/tooling/fixtures/stress/safety_policy.json`.

The checked-in machine-owned artifact and minimization contract lives in
`tests/tooling/fixtures/stress/artifact_surface.json`.

The checked-in public workflow contract lives in
`tests/tooling/fixtures/stress/workflow_surface.json`.

The checked-in release-claim gate lives in
`tests/tooling/fixtures/stress/claim_gate.json`. It names the stress claims that
can be cited, the durable input catalogs behind them, the generated `tmp/`
reports that must be reproduced, and the unsupported surfaces that remain
fail-closed.

The checked-in crash triage fixture catalog lives in
`tests/tooling/fixtures/stress/crash_triage_fixture_manifest.json`. It names the
positive replayable-signature case and fail-closed malformed-summary cases that
the crash triage tests exercise.
