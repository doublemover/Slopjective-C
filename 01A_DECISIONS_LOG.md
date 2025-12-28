# Objective‑C 3.0 — Design Decisions Log (v0.7)
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
**Decision:** Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword or statement form. Instead, the compiler recognizes task-related APIs via standardized attributes so diagnostics and safety checks can be applied consistently.

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
**Decision:** Features intended to appear in public headers shall have a **canonical Clang-style attribute spelling** (`__attribute__((...))`) so that:
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


---

## D‑008: Nonnull-by-default regions — canonical spelling (v1)
**Decision:** Nonnull-by-default regions are established using standardized pragmas.

**Canonical spelling:**
```c
#pragma objc assume_nonnull begin
#pragma objc assume_nonnull end
```

**Recommended alias support (Clang-family toolchains):**
```c
#pragma clang assume_nonnull begin
#pragma clang assume_nonnull end
```

**Rationale:**
- Nonnull-by-default must be representable in module interfaces and must not depend on fragile macro include ordering.
- Pragmas are widely used in existing Objective‑C ecosystems and are implementable without adding new `@` directives.

**Spec impact:** Part 3 §3.2.4 is updated; “directive vs pragma vs module metadata” spelling is resolved for v1.

---

## D‑009: Generic methods are deferred (v1)
**Decision:** Objective‑C 3.0 v1 supports **generic types** (parameterized interfaces/protocols and standard library generic types such as `Result<T,E>` and `KeyPath<Root,Value>`), but **does not** add generic parameter clauses or `where` clauses on Objective‑C methods/functions.

**Rationale:**
- Objective‑C method grammar (selectors with multiple segments) makes “template-like” syntax difficult to add without ambiguity and migration pain.
- The ecosystem can achieve most value through generic types + protocol constraints + library APIs, while leaving method generics for a future revision.

**Spec impact:** Part 3 §3.5 removes generic method grammar from v1 and treats it as a future extension.

---

## D‑010: No new `@`-prefixed declaration modifiers are required in v1
**Decision:** Aside from required Objective‑C constructs (e.g., `@interface`, `@implementation`, `@keypath`, and module-qualified identifiers `@Module.Symbol`), the language does not require new `@`-prefixed declaration modifiers (e.g., `@final`, `@sealed`, `@task_spawn`, `@derive`) in v1.

Instead, the canonical spelling for such features is `__attribute__((...))` as cataloged in **01B_ATTRIBUTE_AND_SYNTAX_CATALOG.md**.

**Rationale:**
- Keeps parsing surface small and reduces source compatibility hazards.
- Ensures features can be expressed in headers/module interfaces using well-established mechanisms.

**Spec impact:** Parts 7, 9, and 10 are updated to present attribute spellings as normative and treat sugar as optional/non-required.

---

## D‑011: Derives — canonical spelling (v1)
**Decision:** Derives are applied using an attribute.

**Canonical spelling:**
```c
__attribute__((objc_derive(Equatable, Hashable)))
@interface Person : NSObject
@end
```

**Rationale:**
- Derives must be visible to tooling and survive module interface generation.
- Attributes compose well with existing declaration syntax.

**Spec impact:** Part 10 §10.2.1 is updated to remove “provisional” derive syntax.


---

## D‑012: Optional sugar applies to `id` and `Class` (v1)
**Decision:** The optional-type suffix `?` is permitted on the special Objective‑C object reference types `id` and `Class`, and on protocol-qualified `id` types.

Examples:
```objc
id? maybeObject;
Class? maybeClass;
id<NSCopying>? maybeKey;
```

**Canonicalization:** These spellings canonicalize to `id _Nullable`, `Class _Nullable`, and `id<Proto> _Nullable` respectively (Part 3 §3.3.1.1).

**Rationale:**
- Aligns with how developers think about nullability for these ubiquitous types.
- Removes an ergonomic footgun where `id`/`Class` require different spelling than ordinary object pointers.


---

## D‑013: Module-qualified identifiers use `@<module-path>.<TopLevelName>` (v1)
**Decision:** Objective‑C 3.0 introduces a module-qualified identifier spelling that is unambiguous in ObjC++ translation units.

**Canonical spelling:**
```text
@<module-path>.<TopLevelName>
```

Where `<module-path>` is one or more identifiers separated by `.` (modules/submodules), and `<TopLevelName>` is an identifier naming a top-level exported declaration.

**Parsing rule:** The final identifier after the last `.` is the declaration name; all preceding components form the module path.

**Rationale:**
- Avoids introducing `::`-style scope that conflicts with C++ expectations and style.
- Allows submodule paths without additional quoting or separators.
