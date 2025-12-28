# Part 4 — Memory Management and Ownership
_Working draft v0.6 — last updated 2025-12-28_

## 4.1 Purpose
This part defines how Objective‑C 3.0:
- builds on ARC as the baseline ownership system,
- standardizes ownership qualifiers and lifetime guarantees,
- introduces transfer/consumption annotations that improve correctness and performance,
- integrates “retainable families” beyond ObjC objects (specified in Part 8),
- and specifies how async suspension interacts with autorelease pools (cross-cutting with Part 7).

## 4.2 ARC as normative baseline
In ObjC 3.0 mode with ARC enabled:
- the compiler shall insert retains/releases/autoreleases according to ARC rules,
- ownership qualifiers (`__strong`, `__weak`, `__autoreleasing`, `__unsafe_unretained`) shall be honored.

Objective‑C 3.0 does not change ARC’s fundamental model: it remains reference counting without cycle collection.

## 4.3 Ownership qualifiers (standardized)

### 4.3.1 `__strong`
Strong references keep objects alive.

### 4.3.2 `__weak`
Weak references are zeroing and do not keep objects alive.
- Reading a weak reference yields a value that may be `nil`.
- In strict nullability mode, weak reads are treated as nullable unless proven otherwise by flow analysis.

### 4.3.3 `__unsafe_unretained`
Non-owning and non-zeroing; use-after-free is possible.
- Its usage is discouraged and diagnosed in strict modes unless in explicitly marked unsafe contexts.

### 4.3.4 `__autoreleasing`
Used primarily for NSError-style out parameters and autorelease pool boundaries.

### 4.3.5 Interaction with optional types (`T?`, `T!`)
Optional types are a *type system* feature (Part 3). Ownership qualifiers remain separate.
Examples:

```objc
__weak NSObject * _Nullable maybeObj;   // weak + nullable (baseline style)
NSObject*? strongMaybeObj;              // strong by default + optional sugar (ObjC 3.0)
```

A compiler shall not infer ownership (strong/weak) solely from `T?` / `T!` sugar.

## 4.4 Precise lifetime and lifetime shortening

### 4.4.1 Lifetime shortening (baseline behavior)
Optimizing compilers may shorten object lifetimes relative to source-level scope when it is safe under ARC rules.

This can break code that depends on an owner remaining alive while an interior pointer is used.

### 4.4.2 Precise lifetime annotation
Objective‑C 3.0 standardizes a precise lifetime mechanism:

- `__attribute__((objc_precise_lifetime))`

A local variable annotated with precise lifetime shall not be released before the end of its lexical scope.

### 4.4.3 `withLifetime` / `keepAlive`
To make lifetime extension explicit and reviewable, ObjC 3.0 standardizes:

- `withLifetime(expr) Ellipsis` (Part 8)
- `keepAlive(expr);` (Part 8)

These constructs create an implicit strong precise-lifetime binding to prevent premature release.

## 4.5 Transfer and consumption annotations

### 4.5.1 Consumed parameters (`@consumes`)
A consumed parameter indicates that the callee takes over responsibility for releasing/disposing the argument.

**Normative rule:** In strict mode, using a consumed argument after the call is an error (use-after-move).

**Canonical header spelling:** Implementations should support a canonical attribute spelling such as:

- `__attribute__((objc_consumed))` on parameters

(Exact spelling may vary by toolchain; the semantics are normative.)

### 4.5.2 Owned / borrowed returns
Return values may be annotated as:
- **owned** (caller receives ownership responsibility), or
- **borrowed/unowned** (caller must not outlive the owner; primarily used for borrowed pointer models, Part 8).

Canonical header spellings (illustrative):
- `__attribute__((objc_returns_owned))`
- `__attribute__((objc_returns_borrowed))`

### 4.5.3 Diagnostics for cleanup-requiring types
For types annotated as requiring cleanup (Part 8), strict-system mode shall diagnose:
- missing cleanup,
- double cleanup,
- wrong cleanup family usage.

These diagnostics apply to owned returns and resources, not to ordinary Objective‑C objects under ARC.

## 4.6 Move-like rules for one-time values
Objective‑C 3.0 introduces move-like rules where explicitly annotated:
- `@consumes` parameters (use-after-move diagnostics),
- resource handles (`take(x)`, Part 8),
- capture list `move` items (Part 8).

Move is not a general language feature for all Objective‑C objects in v1; it is scoped to explicit annotations and handle/resource types.

## 4.7 Autorelease pools and async suspension

### 4.7.1 Baseline autorelease pools
Autorelease pools remain part of the baseline language/runtime contract.
The language construct `@autoreleasepool Ellipsis` behaves as in baseline Objective‑C.

### 4.7.2 Async suspension pool boundaries (normative)
On platforms that support Objective‑C autorelease pools, Objective‑C 3.0 adopts the concurrency/memory contract recorded in Decision D‑006 (Part 7 §7.9.4):

- Each async **task execution slice** (from resume to the next suspension/completion) shall execute within an **implicit autorelease pool**.
- If an `await` suspends, the implicit pool shall be drained before control returns to the executor.
- The implicit pool shall be drained at task completion (normal return, throw, or cancellation unwind).

This rule prevents unbounded autorelease accumulation in long-lived async tasks.

### 4.7.3 Interaction with explicit `@autoreleasepool`
Nested explicit pools remain valid inside async functions; draining behavior is the usual nesting behavior:
- explicit pools drain at their lexical end,
- implicit per-slice pools drain at suspend/completion boundaries.

## 4.8 Retainable C families and ObjC-integrated objects
Some system libraries declare “retainable C families” (CoreFoundation-like, dispatch, XPC) that participate in ARC-like management when compiled as Objective‑C.

Part 8 defines the normative model and annotations. Part 4’s requirement is:
- if a type is declared as a retainable family, ARC and ownership transfer annotations shall apply to it consistently,
- and tooling shall expose the semantics to optimizers and analyzers.

## 4.9 Exceptions and unwinding
Scope-exit actions (`defer`, resource cleanup) shall run on normal scope exit and on stack unwinding due to ObjC/C++ exceptions, where the platform ABI runs cleanups.

Non-local unwinding mechanisms that bypass cleanup (e.g., `longjmp`) are outside the guarantees of ObjC 3.0 cleanup scopes (Part 8).

## 4.10 Open issues
- Whether to introduce a first-class “unmanaged” wrapper type (similar in intent to Swift’s `Unmanaged`) for explicit retain/release in non-ARC contexts.
- How to specify and test lifetime shortening behavior precisely across optimizations (requires conformance-test guidance, Part 12).
