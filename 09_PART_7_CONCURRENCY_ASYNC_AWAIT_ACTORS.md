# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors
_Working draft v0.3 — last updated 2025-12-28_

## 7.0 Overview
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

### 7.4.3 Executor annotations
Provisional attribute:
- `@executor(main|global|custom...)`

Cross-executor entry requires await/hop unless proven already on executor.

---

## 7.5 Tasks and structured concurrency (standard library contract)

### 7.5.1 Task creation
Provide:
- child task spawn (inherits context)
- detached task spawn (does not inherit cancellation)

### 7.5.2 Joining
Provide `await`-join.

### 7.5.3 Task groups
Provide group creation/spawn/join with structured lifetime rule.

---

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
- recommended autorelease pool draining policy

---

## 7.10 Diagnostics
Minimum diagnostics:
- await outside async (error)
- calling async without await (error)
- cross-actor isolated access without await (error in strict)
- sendable violations in strict concurrency mode (error)

---

## 7.11 Open issues
1. Final surface syntax for executor annotations.
2. Whether to add dedicated `task {}` syntax.
3. Precise autorelease pool rules.
4. Extent of static race checking vs marker-based checking.
