# Objective‑C 3.0 — Design Decisions Log (v0.6)
_Last updated: 2025-12-28_

This log captures explicit ship/no‑ship decisions that resolve prior open issues. Each decision is intended to be reflected normatively in the corresponding part(s).

## D‑001: Optional chaining is reference-only in v1
**Decision:** Optional member access (`?.`) and optional message sends (`[receiver? selector]`) are supported only when the accessed member/method returns:
- an Objective‑C object pointer type, or
- a block pointer type, or
- `void` (for optional message sends).

Optional chaining for scalar/struct returns is **not** supported in v1.

**Rationale:** Objective‑C’s historic “nil messaging returns 0” behavior for scalars is a major source of silent bugs; scalar optional chaining would require a value-optional ABI in v1.

**Spec impact:** Part 3 §3.4 is normative.

**Future direction:** Introduce an `Optional<T>` value carrier (or `OptionalScalar<T>`) as a later revision if needed.

---

## D‑002: `throws` is untyped in v1; typed throws deferred
**Decision:** The v1 `throws` effect is **untyped**, with thrown values of type `id<Error>`.

Typed throws syntax `throws(E)` is reserved for future consideration but is not part of v1 grammar/semantics.

**Rationale:** Interop with NSError and status-code APIs favors a single error supertype; typed throws adds significant complexity and is not required for v1.

**Spec impact:** Part 6 §6.3 is normative.

---

## D‑003: Task spawning is library-defined in v1 (no `task {}` keyword)
**Decision:** Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided via the **standard library**, with compiler-recognized attributes to enable diagnostics and safety checks.

**Rationale:** Avoids freezing syntax/semantics prematurely; keeps parsing surface small; still enables strong checking.

**Spec impact:** Part 7 §7.5 is normative.

---

## D‑004: Executor annotations — canonical spelling and meaning (v1)
**Decision:** Executor affinity is declared using a standardized Clang-style attribute spelling:

- `__attribute__((objc_executor(main)))`
- `__attribute__((objc_executor(global)))`
- `__attribute__((objc_executor(named("..."))))`

These attributes may be applied to:
- functions,
- Objective‑C methods (instance/class),
- Objective‑C types (classes/actors) to indicate a default executor for isolated members.

**Meaning:** A declaration annotated with `objc_executor(X)` requires that execution occurs on executor `X`. Entering such a declaration from another executor requires an async hop and therefore must be expressible in an `async` context (i.e., at or under `await`).

**Spec impact:** Part 7 §7.4.3 is normative.

---

## D‑005: Optional propagation (`T?` with postfix `?`) follows carrier rules (v1)
**Decision:** Postfix propagation `e?` on `e : T?` is allowed **only** when the enclosing function returns an optional type.

- In an optional-returning function, `e?` yields `T` when non-`nil`, otherwise performs `return nil;`.
- Using `e?` in `throws` or `Result` contexts is **ill‑formed** in v1 (no implicit nil→error mapping).

**Spec impact:** Part 6 §6.6.4 is normative.

---

## D‑006: Autorelease pool boundaries at suspension points (v1)
**Decision:** On platforms with Objective‑C autorelease pools, each async **task execution slice** (from resume to the next suspension/completion) executes within an implicit autorelease pool that is drained:
- immediately before the task suspends at an `await` that actually suspends, and
- at task completion (normal return, throw, or cancellation unwind).

Implementations may drain at additional safe points, but shall not permit unbounded pool growth across long-lived tasks that repeatedly suspend.

**Spec impact:** Part 7 §7.9.4 is normative; Part 4 references this as the concurrency/memory-management contract.

---

## D‑007: Canonical attribute spellings are Clang-style for header interoperability (v1)
**Decision:** Features intended to appear in public headers shall have a **canonical Clang-style attribute spelling** (`__attribute__((...)))`) so that:
- headers remain valid in ObjC/ObjC++ translation units,
- tooling (indexers/analyzers) can reason about semantics without macro folklore,
- and module interface extraction can preserve intent.

**Policy:**
- The specification may present ergonomic ObjC‑surface sugar (e.g., `@resource(...)`) in examples.
- However, the *canonical* spelling for header emission and module interfaces is the attribute form.
- Implementations may accept both spellings in ObjC 3.0 mode.

**Illustrative canonical spellings (non-exhaustive; details in Parts 6–10):**
- Executors: `__attribute__((objc_executor(...)))` (D‑004)
- Task creation hooks: `__attribute__((objc_task_spawn))`, `__attribute__((objc_task_detached))`, `__attribute__((objc_task_group))`
- Resource cleanup: `__attribute__((cleanup(F)))` and/or `__attribute__((objc3_resource(close=F, invalid=...)))`
- Ownership transfer: `__attribute__((objc_consumed))`, `__attribute__((objc_returns_owned))`, `__attribute__((objc_returns_borrowed))`
- Borrowed relationships: `__attribute__((objc3_returns_borrowed(owner_index=N)))`
- Performance controls: `__attribute__((objc_direct))`, `__attribute__((objc_subclassing_restricted))` (or equivalent)

**Rationale:** This draft’s “borrow from LLVM/Clang” philosophy is that the front end should lean on attribute infrastructure for cross-language and tooling stability.

**Spec impact:** Parts 2, 6, 8, 9, and 11 are updated to include canonical spellings and header guidance.
