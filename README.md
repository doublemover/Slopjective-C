# Slopjective-C

Burning the slurp juice at both ends to make the future we deserve

Try not to expend any real human effort on this, instead go buy the pro subscription for the LLM of your choice and have it do that for you.

Additionally, you should consider giving me money so I can buy more slurp juice (tokens)

---

# Objective‑C 3.0 Draft Specification (Working Draft)

**Working draft:** v0.11  
**Last generated:** 2026-02-27

This repository contains a working draft specification for **Objective‑C 3.0**. The draft is organized into small, implementable parts, with explicit contracts for **separate compilation**, **module metadata**, and (where needed) **ABI/runtime hooks**.

## Published draft

The stitched draft is published on GitHub Pages: [https://doublemover.github.io/Slopjective-C/](https://doublemover.github.io/Slopjective-C/)
The build uses the [Table of Contents](https://doublemover.github.io/Slopjective-C/#toc) ordering and excludes this README.
Source markdown lives in `spec/` for local edits.

## What this spec is

- A language-mode specification (“ObjC 3.0 mode”) layered on top of today’s de‑facto Objective‑C (Clang/LLVM + modern runtime behavior).
- A set of **normative requirements** (RFC-style “shall/must”) intended to be testable by compilers/toolchains.
- A design that prioritizes **incremental adoption**: new defaults and stricter rules are opt‑in via mode + strictness levels.

## What this spec is not

- Not a rewrite of baseline Objective‑C, ARC, or Blocks. Where those are relied upon, this draft references them as baseline behavior and specifies only the _extensions/constraints_ needed for ObjC 3.0.
- Not a full “standard library spec” for every API. Where features are library-defined (e.g., task spawning), this draft specifies the minimum required surface and the compiler hooks needed for diagnostics.

## How the documents are organized

Start here:

- [Table of Contents](https://doublemover.github.io/Slopjective-C/#toc) — Index of all parts.
- [Introduction](https://doublemover.github.io/Slopjective-C/#intro) — Goals, non‑goals, and cross‑cutting principles.

Front matter (implementation-facing):

- [Decisions Log](https://doublemover.github.io/Slopjective-C/#decisions) — Explicit ship/no‑ship decisions to keep v1 implementable.
- [Attribute and Syntax Catalog](https://doublemover.github.io/Slopjective-C/#b) — Canonical spellings for attributes/pragmas that must survive interface emission.
- [Lowering and Runtime Contracts](https://doublemover.github.io/Slopjective-C/#c) — ABI/lowering/runtime obligations (separate compilation contract).
- [Module Metadata and ABI Tables](https://doublemover.github.io/Slopjective-C/#d) — Normative tables for required module metadata and ABI boundaries.
- [Conformance Profile Checklist](https://doublemover.github.io/Slopjective-C/#e) — Checklists for conformance claims (Core/Strict/Strict Concurrency/Strict System + optional feature sets).

Language parts:

- [Part 0 — Baseline and Normative References](https://doublemover.github.io/Slopjective-C/#part-0) — Baseline definitions and normative terminology.
- [Part 1 — Versioning, Compatibility, and Conformance](https://doublemover.github.io/Slopjective-C/#part-1) — Mode selection, feature tests, strictness levels, and migration requirements.
- [Part 2 — Modules, Namespacing, and API Surfaces](https://doublemover.github.io/Slopjective-C/#part-2) — Modules, module-qualified names, API surface contracts, interface emission.
- [Part 3 — Types: Nullability, Optionals, Pragmatic Generics, and Typed Key Paths](https://doublemover.github.io/Slopjective-C/#part-3) — Nullability defaults, optionals, pragmatic generics, typed key paths.
- [Part 4 — Memory Management and Ownership](https://doublemover.github.io/Slopjective-C/#part-4) — ARC baseline + ownership/transfer/lifetime rules used by other features.
- [Part 5 — Control Flow and Safety Constructs](https://doublemover.github.io/Slopjective-C/#part-5) — `defer`, `guard`, `match`, patterns.
- [Part 6 — Errors: Result, throws, try, and Propagation](https://doublemover.github.io/Slopjective-C/#part-6) — `throws`/`try`/`do-catch`, `Result`, propagation operator `?`, NSError/return-code bridging.
- [Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors](https://doublemover.github.io/Slopjective-C/#part-7) — `async/await`, executors, tasks, cancellation, actors, Sendable-like checking.
- [Part 8 — System Programming Extensions](https://doublemover.github.io/Slopjective-C/#part-8) — Resource cleanup, retainable C families, borrowed pointers, capture lists.
- [Part 9 — Performance and Dynamism Controls](https://doublemover.github.io/Slopjective-C/#part-9) — direct/final/sealed intent and dynamism boundaries.
- [Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors](https://doublemover.github.io/Slopjective-C/#part-10) — derives, AST macros, property behaviors.
- [Part 11 — Interoperability: C, C++, and Swift](https://doublemover.github.io/Slopjective-C/#part-11) — C/C++/Swift interop expectations.
- [Part 12 — Diagnostics, Tooling, and Test Suites](https://doublemover.github.io/Slopjective-C/#part-12) — Required diagnostics, migrators, and conformance test suite expectations.

## Suggested reading order

1. [Introduction](https://doublemover.github.io/Slopjective-C/#intro) (what the project is trying to accomplish)
2. [Part 1 — Versioning, Compatibility, and Conformance](https://doublemover.github.io/Slopjective-C/#part-1) (how mode/strictness/conformance works)
3. [Attribute and Syntax Catalog](https://doublemover.github.io/Slopjective-C/#b) / [Lowering and Runtime Contracts](https://doublemover.github.io/Slopjective-C/#c) / [Module Metadata and ABI Tables](https://doublemover.github.io/Slopjective-C/#d) (the “separate compilation contract” and canonical spellings)
4. Then pick a feature slice:
   - Types/Optionals: [Part 3](https://doublemover.github.io/Slopjective-C/#part-3)
   - Errors: [Part 6](https://doublemover.github.io/Slopjective-C/#part-6)
   - Concurrency: [Part 7](https://doublemover.github.io/Slopjective-C/#part-7)
   - System programming: [Part 8](https://doublemover.github.io/Slopjective-C/#part-8)

## Conformance model (quick summary)

A toolchain makes a conformance claim against named profiles:

- **Core**: language mode + modules/metadata + types/errors/concurrency baseline.
- **Strict**: stronger diagnostics and migration support.
- **Strict Concurrency**: actor/executor/Sendable-like enforcement.
- **Strict System**: resource/borrowed-pointer/capture-list enforcement.

See the [Conformance Profile Checklist](https://doublemover.github.io/Slopjective-C/#e) for the definitive checklist.

## Contributing / feedback

This is a working draft. Each part ends with an **Open issues** section.

A practical contribution style:

- pick one “Open issue”,
- propose a concrete rule + lowering/metadata implications,
- add (or outline) conformance tests for it.

Lint and CI requirements are documented in `CONTRIBUTING.md`.

## License

(Not specified in this draft bundle — add a repository-level license file if you plan to publish/distribute.)
