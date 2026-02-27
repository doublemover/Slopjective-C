# Part 4 — Memory Management and Ownership {#part-4}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 4.1 Purpose {#part-4-1}

This part defines how ObjC 3.0:

- builds on ARC as the baseline ownership system,
- standardizes ownership qualifiers and lifetime guarantees,
- introduces transfer/consumption annotations that improve correctness and performance,
- integrates “retainable families” beyond ObjC objects (delegated to [Part 8](#part-8) for system libraries).

## 4.2 ARC as normative baseline {#part-4-2}

In ObjC 3.0 mode with ARC enabled:

- the compiler shall insert retains/releases/autoreleases according to ARC rules.
- ownership qualifiers (`__strong`, `__weak`, `__autoreleasing`, `__unsafe_unretained`) shall be honored.

ObjC 3.0 does not change ARC’s fundamental model: it remains reference counting without cycle collection.

## 4.3 Ownership qualifiers (standardized) {#part-4-3}

### 4.3.1 `__strong` {#part-4-3-1}

Strong references keep objects alive.

### 4.3.2 `__weak` {#part-4-3-2}

Weak references are zeroing and do not keep objects alive.

- Reading a weak reference yields a value that may be nil.
- In strict nullability mode, weak reads are treated as nullable unless proven.

### 4.3.3 `__unsafe_unretained` {#part-4-3-3}

Non-owning and non-zeroing; use-after-free is possible.

- Its usage is discouraged and diagnosed in strict modes unless in explicitly marked unsafe contexts.

### 4.3.4 `__autoreleasing` {#part-4-3-4}

Used primarily for NSError-style out parameters and bridging to autorelease pools.

## 4.4 Precise lifetime and lifetime shortening {#part-4-4}

Optimizing compilers may shorten object lifetimes relative to source-level scope when it is safe under ARC rules.

Objective‑C 3.0 standardizes a precise lifetime mechanism:

- locals annotated with `objc_precise_lifetime` shall not be released before the end of their lexical scope.

This is foundational for borrowed interior pointers ([Part 8](#part-8)).

### 4.4.1 Lifetime shortening by conformance profile (normative) {#part-4-4-1}

For strong retainable locals that are _not_ annotated with `objc_precise_lifetime`:

- **Core**: lifetime shortening is permitted to any point after the last semantic use, including before lexical scope end, only when no scope-exit action (`defer`, `@cleanup`, `@resource`) and no post-`await` continuation can observe the value.
- **Strict**: same permitted shortening as Core.
- **Strict Concurrency**: same permitted shortening as Strict; values that are needed after a potential suspension shall be kept alive in async-frame storage across suspension/resume.
- **Strict System**: shortening is permitted only when the implementation can additionally prove that borrowed-pointer and resource-correctness invariants from [Part 8](#part-8) are preserved.

Forbidden shortening:

- **All profiles**: releasing before a potential semantic use, before required scope-exit actions that may access the value, or before lexical scope end for a local annotated with `objc_precise_lifetime`.
- **Strict / Strict Concurrency / Strict System**: when proof is insufficient, the implementation shall choose the conservative longer lifetime.
- **Strict System**: releasing an owner while any derived `borrowed` value may still be used is ill-formed.

### 4.4.2 `objc_precise_lifetime` interactions (normative) {#part-4-4-2}

For a local annotated with `objc_precise_lifetime`:

- Scope-exit actions in the same lexical scope (`defer`, `@cleanup`, `@resource`) shall execute before the local's implicit ARC release, so the value remains alive while those actions run.
- In async functions, if the lexical scope extends across an `await`, the value shall remain retained across suspension/resume until scope exit.
- Draining implicit autorelease pools at suspension points ([§4.7.1](#part-4-7-2), [C.7](#c-7)) shall not be used to release a `objc_precise_lifetime` local before lexical scope exit.

## 4.5 Transfer and consumption annotations {#part-4-5}

Objective‑C 3.0 introduces standardized attributes for ownership transfer in APIs.

### 4.5.1 `@consumes` parameter {#part-4-5-1}

A `@consumes` parameter indicates that:

- the callee takes over responsibility for releasing/disposing the argument.
- the caller must not use or dispose of it after the call unless it retains/copies explicitly.

### 4.5.2 `@returns_owned` / `@returns_unowned` {#part-4-5-2}

For return values:

- `@returns_owned` means the caller receives ownership (must dispose under the appropriate rules).
- `@returns_unowned` means the caller borrows (must not outlive the owner; see [Part 8](#part-8) for borrowed pointers).

### 4.5.3 Diagnostics {#part-4-5-3}

In strict mode:

- using a consumed argument after the call is an error (use-after-move).
- failing to dispose an owned return before it becomes unreachable is an error when the type is annotated as requiring cleanup ([Part 8](#part-8)).

## 4.6 Move and “one-time” values {#part-4-6}

Objective‑C 3.0 introduces the concept of move for:

- resource handles ([Part 8](#part-8)),
- and optionally for certain object wrappers.

Move semantics are explicit:

- `take(x)` and `move x` capture list items ([Part 8](#part-8)) are the standardized spellings.

## 4.7 Autorelease pools and bridging {#part-4-7}

ObjC 3.0 retains autorelease pool semantics and requires that new language constructs (`defer`, `async`) preserve correctness:

- defers execute before local releases in the same scope ([Part 8](#part-8)).
- `async` suspension points shall preserve pool semantics according to the concurrency runtime ([Part 7](#part-7)).

Per-task autorelease pool behavior at suspension points is defined normatively in [Part 7](#part-7) and [C](#c) ([Decision D-006](#decisions-d-006)).

### 4.7.1 Async suspension and autorelease pools {#part-4-7-2}

#### 4.7.1.1 Normative rule (Objective‑C runtimes) {#part-4-7-1}

On Objective‑C runtimes with autorelease semantics, a conforming implementation shall ensure:

- Each _task execution slice_ (resume → next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule is required to avoid unbounded autorelease growth across long async chains and to make memory behavior predictable for Foundation-heavy code.

See [Part 7](#part-7) [§7.9](#part-7-9) and [C.7](#c-7).

## 4.8 Interactions with retainable C families {#part-4-8}

Retainable families (CoreFoundation-like, dispatch/xpc integration, custom refcounted C objects) are specified in [Part 8](#part-8). [Part 4](#part-4) defines their participation in the ownership model:

- retainable family types may be treated as retainable under ARC.
- transfer annotations may apply to them.

## 4.9 Explicit unmanaged references (`Unmanaged`) {#part-4-9}

Objective‑C 3.0 v1 standardizes an Unmanaged-like wrapper for explicit retain/release control.

### 4.9.1 Module placement and availability {#part-4-9-1}

- The standardized surface shall be provided by `objc3.system`.
- Strict System conformance claims shall provide this surface.
- Core/Strict/Strict Concurrency implementations may provide it as an extension; if provided, semantics shall match this section.

### 4.9.2 Required API surface (normative minimum) {#part-4-9-2}

`objc3.system` shall expose an API equivalent to:

- `Unmanaged<T>.passRetained(T value) -> Unmanaged<T>`
- `Unmanaged<T>.passUnretained(T value) -> Unmanaged<T>`
- `Unmanaged<T>.takeRetainedValue() -> T`
- `Unmanaged<T>.takeUnretainedValue() -> T`
- `Unmanaged<T>.retain() -> Unmanaged<T>`
- `Unmanaged<T>.release()`

Concrete names/spellings may differ, but module metadata and documentation shall publish a stable mapping to this semantic surface.

### 4.9.3 Ownership rules (normative) {#part-4-9-3}

- `passRetained` consumes one owned (+1) reference from the caller into the wrapper.
- `passUnretained` stores a non-owning reference and does not retain.
- `takeRetainedValue` consumes one owned (+1) reference from the wrapper and returns it to normal ARC-managed ownership in the caller.
- `takeUnretainedValue` returns a non-owning reference; callers that need ownership must retain explicitly.
- `retain` performs one explicit retain on the wrapped reference.
- `release` performs one explicit release on the wrapped reference.

### 4.9.4 ARC interaction (normative) {#part-4-9-4}

- In ARC mode, this wrapper is the standardized escape hatch for explicit retain/release operations.
- Direct manual retain/release calls on ObjC-integrated retainable families remain governed by [Part 8](#part-8) [§8.4.2](#part-8-4-2).
- ARC and optimizer transformations shall preserve the explicit side effects of wrapper `retain`/`release` operations.
- Values extracted by `takeRetainedValue`/`takeUnretainedValue` re-enter normal ARC lifetime rules in this part.

## 4.10 Open issues {#part-4-10}

No open issues are tracked in this part for v1.

