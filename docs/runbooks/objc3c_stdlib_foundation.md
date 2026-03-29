# objc3c Standard Library Foundation

This runbook defines the live Objective-C 3 standard-library foundation surface.

It is the working source of truth for `M305` and the stdlib bootstrap path.
Downstream issues must stay on the exact repo-owned roots defined here instead of
inventing sidecar trees or milestone-local scaffolding.

## Working boundary

The checked-in standard-library foundation is rooted at:

- `stdlib/`
- `stdlib/modules/`
- `tmp/artifacts/stdlib/`
- `tmp/reports/stdlib/`
- `tmp/pkg/objc3c-native-runnable-toolchain/`

Authoritative inputs:

- `spec/STANDARD_LIBRARY_CONTRACT.md`
- `stdlib/README.md`
- `stdlib/workspace.json`
- `stdlib/core_architecture.json`
- `stdlib/semantic_policy.json`
- `docs/runbooks/objc3c_stdlib_core.md`
- `scripts/objc3c_public_workflow_runner.py`
- `scripts/package_objc3c_runnable_toolchain.ps1`

## Non-goals

This milestone does not claim:

- a completed high-level standard library
- a new frontend-only module emitter surface
- hidden bootstrap scripts outside the public runner and existing package flow
- a second documentation tree outside `docs/runbooks/` and `stdlib/`

## Expected end state

The `M305` implementation path should leave behind:

- one checked-in stdlib root
- canonical module partitions using the names from `spec/STANDARD_LIBRARY_CONTRACT.md`
- a machine-readable workspace contract under `stdlib/`
- runner/package integration through the existing public command surface
- runnable validation rooted in the existing package and compile workflow

## Public actions

- `npm run check:stdlib:surface`
- `npm run build:objc3c:stdlib`
- `npm run test:stdlib`
- `npm run test:stdlib:e2e`
