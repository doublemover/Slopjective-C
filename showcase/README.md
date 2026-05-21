# Showcase Examples

This directory is the live home for runnable narrative examples.

Use it for small examples that prove real compiler and runtime capabilities
through the normal repo command surface.

## Capability-First Entry Points

Choose the story that matches the question before you read deeper docs:

- `auroraBoard`
  - use this when the question starts with categories, reflection, or synthesized behaviors
  - pair it with `docs/tutorials/getting_started.md`
  - pair it with `docs/tutorials/objc2_to_objc3_migration.md` for ObjC2 pattern conversion notes
  - stdlib follow-up modules: `objc3.core`, `objc3.errors`, `objc3.keypath`
- `signalMesh`
  - use this when the question is about actor-shaped messaging, status bridging, or runtime messaging
  - pair it with `docs/tutorials/guided_walkthrough.md`
  - pair it with `docs/tutorials/objc2_swift_cpp_comparison.md` for Swift-facing comparison framing
  - stdlib follow-up modules: `objc3.concurrency`, `objc3.system`
- `patchKit`
  - use this when the question is about derives, macros, property behaviors, or interop
  - pair it with `docs/tutorials/objc2_swift_cpp_comparison.md`
  - use it when the question is about feature shape rather than the smallest possible first compile
  - stdlib follow-up modules: `objc3.keypath`, `objc3.system`

Use `stdlib/README.md` after choosing the example when you want the checked-in
stdlib module surface that matches the same story.

Use `applicationFrameworkSamples/README.md` when you want package-aware samples
instead of the compact three-example portfolio. That surface contains library,
CLI, runtime, and interop samples routed by
`npm run objc3c -- validate-application-framework-samples` and backed by the
application-framework capability rows in the support matrix.

## Adoption Evidence Anchors

The adoption evidence generator treats this portfolio as the runnable proof for
onboarding and comparison claims:

- `auroraBoard` anchors Objective-C 2 object-model pattern-conversion guidance.
- `signalMesh` anchors Swift-facing async, executor, and messaging comparison.
- `patchKit` anchors macro, derive, property-behavior, and C++-facing interop
  comparison.

Replay the adoption evidence with:

- `npm run objc3c -- validate-adoption-legibility`

The adoption evidence action writes transient adoption reports and artifacts.
Those outputs are not portfolio owner inputs; showcase support claims still
resolve through the capability matrix, evidence map, and checked-in example
sources.

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
- `showcase/applicationFrameworkSamples/manifest.json`
- `showcase/applicationFrameworkSamples/README.md`
- `docs/tutorials/application-framework-samples.md`

Shared live tooling:

- `package.json`
- `npm run objc3c -- <action>`
- `docs/tutorials/build_run_verify.md`
- `docs/tutorials/guided_walkthrough.md`

Retired command surfaces, direct helper scripts, showcase-local wrappers,
alternate compile/runtime support lanes, and retired-source support claims are not
public entrypoints for this portfolio.

Machine-owned outputs are transient build, package, showcase-report, and public
workflow report roots. They are not canonical example sources.

## Portfolio Stories

- `auroraBoard`
  - target story: categories, reflection, synthesized behaviors
  - stdlib follow-up: `objc3.core`, `objc3.errors`, `objc3.keypath`
- `signalMesh`
  - target story: status bridging, actor-shaped messaging, runtime messaging
  - stdlib follow-up: `objc3.concurrency`, `objc3.system`
- `patchKit`
  - target story: derives, macros, property behaviors, interop
  - stdlib follow-up: `objc3.keypath`, `objc3.system`

These examples must stay small and compile through the live native compiler
path. Later issues can expand the sources, but they must keep the same repo
roots and public command surface.

Selection model:

- compile the full portfolio with `npm run objc3c -- check-showcase-surface`
- compile one named example with
  `npm run objc3c -- check-showcase-surface --example auroraBoard`
- compile by story capability with
  `npm run objc3c -- check-showcase-surface --capability actor-shaped-messaging`

## Build Run Package Surface

The checked-in showcase contract is rooted at `showcase/portfolio.json`.
Each example directory also carries its own checked-in workspace contract at
`showcase/<example-id>/workspace.json`.
The ordered tutorial asset for this same surface is `showcase/tutorial_walkthrough.json`.

The tutorial-facing command and artifact map for this same surface lives in
`docs/tutorials/build_run_verify.md`.

Build and artifact entrypoints:

- `npm run objc3c -- build-native-binaries`
- `npm run objc3c -- check-showcase-surface`
- `npm run objc3c -- validate-showcase`
- `npm run objc3c -- validate-runnable-showcase`
- `npm run objc3c -- package-runnable-toolchain`
- implementation checker: `scripts/check_showcase_surface.py`
- showcase artifact root: `tmp/artifacts/showcase/`

Runtime-backed shared commands used by the showcase surface:

- `npm run objc3c -- test-execution-smoke`
- `npm run objc3c -- test-execution-replay`
- `npm run objc3c -- validate-showcase-runtime`

The live compile path emits object and manifest artifacts with the fixed emit
prefix `module`. Package staging and showcase report outputs stay in transient
output roots selected by the public workflow.

Runtime and presentation contracts are checked in per example under
`showcase/<example-id>/workspace.json`. Those workspace contracts declare the
runtime launch-contract helper, the authoritative runtime-library/linker-flag
resolution model, the expected process exit code, and the presentation headline
for the example.

## Explicit Non-Goals

- screenshots or image-only demos
- sidecar-only example manifests with no checked-in source
- example-specific compiler wrappers or milestone-scoped validation paths
- treating transient outputs as example owner sources
