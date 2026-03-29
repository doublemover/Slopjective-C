# ObjC2 Swift And C++ Comparison Surface

This file is the live comparison boundary for migration-oriented teaching work.

Use it to explain Objective-C 3 choices against:

- Objective-C 2 source habits,
- Swift-facing API and ownership expectations,
- and C/C++ interop boundary expectations.

Start with `docs/tutorials/objc2_to_objc3_migration.md` when you need the actual migration sequence. Use this file for the broader comparison boundary after the runnable examples and migration path are already clear.

## Comparison Boundary

This comparison surface should stay grounded in checked-in implementation truth.

- compare against the current runnable subset, not future wish lists
- use the checked-in showcase examples when a comparison needs executable source
- prefer concrete language-surface and command-surface differences over marketing language
- keep maintainer-only implementation detail out of the reader path unless it changes a user-visible migration choice

## Canonical Inputs

Downstream comparison and migration work should use these live inputs directly:

- `docs/tutorials/objc2_swift_cpp_comparison.md`
- `docs/tutorials/objc2_to_objc3_migration.md`
- `docs/tutorials/README.md`
- `README.md`
- `site/src/index.body.md`
- `showcase/README.md`
- `showcase/portfolio.json`
- `showcase/patchKit/main.objc3`
- `showcase/signalMesh/main.objc3`
- `docs/runbooks/objc3c_public_command_surface.md`

## Exact Live Paths For Downstream Work

- comparison narrative and migration language:
  - `docs/tutorials/objc2_to_objc3_migration.md`
  - `docs/tutorials/objc2_swift_cpp_comparison.md`
  - `docs/tutorials/README.md`
- onboarding and public routing:
  - `README.md`
  - `site/src/index.body.md`
- compile-coupled examples used in teaching material:
  - `showcase/README.md`
  - `showcase/portfolio.json`
  - `showcase/patchKit/main.objc3`
  - `showcase/signalMesh/main.objc3`
- command truth and validation references:
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `scripts/check_documentation_surface.py`

## Explicit Non-Goals

- no parity-table claims for unsupported Swift or C++ behavior
- no tutorial text that treats archived ObjC2 material as a normative source
- no fake migration promises that are not backed by the runnable subset or checked-in examples
- no separate sidecar command list when `package.json` and the public command surface already define the entrypoints
