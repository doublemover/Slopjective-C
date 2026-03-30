# ObjC2 Swift And C++ Comparison Surface

This file is the live comparison boundary for migration-oriented teaching work.

Use it to explain Objective-C 3 choices against:

- Objective-C 2 source habits,
- Swift-facing API and ownership expectations,
- and C/C++ interop boundary expectations.

Start with `docs/tutorials/objc2_to_objc3_migration.md` when you need the actual migration sequence. Use this file for the broader comparison boundary after the runnable examples and migration path are already clear.

## Start With The Capability That Matches The Question

Do not start with abstract parity claims. Start with the checked-in example that
matches the capability in question:

| Question | Checked-in example | Then read |
| --- | --- | --- |
| How do familiar ObjC2 categories and object-model habits map forward? | `showcase/auroraBoard/main.objc3` | `docs/tutorials/objc2_to_objc3_migration.md` |
| What does current ObjC3 messaging and actor-shaped workflow look like next to Swift expectations? | `showcase/signalMesh/main.objc3` | this file |
| What is the current macro derive property-behavior and interop story next to Swift and C++ expectations? | `showcase/patchKit/main.objc3` | this file |

The comparison text should explain the current runnable and compile-coupled
surface, not an imagined future language.

Stdlib follow-up after the comparison examples:

- `signalMesh` -> `objc3.concurrency`, `objc3.system`
- `patchKit` -> `objc3.keypath`, `objc3.system`
- use `stdlib/README.md` when you need the checked-in stdlib boundary behind the
  same examples

## Comparison Boundary

This comparison surface should stay grounded in checked-in implementation truth.

- compare against the current runnable subset, not future wish lists
- use the checked-in showcase examples when a comparison needs executable source
- prefer concrete language-surface and command-surface differences over marketing language
- keep maintainer-only implementation detail out of the reader path unless it changes a user-visible migration choice

## Current Truthful Comparison Shape

Today the comparison surface can make these kinds of claims truthfully:

- ObjC3 already has a real compile-and-run path backed by checked-in examples
- the showcase stories are the evidence for what the docs say about categories,
  reflection, messaging, derives, macros, property behaviors, and current
  interop shape
- Swift and C++ comparisons should stay at the level of current capability and
  ergonomics, not pretend that unsupported parity already exists
- ObjC2 comparisons should focus on what a reader can migrate or evaluate from
  the runnable subset today

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
- `tests/tooling/fixtures/performance/benchmark_portfolio.json`
- `tests/tooling/fixtures/performance/baselines/objc2_reference_workload.m`
- `tests/tooling/fixtures/performance/baselines/swift_reference_workload.swift`
- `tests/tooling/fixtures/performance/baselines/cpp_reference_workload.cpp`
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
- checked-in comparative workload inventory:
  - `tests/tooling/fixtures/performance/benchmark_portfolio.json`
  - `tests/tooling/fixtures/performance/baselines/objc2_reference_workload.m`
  - `tests/tooling/fixtures/performance/baselines/swift_reference_workload.swift`
  - `tests/tooling/fixtures/performance/baselines/cpp_reference_workload.cpp`
- command truth and validation references:
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `scripts/check_documentation_surface.py`

## Explicit Non-Goals

- no parity-table claims for unsupported Swift or C++ behavior
- no tutorial text that treats archived ObjC2 material as a normative source
- no fake migration promises that are not backed by the runnable subset or checked-in examples
- no separate sidecar command list when `package.json` and the public command surface already define the entrypoints
