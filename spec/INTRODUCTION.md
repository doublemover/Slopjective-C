# Objective‑C 3.0 Draft Specification (Working Draft) — Introduction {#intro}

_Last generated: 2026-02-27_
_Working draft: v0.11_

## 1. What Objective‑C 3.0 is trying to be {#intro-1}

Objective‑C 3.0 is a modernization of Objective‑C focused on:

- **Safety by default (when provable)**: eliminate the most common crash classes and logic bugs without abandoning Objective‑C’s dynamic runtime model.
- **Composable concurrency**: upgrade from “blocks + conventions” to structured `async/await`, cancellation, and actor isolation.
- **Ergonomics without magic**: reduce boilerplate through language-supported patterns (optionals, Result/throws, key paths, derives/macros) that still lower to transparent, testable behavior.
- **System-library excellence**: make Apple platform “library-defined subsets” (CoreFoundation, libdispatch, XPC, IOKit/Mach handles) safer and more ergonomic through first-class annotations and diagnostics.
- **Incremental adoption**: preserve existing ABI and interoperability, and provide an opt-in path that does not require rewriting codebases.

This draft is written to be implementable primarily in **Clang/LLVM**, with minimal required runtime changes. Where runtime support is needed, it is specified as small libraries or well-defined hooks.

## 2. Guiding design principles {#intro-2}

### 2.1 Compatibility boundaries are explicit {#intro-2-1}

Objective‑C 3.0 uses a **language mode** (and optional conformance “strictness” levels) so that:

- existing Objective‑C remains valid without surprise behavior changes;
- new defaults (nonnull-by-default, stricter diagnostics) are only enabled when explicitly selected.

### 2.2 New syntax must map to existing concepts {#intro-2-2}

New features should lower to:

- message sends and existing ObjC runtime rules where appropriate;
- compiler-inserted helper calls (ARC-style) where necessary;
- well-defined control-flow lowering (coroutines, defers, matches).

### 2.3 Safety is enforced where it can be proven; escape hatches are explicit {#intro-2-3}

If the compiler can prove a property (nonnullness, non-escaping borrowed pointers, cleanup correctness), it shall enforce it in strict modes.
If it cannot, the language must provide explicit annotations/constructs (e.g., `unsafeEscapeBorrowed`, `@unsafeSendable`, `unowned`) that make risk obvious in code review.

### 2.4 “System programming” is a first-class audience {#intro-2-4}

The language shall accommodate:

- handle-based APIs with strict cleanup requirements;
- “retainable C families” where ARC integration exists today through headers/macros;
- borrowed interior-pointer APIs that require careful lifetime management;
- performance-sensitive patterns where dynamic dispatch must be controlled.

### 2.5 Tooling is part of the spec {#intro-2-5}

Objective‑C 3.0 is not just grammar: it includes required diagnostics, fix-its, and a conformance test suite model. A feature that cannot be audited and migrated is not “done”.

## 3. Non-goals (important) {#intro-3}

Objective‑C 3.0 does not aim to:

- become Swift, Rust, or Go;
- introduce a global borrow checker over all code;
- remove dynamic dispatch, swizzling, forwarding, or the Objective‑C runtime;
- require a new ABI for existing frameworks.

## 4. The “big rocks” of ObjC 3.0 (high-level) {#intro-4}

- **Types**: nonnull-by-default regions; optionals and binding; pragmatic generics; typed key paths.
- **Errors**: `Result<T, E>`, `throws`, and propagation (`?`) with bridging to NSError and return codes.
- **Concurrency**: `async/await`, structured tasks, cancellation, executors, actors, sendable-like checking.
- **System extensions**: `defer`, resource handles/cleanup contracts, retainable C families, borrowed pointers, capture lists, lifetime controls.
- **Performance**: direct methods, `final`/`sealed`, and explicit dynamic/static boundaries.
- **Boilerplate elimination**: derives, AST macros, property behaviors/wrappers.

## 5. Document organization {#intro-5}

Each part is a separate Markdown file. The system-programming chapter ([Part 8](#part-8)) is intentionally detailed because it anchors “library-defined subset” accommodation, but it is only one part of a broader specification.

## 6. Open issues and expected iteration {#intro-6}

This is a working draft. Each part includes “Open Issues” subsections for unresolved syntax bikeshedding and deeper ABI questions. The goal is to converge by:

- shipping implementable subsets early (nullability defaults, defer, Result/?, capture lists),
- then layering bigger features (async/await, actors, macros).

## v0.3 update {#intro-v0-3-update}

This bundle expands [Part 3](#part-3) (types/optionals), [Part 6](#part-6) (errors), and [Part 7](#part-7) (concurrency) with concrete grammar and enforceable rules, including optional message sends (`[x? foo]`), `try`/`throw`/`do`-`catch`, and executor/actor isolation scaffolding.

## v0.4 update {#intro-v0-4-update}

This revision makes three “ship/no‑ship” decisions:

1. Optional chaining (`?.`, `[x? foo]`) is **reference-only** in v1 (no scalar/struct chaining).
2. `throws` is **untyped** in v1 (always `id<Error>`); typed throws is deferred.
3. Task spawning remains **library-defined** in v1; no `task {}` keyword syntax (compiler recognition uses standardized attributes).

## v0.5 update {#intro-v0-5-update}

This pass resolves:

- the exact surface spelling for executor annotations ([Part 7](#part-7)),
- the v1 policy for optional (`T?`) propagation with `?` ([Part 6](#part-6)), and
- the normative autorelease pool contract at suspension points ([Part 7](#part-7)).

## v0.8 pass focus {#intro-v0-8-pass-focus}

This pass tightens the specification specifically for **separate compilation** and **implementability**:

- **Canonical spellings** are centralized in **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)** (attributes/pragmas that are stable for module interfaces).
- **Lowering / ABI / runtime contracts** are centralized in **[LOWERING_AND_RUNTIME_CONTRACTS.md](#c)**.
- Parts [6](#part-6)/[7](#part-7)/[9](#part-9)/[11](#part-11)/[12](#part-12) are updated to cross-reference [B](#b)/[C](#c) where semantics depend on lowering choices.

This is a deliberate step toward a spec that can be implemented in Clang/LLVM with predictable behavior across modules.

## v0.9 pass focus {#intro-v0-9-pass-focus}

This pass makes the draft more “engineer-ready” by tightening the boundary between **source semantics** and **cross-module implementation reality**:

- Adds **[MODULE_METADATA_AND_ABI_TABLES.md](#d)**: explicit tables for what must be preserved in module metadata and emitted interfaces, and which features are ABI-affecting.
- Tightens the meaning of `await`: it is required for **any potentially suspending operation**, including cross-executor and cross-actor access (even when the callee is not explicitly `async`), aligning the surface model with implementable executor/actor hops.
- Expands C with more concrete lowering obligations where separate compilation would otherwise be ambiguous.

