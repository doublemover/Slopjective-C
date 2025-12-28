# Part 4 — Memory Management and Ownership
_Working draft v0.8 — last updated 2025-12-28_

## 4.1 Purpose
This part defines how ObjC 3.0:
- builds on ARC as the baseline ownership system,
- standardizes ownership qualifiers and lifetime guarantees,
- introduces transfer/consumption annotations that improve correctness and performance,
- integrates “retainable families” beyond ObjC objects (delegated to Part 8 for system libraries).

## 4.2 ARC as normative baseline
In ObjC 3.0 mode with ARC enabled:
- the compiler shall insert retains/releases/autoreleases according to ARC rules.
- ownership qualifiers (`__strong`, `__weak`, `__autoreleasing`, `__unsafe_unretained`) shall be honored.

ObjC 3.0 does not change ARC’s fundamental model: it remains reference counting without cycle collection.

## 4.3 Ownership qualifiers (standardized)
### 4.3.1 `__strong`
Strong references keep objects alive.

### 4.3.2 `__weak`
Weak references are zeroing and do not keep objects alive.
- Reading a weak reference yields a value that may be nil.
- In strict nullability mode, weak reads are treated as nullable unless proven.

### 4.3.3 `__unsafe_unretained`
Non-owning and non-zeroing; use-after-free is possible.
- Its usage is discouraged and diagnosed in strict modes unless in explicitly marked unsafe contexts.

### 4.3.4 `__autoreleasing`
Used primarily for NSError-style out parameters and bridging to autorelease pools.

## 4.4 Precise lifetime and lifetime shortening
Optimizing compilers may shorten object lifetimes relative to source-level scope when it is safe under ARC rules.

Objective‑C 3.0 standardizes a precise lifetime mechanism:
- variables annotated with precise lifetime shall not be released before the end of their lexical scope.

This is foundational for borrowed interior pointers (Part 8).

## 4.5 Transfer and consumption annotations
Objective‑C 3.0 introduces standardized attributes for ownership transfer in APIs.

### 4.5.1 `@consumes` parameter
A `@consumes` parameter indicates that:
- the callee takes over responsibility for releasing/disposing the argument.
- the caller must not use or dispose of it after the call unless it retains/copies explicitly.

### 4.5.2 `@returns_owned` / `@returns_unowned`
For return values:
- `@returns_owned` means the caller receives ownership (must dispose under the appropriate rules).
- `@returns_unowned` means the caller borrows (must not outlive the owner; see Part 8 for borrowed pointers).

### 4.5.3 Diagnostics
In strict mode:
- using a consumed argument after the call is an error (use-after-move).
- failing to dispose an owned return before it becomes unreachable is an error when the type is annotated as requiring cleanup (Part 8).

## 4.6 Move and “one-time” values
Objective‑C 3.0 introduces the concept of move for:
- resource handles (Part 8),
- and optionally for certain object wrappers.

Move semantics are explicit:
- `take(x)` and `move x` capture list items (Part 8) are the standardized spellings.

## 4.7 Autorelease pools and bridging
ObjC 3.0 retains autorelease pool semantics and requires that new language constructs (`defer`, `async`) preserve correctness:
- defers execute before local releases in the same scope (Part 8).
- `async` suspension points shall preserve pool semantics according to the concurrency runtime (Part 7).

Per-task autorelease pool behavior at suspension points is defined normatively in Part 7 and 01C (Decision D‑006).



## 4.7 Async suspension and autorelease pools

### 4.7.1 Normative rule (Objective‑C runtimes)
On Objective‑C runtimes with autorelease semantics, a conforming implementation shall ensure:

- Each *task execution slice* (resume → next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule is required to avoid unbounded autorelease growth across long async chains and to make memory behavior predictable for Foundation-heavy code.

See Part 7 §7.9 and 01C.7.


## 4.8 Interactions with retainable C families
Retainable families (CoreFoundation-like, dispatch/xpc integration, custom refcounted C objects) are specified in Part 8. Part 4 defines their participation in the ownership model:
- retainable family types may be treated as retainable under ARC.
- transfer annotations may apply to them.

## 4.9 Open issues
- Whether to introduce a first-class “unmanaged” wrapper type (like Swift’s `Unmanaged`) for explicit retain/release in non-ARC contexts.
- How to specify and test lifetime shortening behavior precisely across optimizations (requires conformance test suite guidance, Part 12).
