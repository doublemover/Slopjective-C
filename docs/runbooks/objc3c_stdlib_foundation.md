# objc3c Standard Library Foundation

This runbook defines the live Objective-C 3 standard-library foundation surface.

It is the working owner surface for `objc3c.stdlib.foundation.v1` and the stdlib bootstrap path.
Downstream issues must stay on the exact repo-owned roots defined here instead of
inventing sidecar trees or release-scope scaffolding.

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
- `stdlib/advanced_architecture.json`
- `stdlib/semantic_policy.json`
- `stdlib/lowering_import_surface.json`
- `stdlib/advanced_helper_package_surface.json`
- `stdlib/program_surface.json`
- `docs/runbooks/objc3c_stdlib_core.md`
- `docs/runbooks/objc3c_stdlib_advanced.md`
- `docs/runbooks/objc3c_stdlib_program.md`
- package bridge: `npm run objc3c -- <action>`

## Non-goals

This milestone does not claim:

- a completed high-level standard library
- a new frontend-only module emitter surface
- hidden bootstrap scripts outside the public runner and existing package flow
- a second documentation tree outside `docs/runbooks/` and `stdlib/`

## Expected end state

The stdlib-foundation implementation path should leave behind:

- one checked-in stdlib root
- canonical module partitions using the names from `spec/STANDARD_LIBRARY_CONTRACT.md`
- a machine-readable workspace contract under `stdlib/`
- runner/package integration through the existing `npm run objc3c -- <action>` bridge
- runnable validation rooted in the existing package and compile workflow

## Lowering And Import Artifact Surface

The checked-in stdlib lowering/import contract is:

- `stdlib/lowering_import_surface.json`

It defines the real emitted artifact names consumed by the live stdlib smoke
path:

- `module.obj`
- `module.manifest.json`
- `module.runtime-registration-manifest.json`

It also freezes the import identity handoff between canonical spec module names
and identifier-safe implementation module declarations.

## Exact Live Implementation Paths

- package bridge: `npm run objc3c -- <action>`
- public actions:
  - `npm run objc3c -- check-stdlib-surface`
  - `npm run objc3c -- materialize-stdlib-workspace`
  - `npm run objc3c -- validate-stdlib-foundation`
  - `npm run objc3c -- validate-runnable-stdlib-foundation`
  - `npm run objc3c -- package-runnable-toolchain`
- `stdlib/advanced_architecture.json`
- `stdlib/advanced_helper_package_surface.json`
- `stdlib/program_surface.json`
- `docs/runbooks/objc3c_stdlib_advanced.md`
- `docs/runbooks/objc3c_stdlib_program.md`
- `scripts/check_objc3c_stdlib_foundation_integration.py`
- `scripts/check_objc3c_runnable_stdlib_foundation_end_to_end.py`

## Exact Live Artifact And Output Paths

- `tmp/artifacts/stdlib/workspace/`
- `tmp/artifacts/stdlib/smoke/`
- `tmp/reports/stdlib/surface-summary.json`
- `tmp/reports/stdlib/workspace-smoke-summary.json`
- `tmp/reports/stdlib/integration-summary.json`
- `tmp/reports/stdlib/runnable-end-to-end-summary.json`
- `tmp/pkg/objc3c-native-runnable-toolchain/`

## Exact Live Commands

- `npm run objc3c -- check-stdlib-surface`
- `npm run objc3c -- materialize-stdlib-workspace`
- `npm run objc3c -- validate-stdlib-foundation`
- `npm run objc3c -- validate-runnable-stdlib-foundation`
- `npm run objc3c -- package-runnable-toolchain`

Stdlib helper scripts are implementation anchors owned by the action catalog,
not separate current-facing commands.

## Public actions

- `npm run objc3c -- check-stdlib-surface`
- `npm run objc3c -- materialize-stdlib-workspace`
- `npm run objc3c -- validate-stdlib-foundation`
- `npm run objc3c -- validate-runnable-stdlib-foundation`
