# Objective‑C 3.0 Draft Specification (Working Draft) — Table of Contents
_Last generated: 2025-12-28_

This is a working draft of an **Objective‑C 3.0** specification skeleton expanded into normative, implementable parts. Each part is a separate Markdown file.

## Front matter
- **00_TABLE_OF_CONTENTS.md** — This table of contents (you are here)
- **01_INTRODUCTION.md** — Overview, goals, guiding principles, non‑goals, and cross‑cutting design choices

## Parts
- **02_PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md** — Baseline Objective‑C (as implemented by Clang) and normative reference model
- **03_PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md** — Language modes (“edition-like”), feature gating, conformance levels, and migration tooling requirements
- **04_PART_2_MODULES_NAMESPACING_API_SURFACES.md** — Modules, imports, module-qualified names, visibility, and API surface contracts
- **05_PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md** — Nullability by default, optional binding, pragmatic generics, and typed key paths
- **06_PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md** — ARC, ownership qualifiers, lifetime rules, transfer/consumption, and interaction with retainable families
- **07_PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md** — `defer`, `guard`, `match`, patterns, and control-flow rules
- **08_PART_6_ERRORS_RESULTS_THROWS.md** — `Result`, `throws`, propagation (`?`), and interoperability with NSError and return-code APIs
- **09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md** — `async/await`, executors, cancellation, structured concurrency, actors, and sendable-like checking
- **10_PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md** — System-library accommodation: resource handles, retainable C families, borrowed pointers, capture lists, lifetime control
- **11_PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md** — Direct methods, `final`/`sealed`, dynamic/static boundaries, and optimization contracts
- **12_PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md** — Derives, AST macros, property behaviors/wrappers, and safety constraints
- **13_PART_11_INTEROPERABILITY_C_CPP_SWIFT.md** — C/ObjC/ObjC++/Swift interop rules and ABI mapping
- **14_PART_12_DIAGNOSTICS_TOOLING_TESTS.md** — Required diagnostics, fix-its, migrators, analyzers, and conformance test suites

## How to read this draft
1. Start with **01_INTRODUCTION.md** (goals + cross-cutting constraints).
2. Read **Part 1** next (versioning and migration).
3. Then choose a “slice” (types/errors/concurrency/system/perf) depending on what you’re implementing.

## Status / scope note
This draft is intentionally **ambitious but implementable**: it prefers features that can be lowered by a compiler front end (Clang) to existing ABIs and/or small runtime libraries, and it makes “strictness” opt-in via conformance levels.

## v0.3 note
Parts 3, 6, and 7 have been expanded into spec‑grade drafts (grammar, semantics, diagnostics) in this v0.3 bundle.
