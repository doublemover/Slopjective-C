# Objective‑C 3.0 Draft Specification (Working Draft v0.7) — Table of Contents
_Last generated: 2025-12-28_

This bundle contains a working draft of an **Objective‑C 3.0** specification. Each part is a separate Markdown file to keep the document modular and reviewable.

**Normative language:** The keywords **shall**, **must**, **shall not**, **should**, and **may** are used as in standards documents (see Part 0).

## Front matter
- **00_TABLE_OF_CONTENTS.md** — This table of contents (you are here)
- **01_INTRODUCTION.md** — Overview, goals, guiding principles, non‑goals, and cross‑cutting design choices
- **01A_DECISIONS_LOG.md** — Explicit design decisions (ship/no‑ship) recorded as the draft evolves
- **01B_ATTRIBUTE_AND_SYNTAX_CATALOG.md** — Canonical spellings for attributes, pragmas, and required surface syntax

## Parts
- **02_PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md** — Baseline Objective‑C (as implemented by Clang-family compilers) and normative reference model
- **03_PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md** — Language modes (“edition-like”), feature gating, conformance levels, and migration tooling requirements
- **04_PART_2_MODULES_NAMESPACING_API_SURFACES.md** — Modules, imports, module-qualified names, visibility, and API surface contracts
- **05_PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md** — Nullability by default, optionals and binding, pragmatic generics, and typed key paths
- **06_PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md** — ARC, ownership qualifiers, lifetime rules, transfer/consumption, and interaction with retainable families
- **07_PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md** — `defer`, `guard`, `match`, patterns, and control-flow rules
- **08_PART_6_ERRORS_RESULTS_THROWS.md** — `Result`, `throws`, propagation (`?`), and interoperability with NSError and return-code APIs
- **09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md** — `async/await`, executors, cancellation, structured concurrency, actors, and Sendable-like checking
- **10_PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md** — System-library accommodation: resources/handles, retainable C families, borrowed pointers, capture lists, lifetime control
- **11_PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md** — Direct methods, `final`/`sealed`, dynamic/static boundaries, and optimization contracts
- **12_PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md** — Derives, AST macros, property behaviors/wrappers, and safety constraints
- **13_PART_11_INTEROPERABILITY_C_CPP_SWIFT.md** — C/ObjC/ObjC++/Swift interop rules and ABI mapping guidance
- **14_PART_12_DIAGNOSTICS_TOOLING_TESTS.md** — Required diagnostics, fix-its, migrators, analyzers, and conformance test suites

## How to read this draft
1. Read **01_INTRODUCTION.md** (goals + cross-cutting constraints).
2. Read **03_PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md** (how to turn ObjC 3.0 on and what “strictness” means).
3. Then choose a slice depending on what you care about:
   - *Safety:* Part 3 (types), Part 6 (errors), Part 8 (system safety)
   - *Concurrency:* Part 7 + Part 8 (lifetimes/borrow rules)
   - *Framework authoring:* Part 2 (modules) + Part 9 (performance controls) + Part 12 (tooling)
4. Keep **01A_DECISIONS_LOG.md** open while reading; it records which open questions have been decided.

## Status note
This draft is intentionally **ambitious but implementable**: it prefers features that can be lowered by a compiler front end (Clang-family) to existing ABIs and/or small runtime libraries, and it makes “strictness” opt-in via conformance levels.

## v0.6 note
This pass performs a **full-document consistency sweep**:
- brings Part 3 forward to match later decisions (optional chaining restrictions and evaluation order),
- propagates concurrency autorelease pool rules to memory-management guidance,
- fleshes out skeletal parts (control flow, modules, performance controls, interop),
- and standardizes “canonical spellings” for attributes intended to appear in headers.
