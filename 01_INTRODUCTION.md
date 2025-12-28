# Objective‑C 3.0 Draft Specification (Working Draft v0.6) — Introduction
_Last generated: 2025-12-28_

## 1. What Objective‑C 3.0 is trying to be
Objective‑C 3.0 is a modernization of Objective‑C focused on:

- **Safety by default (when provable)**: eliminate common crash classes and silent logic bugs without abandoning Objective‑C’s dynamic runtime model.
- **Composable concurrency**: upgrade from “blocks + conventions” to structured `async/await`, cancellation, and actor/executor isolation.
- **Ergonomics without magic**: reduce boilerplate through language-supported patterns (optionals, `Result`/`throws`, key paths, derives/macros) that still lower to transparent behavior.
- **System-library excellence**: make Apple‑style “library-defined subsets” (CoreFoundation, libdispatch, XPC, IOKit/Mach handles) safer and more ergonomic through first-class annotations and diagnostics.
- **Incremental adoption**: preserve existing ABI and interoperability; allow piecemeal migration by module/target without rewriting codebases.

This draft is written to be implementable primarily in **Clang/LLVM‑family toolchains**, with minimal required runtime changes. Where runtime support is needed, it is specified as small libraries or well-defined hooks (Part 7/8).

## 2. Document conventions
### 2.1 Normative terms
The terms **shall**, **must**, **shall not**, **should**, and **may** are used as in standards documents:
- **shall / must**: a conforming implementation is required to do this
- **shall not / must not**: a conforming implementation is forbidden from doing this
- **should**: recommended; deviation should be justified
- **may**: optional behavior

Part 0 defines shared terminology and the “normative reference” model.

### 2.2 Baseline assumption
Unless a part says otherwise, Objective‑C 3.0 is an **extension mode** enabled explicitly (Part 1), layered on top of “Objective‑C as implemented by a conforming compiler” (Part 0). Existing Objective‑C code compiled outside ObjC 3.0 mode shall not change meaning because of this document.

## 3. Guiding design principles
### 3.1 Compatibility boundaries are explicit
Objective‑C 3.0 uses a **language mode** and **conformance levels** so that:
- existing Objective‑C remains valid without surprising behavior changes;
- new defaults (nonnull-by-default regions, stricter diagnostics) are enabled only when explicitly selected.

### 3.2 New syntax must lower to existing concepts
New features should lower to:
- message sends and existing ObjC runtime rules where appropriate;
- compiler-inserted helper calls (ARC-style) where necessary;
- well-defined control-flow lowering (e.g., `defer`, `match`, coroutines).

### 3.3 Safety ladders, not all-or-nothing
If the compiler can prove a property (nonnullness, non-escaping borrowed pointers, executor affinity), it shall enforce it in strict modes.
If it cannot, the language must provide explicit escape hatches (e.g., `@unsafeSendable`, `unsafeEscapeBorrowed`, `unsafe_unretained`) that make risk obvious in code review.

### 3.4 System programming is a first-class audience
The language shall accommodate:
- handle-based APIs with strict cleanup requirements;
- “retainable C families” where ARC integration exists today through headers/macros;
- borrowed interior-pointer APIs that require careful lifetime management;
- performance-sensitive patterns where dynamic dispatch must be controlled.

### 3.5 Tooling is part of the spec
Objective‑C 3.0 is not just grammar: it includes required diagnostics, fix-its, and a conformance test suite model. A feature that cannot be audited and migrated is not “done” (Part 12).

## 4. Non-goals (for v1)
This draft intentionally does **not** attempt to:
- replace the Objective‑C runtime’s dynamism (method swizzling/forwarding remain possible by default);
- introduce a whole new generics/ownership system that changes ABI (generics remain pragmatic, ownership stays ARC-based);
- add full algebraic data types or borrow-checked memory safety across the entire language (we instead target focused “system profiles” + analyzers);
- deprecate Objective‑C exceptions (they remain for legacy; ObjC 3.0 error handling is separate).

## 5. Core feature set at a glance
Objective‑C 3.0 v1 targets these “big rocks”:

1. **Types & safety** (Part 3)
   - nonnull-by-default regions with enforceable nullability
   - optional type sugar (`T?`, `T!`) and binding (`if let`, `guard let`)
   - optional member access (`?.`) and optional message sends (`[x? foo]`) with explicit restrictions
   - pragmatic generics and typed key paths

2. **Errors** (Part 6)
   - `throws`, `throw`, `try`, `do/catch`
   - `Result<T,E>` carrier
   - postfix propagation `?` with strict carrier rules (no implicit nil→error)

3. **Concurrency** (Part 7)
   - `async/await`
   - executors and executor affinity annotations
   - library-defined tasks and task groups with compiler checking hooks
   - actors and Sendable-like checking ladder

4. **System‑library accommodation** (Part 8)
   - `defer` ordering model
   - `@resource` / cleanup annotations with move/reset primitives
   - retainable C families model
   - borrowed pointers + lifetime control
   - block capture lists (weak/unowned/move)

5. **Performance controls** (Part 9)
   - direct methods and “closed world” hints (final/sealed, static regions)

6. **Metaprogramming** (Part 10)
   - derives and macros with deterministic tooling requirements
   - property behaviors/wrappers (auditable, ABI-conscious)

## 6. Conformance levels and checking modes
Objective‑C 3.0 defines a safety ladder (Part 1):

- **Permissive**: new syntax accepted; most safety checks are warnings; legacy patterns allowed.
- **Strict**: key safety checks become errors; opt-outs require explicit unsafe spellings.
- **Strict-system**: strict + additional system-API safety checks (resource cleanup, borrowed pointer escapes, etc.).
- **Strict-concurrency** (orthogonal): additional concurrency/race checks; may be enabled as a submode of strictness.

This document uses “strict mode” as shorthand for the appropriate strictness/concurrency profile described in Part 1.

## 7. Cross-cutting semantics (important to keep consistent)
### 7.1 Effects are part of type
`async` and `throws` are effects and are part of a function’s type (Part 6/7). This matters for blocks, typedefs, and interop.

### 7.2 Carrier-preserving propagation
The postfix propagation operator `?` (Part 6) is **carrier-preserving**:
- `Result` propagates only within `Result` or `throws`
- `T?` propagates only within optional-returning functions

There is no implicit “nil→error” mapping in v1.

### 7.3 Optional sends are conditional calls
Optional message sends (`[receiver? ...]`) do **not** evaluate arguments if the receiver is `nil`. This is a deliberate semantic difference from ordinary Objective‑C sends (Part 3).

### 7.4 Autorelease pools at suspension points
On runtimes that support autorelease pools, async task execution slices run inside implicit pools drained on suspension and completion (Part 7). This is a correctness + memory-behavior guarantee for Foundation-heavy async code.

## v0.3 update
Parts 3, 6, and 7 were expanded from a skeleton into implementable drafts with concrete grammar and enforceable rules.

## v0.4 update
Three ship/no‑ship decisions were made:
1) Optional chaining is reference-only in v1.
2) `throws` is untyped in v1 (typed throws deferred).
3) Task spawning is library-defined in v1 (no `task {}` keyword).

## v0.5 update
Three more decisions were locked down:
1) Executor affinity has a canonical Clang-style spelling.
2) Optional propagation `e?` is carrier-preserving (optional-returning only).
3) Autorelease pool draining at suspension points is normative.

## v0.6 update
This pass:
- brings earlier parts into consistency with later decisions,
- fleshes out previously skeletal parts (modules, control flow, performance controls, interop),
- and standardizes “header-facing” attribute spellings so frameworks can adopt ObjC 3.0 features without inventing ad-hoc macros.
