# Getting Started With The Runnable Subset

This file is the live reader path for learning ObjC3 from the checked-in runnable subset.

Use it when you want to:

- prove the repo is healthy before reading implementation code,
- learn the surface area through one small compileable example at a time,
- understand which commands are the normal user path,
- and choose the next tutorial or migration guide based on the capability you actually care about.

## What This Tutorial Proves

By the end of this file, a reader should know:

- which commands count as the normal public path
- which example is the right first compile target
- which showcase story matches the capability they care about next
- which document to open for migration or comparison questions instead of guessing

## Teaching Model

Teach from checked-in runnable sources first.

- start with the smallest command that proves the compiler and runtime are alive
- read one showcase story at a time instead of scanning the whole implementation tree
- move from compile, to run, to comparison, to deeper implementation notes in that order
- treat the public package scripts and workflow runner as the command truth
- keep maintainer-only workflow detail out of the primary reader path

## Step 1 Verify The Toolchain

Run the normal repo checks before you trust any tutorial claim:

```sh
npm run build:objc3c-native
npm run test:fast
```

If those fail, stop there and fix the repo state first.

## Step 2 Compile One Runnable Example

Use the smallest example-first compile path:

```sh
npm run compile:objc3c -- showcase/auroraBoard/main.objc3
```

Why `auroraBoard` first:

- it stays close to the canonical runtime acceptance shape
- it demonstrates categories, reflection, and synthesized behaviors without requiring the whole showcase portfolio at once

## Step 3 Use The Showcase Surface As The Tutorial Backbone

Once one example compiles, move to the checked-in showcase surface:

```sh
npm run check:showcase:surface
npm run test:showcase
```

Use `showcase/README.md` and `showcase/portfolio.json` as the example map:

- `auroraBoard` for categories, reflection, and synthesized behaviors
- `signalMesh` for actor-shaped messaging, status bridging, and runtime messaging
- `patchKit` for derives, macros, property behaviors, and interop

Stdlib follow-up modules after the first compile:

- `auroraBoard` -> `objc3.core`, `objc3.errors`, `objc3.keypath`
- `signalMesh` -> `objc3.concurrency`, `objc3.system`
- `patchKit` -> `objc3.keypath`, `objc3.system`

If the first compile worked and you already know your question, branch
immediately:

- stay on `auroraBoard` if the question starts with ObjC2 object-model habits
- move to `signalMesh` if the question is about messaging or actor-shaped flows
- move to `patchKit` if the question is about macros, derives, or interop
- then use `stdlib/README.md` if you want the checked-in stdlib module surface
  that backs the same story

## Step 4 Choose The Next Learning Path

Choose the next document based on the question you actually have:

- if you need the exact build, run, and verify flow, continue to `docs/tutorials/build_run_verify.md`
- if you need migration language, continue to `docs/tutorials/objc2_swift_cpp_comparison.md`
- if you need runnable example context, continue to `showcase/README.md`
- if you need exact command mapping, use `docs/runbooks/objc3c_public_command_surface.md`
- if you need implementation boundaries after the tutorial, then open `docs/objc3c-native.md`

Use this decision rule:

- compile first
- then choose the showcase example that matches the capability
- then open the migration or comparison guide only after the runnable source is in view

## Canonical Inputs

This getting-started tutorial should stay coupled to these live inputs:

- `docs/tutorials/getting_started.md`
- `docs/tutorials/build_run_verify.md`
- `docs/tutorials/README.md`
- `README.md`
- `site/src/index.body.md`
- `showcase/README.md`
- `showcase/portfolio.json`
- `showcase/auroraBoard/main.objc3`
- `showcase/signalMesh/main.objc3`
- `showcase/patchKit/main.objc3`
- `docs/runbooks/objc3c_public_command_surface.md`

## Exact Live Paths For Downstream Work

- tutorial narrative and reader sequencing:
  - `docs/tutorials/getting_started.md`
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/README.md`
- onboarding and public routing:
  - `README.md`
  - `site/src/index.body.md`
- runnable example sources and portfolio shape:
  - `showcase/README.md`
  - `showcase/portfolio.json`
  - `showcase/auroraBoard/main.objc3`
  - `showcase/signalMesh/main.objc3`
  - `showcase/patchKit/main.objc3`
- command truth and documentation guardrails:
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `scripts/check_documentation_surface.py`

## Explicit Non-Goals

- no tutorial path rooted in `tmp/` or other machine outputs
- no slide-deck or screenshot-first onboarding path
- no maintainer-only workflow notes in the primary getting-started flow
- no extra command aliases beyond the public package-script surface
