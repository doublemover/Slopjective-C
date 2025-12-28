# Part 8 — System Programming Extensions {#part-8}

_Working draft v0.10 — last updated 2025-12-28_

## 8.0 Scope and goals {#part-8-0}

This part defines language features and attributes designed to accommodate “library-defined subsets” and system APIs, with improved safety, ergonomics, and performance predictability:

- Handle-based resources with strict cleanup requirements (IOKit, Mach).
- C libraries whose objects become ARC-managed under Objective‑C compilation (dispatch, XPC).
- CoreFoundation-style retainable APIs and ownership transfer attributes.
- Borrowed interior-pointer APIs that require explicit lifetime extension.
- Block capture patterns that commonly cause retain cycles (event handlers, callbacks).

This part intentionally provides _detailed normative semantics_ because system-library correctness depends on precise ordering.

## 8.0.1 Canonical attribute spellings (header interoperability) {#part-8-0-1}

**Normative note:** The canonical spellings for all attributes and pragmas used in this part are cataloged in **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)**. Inline spellings here are illustrative; emitted interfaces and module metadata shall use the catalog spellings.
Many system-facing features in this part are intended to be expressed in headers and preserved by module interfaces. Per [Decision D-007](#decisions-d-007), a conforming implementation shall support canonical `__attribute__((...))` spellings for these features, so that:

- APIs can be declared without macro folklore,
- analyzers can reason about semantics,
- and mixed ObjC/ObjC++ builds remain source-compatible.

This part uses ergonomic `@resource(...)`/`@cleanup(...)`/`borrowed` spellings in examples; implementations may accept these as ObjC 3.0 sugar. The canonical header spellings are described inline where relevant.

---

## 8.1 Unified scope-exit action model {#part-8-1}

Within a lexical scope, the abstract machine maintains a stack of scope-exit actions. Actions are registered when:

- a `defer` statement executes, or
- a resource variable finishes successful initialization.

On scope exit, actions execute in reverse order of registration (LIFO).

Scope exit includes:

- fallthrough end of scope
- `return`, `break`, `continue`, `goto` leaving the scope
- stack unwinding due to ObjC/C++ exceptions (if applicable)

Non-local unwinding mechanisms that do not run cleanups (e.g., `longjmp`) are outside the guarantees of this part; crossing a cleanup scope using such mechanisms is undefined unless the platform ABI states otherwise.

---

## 8.2 `defer` (normative) {#part-8-2}

### 8.2.1 Syntax {#part-8-2-1}

```text
defer compound-statement
```

### 8.2.2 Semantics {#part-8-2-2}

Executing a `defer` registers its body as a scope-exit action in the innermost enclosing lexical scope.

### 8.2.3 Ordering with ARC releases {#part-8-2-3}

In a scope:

- `defer` actions shall execute **before** implicit ARC releases of strong locals in that scope.

Rationale: ensures cleanup code can safely reference locals without surprising early releases.

### 8.2.4 Restrictions {#part-8-2-4}

A deferred body shall not contain a statement that performs a non-local exit from the enclosing scope. Such code is ill-formed.

---

## 8.3 Resource values and cleanup {#part-8-3}

### 8.3.1 `@cleanup(F)` {#part-8-3-1}

A local variable may be annotated with a cleanup function.

**Canonical header spelling (illustrative):**

- `__attribute__((cleanup(F)))` on the local variable.

A local variable may be annotated with a cleanup function:

```objc
@cleanup(F) Type name = initializer;
```

Semantics:

- after successful initialization, register a scope-exit action that calls `F` with the variable’s value (or address, if applicable) at scope exit.

### 8.3.2 `@resource(F, invalid: X)` {#part-8-3-2}

A `@resource` variable has a cleanup function and an invalid sentinel.

**Canonical header spelling (illustrative):**

- `__attribute__((objc_resource(close=F, invalid=X)))` on the local variable.

A `@resource` variable has a cleanup function and an invalid sentinel:

```objc
@resource(CloseFn, invalid: 0) io_connect_t conn = 0;
```

At scope exit:

- if `conn != 0`, call `CloseFn(conn)`;
- set `conn` to invalid after cleanup (conceptually).

### 8.3.3 Cleanup signature matching {#part-8-3-3}

Cleanup function `F` is applicable if it can be called as:

- `F(value)` or `F(&value)`.

### 8.3.4 Move support for resources {#part-8-3-4}

#### 8.3.4.1 `take(x)` {#part-8-3-4-1}

`take(x)` yields the current value of resource variable `x` and sets `x` to its invalid sentinel.

Using `x` after `take` yields the invalid value; in strict-system mode, using it as if valid without reinitialization is diagnosed.

#### 8.3.4.2 `reset(x, v)` {#part-8-3-4-2}

`reset(x, v)`:

1. cleans up the current value of `x` if valid,
2. assigns `v` to `x`.

#### 8.3.4.3 `discard(x)` {#part-8-3-4-3}

`discard(x)` cleans up and invalidates `x` immediately.

### 8.3.5 Type-directed cleanup diagnostics {#part-8-3-5}

Objective‑C 3.0 introduces type annotations usable on typedefs:

- `@requires_cleanup(F)`
- `@resource_family(Name)`
- `@invalid(X)`

In strict-system mode, the compiler shall diagnose:

- forgetting to cleanup values of types with `@requires_cleanup`,
- calling a cleanup function not associated with the resource family,
- double-cleanup and use-after-take patterns.

> Note: This is explicitly intended to prevent “wrong close vs release” bugs in handle-heavy APIs.

---

## 8.4 Retainable C families and ObjC-integrated objects {#part-8-4}

### 8.4.1 Motivation {#part-8-4-1}

Some system libraries declare their objects as Objective‑C object types when compiling as Objective‑C so they can participate in ARC, blocks, and analyzer tooling. This behavior exists today in dispatch and XPC ecosystems.

Objective‑C 3.0 defines a language-level model so this is not “macro folklore.”

### 8.4.2 `@retainable_family` {#part-8-4-2}

A library may declare a retainable family type by specifying:

- retain function
- release function
- optional autorelease function
- whether the family is ObjC-integrated under ObjC compilation

If ObjC-integrated:

- under ARC, manual retain/release functions for that family are ill-formed (diagnosed as errors).

### 8.4.3 Analyzer integration {#part-8-4-3}

A conforming implementation shall expose retainable family semantics to:

- the ARC optimizer,
- the static analyzer.

---

## 8.5 CoreFoundation ownership attributes (normative) {#part-8-5}

Objective‑C 3.0 standardizes ownership transfer attributes commonly used with ARC and analyzer:

- returns retained / not retained
- consumed parameters

The compiler shall use these to:

- insert correct ownership operations under ARC,
- and diagnose leaks and over-releases in strict modes.

---

## 8.6 Explicit lifetime control {#part-8-6}

### 8.6.1 `withLifetime(expr) stmt` {#part-8-6-1}

Syntax:

```objc
withLifetime(expr) { ... }
```

Semantics:

- evaluate `expr` once,
- bind it to an implicit strong precise-lifetime temporary,
- execute the statement,
- release the temporary after the statement.

### 8.6.2 `keepAlive(expr);` {#part-8-6-2}

Extends the lifetime of `expr` to at least the end of the current lexical scope via an implicit precise-lifetime binding.

### 8.6.3 `objc_precise_lifetime` {#part-8-6-3}

The implementation shall support a precise lifetime annotation for locals and define its effect: prevent lifetime shortening below lexical scope.

---

## 8.7 Borrowed interior pointers {#part-8-7}

### 8.7.1 `borrowed T *` {#part-8-7-1}

A borrowed pointer type indicates “valid only while some owner stays alive.”

Rules in strict-system mode:

- borrowed pointers shall not be stored into globals, heap storage, or escaping blocks without explicit unsafe escape.
- borrowed pointers derived from a non-stable owner require explicit lifetime control (`withLifetime`/`keepAlive`) or automatic owner materialization with a warning.

### 8.7.2 `@returns_borrowed(owner: N)` {#part-8-7-2}

Functions may declare borrowed return relationships, enabling the compiler to enforce owner lifetime.

**Canonical header spelling (illustrative):**

- `__attribute__((objc_returns_borrowed(owner_index=N)))` on the function/return.

### 8.7.3 Escape analysis (normative in strict-system) {#part-8-7-3}

In strict-system mode, a borrowed pointer value shall be treated as **non-escaping** unless an explicit unsafe escape is used.

The following are considered _escaping uses_ and are ill-formed in strict-system mode:

- storing the borrowed pointer into a global/static variable,
- storing it into heap storage (including Objective‑C ivars/properties) without an explicit copy/retain of the owner,
- capturing it by an escaping block or passing it to a parameter not proven non-escaping.

The compiler shall provide diagnostics that suggest:

- extending the owner’s lifetime (`withLifetime` / `keepAlive`), or
- copying data out of the borrowed region into an owned buffer, or
- marking the operation explicitly unsafe.

A conforming implementation may rely on a combination of front-end rules and static analysis to enforce this.

---

## 8.8 Block capture lists {#part-8-8}

### 8.8.1 Syntax {#part-8-8-1}

Blocks may include an explicit capture list:

```objc
^[weak self, move handle] { ... }
```

A capture list is a comma-separated list of _capture items_:

```text
capture-list:
    '[' capture-item (',' capture-item)* ']'

capture-item:
    capture-modifier? identifier
```

Capture modifiers (v1):

- `strong` — capture strongly (default if omitted)
- `weak` — capture as zeroing weak under ARC
- `unowned` — capture as unsafe unretained (explicitly unsafe)
- `move` — move a one-time value into the block capture storage

### 8.8.2 Semantics (normative) {#part-8-8-2}

Capture list items are evaluated at **block creation time**, not at block invocation time.

For a capture item `m x`:

- the current value of `x` in the enclosing scope is read,
- and stored into capture storage according to modifier `m`.

Modifier semantics:

- `strong x` stores a strong reference (ARC retain) to `x`.
- `weak x` stores a zeroing weak reference to `x`.
- `unowned x` stores an unsafe non-zeroing reference to `x` (may dangle).
- `move x` transfers the current value of `x` into the block’s capture storage and treats `x` as moved-from afterward (see [§8.8.4](#part-8-8-4)).

### 8.8.3 Capture evaluation order (normative) {#part-8-8-3}

Capture list entries are evaluated **left-to-right**.
This ordering matters when later captures depend on earlier ones (or when move semantics are involved).

### 8.8.4 Move capture rules {#part-8-8-4}

If `x` is captured with `move`:

- the capture stores the previous value of `x`,
- and `x` is set to a moved-from state.

For ordinary Objective‑C object pointers, “moved-from” is a safe state (typically `nil`) and use-after-move diagnostics are optional.
For resource/handle types annotated as requiring cleanup ([Part 8](#part-8).3/8.3.5), strict-system mode shall diagnose:

- use-after-move as an error, and
- double-cleanup hazards (because a moved-from handle must not be cleaned up twice).

### 8.8.5 Interaction with retain cycles {#part-8-8-5}

Capture lists provide a standard spelling for the “weak‑strong dance” pattern.

Example:

```objc
__weak typeof(self) weakSelf = self;
[obj setHandler:^{
  typeof(self) self = weakSelf;
  if (!self) return;
  [self doThing];
}];
```

can be expressed as:

```objc
[obj setHandler:^[weak self] {
  guard let self = self else { return; }
  [self doThing];
}];
```

### 8.8.6 Diagnostics {#part-8-8-6}

In strict-system mode, toolchains should:

- warn on likely retain cycles (e.g., capturing `self` strongly in long-lived handler registrations),
- warn/error when capturing a borrowed pointer into an escaping block ([Part 8](#part-8).7),
- diagnose use-after-move for resource/handle captures.

---

## 8.9 Conformance and migration {#part-8-9}

- In permissive mode, most violations are warnings with fix-its.
- In strict-system mode, key violations are errors:
  - escaping borrowed pointers,
  - missing required cleanup,
  - wrong cleanup function usage.

---

## 8.10 Open issues {#part-8-10}

- Final spelling for `@retainable_family` and whether it maps to existing Clang attributes.
- Whether borrowed-pointer enforcement is front-end only or requires analyzer for complete coverage.
- Whether resource annotations can be extended to struct fields (RAII-like aggregates).
