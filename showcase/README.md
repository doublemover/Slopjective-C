# Showcase Examples

This directory is the live home for runnable narrative examples.

Use it for small examples that prove real compiler and runtime capabilities
through the normal repo command surface.

## Portfolio Boundary

Canonical checked-in inputs:

- `showcase/README.md`
- `showcase/portfolio.json`
- `showcase/auroraBoard/main.objc3`
- `showcase/auroraBoard/workspace.json`
- `showcase/signalMesh/main.objc3`
- `showcase/signalMesh/workspace.json`
- `showcase/patchKit/main.objc3`
- `showcase/patchKit/workspace.json`

Shared live tooling:

- `package.json`
- `scripts/objc3c_public_workflow_runner.py`
- `docs/tutorials/build_run_verify.md`
- `scripts/objc3c_native_compile.ps1`
- `scripts/check_showcase_surface.py`
- `scripts/check_objc3c_native_execution_smoke.ps1`
- `scripts/check_objc3c_execution_replay_proof.ps1`

Machine-owned outputs only:

- `tmp/artifacts/showcase/`
- `tmp/reports/showcase/`
- `tmp/pkg/objc3c-native-runnable-toolchain/`
- `tmp/reports/objc3c-public-workflow/`

## Portfolio Stories

- `auroraBoard`
  - target story: categories, reflection, synthesized behaviors
- `signalMesh`
  - target story: status bridging, actors, runtime messaging
- `patchKit`
  - target story: derives, macros, property behaviors, interop

These examples must stay small and compile through the live native compiler
path. Later issues can expand the sources, but they must keep the same repo
roots and public command surface.

Selection model:

- compile the full portfolio with `npm run check:showcase:surface`
- compile one named example with
  `python scripts/objc3c_public_workflow_runner.py check-showcase-surface --example auroraBoard`
- compile by story capability with
  `python scripts/objc3c_public_workflow_runner.py check-showcase-surface --capability actors`

## Build Run Package Surface

The checked-in showcase contract is rooted at `showcase/portfolio.json`.
Each example directory also carries its own checked-in workspace contract at
`showcase/<example-id>/workspace.json`.

The tutorial-facing command and artifact map for this same surface lives in
`docs/tutorials/build_run_verify.md`.

Build and artifact entrypoints:

- `npm run build:objc3c-native`
- `npm run check:showcase:surface`
- `npm run test:showcase`
- `npm run test:showcase:e2e`
- `npm run package:objc3c-native:runnable-toolchain`

Runtime-backed shared commands used by the showcase surface:

- `npm run test:objc3c:execution-smoke`
- `npm run test:objc3c:execution-replay-proof`
- `python scripts/objc3c_public_workflow_runner.py validate-showcase-runtime`

The live compile path emits object and manifest artifacts under
`tmp/artifacts/showcase/<example-id>/` with the fixed emit prefix `module`.
Package staging stays under `tmp/pkg/objc3c-native-runnable-toolchain/`, and
showcase report artifacts stay under `tmp/reports/showcase/`, rooted at
`tmp/reports/showcase/summary.json`.

Runtime and presentation contracts are checked in per example under
`showcase/<example-id>/workspace.json`. Those workspace contracts declare the
runtime launch-contract helper, the authoritative runtime-library/linker-flag
resolution model, the expected process exit code, and the presentation headline
for the example.

## Explicit Non-Goals

- screenshots or image-only demos
- sidecar-only example manifests with no checked-in source
- example-specific compiler wrappers or milestone-local validation paths
- treating `tmp/` outputs as canonical example sources
