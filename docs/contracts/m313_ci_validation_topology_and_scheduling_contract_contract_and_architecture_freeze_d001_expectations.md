# M313-D001 Expectations

Contract ID: `objc3c-cleanup-ci-validation-topology-scheduling-contract/m313-d001-v1`

## Purpose
- Freeze one explicit CI validation topology for the `M313` cleanup program.
- Treat executable acceptance-suite summaries and compatibility-bridge summaries as the primary CI truth surfaces.
- Keep retained static guards limited to inventory, schema, policy-budget, and exception-policy checks that still add unique value.

## Required truths
- The CI topology defines a single staged runner entrypoint and a single dedicated workflow path.
- Stage order is frozen as retained static guards, then acceptance suites, then compatibility bridges, then the integrated topology summary.
- Canonical CI summary roots live under `tmp/reports/m313/ci/`.
- The retained static-guard set is explicit and finite.
- Existing general workflows remain referenced for context, but the `M313` acceptance-first path is isolated in its own workflow.

## Closeout notes
- This issue freezes the contract only; the dedicated workflow and runner land in `M313-D002`.
- Next issue: `M313-D002`.
