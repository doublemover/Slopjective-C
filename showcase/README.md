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

Build and artifact entrypoints:

- `npm run build:objc3c-native`
- `npm run check:showcase:surface`
- `npm run package:objc3c-native:runnable-toolchain`

Runtime-backed shared commands used by the showcase surface:

- `npm run test:objc3c:execution-smoke`
- `npm run test:objc3c:execution-replay-proof`

The live compile path emits object and manifest artifacts under
`tmp/artifacts/showcase/<example-id>/` with the fixed emit prefix `module`.
Package staging stays under `tmp/pkg/objc3c-native-runnable-toolchain/`, and
showcase report artifacts stay under `tmp/reports/showcase/`.

## Explicit Non-Goals

- screenshots or image-only demos
- sidecar-only example manifests with no checked-in source
- example-specific compiler wrappers or milestone-local validation paths
- treating `tmp/` outputs as canonical example sources
