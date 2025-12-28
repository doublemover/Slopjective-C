# Part 11 — Interoperability: C, C++, and Swift
_Working draft v0.10 — last updated 2025-12-28_

## 11.1 Purpose
Objective‑C’s success depends on interoperability:
- with **C** (system APIs),
- with **C++** (ObjC++),
- and with **Swift** (modern app frameworks).

This part defines interoperability expectations so ObjC 3.0 features can be adopted without breaking the ecosystem.

This part is intentionally conservative about ABI: where a feature affects ABI or lowering, it is cross-referenced to **C** and summarized in **D**.

## 11.2 C interoperability

### 11.2.1 Baseline ABI compatibility
Objective‑C 3.0 preserves baseline C ABI calling conventions for declarations that do not use new effects.

For declarations that use ObjC 3.0 effects (`throws`, `async`), a conforming toolchain shall provide a stable, documented calling convention (C.4, C.5).

### 11.2.2 Exporting `throws` to C (normative intent)
When a throwing function is made visible to C, it shall be representable using a C-callable surface.

Recommended representation:
- a trailing error-out parameter (`id<Error> * _Nullable outError`) as described in C.4.

This aligns with Cocoa NSError patterns and enables C callers to handle errors without language support for `try`.

### 11.2.3 Exporting `async` to C (normative intent)
When an `async` function is made visible to C, it shall be representable using a C-callable surface.

This draft does not require a single canonical signature, but a conforming implementation shall provide at least one of:
- an automatically-generated completion-handler thunk, or
- a runtime entry point that can schedule the async function and invoke a completion callback when finished.

The chosen representation must be stable under separate compilation and recordable in module metadata (C.2).

## 11.3 C++ / ObjC++ interoperability

### 11.3.1 Parsing and canonical spellings
Canonical spellings in B are chosen to be compatible with ObjC++ translation units:
- `__attribute__((...))` is accepted in C++ mode,
- `#pragma ...` is available in C++ mode.

Implementations may additionally support C++11 attribute spellings (`[[...]]`) in ObjC++ as sugar, but interface emission shall use canonical spellings (B.7).

### 11.3.2 Exceptions
Objective‑C exceptions (`@throw/@try/@catch`) remain distinct from ObjC 3.0 `throws`.
C++ exceptions remain distinct as well.

Interoperability guidance:
- Do not translate ObjC 3.0 `throws` into C++ exceptions implicitly.
- If an implementation provides bridging, it must be explicit and toolable.

## 11.4 Swift interoperability

### 11.4.1 Nullability and optionals
ObjC 3.0 nullability annotations and `T?` sugar import naturally as Swift optionals where applicable, consistent with existing Swift/ObjC interop rules.

### 11.4.2 Errors
- ObjC 3.0 `throws` imports as Swift `throws` when the declaration is visible through a module interface that preserves the effect.
- NSError-out parameter APIs annotated with `objc_nserror` (Part 6) should import as Swift `throws` where possible.

### 11.4.3 Concurrency
- ObjC 3.0 `async` imports as Swift `async` when preserved through module metadata/interfaces.
- Completion-handler APIs may be imported/exported across languages using implementation-defined annotations; this draft encourages toolchains to support automated thunk generation.

### 11.4.4 Executors and isolation
Executor affinity (`objc_executor(main)`) is intended to map cleanly to Swift main-thread isolation concepts.
The exact imported attribute spelling is implementation-defined, but semantics must be preserved across the boundary.

Actor isolation metadata should be preserved such that Swift clients do not accidentally violate ObjC actor invariants (and vice versa).

### 11.4.5 Performance/dynamism controls
Direct/final/sealed intent should map, where possible, to Swift’s:
- `final`,
- non-overridable members,
- and restricted subclassing patterns.

The mapping is implementation-defined but should preserve the core constraints.

## 11.5 Required diagnostics (minimum)
- Importing an API where effects (`async`/`throws`) are lost due to missing interface metadata: warn and suggest enabling module/interface emission.
- Crossing a language boundary with non-Sendable-like values in strict concurrency mode: warn/error depending on policy (Part 7, Part 12).

## 11.6 Open issues
- A precise, portable “async-to-C” completion thunk signature (this may be a platform profile item).
- Exact mapping rules for strict nullability completeness in mixed-language modules.
