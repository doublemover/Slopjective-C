# Objective‑C 3.0 — Design Decisions Log (v0.11) {#decisions}

_Last updated: 2026-02-23_

This log captures explicit “ship/no‑ship” decisions made to keep Objective‑C 3.0 **ambitious but implementable** (especially under separate compilation).

---

## D-001: Optional chaining is reference-only in v1 {#decisions-d-001}

**Decision:** Optional member access (`?.`) and optional message sends (`[receiver? selector]`) are supported only when the accessed member/method returns:

- an Objective‑C object pointer type, or
- a block pointer type, or
- `void` (for optional message sends).

Optional chaining for scalar/struct returns is **not** supported in v1.

**Rationale:**

- Objective‑C’s historic “messaging `nil` returns 0” behavior for scalars is a major source of silent bugs.
- Supporting scalar/struct optionals well would require a value-optional ABI and conversion rules that are too large for v1.
- The safe alternative is explicit unwrapping/binding of the receiver (`if let` / `guard let`), which v1 supports ergonomically.

**Spec impact:** [Part 3](#part-3) [§3.4](#part-3-4).

---

## D-002: `throws` is untyped in v1; typed throws deferred {#decisions-d-002}

**Decision:** The v1 `throws` effect is always **untyped**, with thrown values of type `id<Error>`.

Typed throws syntax (e.g., `throws(E)`) is reserved for future extension but is not part of v1 grammar/semantics.

**Rationale:**

- Objective‑C’s runtime dynamism and mixed-language interop (NSError, C return codes) favor a single error supertype.
- Typed throws adds significant complexity to generics, bridging, and ABI/lowering.

**Spec impact:** [Part 6](#part-6).

---

## D-003: Task spawning is library-defined in v1 (no `task {}` keyword) {#decisions-d-003}

**Decision:** Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided via the **standard library**.

The compiler recognizes task entry points via attributes (see [D-007](#decisions-d-007) and [Part 7](#part-7)).

**Rationale:** Keeps parsing surface small; avoids freezing spawn semantics before runtime patterns stabilize.

**Spec impact:** [Part 7](#part-7) [§7.5](#part-7-5).

---

## D-004: Executor annotations — canonical spelling and meaning (v1) {#decisions-d-004}

**Decision:** Executor affinity is expressed with the canonical spelling:

- `__attribute__((objc_executor(main)))`
- `__attribute__((objc_executor(global)))`
- `__attribute__((objc_executor(named("..."))))`

If a declaration is annotated `objc_executor(X)`, then:

- entering it from another executor requires an executor hop (typically by `await`ing the call), and
- the compiler enforces the hop requirement in strict concurrency checking mode.

**Rationale:** `__attribute__((...))` integrates into existing LLVM/Clang pipelines and is stable for module interface emission.

**Spec impact:** [Part 7](#part-7) and [B](#b)/[C](#c).

---

## D-005: Optional propagation (`T?` with postfix `?`) follows carrier rules (v1) {#decisions-d-005}

**Decision:** Postfix propagation `e?` is allowed on an optional `e : T?` **only** when the enclosing function returns an optional type.

- In an optional-returning function, `e?` yields `T` when non-`nil`, otherwise performs `return nil;`.
- Using `e?` in a function returning `Result<…>` or `throws` is **ill‑formed** in v1 (no implicit nil→error mapping).

**Rationale:** Prevents accidental “nil becomes an error” magic; keeps control-flow explicit.

**Spec impact:** [Part 3](#part-3) and [Part 6](#part-6).

---

## D-006: Autorelease pool boundaries at suspension points (v1) {#decisions-d-006}

**Decision:** On Objective‑C runtimes with autorelease semantics, each task _execution slice_ (resume → next suspension or completion) runs inside an implicit autorelease pool that is drained:

- before suspending at an `await`, and
- when the task completes.

**Rationale:** Predictable memory behavior for Foundation-heavy code; avoids autorelease buildup across long async chains.

**Spec impact:** [Part 7](#part-7), [Part 4](#part-4), and [C](#c).

---

## D-007: Canonical spellings and interface emission are attribute/pragma-first (v1) {#decisions-d-007}

**Decision:** Features that must survive **module interface emission** and **separate compilation** have canonical spellings defined in:

- **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)**

Sugar spellings (macros, `@`-directives, alternate attribute syntaxes) may exist, but the canonical emitted form shall use the catalog spellings.

**Rationale:** Keeps generated interfaces unambiguous and independent of user macro environments.

**Spec impact:** [B](#b), [Part 2](#part-2), [Part 12](#part-12).

---

## D-008: Generic methods are deferred in v1 {#decisions-d-008}

**Decision:** v1 includes **generic types** (pragmatic, erased generics) but defers **generic methods/functions**.

**Rationale:**

- Generic methods create difficult interactions with Objective‑C selector syntax, method redeclaration/overload rules, and module interface printing.
- The majority of practical value on Apple platforms comes from generic container types and constrained protocols.

**Spec impact:** [Part 3](#part-3) [§3.5](#part-3-5) (generic methods moved to future extensions).

---

## D-009: `throws` uses a stable “error-out” calling convention (v1) {#decisions-d-009}

**Decision:** v1 requires a stable ABI for `throws` that supports separate compilation. The recommended (and default) convention is a trailing error-out parameter (`outError`) of type `id<Error> _Nullable * _Nullable`.

**Rationale:** Matches long-standing Cocoa patterns (NSError-out) and is easy to lower in LLVM without stack unwinding.

**Spec impact:** C and [Part 6](#part-6).

---

## D-010: `async` lowers to coroutines scheduled by executors (v1) {#decisions-d-010}

**Decision:** v1 `async` semantics are implemented as coroutine state machines with suspension at `await`. Resumption is scheduled by the active executor, and the runtime/stdlib exposes enough primitives to:

- create tasks,
- hop executors,
- enqueue actor-isolated work, and
- propagate cancellation.

**Rationale:** This is the most direct path to implement structured `async/await` in LLVM while preserving ARC and Objective‑C runtime behavior.

**Spec impact:** C and [Part 7](#part-7).

---

## D-011: `await` is required for any potentially suspending operation (v1) {#decisions-d-011}

**Decision:** In v1, `await` is not restricted to “calling explicitly-async functions.”  
It is required for **any operation that may suspend**, including:

- calls to `async` functions,
- cross-executor entry into `objc_executor(X)` declarations when not proven already on `X`,
- cross-actor access to actor-isolated members when not already on the actor’s executor,
- joining tasks / iterating task-group results where suspension may occur.

`await` remains permitted only in `async` contexts.

**Rationale:** Executor and actor isolation are implemented by _hops_ that may suspend even when the callee’s body is “logically synchronous.” Requiring `await` keeps suspension explicit at the call site while preserving ergonomic isolation.

**Spec impact:** [Part 7](#part-7), [C](#c), [D](#d).

---

## D-012: Required module metadata set is normative (v1) {#decisions-d-012}

**Decision:** For ObjC 3.0 semantics to survive separate compilation, the required information enumerated in **[D](#d)** is normative: a conforming toolchain shall preserve that information in module metadata and in emitted textual interfaces.

**Rationale:** Without an explicit checklist, toolchains drift into “works in a single TU” but fails at module boundaries. [D](#d) makes the “separate compilation contract” testable.

**Spec impact:** [C](#c), [D](#d), [Part 2](#part-2), [Part 12](#part-12).

---

## D-013: Future value-optionals use canonical `Optional<T>` spelling {#decisions-d-013}

**Decision:** If a future value-optional feature is standardized, its canonical source spelling is `Optional<T>`.
`optional<T>` is not canonical and remains a reserved compatibility surface.

In conforming modes:

- parsers and interface emitters shall treat `Optional<T>` as the canonical spelling,
- textual interfaces shall emit `Optional<T>` when value-optionals are represented,
- any compatibility acceptance of `optional<T>` shall diagnose and offer a migration fix-it to `Optional<T>`.

**Rationale:** A single canonical spelling avoids dual-surface drift in tooling, formatting, metadata round-trips, and diagnostics while preserving a migration path for compatibility aliases.

**Spec impact:** [Part 3](#part-3) [§3.3.5](#part-3-3-5) and [§3.9](#part-3-9).

---

## D-014: Generic free-function mangling standardizes invariants, not one universal symbol string {#decisions-d-014}

**Decision:** v0.11 does not require one byte-for-byte generic free-function mangling string across all toolchains.
Instead, conformance standardizes:

- semantic mangling invariants (base name, generic arity, normalized constraints),
- deterministic reproduction within a toolchain/policy,
- stable publication of a mangling policy identifier,
- preserved semantic signature data in metadata/interfaces for cross-tool verification.

Direct symbol-string equality is only required within the same declared mangling policy.

**Rationale:** This preserves portability and testability without freezing all toolchains onto one encoding format, while still preventing semantic ambiguity.

**Spec impact:** [Part 3](#part-3) [§3.5.3.3](#part-3-5-3-3) and [§3.9](#part-3-9), plus metadata validation surfaces in [D](#d).

---

## D-015: Future generic reification control is declaration-scoped {#decisions-d-015}

**Decision:** Any future explicit generic reification mode applies per declaration via `@reify_generics` (or equivalent canonical declaration-level form).
Module/profile switches may gate whether declaration-level syntax is allowed, but shall not implicitly reify declarations that omit explicit markers.

Conforming metadata/interface behavior shall preserve whether a declaration is erased or explicitly reified.

**Rationale:** Declaration-scoped control supports gradual adoption, avoids module-wide semantic surprises, and keeps mixed erased/reified codebases tractable.

**Spec impact:** [Part 3](#part-3) [§3.5.5](#part-3-5-5) and [§3.9](#part-3-9).
