# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors
_Working draft v0.5 — last updated 2025-12-28_

## 7.0 Overview

### v0.4 resolved decisions
- v1 does **not** add `task {}` keyword syntax. Task spawning is provided by the standard library and recognized by the compiler via standardized attributes.

### v0.5 resolved decisions
- Executor affinity/isolation uses standardized syntax `@executor(main)` in v1.
- Autorelease pools are drained at task suspension boundaries by the concurrency runtime (normative contract).

Objective‑C 3.0 introduces structured concurrency that is composable, auditable, and implementable.

This part defines:
- the `async` effect and `await` expression,
- executor model and annotations,
- structured tasks and cancellation (standard library contract),
- actor isolation,
- sendable-like checking as an opt-in safety ladder.

---

## 7.1 Lexical and grammar additions

### 7.1.1 New keywords
In ObjC 3.0 mode, the following are reserved by this part:

- `async`, `await`, `actor`

### 7.1.2 Effects
`async` and `throws` are effects that become part of a function’s type.

---

## 7.2 Async declarations

### 7.2.1 Grammar (illustrative)
```text
function-effect-specifier:
    'async'
  | 'throws'
  | 'async' 'throws'
  | 'throws' 'async'
```

### 7.2.2 Static semantics
- `await` is permitted only in async contexts.
- calling an async function requires `await`.

### 7.2.3 Block types
Async is part of a block’s type:
- `R (^)(Args) async`
- `R (^)(Args)` (sync)

---

## 7.3 `await` expressions

### 7.3.1 Grammar
```text
await-expression:
    'await' expression
```

### 7.3.2 Static semantics
An operation that may suspend shall appear within `await`.

At minimum, this includes:
- any call to an `async` function,
- any cross-actor access (calling an actor-isolated method or accessing actor-isolated state from outside the actor), and
- any call/access that requires an **executor hop** due to executor isolation (e.g., calling a `@executor(main)` declaration from a non-main executor context).

### 7.3.3 Dynamic semantics
`await e` may suspend:
- suspension returns control to the current executor,
- later resumes on an executor consistent with isolation rules.

---

## 7.4 Executors

### 7.4.1 Concept
An executor schedules task continuations.

### 7.4.2 Default executors
Runtime shall provide:
- main executor
- global executor

### 7.4.3 Executor annotations (v1)

#### 7.4.3.1 Syntax (normative)
Objective‑C 3.0 v1 standardizes a single executor isolation annotation:

- `@executor(main)`

This annotation may be applied to:
- function and method declarations,
- type declarations (including `actor class`), and
- property declarations (affecting synthesized accessors).

#### 7.4.3.2 Meaning
A declaration annotated `@executor(main)` is **main-executor isolated**:
- its dynamic execution shall occur on the main executor.

#### 7.4.3.3 Calling rules and `await`
Calling a `@executor(main)` declaration from a context not known to already be on the main executor requires an **executor hop**.

- In an `async` context, such a hop is a potential suspension point and therefore requires `await`, even if the callee is not declared `async`.
- In a non-`async` context, an implicit hop is not permitted in v1. Such calls are ill‑formed in strict mode and diagnosed in permissive mode, with guidance to:
  - make the calling context `async` and use `await`, or
  - use an explicit library bridging primitive (e.g., “run on main executor”) if synchronous behavior is required.

#### 7.4.3.4 Future design space (non-normative)
Future revisions may extend `@executor(...)` to support custom executor designators. v1 intentionally standardizes only `main`.


---


## 7.5 Tasks and structured concurrency (standard library contract)

### 7.5.1 Design decision (v1)
Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided by the **standard library**, but the compiler shall recognize task-related APIs via attributes so that:
- Sendable-like checking can be enforced for concurrent captures,
- cancellation inheritance and “child vs detached” semantics are predictable,
- diagnostics can be issued for misuse (e.g., discarding a Task handle).

### 7.5.2 Standard attributes (normative)

#### 7.5.2.1 `@task_spawn`
Applied to a function/method that creates a **child task**.

Requirements:
- The spawned task shall inherit:
  - the current cancellation state,
  - executor context (unless the API explicitly overrides executor),
  - and (optionally) priority metadata.
- The task is considered a child of the current task for cancellation propagation.

#### 7.5.2.2 `@task_detached`
Applied to a function/method that creates a **detached task**.

Requirements:
- Detached tasks do **not** inherit cancellation by default.
- Executor selection is implementation-defined unless explicitly specified.

#### 7.5.2.3 `@task_group`
Applied to a function/method that establishes a structured task group scope.

Requirements:
- All tasks added to the group shall complete (or be cancelled) before the group scope returns.
- If the group scope exits by throwing, the group shall cancel remaining tasks before unwinding completes.

### 7.5.3 Required standard library surface (abstract)

A conforming implementation shall provide, in a standard concurrency module, APIs equivalent in expressive power to:

- **Spawn child task**
  - input: an `async` block (possibly throwing)
  - output: a task handle representing the child task

- **Spawn detached task**
  - same, but detached semantics

- **Join**
  - `await`-join a task handle to retrieve its result (or throw its error)

- **Task groups**
  - create group, add tasks, iterate results, join implicitly at scope end

The exact naming and Objective‑C surface syntax are implementation-defined, but the compiler shall be able to identify the spawn/group APIs via the attributes above.

### 7.5.4 Compiler checking hooks

In strict concurrency checking mode:
- The compiler shall enforce Sendable-like requirements for values captured by `@task_spawn` / `@task_detached` async blocks.
- The compiler should warn when a returned task handle is unused (likely “fire and forget”); suggest using the detached API explicitly or awaiting/joining.

> Note: A future revision may add `task { ... }` as sugar (possibly via macros) once patterns stabilize.


## 7.6 Cancellation

- cooperative cancellation
- propagation to child tasks
- cancellation check function(s)
- cancellation must still run defers/cleanups

---

## 7.7 Actors

### 7.7.1 Actor types
```objc
actor class Name : NSObject
@end
```

### 7.7.2 Isolation
- mutable state is isolated
- cross-actor access requires await

### 7.7.3 Reentrancy
Await within actor may permit reentrancy.

---

## 7.8 Sendable-like checking

### 7.8.1 Marker protocol
```objc
@protocol Sendable
@end
```

### 7.8.2 Enforcement sites
- captures into concurrent tasks
- cross-actor arguments/returns

Provide `@unsafeSendable` escape hatch.

---

## 7.9 Interactions

- `try await` composition
- `defer` in async functions preserves semantics
- ARC lifetimes across suspension must be preserved
- autorelease pool behavior at suspension boundaries is specified (normative)


### 7.9.1 Autorelease pools at suspension boundaries (normative)
On platforms where Objective‑C autorelease pools are supported, the concurrency runtime shall ensure that autoreleased objects do not accumulate unboundedly across suspension points.

A conforming implementation shall satisfy the following contract:

- Each time an async task/job is begun or resumed on an executor, execution of that job occurs within an implicit autorelease pool.
- That implicit autorelease pool is drained when the job yields back to the executor, whether by:
  - completing normally,
  - throwing, or
  - suspending at an `await` point.

This requirement may be satisfied by wrapping each executor job invocation in an `@autoreleasepool { ... }` region (or an equivalent runtime mechanism).

**Note:** Explicit `@autoreleasepool { ... }` blocks inside user code remain well-defined and may be used for tighter scoping; they nest within the implicit pool.


---

## 7.10 Diagnostics
Minimum diagnostics:
- await outside async (error)
- calling async without await (error)
- cross-actor isolated access without await (error in strict)
- `@executor(main)` isolated access without required `await` hop (error in strict concurrency mode)
- `@executor(main)` call requiring a hop from a non-async context (diagnose; strict mode error)
- sendable violations in strict concurrency mode (error)

---

## 7.11 Open issues
1. (Resolved in v0.5) v1 uses `@executor(main)`; custom executors deferred.
2. (Resolved in v0.4) v1 is library-defined for spawning; `task {}` keyword syntax is deferred (possible macro sugar later).
3. (Resolved in v0.5) Autorelease pools are drained at job boundaries (resume/suspend/complete).
4. Extent of static race checking vs marker-based checking.
