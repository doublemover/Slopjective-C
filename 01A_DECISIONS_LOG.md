# Objective‑C 3.0 — Design Decisions Log (v0.5)
_Last updated: 2025-12-28_

This log captures explicit “ship/no‑ship” decisions made to resolve prior open issues.

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

## D‑004: Optional propagation (`?`) does not map `nil` to errors in v1
**Decision:** In v1, postfix propagation `?` applied to an optional (`T?`) may only be used when the enclosing function/method returns an optional type compatible with `T?`. In that case, `e?` early-returns `nil` from the enclosing function.

Using `e?` where `e` is an optional in a `throws` function or a `Result`-returning function is **ill‑formed** in v1.

**Rationale:**
- `nil` often encodes *absence*, not necessarily *failure*.
- Implicitly converting `nil` to an error risks surprising control flow and poor diagnostics (“why did this throw?”).
- The ergonomics gap is already addressed by `guard let` / `if let` plus explicit `throw`/`return Err(...)`.

**Spec impact:** Part 6 §6.6.4 is tightened; prior “NullError mapping” is removed from v1.

**Future direction:** Add explicit sugar for mapping, e.g. `x ?! errorExpr` or `x ?? throw errorExpr`, in a future revision.

---

## D‑005: Executor annotation surface syntax in v1 is `@executor(main)`
**Decision:** The v1 executor annotation is standardized as:

- `@executor(main)`

This annotation may be applied to functions/methods/types to indicate **main-executor isolation**.

Custom executor designators (beyond `main`) are deferred.

**Semantics (summary):**
- Calling a `@executor(main)` declaration from a context not known to be on the main executor requires an **executor hop**.
- Executor hops are suspension points; in async contexts they require `await`.
- In non-async contexts, hops cannot be implicit; cross-executor calls are ill‑formed in strict mode (diagnosed in permissive mode), with guidance to use explicit bridging APIs.

**Rationale:**
- Captures the highest-value use case (UI/main-thread affinity) with minimal surface area.
- Avoids freezing a custom-executor design prematurely.
- Matches common ecosystem expectations and existing Swift experience.

**Spec impact:** Part 7 §7.4.3 defines the grammar and rules; Part 12 adds diagnostics.

---

## D‑006: Autorelease pool draining at task suspension boundaries is required
**Decision:** On platforms with Objective‑C autorelease pools, the ObjC 3.0 concurrency runtime must ensure that **autoreleased objects do not accumulate unboundedly across suspension points**. Concretely:

- Each time a task/job is executed or resumed on an executor, execution occurs within an implicit autorelease pool.
- That pool is drained when the job returns to the executor (by completion or suspension).

**Rationale:**
- Prevents memory spikes in async-heavy code that creates autoreleased temporaries.
- Aligns with the conceptual “runloop iteration drains the pool” model, but for executors/tasks.

**Spec impact:** Part 7 adds a normative runtime contract for autorelease pool behavior.
