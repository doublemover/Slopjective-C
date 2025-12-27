# Part 8 — System Programming Extensions

## 8.0 Scope and goals
This part defines language features and attributes designed to accommodate “library-defined subsets” and system APIs, with improved safety, ergonomics, and performance predictability:

- Handle-based resources with strict cleanup requirements (IOKit, Mach).
- C libraries whose objects become ARC-managed under Objective‑C compilation (dispatch, XPC).
- CoreFoundation-style retainable APIs and ownership transfer attributes.
- Borrowed interior-pointer APIs that require explicit lifetime extension.
- Block capture patterns that commonly cause retain cycles (event handlers, callbacks).

This part intentionally provides *detailed normative semantics* because system-library correctness depends on precise ordering.

---

## 8.1 Unified scope-exit action model
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

## 8.2 `defer` (normative)
### 8.2.1 Syntax
```text
defer compound-statement
```

### 8.2.2 Semantics
Executing a `defer` registers its body as a scope-exit action in the innermost enclosing lexical scope.

### 8.2.3 Ordering with ARC releases
In a scope:
- `defer` actions shall execute **before** implicit ARC releases of strong locals in that scope.

Rationale: ensures cleanup code can safely reference locals without surprising early releases.

### 8.2.4 Restrictions
A deferred body shall not contain a statement that performs a non-local exit from the enclosing scope. Such code is ill-formed.

---

## 8.3 Resource values and cleanup

### 8.3.1 `@cleanup(F)`
A local variable may be annotated with a cleanup function:

```objc
@cleanup(F) Type name = initializer;
```

Semantics:
- after successful initialization, register a scope-exit action that calls `F` with the variable’s value (or address, if applicable) at scope exit.

### 8.3.2 `@resource(F, invalid: X)`
A `@resource` variable has a cleanup function and an invalid sentinel:

```objc
@resource(CloseFn, invalid: 0) io_connect_t conn = 0;
```

At scope exit:
- if `conn != 0`, call `CloseFn(conn)`;
- set `conn` to invalid after cleanup (conceptually).

### 8.3.3 Cleanup signature matching
Cleanup function `F` is applicable if it can be called as:
- `F(value)` or `F(&value)`.

### 8.3.4 Move support for resources
#### 8.3.4.1 `take(x)`
`take(x)` yields the current value of resource variable `x` and sets `x` to its invalid sentinel.

Using `x` after `take` yields the invalid value; in strict-system mode, using it as if valid without reinitialization is diagnosed.

#### 8.3.4.2 `reset(x, v)`
`reset(x, v)`:
1. cleans up the current value of `x` if valid,
2. assigns `v` to `x`.

#### 8.3.4.3 `discard(x)`
`discard(x)` cleans up and invalidates `x` immediately.

### 8.3.5 Type-directed cleanup diagnostics
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

## 8.4 Retainable C families and ObjC-integrated objects

### 8.4.1 Motivation
Some system libraries declare their objects as Objective‑C object types when compiling as Objective‑C so they can participate in ARC, blocks, and analyzer tooling. This behavior exists today in dispatch and XPC ecosystems.

Objective‑C 3.0 defines a language-level model so this is not “macro folklore.”

### 8.4.2 `@retainable_family`
A library may declare a retainable family type by specifying:
- retain function
- release function
- optional autorelease function
- whether the family is ObjC-integrated under ObjC compilation

If ObjC-integrated:
- under ARC, manual retain/release functions for that family are ill-formed (diagnosed as errors).

### 8.4.3 Analyzer integration
A conforming implementation shall expose retainable family semantics to:
- the ARC optimizer,
- the static analyzer.

---

## 8.5 CoreFoundation ownership attributes (normative)
Objective‑C 3.0 standardizes ownership transfer attributes commonly used with ARC and analyzer:
- returns retained / not retained
- consumed parameters

The compiler shall use these to:
- insert correct ownership operations under ARC,
- and diagnose leaks and over-releases in strict modes.

---

## 8.6 Explicit lifetime control

### 8.6.1 `withLifetime(expr) stmt`
Syntax:
```objc
withLifetime(expr) { ... }
```

Semantics:
- evaluate `expr` once,
- bind it to an implicit strong precise-lifetime temporary,
- execute the statement,
- release the temporary after the statement.

### 8.6.2 `keepAlive(expr);`
Extends the lifetime of `expr` to at least the end of the current lexical scope via an implicit precise-lifetime binding.

### 8.6.3 `objc_precise_lifetime`
The implementation shall support a precise lifetime annotation for locals and define its effect: prevent lifetime shortening below lexical scope.

---

## 8.7 Borrowed interior pointers

### 8.7.1 `borrowed T *`
A borrowed pointer type indicates “valid only while some owner stays alive.”

Rules in strict-system mode:
- borrowed pointers shall not be stored into globals, heap storage, or escaping blocks without explicit unsafe escape.
- borrowed pointers derived from a non-stable owner require explicit lifetime control (`withLifetime`/`keepAlive`) or automatic owner materialization with a warning.

### 8.7.2 `@returns_borrowed(owner: N)`
Functions may declare borrowed return relationships, enabling the compiler to enforce owner lifetime.

---

## 8.8 Block capture lists

### 8.8.1 Syntax
Blocks may include a capture list:

```objc
^[weak self, move handle] { ... }
```

Capture modifiers:
- `weak`, `unowned`, `strong`, `move`.

### 8.8.2 Semantics
- `weak` captures as zeroing weak under ARC.
- `unowned` captures as unsafe unretained (explicitly unsafe).
- `move` transfers ownership into the block capture storage; the original is treated as moved-from.

### 8.8.3 Diagnostics
In strict-system mode, the compiler/analyzer should diagnose likely retain cycles in handler-style APIs and recommend `weak` captures.

---

## 8.9 Conformance and migration
- In permissive mode, most violations are warnings with fix-its.
- In strict-system mode, key violations are errors:
  - escaping borrowed pointers,
  - missing required cleanup,
  - wrong cleanup function usage.

---

## 8.10 Open issues
- Final spelling for `@retainable_family` and whether it maps to existing Clang attributes.
- Whether borrowed-pointer enforcement is front-end only or requires analyzer for complete coverage.
- Whether resource annotations can be extended to struct fields (RAII-like aggregates).
