# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors
_Working draft v0.7 — last updated 2025-12-28_

## 7.0 Overview

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

> Canonical spelling is the attribute form `__attribute__((objc_executor(...)))` (Decision D‑004). No new `@MainActor`-style surface annotations are required in v1 (Decision D‑010).

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

**Canonical header spellings (Decision D‑007):**
Implementations should provide canonical attribute spellings so task APIs can be declared in headers, e.g.:
- `__attribute__((objc_task_spawn))`
- `__attribute__((objc_task_detached))`
- `__attribute__((objc_task_group))`



#### 7.5.2.1 `objc_task_spawn`
Applied to a function/method that creates a **child task**.

**Canonical spelling:** `__attribute__((objc_task_spawn))`

Requirements:
- The spawned task shall inherit:
  - the current cancellation state,
  - executor context (unless the API explicitly overrides executor),
  - and (optionally) priority metadata.
- The task is considered a child of the current task for cancellation propagation.

#### 7.5.2.2 `objc_task_detached`
Applied to a function/method that creates a **detached task**.

**Canonical spelling:** `__attribute__((objc_task_detached))`

Requirements:
- Detached tasks do **not** inherit cancellation by default.
- Executor selection is implementation-defined unless explicitly specified.

#### 7.5.2.3 `objc_task_group`
Applied to a function/method that establishes a structured task group scope.

**Canonical spelling:** `__attribute__((objc_task_group))`

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
- The compiler shall enforce Sendable-like requirements for values captured by async blocks passed to APIs annotated `__attribute__((objc_task_spawn))` / `__attribute__((objc_task_detached))`.
- The compiler should warn when a returned task handle is unused (likely “fire and forget”); suggest using the detached API explicitly or awaiting/joining.

> Note: A future revision may add `task { ... }` as sugar (possibly via macros) once patterns stabilize.



## 7.6 Cancellation

### 7.6.1 Model (normative)
Cancellation in Objective‑C 3.0 is **cooperative**:
- cancellation is a request, not a preemptive termination;
- tasks and libraries must check for cancellation at defined points and react by returning/throwing early.

Each task has a cancellation state with at least:
- a boolean “cancelled” flag, and
- optional implementation-defined metadata (reason, deadline, etc.).

### 7.6.2 Cancellation propagation (normative)
- Child tasks spawned via an API annotated `__attribute__((objc_task_spawn))` / `objc_task_spawn` shall **inherit** the parent’s cancellation state and shall receive cancellation requests when the parent is cancelled.
- Detached tasks spawned via `__attribute__((objc_task_detached))` / `objc_task_detached` shall **not** inherit cancellation by default.

Task groups (`__attribute__((objc_task_group))` / `objc_task_group`) shall enforce:
- cancellation of remaining group tasks when the group scope exits by error/throw,
- cancellation inheritance from the creating task into tasks added to the group.

### 7.6.3 Required standard library surface (abstract)
A conforming implementation shall provide cancellation APIs equivalent in expressive power to:

- **Request cancellation**
  - `cancel(taskHandle)` and/or `Task.cancel()`.

- **Query cancellation state**
  - `Task.isCancelled` returning a boolean.

- **Check and react**
  - a function callable from `async` code that either:
    - throws a standard cancellation error, or
    - returns a status that can be handled by the caller.

The exact names are implementation-defined, but the compiler should be able to recognize common patterns for diagnostics (Part 12).

### 7.6.4 Cancellation and cleanup (normative)
Cancellation shall not bypass cleanup:
- `defer` actions and resource cleanups execute on cancellation-driven unwinding in the same way they execute on error unwinding (Parts 5/8).
- ARC-managed values captured by an async frame shall be released when the frame is destroyed (Part 7.9.3).

### 7.6.5 Cancellation and `await`
An `await` point **may** be a convenient place for libraries to check cancellation, but this specification does not require that `await` implicitly throws or returns on cancellation.
Instead, cancellation checking is defined by the standard library surface and library conventions.

> Rationale: Objective‑C codebases frequently interoperate with C/dispatch APIs where implicit cancellation behavior can be surprising. Making cancellation checks explicit improves auditability.

---


## 7.7 Actors

### 7.7.1 Actor declaration
Actor types are declared using `actor`:

```objc
actor class Name : NSObject
@end
```

An actor is a reference type with **isolated mutable state**. The compiler and runtime enforce that isolated state is accessed only:
- from code already executing on the actor’s isolation context, or
- via an `async` hop (i.e., using `await`) that re-enters the actor.

### 7.7.2 Isolation rules (normative)
For an actor instance `a`:

- Accessing an **isolated** instance member of `a` from outside the actor requires `await`.
- Accessing **nonisolated** members is permitted from any context.

By default in v1:
- instance methods and properties of an actor are isolated unless explicitly marked nonisolated.

### 7.7.3 Nonisolated members (header-facing)
A conforming implementation shall provide a way to mark a member as nonisolated.

**Canonical spelling (illustrative):**
- `__attribute__((objc_nonisolated))` on the declaration.

ObjC 3.0 mode may accept `@nonisolated` as sugar.

Nonisolated members:
- shall not access actor-isolated mutable state unless they re-enter the actor (which requires `await`).

### 7.7.4 Actor executors
Each actor has an associated executor that serializes isolated execution.

- If the actor type is annotated with `__attribute__((objc_executor(X)))`, then isolated execution occurs on executor `X`.
- Otherwise, the actor uses an implementation-defined **actor default executor** (typically a serial executor).

### 7.7.5 Reentrancy
An isolated actor method may `await`. While it is suspended, other work may execute on the actor.
Therefore, actor code must treat each `await` as a potential reentrancy point.

This draft does not (yet) define a v1 “nonreentrant” actor mode; it is a potential future extension.

### 7.7.6 Cross-actor calls
A call from outside an actor to an isolated member of that actor shall be treated as an `async` call site requiring `await`.

If the member is additionally `throws`, the canonical spelling is `try await` (Part 7.9.1).

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

Provide an escape hatch for types/values that are intentionally shared across concurrency domains.

**Canonical spelling (illustrative):** `__attribute__((objc_unsafe_sendable))` (see **01B_ATTRIBUTE_AND_SYNTAX_CATALOG.md**).

> Note: A toolchain may offer nicer surface sugar, but such sugar is not required in v1 (Decision D‑010).

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

## 7.10 Diagnostics
Minimum diagnostics:
- `await` outside async (error)
- calling async without await (error)
- calling executor-annotated declarations from the wrong executor without an async hop (error in strict concurrency mode)
- cross-actor isolated access without await (error in strict)
- Sendable violations in strict concurrency mode (error)
- unused task handles returned from APIs annotated `__attribute__((objc_task_spawn))` (warning in strict concurrency mode)



## 7.11 Open issues
1. (Resolved in v0.5) Executor annotations use `__attribute__((objc_executor(...)))` canonical spelling.
2. (Resolved in v0.4) v1 is library-defined for spawning; `task {}` keyword syntax is deferred (possible macro sugar later).
3. (Resolved in v0.5) Implicit per-slice autorelease pools drained on suspend and completion are normative.
4. Extent of static race checking vs marker-based checking.
