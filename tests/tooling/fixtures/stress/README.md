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

The checked-in safety policy lives in
`tests/tooling/fixtures/stress/safety_policy.json`.

The checked-in machine-owned artifact and minimization contract lives in
`tests/tooling/fixtures/stress/artifact_surface.json`.
