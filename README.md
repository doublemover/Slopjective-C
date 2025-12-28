# Slopjective-C
Burning the slurp juice at both ends to make the future we deserve 

Try not to expend any real human effort on this, instead go buy the pro subscription for the LLM of your choice and have it do that for you. 

Additionally, you should consider giving me money so I can buy more slurp juice (tokens)

---

# Objective‑C 3.0 Draft Specification (Working Draft)

**Working draft:** v0.10  
**Last generated:** 2025-12-28  

This repository contains a working draft specification for **Objective‑C 3.0**. The draft is organized into small, implementable parts, with explicit contracts for **separate compilation**, **module metadata**, and (where needed) **ABI/runtime hooks**.

## Published draft
The stitched draft is published on GitHub Pages: https://doublemover.github.io/Slopjective-C/
The build uses **TABLE_OF_CONTENTS.md** ordering and excludes **README.md**.

## What this spec is
- A language-mode specification (“ObjC 3.0 mode”) layered on top of today’s de‑facto Objective‑C (Clang/LLVM + modern runtime behavior).
- A set of **normative requirements** (RFC-style “shall/must”) intended to be testable by compilers/toolchains.
- A design that prioritizes **incremental adoption**: new defaults and stricter rules are opt‑in via mode + strictness levels.

## What this spec is not
- Not a rewrite of baseline Objective‑C, ARC, or Blocks. Where those are relied upon, this draft references them as baseline behavior and specifies only the *extensions/constraints* needed for ObjC 3.0.
- Not a full “standard library spec” for every API. Where features are library-defined (e.g., task spawning), this draft specifies the minimum required surface and the compiler hooks needed for diagnostics.

## How the documents are organized
Start here:
- **TABLE_OF_CONTENTS.md** — Index of all parts.
- **INTRODUCTION.md** — Goals, non‑goals, and cross‑cutting principles.      

Front matter (implementation-facing):
- **DECISIONS_LOG.md** — Explicit ship/no‑ship decisions to keep v1 implementable.
- **ATTRIBUTE_AND_SYNTAX_CATALOG.md** — Canonical spellings for attributes/pragmas that must survive interface emission.
- **LOWERING_AND_RUNTIME_CONTRACTS.md** — ABI/lowering/runtime obligations (separate compilation contract).
- **MODULE_METADATA_AND_ABI_TABLES.md** — Normative tables for required module metadata and ABI boundaries.
- **CONFORMANCE_PROFILE_CHECKLIST.md** — Checklists for conformance claims (Core/Strict/Strict Concurrency/Strict System + optional feature sets).

Language parts:
- **PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md** — Baseline definitions and normative terminology.
- **PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md** — Mode selection, feature tests, strictness levels, and migration requirements.
- **PART_2_MODULES_NAMESPACING_API_SURFACES.md** — Modules, module-qualified names, API surface contracts, interface emission.
- **PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md** — Nullability defaults, optionals, pragmatic generics, typed key paths.
- **PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md** — ARC baseline + ownership/transfer/lifetime rules used by other features.
- **PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md** — `defer`, `guard`, `match`, patterns.
- **PART_6_ERRORS_RESULTS_THROWS.md** — `throws`/`try`/`do-catch`, `Result`, propagation operator `?`, NSError/return-code bridging.
- **PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md** — `async/await`, executors, tasks, cancellation, actors, Sendable-like checking.
- **PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md** — Resource cleanup, retainable C families, borrowed pointers, capture lists.
- **PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md** — direct/final/sealed intent and dynamism boundaries.
- **PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md** — derives, AST macros, property behaviors.
- **PART_11_INTEROPERABILITY_C_CPP_SWIFT.md** — C/C++/Swift interop expectations.
- **PART_12_DIAGNOSTICS_TOOLING_TESTS.md** — Required diagnostics, migrators, and conformance test suite expectations.

## Suggested reading order
1. **INTRODUCTION.md** (what the project is trying to accomplish)
2. **PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md** (how mode/strictness/conformance works)
3. **ATTRIBUTE_AND_SYNTAX_CATALOG / LOWERING_AND_RUNTIME_CONTRACTS / MODULE_METADATA_AND_ABI_TABLES** (the “separate compilation contract” and canonical spellings)
4. Then pick a feature slice:
   - Types/Optionals: **Part 3**
   - Errors: **Part 6**
   - Concurrency: **Part 7**
   - System programming: **Part 8**

## Conformance model (quick summary)
A toolchain makes a conformance claim against named profiles:
- **Core**: language mode + modules/metadata + types/errors/concurrency baseline.
- **Strict**: stronger diagnostics and migration support.
- **Strict Concurrency**: actor/executor/Sendable-like enforcement.
- **Strict System**: resource/borrowed-pointer/capture-list enforcement.

See **CONFORMANCE_PROFILE_CHECKLIST.md** for the definitive checklist.      

## Contributing / feedback
This is a working draft. Each part ends with an **Open issues** section.

A practical contribution style:
- pick one “Open issue”,
- propose a concrete rule + lowering/metadata implications,
- add (or outline) conformance tests for it.

## License
(Not specified in this draft bundle — add a repository-level license file if you plan to publish/distribute.)
