# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors

## 7.1 Purpose
Objective‑C 3.0 introduces first-class concurrency primitives:
- `async` functions/methods
- `await` suspension points
- structured tasks and cancellation
- actor isolation as an opt-in data-race reduction tool
- sendable-like checking as an opt-in safety ladder

The design targets implementability in Clang/LLVM using coroutine lowering and a small runtime/executor layer.

## 7.2 async/await

### 7.2.1 Syntax (provisional)
Functions/methods may be marked `async`:

```objc
- (Data*) fetch:(URL*)url async throws;
```

Awaiting an async call:

```objc
Data* d = await [client fetch:url];
```

> Open issue: choose final placement of `async`/`throws` modifiers consistent with ObjC method declarations.

### 7.2.2 Semantics
- An `async` function may suspend at `await` points.
- Suspension preserves program order but allows other tasks to run.
- `await` is only permitted in an async context unless explicitly bridging.

### 7.2.3 Structured concurrency
Objective‑C 3.0 defines a standard set of structured concurrency constructs (names bikeshed):
- `Task { ... }` to spawn a child task in the current scope
- `withTaskGroup { ... }` for parallel child tasks with join semantics

Rules:
- Child tasks must complete (or be cancelled) before the parent scope exits, unless detached.

### 7.2.4 Lowering (normative intent)
A conforming implementation shall lower async functions to state machines that:
- preserve observable semantics of locals, defers, and ARC ownership,
- and support suspension and resumption.

The specification recommends coroutine lowering via LLVM’s coroutine model and requires that semantics match the abstract machine.

## 7.3 Executors
### 7.3.1 Executor concept
An executor is an abstraction responsible for scheduling the resumption of async tasks.

### 7.3.2 Default executors
The implementation shall provide:
- a default “main executor” suitable for UI work
- one or more background executors

On Apple platforms, dispatch queues are expected to back these executors (informative), but the spec defines the abstraction, not the implementation.

### 7.3.3 Executor annotations
Objective‑C 3.0 introduces an annotation (provisional):
- `@MainActor` equivalent or `@executor(main)` for methods/types
- to indicate that execution must occur on a specific executor.

The compiler shall enforce:
- cross-executor calls require `await`,
- and may insert hops where safe.

## 7.4 Cancellation
### 7.4.1 Cancellation token
Each task has an associated cancellation state.
- Cancellation is cooperative: cancellation requests do not forcibly terminate execution; they signal the task to stop.

### 7.4.2 Cancellation propagation
- Cancelling a parent task cancels its child tasks by default.
- Detaching a task separates cancellation propagation.

### 7.4.3 Cancellation checks
The standard library shall provide a check operation:
- `Task.isCancelled`
- `Task.checkCancellation()` (throws cancellation)

In strict mode, long-running loops in async contexts should be warned if they never check cancellation.

## 7.5 Actors
### 7.5.1 Actor types
An actor is a reference type that provides serialized access to its mutable state.

Syntax (provisional):
```objc
actor class Cache : NSObject
@end
```

### 7.5.2 Isolation rules
- Mutable actor instance state is isolated to the actor.
- Access from outside requires `await` and runs on the actor’s executor.
- Immutable state may be read without awaiting if proven safe.

### 7.5.3 Reentrancy
Actors may be reentrant at suspension points:
- between awaits, other messages may be processed.
- the spec shall define how to avoid invariants being broken (recommend: keep critical sections await-free).

## 7.6 Sendable-like checking
### 7.6.1 Motivation
Not all objects are safe to share across concurrency boundaries.

Objective‑C 3.0 defines a marker protocol/attribute (provisional):
- `@protocol Sendable`
- `@unsafeSendable` escape hatch

### 7.6.2 Rules
In strict concurrency checking mode (optional within strictness levels):
- values captured into concurrent tasks must be Sendable.
- blocks passed to concurrent contexts must be `@concurrent` or `@Sendable` (naming bikeshed).

Violations are errors unless explicitly marked unsafe.

## 7.7 Interactions with ARC and defer
### 7.7.1 ARC across suspension
The implementation must ensure:
- objects that are logically live across an await are retained appropriately.

### 7.7.2 defer in async functions
`defer` registered within an async function shall execute when the scope is exited, including when the function returns after suspension, preserving LIFO ordering.

> Open issue: interaction with task cancellation and unwinding semantics.

## 7.8 Bridging with existing completion-handler APIs
Objective‑C 3.0 defines annotations for completion-handler methods:
- “this is an async method”
- “this completion runs on executor X”
- “this completion is called exactly once” (or may be called multiple times)

These enable safe automatic bridging to async/await forms.

## 7.9 Open issues
- Final syntax and keyword placement.
- Whether to include typed continuations (low-level escape hatch).
- Precise model for autorelease pool behavior per task.
