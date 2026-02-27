# Objective‑C 3.0 — Lowering, ABI, and Runtime Contracts {#c}

_Working draft v0.11 — last updated 2026-02-23_

## C.0 Scope and audience {#c-0}

This document is primarily **normative for implementations** (compiler + runtime + standard library).
It does not change source-level semantics defined elsewhere; it constrains how a conforming implementation supports those semantics under **separate compilation** and across **module boundaries**.

Where this document provides “canonical lowering” patterns, those are:

- **required** when explicitly labeled _normative_; and
- otherwise **recommended** as the default lowering for LLVM/Clang implementations.

### C.0.1 Status tags used in this document {#c-0-1}

Status labels in this document follow [Part 0](#part-0) [§0.3.2](#part-0-3-2):

- **normative** / **minimum** / **normative intent**: contributes to conformance requirements,
- **recommended**: QoI guidance,
- **informative** / **non-normative**: explanatory material.

## C.1 Definitions (informative) {#c-1}

- **ABI**: the binary calling convention and data layout visible to linkers and other languages.
- **Runtime hooks**: functions/types provided by a support library needed to implement language semantics (e.g., executors, task scheduling).
- **Effect**: a property of a callable type that changes call/return behavior (`throws`, `async`).

## C.2 Separate compilation requirements (normative) {#c-2}

A conforming implementation shall ensure:

1. **Effect and isolation consistency is recorded in module metadata.**  
   If a declaration is imported from a module, its:
   - `async`/`throws` effects,
   - executor affinity (`objc_executor(...)`),
   - actor isolation (actor type + isolated/nonisolated members), and
   - any dispatch-affecting attributes (`objc_direct`, `objc_direct_members`, `objc_dynamic`, `objc_sealed`, etc.)
     shall be part of the imported type information.

   The minimum required set is enumerated normatively in **[MODULE_METADATA_AND_ABI_TABLES.md](#d)**.

2. **Mismatched redeclarations are diagnosed.**  
   Redeclaring an imported function/method with a different effect set or incompatible lowering-affecting attributes is ill-formed.

3. **Interface emission is semantics-preserving.**  
   If a textual module interface is emitted, it shall preserve effects and attributes (see [B.7](#b-7)).

## C.2.1 Required module metadata (normative) {#c-2-1}

A conforming implementation shall preserve in module metadata and interface emission the items listed in **[D.3.1](#d-3-1) [Table A](#d-3-1)**.

This requirement is intentionally testable: if importing a module can change whether a call requires `try`/`await`, or can change dispatch legality (`objc_direct` / `objc_direct_members` / `objc_dynamic`), the implementation is non-conforming.

## C.3 Canonical lowering patterns (mixed; see subsection tags) {#c-3}

### C.3.1 Optional message send `[receiver? ...]` (normative) {#c-3-1}

Optional message send semantics are defined in [Part 3](#part-3). Conforming implementations shall lower:

```objc
[receiver? sel:arg1 other:arg2]
```

as if by the following abstract steps:

1. Evaluate `receiver` exactly once into a temporary.
2. If the receiver temporary is `nil`, produce the “nil case” result:
   - object/block return: `nil`
   - `void` return: no effect
3. Otherwise, evaluate argument expressions in source order and perform the message send.

**Key requirement:** argument expressions shall **not** be evaluated in the `nil` case.

### C.3.2 Nil-coalescing `??` (recommended) {#c-3-2}

`a ?? b` should lower to a single evaluation of `a` with a conditional branch.
Implementations should avoid duplicating retains/releases when `a` is an object pointer.

### C.3.3 Postfix propagation `?` (recommended) {#c-3-3}

The postfix propagation operator in Parts 3 and 6 should lower to a structured early-exit:

- `Optional<T>` carrier: `if (!x) return nil; else use *x;`
- `Result<T,E>` carrier: `if (isErr(x)) return Err(e); else use t;`

Implementations should preserve left-to-right evaluation and should not introduce hidden temporaries with observable lifetimes beyond what ARC already requires.

## C.4 `throws` ABI and lowering (normative for implementations) {#c-4}

### C.4.1 Canonical ABI shape for throwing functions (normative for implementations) {#c-4-1}

For a function declared:

```c
R f(A1 a1, A2 a2) throws;
```

a conforming implementation shall provide a stable calling convention that allows the caller to receive either:

- a normal return of type `R`, or
- an error value of type `id<Error>`.

**Canonical ABI (recommended and permitted as normative):** the implementation uses an additional trailing _error-out parameter_:

```c
R f(A1 a1, A2 a2, id<Error> _Nullable * _Nullable outError);
```

Rules:

- On success: if `outError != NULL`, store `nil` into `*outError`, then return the value.
- On error: if `outError != NULL`, store the thrown error into `*outError`, then return an unspecified value of type `R` (or zero-initialized as QoI).

> Note: For `void` return, the return value is omitted and the outError store alone indicates failure.

An implementation may choose a different ABI shape, but only if it is:

- stable under separate compilation, and
- representable in module metadata so importers use the same ABI.

### C.4.2 Throwing Objective‑C methods (recommended) {#c-4-2}

For Objective‑C methods, the same “trailing error-out parameter” approach is recommended: the selector’s parameter list includes the implicit trailing outError at the ABI level.
The language surface does not expose that parameter; it is introduced/consumed by the compiler as part of the `throws` effect.

### C.4.3 Lowering of `try` (recommended) {#c-4-3}

### C.4.4 Objective‑C selectors vs ABI signatures (normative intent) {#c-4-4}

For Objective‑C methods, the `throws` effect is **not** part of the selector spelling. The selector used for lookup remains the source-level selector.

However, the _call ABI_ of the method’s implementation may include the canonical trailing error-out parameter described in [C.4.1](#c-4-1).

A conforming implementation shall ensure that:

- when a call site is type-checked as calling a `throws` method, the generated call passes the error-out parameter in the canonical position required by that implementation’s ABI; and
- when forming or calling an `IMP` for a `throws` method (e.g., via `methodForSelector:`), the compiler uses a function pointer type whose parameter list includes the error-out parameter.

**Reflection note (non-normative):** baseline Objective‑C runtime type-encoding strings do not represent `throws`. This draft does not require extending the runtime type encoding in v1. Toolchains may provide extended metadata for reflective invocation as an extension.

A `try` call should lower to:

- allocate a local `id<Error> err = nil;`
- perform the call with `&err`
- if `err != nil`, branch to the enclosing error propagation/handler path.

`try?` and `try!` should be implemented as specified in [Part 6](#part-6), using the same underlying error value.

## C.5 `async` ABI and lowering (normative for implementations) {#c-5}

### C.5.1 Coroutine model (normative for implementations) {#c-5-1}

A conforming implementation shall implement `async` functions such that:

- each `await` is a potential suspension point,
- local variables required after an `await` have stable storage across suspension,
- ARC + cleanup semantics are preserved across suspension (see Parts 4 and 7).

**Recommended lowering:** lower `async` functions to LLVM coroutine state machines (e.g., using LLVM’s coroutine intrinsics), with resumption scheduled by the active executor.

### C.5.2 Task context (normative) {#c-5-2}

While executing an `async` function, there exists a _current task context_ that provides at least:

- cancellation state,
- current executor identity,
- and (if actors are used) current actor isolation context.

The concrete representation is implementation-defined, but the values must be queryable by the standard library and usable by the runtime to enforce executor hops.

### C.5.3 Executor hops (recommended) {#c-5-3}

When entering a declaration annotated with `objc_executor(X)`, implementations should:

- check whether the current executor is `X`;
- if not, suspend and schedule resumption on `X`.

Implementations should provide (directly or via the standard library) a primitive equivalent to:

- `await ExecutorHop(X)`

so that the compiler can lower hops in a uniform way.

### C.5.4 Call-site lowering for isolation boundaries (normative intent) {#c-5-4}

[Part 7](#part-7) defines `await` as the marker for **potential suspension** ([Decision D-011](#decisions-d-011)), not merely “calling explicitly-async functions.”

A conforming implementation shall treat the following as potentially-suspending at call sites when the relevant metadata indicates an isolation boundary is being crossed:

- entering an `objc_executor(X)` declaration from code not proven to already be on executor `X`;
- entering an actor-isolated member from outside that actor’s executor.

**Recommended lowering pattern (informative):**

1. If already on the required executor, call directly.
2. Otherwise:
   - suspend the current task,
   - enqueue the continuation onto the required executor (or actor executor),
   - resume and perform the call,
   - then continue.

This may be implemented either as:

- an implicit compiler-inserted hop around the call (still requiring `await` in the source), or
- a lowering through an explicit hop primitive plus a direct call.

The observable requirement is: the call **may suspend**, and the work executes on the correct executor.

## C.6 Actors: runtime representation and dispatch (normative for implementations) {#c-6}

### C.6.1 Actor executor (normative) {#c-6-1}

Each actor instance shall be associated with a serial executor used to protect isolated mutable state.
Cross-actor access requires enqueueing work onto that executor.

### C.6.2 Actor method entry (recommended) {#c-6-2}

A non-reentrant actor method entry should:

- hop to the actor’s executor (if not already on it),
- then execute until the next suspension point,
- preserving the actor’s invariants.

Reentrancy behavior is defined by [Part 7](#part-7); this document only constrains that runtime scheduling is consistent with that behavior.

## C.7 Autorelease pools at suspension points (normative for ObjC runtimes) {#c-7}

On Objective‑C runtimes with autorelease semantics, implementations shall ensure:

- Each _task execution slice_ (from resume to next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule exists to make async code’s memory behavior predictable for Foundation-heavy code.

## C.8 Direct/final/sealed/dynamic lowering (recommended) {#c-8}

- `objc_direct` methods should lower to direct function calls when statically referenced, and may omit dynamic method table entries as permitted by [Part 9](#part-9).
- Methods made effectively direct by `objc_direct_members` should lower identically to explicit `objc_direct`, except for members explicitly marked `objc_dynamic`.
- `objc_dynamic` methods should lower through selector-based dynamic dispatch even when surrounding declarations are eligible for direct/devirtualized lowering.
- `objc_final` should enable devirtualization and dispatch optimization but shall not change observable message lookup semantics unless paired with `objc_direct`.
- `objc_sealed` should enable whole-module reasoning about subclass sets; implementations should treat cross-module subclassing attempts as ill-formed or diagnose at link time where possible.

## C.9 Macro/derive expansion and ABI (normative for implementations) {#c-9}

If macro expansion or derives synthesize declarations that affect layout or vtables/dispatch surfaces (including generated ivars, properties, methods), then:

- the synthesis must occur before ABI layout is finalized for the type;
- the synthesized declarations must appear in any emitted module interface or API dump.

## C.10 System programming analysis hooks (recommended) {#c-10}

This section collects implementation guidance for **[Part 8](#part-8)** features that are primarily enforced by diagnostics and analysis rather than runtime hooks.

### C.10.1 Resources and cleanup {#c-10-1}

`objc_resource(...)` ([Part 8](#part-8)) should lower equivalently to a scope-exit cleanup action (similar to `cleanup(...)`), but implementations should additionally:

- perform use-after-move and double-close diagnostics in strict-system profiles;
- treat the “invalid sentinel” as the moved-from / reset state.

### C.10.2 Borrowed pointers {#c-10-2}

`borrowed T *` and `objc_returns_borrowed(owner_index=...)` ([Part 8](#part-8)) should be represented in the AST and module metadata ([D Table A](#d-3-1)) so that:

- call sites can enforce escape analysis in strict-system profiles;
- importers preserve the qualifier for downstream tooling.

No runtime support is required by these features in v1; the contract is primarily compile-time diagnostics and toolability.

## C.11 Conformance tests (non-normative) {#c-11}

Implementations are encouraged to provide a conformance suite that includes:

- effect mismatch diagnostics across modules,
- correct argument evaluation semantics for optional sends,
- `throws` propagation behavior in nested `do/catch`,
- executor hop correctness and actor isolation enforcement across module boundaries.

