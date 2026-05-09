# Tutorial Build Run And Verify Surface

This file defines the live build, run, and verify workflow for the tutorial and canonicalization path.

Use it when you need the exact commands and artifact expectations behind the reader-facing tutorials.

## Workflow Boundary

The tutorial workflow must stay on the normal public compiler and showcase surfaces.

- build the native toolchain through the public npm bridge surface
- compile showcase examples through `npm run objc3c -- compile-objc3c`
- verify the checked-in example portfolio through the showcase surface and integrated validation
- treat `tmp/artifacts/showcase/` and `tmp/reports/showcase/` as outputs, not as tutorial sources

## Build

Build the native toolchain first:

```sh
npm run objc3c -- build-native-binaries
```

That is the canonical tutorial build step. Do not add a tutorial-only build wrapper.

## Run The First Compile

Compile one checked-in example directly:

```sh
npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3
```

Use `auroraBoard` for the first compile because it stays closest to the current object-model and runtime-acceptance shape.

## Verify The Portfolio Surface

After one direct compile, verify the full checked-in portfolio:

```sh
npm run objc3c -- check-showcase-surface
npm run objc3c -- validate-showcase
```

Use the packaged surface only when you need the staged runnable bundle:

```sh
npm run objc3c -- validate-runnable-showcase
npm run objc3c -- validate-runnable-developer-tooling
```

If you want the ordered example sequence after these commands are clear, continue to `docs/tutorials/guided_walkthrough.md`.

## Validation Surface

The bounded getting-started validation contract is reached through
`npm run objc3c -- validate-getting-started`.
The public integrated entrypoint for the same tutorial and onboarding flow is `npm run objc3c -- validate-getting-started`.

That surface proves:

- the reader-facing documentation structure is still intact
- the walkthrough manifest still points at the right showcase examples
- the walkthrough-selected examples still compile through the normal public compiler path
- the public tutorial and onboarding command surface still runs end to end

The live smoke integration for that same surface is reached through
`npm run objc3c -- validate-getting-started`.

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
- `npm run objc3c -- validate-getting-started`
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
  - `npm run objc3c -- <action>`
- bounded tutorial validation:
  - `npm run objc3c -- validate-getting-started`
- machine-owned outputs:
  - `tmp/artifacts/showcase/`
  - `tmp/reports/showcase/`
  - `tmp/reports/tutorials/`
  - `tmp/pkg/objc3c-native-runnable-toolchain/`

## Explicit Non-Goals

- no tutorial-specific compiler wrapper
- no sidecar validation commands that bypass the public npm bridge surface
- no checked-in tutorial claims rooted in `tmp/` outputs instead of source inputs
