# Guided Showcase Walkthrough

This file is the live step-by-step walkthrough for the checked-in tutorial assets.

Use it after `docs/tutorials/getting_started.md` and `docs/tutorials/build_run_verify.md` when you want one ordered pass through the runnable examples.

## Walkthrough Boundary

This walkthrough stays on the same live command and showcase surfaces as the rest of the tutorial path.

- no custom tutorial runner
- no hidden fixture-only commands
- no step that depends on `tmp/` before the source inputs are named

## Walkthrough Steps

1. Build the native toolchain:

   ```sh
   npm run build:objc3c-native
   ```

2. Compile `auroraBoard` to see the familiar object-model shape:

   ```sh
   npm run compile:objc3c -- showcase/auroraBoard/main.objc3
   ```

3. Compile `signalMesh` to see the async and executor-facing surface:

   ```sh
   npm run compile:objc3c -- showcase/signalMesh/main.objc3
   ```

4. Compile `patchKit` to see the imported-hook and macro-backed interop edge:

   ```sh
   npm run compile:objc3c -- showcase/patchKit/main.objc3
   ```

5. Check the full showcase surface and integrated validation:

   ```sh
   npm run check:showcase:surface
   npm run test:showcase
   ```

## Why This Order

- `auroraBoard` establishes the familiar ObjC shape first
- `signalMesh` adds async and executor behavior without changing the command model
- `patchKit` closes the walkthrough with the current interop and macro surface
- the final showcase checks prove that the same examples still pass the shared repo validation path

## Walkthrough Assets

The checked-in walkthrough manifest lives at `showcase/tutorial_walkthrough.json`.

That manifest is part of the live showcase surface and should stay aligned with:

- `showcase/portfolio.json`
- `showcase/README.md`
- `docs/tutorials/guided_walkthrough.md`
- `docs/tutorials/build_run_verify.md`

The bounded validation surface for this walkthrough is `scripts/check_getting_started_surface.py`.
The live smoke integration for the same walkthrough is `scripts/check_getting_started_integration.py`.
Use `npm run test:getting-started` when you want the public integrated entrypoint for that same flow.

## Canonical Inputs

- `docs/tutorials/guided_walkthrough.md`
- `docs/tutorials/build_run_verify.md`
- `showcase/tutorial_walkthrough.json`
- `showcase/portfolio.json`
- `showcase/auroraBoard/main.objc3`
- `showcase/signalMesh/main.objc3`
- `showcase/patchKit/main.objc3`
- `scripts/check_getting_started_surface.py`
- `scripts/check_getting_started_integration.py`
- `docs/runbooks/objc3c_public_command_surface.md`

## Exact Live Paths For Downstream Work

- tutorial narrative:
  - `docs/tutorials/guided_walkthrough.md`
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/getting_started.md`
- walkthrough contract:
  - `showcase/tutorial_walkthrough.json`
  - `showcase/portfolio.json`
  - `showcase/README.md`
- example sources:
  - `showcase/auroraBoard/main.objc3`
  - `showcase/signalMesh/main.objc3`
  - `showcase/patchKit/main.objc3`
- command truth and validation:
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `scripts/check_getting_started_surface.py`
  - `scripts/check_getting_started_integration.py`
  - `scripts/check_showcase_surface.py`

## Explicit Non-Goals

- no walkthrough asset that bypasses the public command surface
- no sidecar examples outside `showcase/`
- no hidden tutorial step rooted only in `tmp/` outputs
