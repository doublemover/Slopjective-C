---
title: Objective-C 3.0 Draft Specification
layout: default
---

# Objective-C 3.0 Draft Specification <a id="toc"></a>

_Working draft v0.11_  
_Last updated: 2026-05-09_

Objective-C 3.0 is a native compiler and runtime effort aimed at a safer, more explicit, still recognizably Objective-C language mode. This page is the public overview of the draft and the current implementation. It is intentionally curated: support claims route through the registry-backed capability matrix and evidence map instead of archived planning notes, and public commands route through `npm run objc3c -- <action>`.

> Current status: the project has a real native compiler, real LLVM IR/object emission, and a runnable subset. Full runtime realization of the Objective-C 3.0 object model remains unclaimed until the capability matrix marks it implemented with evidence.

## At a Glance <a id="toc-status-scope-note"></a>

Only the capability matrix states are support states. This overview points at
those rows instead of inventing local status vocabulary.

| Area                       | Matrix boundary | Notes                                                                                                                                       |
| -------------------------- | ------------ | ------------------------------------------------------------------------------------------------------------------------------------------- |
| Native compiler pipeline   | internal     | `objc3c` parses `.objc3`, emits diagnostics, manifests, LLVM IR, objects, and executables through split compiler/runtime/pipeline/artifact/IO owner modules. |
| Runnable language subset   | implemented  | Parser, sema, lowering, IR, runtime strict-dispatch diagnostic, and e2e smoke rows carry executable evidence.                                |
| Object-model declarations  | internal     | Parser/sema/metadata support exists, but executable object-model behavior is not widened beyond matrix rows.                                 |
| Runtime metadata emission  | internal     | Class, protocol, category, property, ivar, selector, and string metadata are implementation evidence, not full runtime support claims.        |
| Runtime dispatch result    | internal     | Strict dispatch and registration route through the public C runtime API; no alternate dispatch mode is documented.                           |
| Advanced language features | reserved     | Blocks, ARC automation, `throws`, async/await, actors, tasks, macros, and broader interop stay unavailable until implemented matrix rows say otherwise. |
| Retired/alternate surfaces | not a support state | Old modes, aliases, shims, fallbacks, migration lanes, direct helper commands, and report-only completion are negative evidence only.         |

## How to Read This Draft <a id="toc-how-to-read-this-draft"></a>

Use this page in three passes:

1. Read the status sections below to understand what is real today.
2. Use the spec map to find the normative area you care about.
3. Use the registry-backed capability matrix and evidence map when you need to verify a support claim.

## Quick Routes <a id="toc-quick-routes"></a>

| If you want to...                                        | Start here                                                                                      |
| -------------------------------------------------------- | ----------------------------------------------------------------------------------------------- |
| understand what already works                            | [At a Glance](#toc-status-scope-note)                                                           |
| see the runnable subset                                  | [What Is Implemented and Runnable](#intro)                                                      |
| understand what is still missing                         | [What Remains](#status-remaining)                                                               |
| follow the reader-facing learning path                   | [docs/tutorials/README.md](../docs/tutorials/README.md)                                         |
| start with the runnable getting-started tutorial         | [docs/tutorials/getting_started.md](../docs/tutorials/getting_started.md)                       |
| pick a capability-backed showcase example first          | [showcase/README.md](../showcase/README.md)                                                     |
| see the tutorial build run and verify flow               | [docs/tutorials/build_run_verify.md](../docs/tutorials/build_run_verify.md)                     |
| follow the guided showcase walkthrough                   | [docs/tutorials/guided_walkthrough.md](../docs/tutorials/guided_walkthrough.md)                 |
| map ObjC2 patterns to canonical ObjC3 examples          | [docs/tutorials/objc2_to_objc3_migration.md](../docs/tutorials/objc2_to_objc3_migration.md)     |
| compare ObjC3 against ObjC2, Swift, and C++ expectations | [docs/tutorials/objc2_swift_cpp_comparison.md](../docs/tutorials/objc2_swift_cpp_comparison.md) |
| evaluate adoption, support, and claim boundaries         | [docs/runbooks/objc3c_adoption_legibility.md](../docs/runbooks/objc3c_adoption_legibility.md)   |
| find the right draft section                             | [Specification Map](#toc-front-matter)                                                          |
| build and validate the implementation                    | [README.md](../README.md)                                                                       |
| inspect the native implementation boundary               | [docs/objc3c-native.md](../docs/objc3c-native.md) and `native/objc3c/`                          |
| verify support status and evidence                       | [capability matrix](../docs/support/capability_matrix.md)                                       |
| inspect executable evidence for support claims           | [evidence map](../docs/support/evidence_map.md)                                                 |
| inspect hard-cutover support boundaries                  | [hard-cutover capability truth](../docs/support/hard_cutover_capability_truth.md)                |
| inspect machine-readable capability truth                | [capability matrix JSON](../docs/support/capability_matrix.json), [evidence map JSON](../docs/support/evidence_map.json), and [schema registry](../scripts/objc3c_shared/schema_registry.py) |
| inspect public command ownership                         | [docs/runbooks/objc3c_public_command_surface.md](../docs/runbooks/objc3c_public_command_surface.md) |

## Reader Promises <a id="toc-reader-promises"></a>

This page follows a strict public-doc model:

- status before aspiration,
- plain language before internal jargon,
- direct links before repo scavenger hunts,
- current implementation truth before historical narrative,
- matrix states instead of local support adjectives,
- command examples through `npm run objc3c -- <action>`,
- and tutorial routing through checked-in learning paths and showcase sources instead of archived planning material.

## What Is Implemented and Runnable <a id="intro"></a>

The current native toolchain can compile and run a real subset of Objective-C 3.0:

- modules and global `let` declarations,
- `fn`, `pure fn`, and external function declarations,
- scalar/control-flow semantics including `if`, `while`, `do while`, `for`, `switch`, `break`, `continue`, and `return`,
- integer and boolean values in the canonical runnable subset,
- bracket message-send syntax lowered through the current runtime dispatch path,
- strict runtime dispatch diagnostics through `objc3_runtime_dispatch_i32_checked` and the public runtime result surface,
- workflow dispatch through `npm run objc3c -- <action>`,
- native ownership-baseline runtime behavior for retainable object storage,
- deterministic selector/string pool emission and metadata-bearing object artifacts.

This is a real compiler/runtime path, not parser scaffolding. It is still only a
public support claim where the capability matrix marks a behavior implemented
with evidence.

## Internal Evidence That Is Not Public Runtime Support <a id="decisions"></a>

The compiler already understands much more of the object-model surface than the
runtime can fully execute today. The capability matrix keeps this as internal or
reserved evidence until executable behavior is proven.

Implemented in parser, semantic passes, and emitted metadata:

- `@interface` and `@implementation`,
- protocols and categories,
- methods, properties, and ivars,
- protocol required/optional partitioning,
- category merge/conflict rules,
- object-model legality checks,
- class, metaclass, protocol, category, property, and ivar descriptor families,
- registration/bootstrap metadata and related artifact plumbing.
- split native ownership across compiler, runtime, pipeline,
  artifacts, and IO modules for lowering, IR, JSON/schema artifacts, dispatch
  classification, and public runtime C API boundaries.

What is still incomplete is the last step: consuming all of that emitted
metadata as a fully live runtime object system. The matrix keeps those owner
files as internal evidence until executable behavior is proven.

## Reserved And Unclaimed Runtime Areas <a id="status-remaining"></a>

The biggest remaining gaps are runtime-completion gaps, not parser-only gaps.
They remain unclaimed public support until implemented matrix rows link
executable evidence:

1. Finish runtime bootstrap and multi-image registration.
2. Bind emitted methods, properties, and ivars to live runtime realization.
3. Complete executable class, protocol, category, and property behavior.
4. Complete reflective/runtime consumption of property and layout metadata.
5. Close cross-module runtime import and packaging semantics.
6. Finish blocks, captures, byref state, and ARC automation.
7. Keep `throws`, richer error propagation, async/await, tasks, actors, metaprogramming, and interop closure reserved until exact evidence lands.

## Specification Map <a id="toc-front-matter"></a>

The draft is organized into a small set of cross-cutting reference documents plus the numbered language parts.

### Attribute and Syntax Catalog <a id="b"></a>

Canonical spellings for attributes, pragmas, and source-surface forms that must remain stable across modules and tooling.

### Lowering and Runtime Contracts <a id="c"></a>

Implementation-facing rules for lowering, ABI boundaries, runtime hooks, and deterministic artifact behavior.

### Module Metadata and ABI Surface Tables <a id="d"></a>

The cross-module contract surface: what importers must preserve, what is ABI-affecting, what fails closed, and what the implementation truthfully supports today.

#### D.0 Purpose <a id="d-0"></a>

Section D exists to keep the project honest at module boundaries. It defines what metadata must survive import/export and which surfaces are ABI-significant.

#### D.1 Required Cross-Module Surface <a id="d-1"></a>

At minimum, a conforming implementation needs stable preservation for:

- module identity and versioned importer requirements,
- declaration signatures and dispatch-affecting attributes,
- class/protocol/category/property/ivar metadata,
- layout- and registration-relevant runtime records,
- importer validation and fail-closed capability gates.

#### D.2 Current Implementation Status <a id="d-2"></a>

Today the project has:

- a real native compiler,
- real LLVM/object emission,
- real emitted metadata sections,
- and a runnable subset.

It does **not** yet have the full live Objective-C 3.0 object model end to end.

#### D.3 Current Priorities <a id="d-3"></a>

Current work is concentrated on runtime completion:

- bootstrap and registration,
- live object realization,
- property/ivar/runtime layout behavior,
- module/runtime import closure,
- blocks and ARC.

### Conformance Profile Checklist <a id="e"></a>

Profiles and minimum obligations for claiming support. This is where feature claims are supposed to become testable rather than aspirational.

### Standard Library Contract <a id="s"></a>

Minimum required standard-library surface, distribution expectations, and versioning assumptions for any implementation that wants to claim conformance.

### Abstract Machine and Semantic Core <a id="am"></a>

The common semantic model for evaluation, lifetime, cleanup, and suspension behavior.

### Formal Grammar and Precedence <a id="f"></a>

The integrated grammar and operator-precedence appendix. Use this when surface syntax questions need precise answers.

## Language Parts <a id="toc-parts"></a>

### Part 0 — Baseline and Normative References <a id="part-0"></a>

Defines the baseline language, pinned references, and conflict-resolution model.

### Part 1 — Canonical Language Mode And Conformance <a id="part-1"></a>

Defines the single Objective-C 3.0 language mode, capability states, rejected-form diagnostics, and evidence-backed conformance claims.

### Part 2 — Modules, Namespacing, and API Surfaces <a id="part-2"></a>

Defines import boundaries, visibility, module-owned declarations, and public surface rules.

### Part 3 — Types, Nullability, Optionals, Generics, and Key Paths <a id="part-3"></a>

Defines type-surface modernization: nullability, optionals, pragmatic generics, and typed key-path concepts.

### Part 4 — Memory Management and Ownership <a id="part-4"></a>

Defines ownership qualifiers, retainability, ARC-facing concepts, and lifetime contracts.

### Part 5 — Control Flow and Safety Constructs <a id="part-5"></a>

Defines `defer`, `guard`, pattern/match-style control-flow constructs, and related safety semantics.

### Part 6 — Errors, Result, and Throws <a id="part-6"></a>

Defines structured error propagation, result surfaces, NSError/status bridging, and effect rules.

### Part 7 — Concurrency, Async/Await, Tasks, and Actors <a id="part-7"></a>

Defines the reserved concurrency model: async/await, executor hopping, tasks, cancellation, and actor isolation.

### Part 8 — System Programming Extensions <a id="part-8"></a>

Defines low-level safety and ergonomics for resource handles, borrowed pointers, lifetime-sensitive APIs, and other systems-facing surfaces.

### Part 9 — Performance and Dynamism Controls <a id="part-9"></a>

Defines direct/final/sealed-style controls and explicit dispatch/performance boundaries.

### Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors <a id="part-10"></a>

Defines the reserved boilerplate-reduction surface without turning the language into an opaque macro system.

### Part 11 — Interoperability with C, C++, and Swift <a id="part-11"></a>

Defines foreign-language boundaries, bridging, ABI-facing interop, and distribution expectations.

### Part 12 — Diagnostics, Tooling, and Tests <a id="part-12"></a>

Defines the required diagnostics, canonicalization fix-its, analyzers, and conformance evidence expected from a serious implementation.

## Implementation Reality Check <a id="e-1"></a>

The project is no longer in the “just grammar” phase. The compiler and runtime work are materially real. The missing work is concentrated in the hardest part of the system: closing the loop between emitted object-model metadata and fully realized native runtime behavior.

That is the right place to be honest:

- the compiler can already do useful native work,
- the spec is much broader than the currently runnable subset,
- and the remaining work is reserved runtime closure until evidence moves exact
  rows to `implemented`.
