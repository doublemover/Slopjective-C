# objc3c Stdlib Program Surface

This runbook defines the live `M308` boundary for standard-library docs,
examples, packaging-facing onboarding, and capability-driven adoption work.

It extends `docs/runbooks/objc3c_stdlib_foundation.md`,
`docs/runbooks/objc3c_stdlib_core.md`, and
`docs/runbooks/objc3c_stdlib_advanced.md` with the exact reader-facing paths
that later issues must edit directly.

## Working Boundary

Live `M308` work must stay on these paths:

- `stdlib/program_surface.json`
- `stdlib/package_surface.json`
- `stdlib/README.md`
- `docs/runbooks/objc3c_stdlib_program.md`
- `docs/tutorials/README.md`
- `docs/tutorials/getting_started.md`
- `docs/tutorials/build_run_verify.md`
- `docs/tutorials/guided_walkthrough.md`
- `docs/tutorials/objc2_to_objc3_migration.md`
- `docs/tutorials/objc2_swift_cpp_comparison.md`
- `showcase/README.md`
- `showcase/portfolio.json`
- `showcase/tutorial_walkthrough.json`
- `site/src/index.body.md`
- `tmp/artifacts/stdlib/`
- `tmp/reports/stdlib/`

## Current Truthful Portfolio

The live stdlib adoption surface currently routes through:

- checked-in tutorials under `docs/tutorials/`
- checked-in runnable showcase examples under `showcase/`
- the checked-in site landing surface in `site/src/index.body.md`
- the shared public runner and runnable toolchain package flow

Capability-driven demos should keep using the existing showcase sources instead
of inventing a second examples tree inside `stdlib/`.

## Exact Live Implementation Paths

- `stdlib/program_surface.json`
- `scripts/check_stdlib_surface.py`
- `scripts/check_documentation_surface.py`
- `scripts/check_showcase_surface.py`
- `scripts/check_showcase_integration.py`
- `scripts/check_getting_started_integration.py`
- `scripts/objc3c_public_workflow_runner.py`
- `scripts/package_objc3c_runnable_toolchain.ps1`
- `package.json`
- `showcase/portfolio.json`
- `showcase/auroraBoard/main.objc3`
- `showcase/signalMesh/main.objc3`
- `showcase/patchKit/main.objc3`

## Exact Capability Demo Paths

- `showcase/auroraBoard/main.objc3` for categories, reflection, and synthesized
  behaviors
- `showcase/signalMesh/main.objc3` for actors, status bridging, and runtime
  messaging
- `showcase/patchKit/main.objc3` for derives, macros, property behaviors, and
  interop
- `docs/tutorials/getting_started.md` for the first runnable reader path
- `docs/tutorials/objc2_swift_cpp_comparison.md` for migration and comparison
  framing

## Exact Live Artifact And Output Paths

- `tmp/artifacts/stdlib/`
- `tmp/reports/stdlib/`
- `tmp/artifacts/showcase/`
- `tmp/reports/showcase/`
- `tmp/pkg/objc3c-native-runnable-toolchain/`

## Exact Live Commands

- `python scripts/objc3c_public_workflow_runner.py check-documentation-surface`
- `npm run check:docs:surface`
- `python scripts/objc3c_public_workflow_runner.py check-showcase-surface`
- `npm run check:showcase:surface`
- `python scripts/objc3c_public_workflow_runner.py validate-getting-started`
- `npm run test:getting-started`
- `python scripts/objc3c_public_workflow_runner.py validate-showcase`
- `npm run test:showcase`
- `python scripts/objc3c_public_workflow_runner.py validate-runnable-showcase`
- `npm run test:showcase:e2e`
- `python scripts/objc3c_public_workflow_runner.py inspect-capability-explorer`
- `npm run inspect:objc3c:capabilities`
- `python scripts/objc3c_public_workflow_runner.py package-runnable-toolchain`
- `npm run package:objc3c-native:runnable-toolchain`

## Working Rules For Downstream Issues

- edit the live docs, site, and onboarding files directly
- keep capability demos rooted in checked-in showcase examples
- reuse shared validation and packaging paths instead of adding milestone-local
  wrappers
- keep machine-owned artifacts under `tmp/` and out of the reader-facing tree
- treat the tutorial and comparison guides as user-facing narrative sources, not
  machine-generated appendices

## Explicit Non-Goals

- a second stdlib tutorial tree inside `stdlib/`
- a duplicate example portfolio outside `showcase/`
- screenshot-only capability demos without checked-in source
- milestone-local publish wrappers or sidecar doc indexes
