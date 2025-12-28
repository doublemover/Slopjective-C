# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors
_Working draft v0.8 — last updated 2025-12-28_

## 7.0 Overview

### v0.8 resolved decisions
- Canonical attribute spellings for concurrency/executor features are defined in 01B.
- Lowering/runtime contracts for `async/await`, executors, and actors are defined in 01C.


### v0.5 resolved decisions
- Executor annotations use a canonical Clang-style spelling `__attribute__((objc_executor(...)))`.
- Autorelease pool behavior at suspension points is normative: each task execution slice runs inside an implicit pool drained on suspend and completion.


### v0.4 resolved decisions
- v1 does **not** add `task {}` keyword syntax. Task spawning is provided by the standard library and recognized by the compiler via standardized attributes.

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
A call to an async function shall appear within `await`.

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


### 7.4.3 Executor annotations (normative)

Objective‑C 3.0 defines executor affinity using a standardized attribute spelling.

#### 7.4.3.1 Canonical spelling
Executor affinity shall be expressed using a Clang-style attribute:

- `__attribute__((objc_executor(main)))`
- `__attribute__((objc_executor(global)))`
- `__attribute__((objc_executor(named("..."))))`

The attribute may be applied to:
- functions,
- Objective‑C methods,
- Objective‑C types (including actor classes), establishing a default executor for isolated members unless overridden.

#### 7.4.3.2 Static semantics
If a declaration is annotated `objc_executor(X)`, then:
- entering that declaration from code not already executing on executor `X` requires an executor hop,
- and therefore must occur only in an `async` context (because hops may suspend).

In strict concurrency checking mode, calling an executor-annotated declaration from a context that is not proven to be on that executor is ill-formed unless the call is guarded by an `await` hop.

#### 7.4.3.3 Dynamic semantics (model)
An implementation shall ensure that execution of an executor-annotated declaration occurs on the specified executor. This may be achieved by:
- inserting an implicit hop at the call boundary, or
- requiring the caller to perform an explicit hop API that is `await`ed.

The observable effect is that executor-affine code never runs on an incorrect executor.

#### 7.4.3.4 Notes
- `main` is intended for UI-thread-affine work.
- `global` is intended for background execution.
- `named("...")` is for custom executors defined by the runtime/standard library.


---


## 7.5 Tasks and structured concurrency (standard library contract)

### 7.5.1 Design decision (v1)
Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided by the **standard library**, but the compiler shall recognize task-related APIs via attributes so that:
- Sendable-like checking can be enforced for concurrent captures,
- cancellation inheritance and “child vs detached” semantics are predictable,
- diagnostics can be issued for misuse (e.g., discarding a Task handle).

### 7.5.2 Standard attributes (normative)

#### 7.5.2.1 `__attribute__((objc_task_spawn))`
Applied to a function/method that creates a **child task**.

Requirements:
- The spawned task shall inherit:
  - the current cancellation state,
  - executor context (unless the API explicitly overrides executor),
  - and (optionally) priority metadata.
- The task is considered a child of the current task for cancellation propagation.

#### 7.5.2.2 `__attribute__((objc_task_detached))`
Applied to a function/method that creates a **detached task**.

Requirements:
- Detached tasks do **not** inherit cancellation by default.
- Executor selection is implementation-defined unless explicitly specified.

#### 7.5.2.3 `__attribute__((objc_task_group))`
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
- The compiler shall enforce Sendable-like requirements for values captured by `__attribute__((objc_task_spawn))` / `__attribute__((objc_task_detached))` async blocks.
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

### 7.9.1 `try await` composition
When calling an `async throws` function, both effects must be handled:
- `await` handles the async effect (may suspend),
- `try` handles the throws effect (may throw).

A conforming implementation shall accept `try await f(...)` as the canonical order.

### 7.9.2 `defer` and scope cleanup across suspension
Defers registered in an async function scope execute when that scope exits, even if the function has suspended and resumed multiple times.

### 7.9.3 ARC lifetimes across suspension
Objects referenced across an `await` must remain alive until no longer needed. Implementations shall retain values captured into an async frame and release them when the frame is destroyed (normal completion, throw, or cancellation unwind).

### 7.9.4 Autorelease pools at suspension points (normative on ObjC runtimes)
On platforms that support Objective‑C autorelease pools:

- Each **task execution slice** (from a resume point until the next suspension or completion) shall execute within an **implicit autorelease pool**.
- If an `await` suspends the current task, the implementation shall drain the implicit autorelease pool **before** the task suspends and control returns to the executor.
- The implementation shall drain the implicit pool at task completion (normal return, thrown error, or cancellation unwind).

If an `await` completes synchronously without suspending, draining is permitted but not required.

Implementations may drain at additional safe points, but shall not permit unbounded autorelease pool growth across long-lived tasks that repeatedly suspend.


---



## 7.12 Lowering, ABI, and runtime hooks (normative for implementations)

### 7.12.1 Separate compilation requirements
Conforming implementations shall satisfy the separate compilation requirements in 01C.2 for all concurrency-relevant declarations, including:
- `async` effects,
- executor annotations (`objc_executor(...)`),
- actor isolation metadata,
- and task-spawn recognition attributes (01B.3.2).

### 7.12.2 Coroutine lowering model (informative summary)
The recommended lowering model is:
- `async` functions lower to coroutine state machines (01C.5),
- `await` lowers to a potential suspension + resumption scheduled by the current executor,
- values that cross suspension are retained/lowered in a way that preserves ARC semantics.

### 7.12.3 Executor hop primitive
Implementations should provide (directly or via the standard library) a primitive that represents:
- “suspend here and resume on executor X”

so that the compiler can uniformly lower:
- calls into `objc_executor(main)` declarations, and
- actor isolation hops.

### 7.12.4 Actor runtime representation (minimum)
Each actor instance shall be associated with a serial executor that:
- enqueues actor-isolated work items, and
- ensures mutual exclusion for isolated mutable state.

The concrete queue representation is implementation-defined, but the association must be observable by the runtime/stdlib to implement `await`ed cross-actor calls.

### 7.12.5 Autorelease pools at suspension points
Autorelease pool boundaries at suspension points are required by Decision D‑006 and defined normatively in 01C.7 and §7.9.4.


## 7.10 Diagnostics
Minimum diagnostics:
- `await` outside async (error)
- calling async without await (error)
- calling executor-annotated declarations from the wrong executor without an async hop (error in strict concurrency mode)
- cross-actor isolated access without await (error in strict)
- Sendable violations in strict concurrency mode (error)
- unused task handles returned from `__attribute__((objc_task_spawn))` APIs (warning in strict concurrency mode)



## 7.11 Open issues
- Whether to standardize a macro-based sugar for task spawning (keeping the core library-defined) without freezing semantics too early.
- Whether to make actor reentrancy controls explicit in the type system (e.g., `nonreentrant` markers) in a future revision.
- Whether to standardize a portable interface format for concurrency metadata beyond module files (see Part 2).
