# Objective‑C 3.0 — Design Decisions Log (v0.6)
_Last updated: 2025-12-28_

This log captures explicit “ship/no‑ship” decisions made to resolve prior open issues.

> v0.6 note: This pass is editorial/organizational. The decisions below are unchanged; the rest of the draft is being reconciled to match them.

## D‑001: Optional chaining is reference-only in v1
**Decision:** Optional member access (`?.`) and optional message sends (`[receiver? selector]`) are supported only when the accessed member/method returns:
- an Objective‑C object pointer type, or
- a block pointer type, or
- `void` (for optional message sends).

Optional chaining for scalar/struct returns is **not** supported in v1.

**Rationale:**
- Objective‑C’s historic “nil messaging returns 0” behavior for scalars is a major source of silent bugs.
- Providing scalar optionals would require introducing a new value-optional system (layout, ABI, conversions) that is larger than a v1 feature.
- The safe alternative is explicit unwrapping/binding of the receiver, which is already ergonomically supported via `if let` / `guard let`.

**Spec impact:** Part 3 §3.4 is normative; scalar/struct chaining is ill-formed in all conformance levels (permissive may still offer migration guidance).

**Future direction:** Introduce `OptionalScalar<T>` (or generalized `Optional<T>`) as a v2 feature if warranted.

---

## D‑002: `throws` is untyped in v1; typed throws deferred
**Decision:** The v1 `throws` effect is always **untyped**, with thrown values of type `id<Error>`.

Typed throws syntax `throws(E)` is reserved for future consideration but is not part of the v1 grammar/semantics.

**Rationale:**
- Objective‑C’s runtime dynamism and mixed-language interop (NSError, C return codes) favor a single error supertype.
- Typed throws adds significant complexity to generics, bridging, and ABI/lowering, and does not unlock enough value for v1.

**Spec impact:** Part 6 removes typed throws from normative grammar and treats it as a future extension.

---

## D‑003: Task spawning is library-defined in v1 (no `task {}` keyword)
**Decision:** Objective‑C 3.0 v1 does not introduce a new `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided via the **standard library**, with compiler-recognized attributes to enable diagnostics and safety checks.

**Rationale:**
- Avoids freezing spawn semantics in the language before the runtime model settles.
- Keeps parsing surface small and reduces conflicts with existing code.
- Still enables excellent ergonomics via standard library + attributes + potential macro sugar (Part 10).

**Spec impact:** Part 7 §7.5 specifies a standard-library contract and new attributes (`@task_spawn`, `@task_detached`, `@task_group`) that compilers must honor for checking.

**Future direction:** Provide `task { ... }` as a macro or a v2 sugar once patterns stabilize.

---

## D‑004: Executor annotations — canonical spelling and meaning (v1)
**Decision:** Executor affinity is declared using a standardized Clang-style attribute spelling:

- `__attribute__((objc_executor(main)))`
- `__attribute__((objc_executor(global)))`
- `__attribute__((objc_executor(named("..."))))`

These attributes may be applied to:
- functions,
- Objective‑C methods (instance/class),
- Objective‑C types (classes/actors) to indicate the default executor for isolated members.

**Meaning (high level):**
- A declaration annotated with `objc_executor(X)` requires that execution occurs on executor `X`.
- Entering such a declaration from another executor requires an async hop and therefore **must** be expressed with `await` (either explicitly or by the call being in an async context that performs the hop).

**Rationale:**
- `__attribute__((...))` is implementable in existing LLVM/Clang pipelines without inventing new token-level syntax.
- Executor affinity must be machine-checkable for diagnostics and for safe UI/main-thread isolation patterns.

---

## D‑005: Optional propagation (`T?` with postfix `?`) follows Rust-style carrier rules (v1)
**Decision:** Postfix propagation `e?` is allowed on an optional value `e : T?` **only** when the enclosing function’s early-exit carrier is also an optional (i.e., the enclosing function returns an optional type).

- In an optional-returning function, `e?` yields `T` when non-`nil`, otherwise performs `return nil;`.
- Using `e?` where the enclosing function returns `Result<…>` or is `throws` is **ill‑formed** in v1 (no implicit NullError mapping).

**Rationale:**
- Avoids inventing implicit “nil → error” conversions that silently change program meaning.
- Encourages explicit conversion at boundaries (`guard let … else { throw … }`, `ok_or(...)`, etc.).
- Matches a widely-understood mental model: `?` propagates within the same carrier kind.

---

## D‑006: Autorelease pool boundaries at suspension points (v1)
**Decision:** On platforms with Objective‑C autorelease pools, each async **task execution slice** (from resume to the next suspension/completion) executes within an implicit autorelease pool that is drained:
- immediately before the task suspends at an `await` that actually suspends, and
- at task completion (normal return, throw, or cancellation unwind).

Implementations may drain at additional safe points, but must not allow unbounded autorelease pool growth across long-lived tasks that repeatedly suspend.

**Rationale:**
- Prevents memory blowups in long-lived async tasks that create autoreleased objects between suspension points.
- Provides predictable behavior for mixed Swift/ObjC and Foundation-heavy codebases.
