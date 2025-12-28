# Objective‑C 3.0 Draft Specification (Working Draft) — Table of Contents
_Last generated: 2025-12-28_
_Working draft: v0.10_

This is a working draft of an **Objective‑C 3.0** specification broken into numbered, implementable parts.  
Each part is a separate Markdown file intended to be read independently, but the parts are cross-referenced where needed.

## Front matter
- **TABLE_OF_CONTENTS.md** — This table of contents (you are here)
- **INTRODUCTION.md** — Overview, goals, guiding principles, non‑goals, and cross‑cutting design choices
- **DECISIONS_LOG.md** — Explicit design decisions (ship/no‑ship) made to keep v1 implementable
- **ATTRIBUTE_AND_SYNTAX_CATALOG.md** — Canonical spellings for pragmas/attributes and module-interface emission requirements
- **LOWERING_AND_RUNTIME_CONTRACTS.md** — Implementation contracts for lowering, ABI stability, and runtime hooks
- **MODULE_METADATA_AND_ABI_TABLES.md** — Normative tables for required module metadata and ABI boundaries (implementer checklist)
- **CONFORMANCE_PROFILE_CHECKLIST.md** — Conformance profiles and implementer checklists (compiler/toolchain + optional feature sets)

## Parts
- **PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md** — Baseline Objective‑C (as implemented by Clang) and the normative reference model used by this draft     
- **PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md** — Language versioning, feature gating, conformance levels, and migration tooling requirements
- **PART_2_MODULES_NAMESPACING_API_SURFACES.md** — Modules, imports, module-qualified names, visibility, and API surface contracts
- **PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md** — Nullability, nonnull-by-default, optionals, pragmatic generics, and typed key paths
- **PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md** — ARC, ownership qualifiers, transfer/consumption, and interaction with retainable families
- **PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md** — `defer`, `guard`, `match`, patterns, and control-flow rules
- **PART_6_ERRORS_RESULTS_THROWS.md** — `throws`, `try`, `do/catch`, `Result`, propagation, and interop with NSError/return codes
- **PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md** — `async/await`, executors, cancellation, tasks, and actors
- **PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md** — Library-defined subsets and system programming extensions (IOKit-style ergonomics, safety, performance)
- **PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md** — Direct/final/sealed controls, dynamism boundaries, and performance-oriented constraints
- **PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md** — Derives, AST macros, property behaviors/wrappers, and safety constraints
- **PART_11_INTEROPERABILITY_C_CPP_SWIFT.md** — C/ObjC/ObjC++/Swift interop rules, overlays, and ABI mapping notes
- **PART_12_DIAGNOSTICS_TOOLING_TESTS.md** — Required diagnostics, fix-its, migrators, analyzers, and conformance test suites

## How to read this draft
1. Start with **INTRODUCTION.md** (goals + cross-cutting constraints).       
2. Read **Part 1** next (**PART_1_…**) for conformance levels and migration strategy.
3. Then choose a “slice” (types/errors/concurrency/system/perf) depending on what you’re implementing.

## Status / scope note
This draft is ambitious but implementable:
- It prefers features that can be implemented by a Clang/LLVM toolchain without rewriting the Objective‑C runtime.
- It keeps “strictness” opt-in via conformance levels, while providing a clear path to stricter defaults over time.
- Where a feature affects ABI or runtime, it is documented in **01C** and cross-referenced from the relevant part.

## Pass notes
- **v0.5:** locked major decisions for optionals/errors/concurrency (D‑001…D‑006).
- **v0.8:** adds canonical surface spellings (01B) and a lowering/ABI/runtime contract (01C), and begins tightening ABI-impacting features for separate compilation.
- **v0.9:** adds explicit module-metadata/ABI boundary tables (01D) and tightens `await` requirements for all potentially-suspending operations across executors/actors.
