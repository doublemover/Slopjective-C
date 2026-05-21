# ObjC2 To ObjC3 Canonicalization Guide

This file is the live guide for mapping familiar ObjC2 habits onto the current canonical ObjC3 runnable subset.

The checked-in adoption-legibility contract for this guide is
`tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json`;
replay it through `npm run objc3c -- validate-adoption-legibility`.
This reader-facing guide is canonicalization guidance over runnable examples,
not a retired-source acceptance surface.
Keep this guide narrower than that contract, never broader.

Use it together with the checked-in showcase examples. The guide is only authoritative where the repo already compiles or validates the behavior it describes.

## Canonicalization Boundary

This guide is intentionally narrow.

- start from runnable examples, not archived ObjC2 prose
- preserve the recognizable object-model shape where the current compiler already supports it
- treat async, executor, and imported-module examples as the Swift-facing interop boundary
- use the public package scripts and showcase surface instead of ad hoc compile commands
- treat unsupported parity claims as out of scope until the runnable subset proves them

## Step 1 Keep The Familiar ObjC Shape, Drop The Implicit Assumptions

The fastest path from ObjC2 to the current ObjC3 subset is to keep the recognizable interface and implementation layout, while removing assumptions that the old runtime filled in implicitly.

Use `showcase/auroraBoard/main.objc3` as the first canonicalization example:

- protocols still declare required methods and properties
- `@interface` and `@implementation` remain the main structural units
- categories remain the live extension model
- properties still carry explicit ownership and accessor shape

Compile it through the normal surface:

```sh
npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3
```

What to keep from ObjC2 habits:

- interface and implementation grouping
- protocol-driven API shape
- category-based extension points
- explicit property declarations

What to drop:

- assuming runtime realization is broader than the checked-in runnable subset
- assuming property behavior exists unless the showcase and validation surface already prove it

## Step 2 Map Properties And Categories Through The Runnable Object Model

`auroraBoard` is the best canonical ObjC3 example for object-model habits.

Read it in this order:

1. `BoardReadable` for the protocol/property contract
2. `Tile` for the explicit property ownership and custom accessor names
3. `Tile (Inspection)` for the category extension path

Canonicalization takeaway:

- ObjC3 keeps the surface recognizable, but the contract is more explicit and more tightly tied to the live compiler/runtime path
- if your old ObjC2 code relied on vague synthesized behavior, reduce it to the specific patterns already validated by the showcase and runtime acceptance paths

## Step 3 Treat Swift-Facing Async And Imported Hooks As Explicit Interop Contracts

Use `signalMesh` and `patchKit` for Swift-facing expectations.

`showcase/signalMesh/main.objc3` shows the current async/executor boundary:

- async methods are declared directly on the Objective-C surface
- executor placement is explicit through `objc_executor(...)`
- message sends and async function calls still flow through checked-in runnable sources

Compile it with:

```sh
npm run objc3c -- compile-objc3c showcase/signalMesh/main.objc3
```

`showcase/patchKit/main.objc3` shows the current imported-module and macro-backed interop edge:

- imported hooks are explicit through `objc_import_module(...)`
- derive and macro use is named and provenance-bearing
- the guide should treat this as a real interop boundary, not as evidence for full Swift parity

Compile it with:

```sh
npm run objc3c -- compile-objc3c showcase/patchKit/main.objc3
```

## Step 4 Use The Showcase Validation Surface Before You Claim Support

Do not stop after a single compile.

Run the checked-in portfolio surface:

```sh
npm run objc3c -- check-showcase-surface
npm run objc3c -- validate-showcase
```

That keeps canonicalization teaching tied to the same examples the repo already compiles and runs.

## Step 5 Replay The Checked-In Migration Examples

The migration analyzer has checked-in input and output examples for an ObjC2
source that touches Swift and C++ bridge boundaries. Use them when you want to
see what the current migration tooling can safely rewrite and what it leaves as
manual work.

Analyze the positive example:

```sh
npm run objc3c -- analyze-migration-source tests/tooling/fixtures/adoption_legibility/migration_inputs/objc2_swift_cpp_positive.json
```

Apply the safe rewrite plan to a machine-owned output path:

```sh
npm run objc3c -- rewrite-migration-source tests/tooling/fixtures/adoption_legibility/migration_inputs/objc2_swift_cpp_positive.json --output tmp/artifacts/migration-analyzer/objc2-swift-cpp-positive/rewritten.objc3
```

Validate the positive and fail-closed negative examples together:

```sh
npm run objc3c -- validate-migration-workflow
```

The durable source truth for those examples lives under
`tests/tooling/fixtures/adoption_legibility/`. The analyzer may write reports
under `tmp/`, but those reports are outputs, not the source of the migration
claim.

## Recommended Reading Order

- start with `docs/tutorials/getting_started.md`
- use this guide to map ObjC2 habits onto canonical ObjC3 examples
- use `docs/tutorials/objc2_swift_cpp_comparison.md` when you need the broader comparison boundary
- use `showcase/README.md` when you want the example portfolio and workspace contracts

## Canonical Inputs

- `docs/tutorials/objc2_to_objc3_migration.md`
- `docs/tutorials/getting_started.md`
- `docs/tutorials/objc2_swift_cpp_comparison.md`
- `showcase/README.md`
- `showcase/portfolio.json`
- `showcase/auroraBoard/main.objc3`
- `showcase/signalMesh/main.objc3`
- `showcase/patchKit/main.objc3`
- `tests/tooling/fixtures/adoption_legibility/migration_inputs/objc2_swift_cpp_positive.json`
- `tests/tooling/fixtures/adoption_legibility/migration_outputs/objc2_swift_cpp_positive_rewritten.objc3`
- `tests/tooling/fixtures/adoption_legibility/migration_outputs/objc2_swift_cpp_negative_diagnostics.json`
- `docs/runbooks/objc3c_public_command_surface.md`

## Exact Live Paths For Downstream Work

- canonicalization narrative and comparison routing:
  - `docs/tutorials/objc2_to_objc3_migration.md`
  - `docs/tutorials/objc2_swift_cpp_comparison.md`
  - `docs/tutorials/README.md`
- getting-started and onboarding routes:
  - `docs/tutorials/getting_started.md`
  - `README.md`
  - `site/src/index.body.md`
- executable examples backing the canonicalization claims:
  - `showcase/README.md`
  - `showcase/portfolio.json`
  - `showcase/auroraBoard/main.objc3`
  - `showcase/signalMesh/main.objc3`
  - `showcase/patchKit/main.objc3`
- command truth and documentation guardrails:
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `npm run objc3c -- check-documentation-surface`
  - `scripts/check_documentation_surface.py`

## Explicit Non-Goals

- no promise of full ObjC2 runtime parity
- no claim of broad Swift interop beyond the checked-in runnable examples
- no canonicalization advice rooted in `tmp/`, archived spec material, or maintainer-only notes
- no command names outside the canonical `npm run objc3c -- <action>` surface
