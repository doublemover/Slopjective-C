---
title: Objective-C 3.0 Draft Specification
layout: default
---

# Objective-C 3.0 Draft Specification <a id="toc"></a>

_Working draft v0.11_  
_Last updated: 2026-03-11_

Objective-C 3.0 is a native compiler and runtime effort aimed at a safer, more explicit, still recognizably Objective-C language mode. This page is the public overview of the draft and the current implementation. It is intentionally curated: the archived `spec/` corpus now sits behind a compatibility redirect index, but this page is where the project should explain itself cleanly.

> Current status: the project has a real native compiler, real LLVM IR/object emission, and a runnable subset. Full runtime realization of the Objective-C 3.0 object model is still in progress.

## At a Glance <a id="toc-status-scope-note"></a>

| Area                       | Status           | Notes                                                                                                                            |
| -------------------------- | ---------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| Native compiler pipeline   | Implemented      | `objc3c` parses `.objc3`, emits diagnostics, manifests, LLVM IR, objects, and executables.                                       |
| Runnable language subset   | Implemented      | Functions, control flow, scalar types, and bracket message sends compile and run natively.                                       |
| Object-model declarations  | Partial          | `@interface`, `@implementation`, `@protocol`, `@category`, and `@property` have meaningful parser/sema/metadata support.         |
| Runtime metadata emission  | Partial          | Class, protocol, category, property, ivar, selector, and string metadata now emit into object artifacts.                         |
| Runtime realization        | In progress      | Bootstrap, registration, method binding, property/ivar realization, and reflective/runtime consumption are not fully closed out. |
| Advanced language features | Not yet runnable | Blocks, ARC automation, `throws`, async/await, actors, tasks, macros, and broader interop remain future work.                    |

## How to Read This Draft <a id="toc-how-to-read-this-draft"></a>

Use this page in three passes:

1. Read the status sections below to understand what is real today.
2. Use the spec map to find the normative area you care about.
3. Use the legacy spec redirect index only when you need compatibility links into archived language rules, ABI notes, or compatibility constraints.

## Quick Routes <a id="toc-quick-routes"></a>

| If you want to... | Start here |
| --- | --- |
| understand what already works | [At a Glance](#toc-status-scope-note) |
| see the runnable subset | [What Is Implemented and Runnable](#intro) |
| understand what is still missing | [What Remains](#status-remaining) |
| follow the reader-facing learning path | [docs/tutorials/README.md](../docs/tutorials/README.md) |
| start with the runnable getting-started tutorial | [docs/tutorials/getting_started.md](../docs/tutorials/getting_started.md) |
| pick a capability-backed showcase example first | [showcase/README.md](../showcase/README.md) |
| see the tutorial build run and verify flow | [docs/tutorials/build_run_verify.md](../docs/tutorials/build_run_verify.md) |
| follow the guided showcase walkthrough | [docs/tutorials/guided_walkthrough.md](../docs/tutorials/guided_walkthrough.md) |
| follow the ObjC2-to-ObjC3 migration guide | [docs/tutorials/objc2_to_objc3_migration.md](../docs/tutorials/objc2_to_objc3_migration.md) |
| compare ObjC3 against ObjC2, Swift, and C++ expectations | [docs/tutorials/objc2_swift_cpp_comparison.md](../docs/tutorials/objc2_swift_cpp_comparison.md) |
| find the right draft section | [Specification Map](#toc-front-matter) |
| build and validate the implementation | [README.md](../README.md) |
| inspect the native implementation boundary | [docs/objc3c-native.md](../docs/objc3c-native.md) and `native/objc3c/` |
| follow old spec links | [legacy spec redirects](../docs/reference/legacy_spec_anchor_index.md#legacy-files) |

## Reader Promises <a id="toc-reader-promises"></a>

This page follows a strict public-doc model:

- status before aspiration,
- plain language before internal jargon,
- direct links before repo scavenger hunts,
- current implementation truth before historical narrative,
- and tutorial routing through checked-in learning paths and showcase sources instead of archived planning material.

## What Is Implemented and Runnable <a id="intro"></a>

The current native toolchain can compile and run a real subset of Objective-C 3.0:

- modules and global `let` declarations,
- `fn`, `pure fn`, and external function declarations,
- scalar/control-flow semantics including `if`, `while`, `do while`, `for`, `switch`, `break`, `continue`, and `return`,
- integer, boolean, and baseline alias surfaces such as `BOOL`, `NSInteger`, and `NSUInteger`,
- Objective-C-flavored signature aliases such as `id`, `Class`, `SEL`, `Protocol`, and `instancetype`,
- bracket message-send syntax lowered through the current runtime dispatch path,
- native ownership-baseline runtime behavior for retainable object storage,
- deterministic selector/string pool emission and metadata-bearing object artifacts.

This is a real compiler/runtime path, not just parser scaffolding.

## What Is Implemented but Not Yet Fully Live <a id="decisions"></a>

The compiler already understands much more of the object-model surface than the runtime can fully execute today.

Implemented in parser, semantic passes, and emitted metadata:

- `@interface` and `@implementation`,
- protocols and categories,
- methods, properties, and ivars,
- protocol required/optional partitioning,
- category merge/conflict rules,
- object-model legality checks,
- class, metaclass, protocol, category, property, and ivar descriptor families,
- registration/bootstrap metadata and related artifact plumbing.

What is still incomplete is the last step: consuming all of that emitted metadata as a fully live runtime object system.

## What Remains <a id="status-remaining"></a>

The biggest remaining gaps are runtime-completion gaps, not parser-only gaps:

1. Finish runtime bootstrap and multi-image registration.
2. Bind emitted methods, properties, and ivars to live runtime realization.
3. Complete executable class, protocol, category, and property behavior.
4. Complete reflective/runtime consumption of property and layout metadata.
5. Close cross-module runtime import and packaging semantics.
6. Finish blocks, captures, byref state, and ARC automation.
7. Then extend outward into `throws`, richer error propagation, async/await, tasks, actors, metaprogramming, and interop closure.

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

- module identity and compatibility information,
- declaration signatures and dispatch-affecting attributes,
- class/protocol/category/property/ivar metadata,
- layout- and registration-relevant runtime records,
- importer validation and fail-closed compatibility gates.

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

### Part 1 — Versioning, Compatibility, and Conformance <a id="part-1"></a>

Defines language modes, version claims, feature gating, and what it means to support Objective-C 3.0 truthfully.

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

Defines the future concurrency model: async/await, executor hopping, tasks, cancellation, and actor isolation.

### Part 8 — System Programming Extensions <a id="part-8"></a>

Defines low-level safety and ergonomics for resource handles, borrowed pointers, lifetime-sensitive APIs, and other systems-facing surfaces.

### Part 9 — Performance and Dynamism Controls <a id="part-9"></a>

Defines direct/final/sealed-style controls and explicit dispatch/performance boundaries.

### Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors <a id="part-10"></a>

Defines the future boilerplate-reduction surface without turning the language into an opaque macro system.

### Part 11 — Interoperability with C, C++, and Swift <a id="part-11"></a>

Defines foreign-language boundaries, bridging, ABI-facing interop, and distribution expectations.

### Part 12 — Diagnostics, Tooling, and Tests <a id="part-12"></a>

Defines the required diagnostics, fix-its, migrators, analyzers, and conformance evidence expected from a serious implementation.

## Implementation Reality Check <a id="e-1"></a>

The project is no longer in the “just grammar” phase. The compiler and runtime work are materially real. The missing work is concentrated in the hardest part of the system: closing the loop between emitted object-model metadata and fully realized native runtime behavior.

That is the right place to be honest:

- the compiler can already do useful native work,
- the spec is much broader than the currently runnable subset,
- and the remaining work is runtime completion, not hand-wavy future intent.
