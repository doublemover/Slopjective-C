# Showcase Examples

This directory is the live home for runnable narrative examples.

Use it for small examples that prove real compiler and runtime capabilities
through the normal repo command surface.

## Portfolio Boundary

Canonical checked-in inputs:

- `showcase/README.md`
- `showcase/portfolio.json`
- `showcase/auroraBoard/main.objc3`
- `showcase/signalMesh/main.objc3`
- `showcase/patchKit/main.objc3`

Shared live tooling:

- `package.json`
- `scripts/objc3c_public_workflow_runner.py`
- `scripts/objc3c_native_compile.ps1`
- `scripts/check_showcase_surface.py`

Machine-owned outputs only:

- `tmp/artifacts/showcase/`
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

## Explicit Non-Goals

- screenshots or image-only demos
- sidecar-only example manifests with no checked-in source
- example-specific compiler wrappers or milestone-local validation paths
- treating `tmp/` outputs as canonical example sources
