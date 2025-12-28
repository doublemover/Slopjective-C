# Part 11 — Interoperability: C, C++, and Swift
_Working draft v0.6 — last updated 2025-12-28_

## 11.1 Purpose
Objective‑C’s success depends on interoperability:
- with C (system APIs),
- with C++ (ObjC++),
- and with Swift (modern app frameworks).

This part defines interoperability rules so ObjC 3.0 features can be adopted without breaking the ecosystem. Where full formal mapping is not yet specified, this part states required **preservation goals** and flags open issues.

---

## 11.2 C interoperability

### 11.2.1 ABI compatibility (normative)
ObjC 3.0 shall preserve C ABI calling conventions for C functions and data layouts. Language features like `throws` and `async` must be representable in C-compatible ways at module boundaries (Part 2) or be kept as Objective‑C-only surface features.

### 11.2.2 Ownership annotations in headers (normative intent)
C APIs may be annotated with ownership-transfer and lifetime attributes (Part 4 / Part 8) so that:
- ARC can insert correct retains/releases for retainable C families,
- analyzers can diagnose leaks and use-after-free patterns,
- conversions between ObjC objects and retainable C types are explicit.

### 11.2.3 Error model bridging (informative but intended)
ObjC 3.0 must interoperate with common C error styles:
- return-code + out-parameter (e.g., `int foo(..., T *out)`),
- `errno`,
- “nullable return + out NSError**”-style (Objective‑C, but common in C-like APIs).

The spec defines two primary language-level error carriers:
- `throws` (Part 6),
- `Result<T, E>` (Part 6).

Bridging goals:
- C return-code APIs should be expressible as `Result` without hidden allocations.
- `throws` should be representable without requiring C++ exceptions.

> Open issue: exact canonical header spellings for “this function returns error code / fills out parameter” annotations.

### 11.2.4 Concurrency bridging (informative but intended)
Completion-handler APIs should be bridgeable to `async` via explicit annotations that describe:
- which parameter is the completion handler,
- which completion argument(s) represent success vs failure,
- which executor/queue the completion runs on (if known).

Toolchains should support generating `async` overlays for annotated completion-based APIs.

---

## 11.3 C++ interoperability (ObjC++)

### 11.3.1 Parsing and keywords
ObjC 3.0 introduces new keywords (Part 1). In ObjC++ mode, implementations shall:
- preserve C++ meaning for C++ keywords,
- and avoid introducing ObjC 3.0 spellings that make valid ObjC++ code ambiguous.

Where conflicts arise, the language shall provide a compatibility escape hatch (raw identifiers; Part 1).

### 11.3.2 Name qualification and `::`
This draft’s provisional module qualification syntax uses `Module::Type`.

In ObjC++ translation units, `::` is also used for C++ scope resolution. Therefore:
- module qualification syntax must be specified so it is not ambiguous with C++ lookup, or
- ObjC++ must adopt a distinct spelling for module-qualified names.

> Open issue (high priority): finalize module-qualified name spelling with ObjC++ compatibility as a first-class constraint (Part 2).

### 11.3.3 Exceptions
ObjC 3.0 `throws` is not C++ exceptions; it is a language-level effect and carrier design (Part 6).

In ObjC++ codebases:
- toolchains may provide bridging utilities between C++ exceptions and ObjC `throws`/`Result`,
- but the language does not require implicit exception translation.

---

## 11.4 Swift interoperability

### 11.4.1 Nullability and optionals (normative intent)
ObjC nullability annotations and defaults (Part 3) are the foundation of Swift interop.

Interoperability goals:
- nonnull pointers import as non-optional Swift references,
- nullable pointers import as Swift optionals,
- “unspecified” nullability imports conservatively (as optional) and should be diagnosed for public APIs under strictness policies.

### 11.4.2 Generics
Objective‑C’s pragmatic generics (Part 3) should:
- preserve source compatibility with existing lightweight generics,
- provide enough information for Swift to import container element types where possible,
- avoid requiring runtime reification.

> Open issue: mapping constraints and conditional conformances across the boundary.

### 11.4.3 Errors (`throws` and `Result`)
Interoperability goals:
- ObjC `throws` should import as Swift `throws` where the error is representable.
- ObjC `Result<T, E>` should import as Swift `Result<T, E>` where `E` is bridgeable to `Error`.

Bridging to Cocoa patterns:
- Objective‑C `NSError **` patterns must remain supported and diagnosable (Part 6).
- Toolchains should support generating overlays that “upgrade” NSError patterns to `throws`/`Result` with explicit annotations.

### 11.4.4 Concurrency
Interoperability goals:
- ObjC `async` imports as Swift `async`.
- Annotated completion-handler APIs can import/export across languages safely.
- Executor/actor isolation annotations should be preservable across module interfaces (Part 2) so Swift can respect them.

### 11.4.5 Actors and isolation
Actor annotations must map:
- `@MainActor`/main-executor semantics,
- sendable-like constraints for cross-language boundaries.

> Open issue: the minimal set of actor/isolation attributes required in headers to avoid losing semantics when imported into Swift.

---

## 11.5 Mixed-language modules
A module that contains a mix of C/ObjC/ObjC++/Swift sources should be distributable in a way that preserves:
- nullability completeness,
- ownership/retainable-family semantics,
- error model intent,
- concurrency intent.

This is tightly coupled with the extracted interface story in Part 2.

---

## 11.6 Open issues
- Exact mapping rules for “strict” nullability completeness in mixed-language modules.
- How to express Swift-only features (e.g., rich generic constraints) in ObjC interfaces without losing intent.
- Canonical spelling and stability rules for cross-language overlay generation.
