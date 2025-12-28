# Objective‑C 3.0 Draft Specification (Working Draft) — Table of Contents
_Last generated: 2025-12-28_

This is a working draft of an **Objective‑C 3.0** specification broken into small, implementable parts. Each part is a separate Markdown file.

## Front matter
- **00_TABLE_OF_CONTENTS.md** — This table of contents (you are here)
- **01_INTRODUCTION.md** — Overview, goals, guiding principles, non‑goals, and cross‑cutting design choices
- **01A_DECISIONS_LOG.md** — Explicit “ship/no‑ship” design decisions and their rationales

## Parts
- **02_PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md** — Baseline Objective‑C (as implemented by Clang) and reference model for this spec
- **03_PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md** — Language mode gating, conformance levels, and migration tooling requirements
- **04_PART_2_MODULES_NAMESPACING_API_SURFACES.md** — Modules, module qualification, visibility, and API surface contracts
- **05_PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md** — Nullability-by-default, optionals/binding, pragmatic generics, typed key paths
- **06_PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md** — ARC, ownership qualifiers, transfers/consumption, and retainable families
- **07_PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md** — `defer`, `guard`, `match`/patterns, and cleanup ordering rules
- **08_PART_6_ERRORS_RESULTS_THROWS.md** — `throws`, `Result`, propagation (`?`), and interoperability with NSError/return codes
- **09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md** — `async/await`, executors, cancellation, actors, and sendable-like checking
- **10_PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md** — System-library affordances: retainable C families, borrows, lifetimes, capture lists
- **11_PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md** — Direct/final/sealed, dynamic/static boundaries, and optimization contracts
- **12_PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md** — Derives, AST macros, property behaviors/wrappers, and safety constraints
- **13_PART_11_INTEROPERABILITY_C_CPP_SWIFT.md** — C/ObjC/ObjC++/Swift interop rules and mapping guidance
- **14_PART_12_DIAGNOSTICS_TOOLING_TESTS.md** — Required diagnostics, fix‑its, migrators/analyzers, and test suite structure

## How to read this draft
1. Start with **01_INTRODUCTION.md** (goals + cross‑cutting constraints).
2. Read **01A_DECISIONS_LOG.md** (the “locked” decisions).
3. Read **Part 1** next (versioning, strictness, migration).
4. Then choose a “slice” (types/errors/concurrency/system/perf) depending on what you’re implementing.

## Status / scope note
This draft is intentionally **ambitious but implementable**: it favors compile-time safety and explicitness, but keeps Objective‑C’s dynamic runtime model as a first-class capability.

## v0.6 note
This pass performs a **consistency and completeness sweep**:
- Part 3 is aligned with the v0.4–v0.5 design decisions (optional chaining restrictions; conditional optional message sends).
- Part 4’s async/autorelease discussion is aligned with the normative per-suspension pool rule in Part 7 (Decision D‑006).
- Parts 9, 11, and 12 are expanded to remove “skeleton” gaps and to normalize numbering/cross‑references.
