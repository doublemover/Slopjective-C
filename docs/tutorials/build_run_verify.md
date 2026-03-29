# Tutorial Build Run And Verify Surface

This file defines the live build, run, and verify workflow for the tutorial and migration path.

Use it when you need the exact commands and artifact expectations behind the reader-facing tutorials.

## Workflow Boundary

The tutorial workflow must stay on the normal public compiler and showcase surfaces.

- build the native toolchain through the public package-script surface
- compile showcase examples through `compile:objc3c`
- verify the checked-in example portfolio through the showcase surface and integrated validation
- treat `tmp/artifacts/showcase/` and `tmp/reports/showcase/` as outputs, not as tutorial sources

## Build

Build the native toolchain first:

```sh
npm run build:objc3c-native
```

That is the canonical tutorial build step. Do not add a tutorial-only build wrapper.

## Run The First Compile

Compile one checked-in example directly:

```sh
npm run compile:objc3c -- showcase/auroraBoard/main.objc3
```

Use `auroraBoard` for the first compile because it stays closest to the current object-model and runtime-acceptance shape.

## Verify The Portfolio Surface

After one direct compile, verify the full checked-in portfolio:

```sh
npm run check:showcase:surface
npm run test:showcase
```

Use the packaged surface only when you need the staged runnable bundle:

```sh
npm run test:showcase:e2e
```

If you want the ordered example sequence after these commands are clear, continue to `docs/tutorials/guided_walkthrough.md`.

## Validation Surface

The bounded getting-started validation contract is implemented in `scripts/check_getting_started_surface.py`.

That surface proves:

- the reader-facing documentation structure is still intact
- the walkthrough manifest still points at the right showcase examples
- the walkthrough-selected examples still compile through the normal public compiler path

## Artifact And Report Expectations

The tutorial workflow is coupled to the existing showcase outputs:

- emitted artifacts live under `tmp/artifacts/showcase/<example-id>/`
- integrated showcase reports live under `tmp/reports/showcase/`
- staged runnable package output lives under `tmp/pkg/objc3c-native-runnable-toolchain/`

Those paths are machine-owned. They support the tutorial, but they are not the tutorial itself.

## Canonical Inputs

- `docs/tutorials/build_run_verify.md`
- `docs/tutorials/guided_walkthrough.md`
- `docs/tutorials/getting_started.md`
- `scripts/check_getting_started_surface.py`
- `docs/tutorials/objc2_to_objc3_migration.md`
- `showcase/README.md`
- `showcase/portfolio.json`
- `showcase/auroraBoard/main.objc3`
- `docs/runbooks/objc3c_public_command_surface.md`
- `package.json`

## Exact Live Paths For Downstream Work

- tutorial command narrative:
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/guided_walkthrough.md`
  - `docs/tutorials/getting_started.md`
  - `docs/tutorials/README.md`
- checked-in workflow contract:
  - `showcase/portfolio.json`
  - `showcase/README.md`
- command truth:
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `scripts/objc3c_public_workflow_runner.py`
- bounded tutorial validation:
  - `scripts/check_getting_started_surface.py`
- machine-owned outputs:
  - `tmp/artifacts/showcase/`
  - `tmp/reports/showcase/`
  - `tmp/reports/tutorials/`
  - `tmp/pkg/objc3c-native-runnable-toolchain/`

## Explicit Non-Goals

- no tutorial-specific compiler wrapper
- no sidecar validation commands that bypass the public package-script surface
- no checked-in tutorial claims rooted in `tmp/` outputs instead of source inputs
