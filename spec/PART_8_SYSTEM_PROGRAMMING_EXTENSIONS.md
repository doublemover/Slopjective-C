# Part 8 — System Programming Extensions {#part-8}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

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

The core `defer` scope-exit semantics (registration point, exit kinds, and LIFO execution) are defined in [Part 5](#part-5) [§5.2.2](#part-5-2-2).

This part extends that model: after successful initialization of a variable annotated under [§8.3](#part-8-3), the implementation shall register its cleanup action on the same scope-exit stack.

A _cleanup scope_ is any lexical scope that currently has one or more registered scope-exit actions (from `defer`, `@cleanup`, or `@resource`).

Normative non-local control-flow interaction rules:

- ObjC/C++ exception unwinding that crosses cleanup scopes shall execute each exited scope's registered actions exactly once, in LIFO order, before unwinding continues.
- A `longjmp`/`siglongjmp` transfer that bypasses cleanup scopes does not execute skipped actions; behavior is undefined unless the platform ABI explicitly guarantees cleanup execution for that transfer.
- In `strict` and `strict-system` profiles, if the implementation can prove a `longjmp`/`siglongjmp` may bypass a cleanup scope with pending actions (and no active ABI guarantee applies), the program is ill-formed and a diagnostic is required.

---

## 8.2 `defer` (normative) {#part-8-2}

### 8.2.1 Syntax {#part-8-2-1}

Canonical `defer` syntax is defined in [Part 5](#part-5) [§5.2.1](#part-5-2-1).

### 8.2.2 Semantics {#part-8-2-2}

Canonical `defer` registration semantics are defined in [Part 5](#part-5) [§5.2.2](#part-5-2-2). For system constructs in this part, `defer` participates in the unified scope-exit model in [§8.1](#part-8-1).

### 8.2.3 Ordering with ARC releases {#part-8-2-3}

In a scope:

- `defer` actions shall execute **before** implicit ARC releases of strong locals in that scope.

Rationale: ensures cleanup code can safely reference locals without surprising early releases.

### 8.2.4 Restrictions {#part-8-2-4}

Canonical `defer` body restrictions are defined in [Part 5](#part-5) [§5.2.3](#part-5-2-3). Non-local transfer behavior when cleanup scopes are crossed is defined in [§8.1](#part-8-1).

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

### 8.3.6 Struct-field resource annotations (v2 capability; rejected in v1) {#part-8-3-6}

#### 8.3.6.1 v1 status (normative) {#part-8-3-6-1}

Objective‑C 3.0 v1 does not include `objc_resource(...)` / `@resource(...)` on struct/union fields.
In conforming v1 mode, such field annotations are ill-formed and shall be diagnosed.

#### 8.3.6.2 v2 capability and lowering contract (normative for claimants) {#part-8-3-6-2}

A future revision may enable struct-field resource annotations via capability `objc3.system.aggregate_resources`.
Implementations claiming that capability shall enforce:

- aggregate initialization proceeds in field declaration order,
- each successfully initialized annotated field is registered for cleanup,
- aggregate destruction (normal scope exit) performs cleanup in reverse declaration order for annotated fields that are initialized and valid,
- if initialization of field `k` fails, previously initialized annotated fields `[0..k-1]` are cleaned exactly once in reverse declaration order before propagating failure,
- lowering preserves the same LIFO/once semantics as the scope-exit model in [§8.1](#part-8-1).

For this capability, unions with annotated resource fields remain out of scope unless and until a future revision defines active-member cleanup semantics.

#### 8.3.6.3 Illustrative cleanup ordering example {#part-8-3-6-3}

For:

```c
struct Pair {
  __attribute__((objc_resource(close=CloseA, invalid=0))) Handle a;
  __attribute__((objc_resource(close=CloseB, invalid=0))) Handle b;
};
```

- if `a` initializes and `b` initialization fails, cleanup runs `CloseA(a)` only;
- if both initialize and scope exits normally, cleanup runs `CloseB(b)` then `CloseA(a)`.

---

## 8.4 Retainable C families and ObjC-integrated objects {#part-8-4}

### 8.4.1 Motivation {#part-8-4-1}

Some system libraries declare their objects as Objective‑C object types when compiling as Objective‑C so they can participate in ARC, blocks, and analyzer tooling. This behavior exists today in dispatch and XPC ecosystems.

Objective‑C 3.0 defines a language-level model so this is not “macro folklore.”

### 8.4.2 Retainable family declaration and canonical header forms {#part-8-4-2}

Objective‑C 3.0 finalizes the canonical retainable-family marker as:

- `__attribute__((objc_retainable_family(FamilyName)))` on the family typedef.

Canonical headers shall identify family operations with:

- `__attribute__((objc_family_retain(FamilyName)))` on the retain function declaration,
- `__attribute__((objc_family_release(FamilyName)))` on the release function declaration,
- `__attribute__((objc_family_autorelease(FamilyName)))` on the autorelease function declaration (optional).

For families that are ObjC-integrated under Objective‑C compilation, the typedef shall additionally use `__attribute__((NSObject))` (or a canonical alias with identical semantics).

Illustrative header form:

```c
typedef struct OpaqueFoo *FooRef
  __attribute__((objc_retainable_family(Foo)));

FooRef FooRetain(FooRef x)
  __attribute__((objc_family_retain(Foo)));

void FooRelease(FooRef x)
  __attribute__((objc_family_release(Foo)));
```

Optional sugar (non-normative):

- `@retainable_family(FamilyName, retain: RetainFn, release: ReleaseFn, autorelease: AutoFn?)`.

### 8.4.3 Mapping to existing Clang attributes (normative compatibility) {#part-8-4-3}

A conforming implementation shall accept the following widely deployed Clang attributes as compatibility aliases when they represent equivalent semantics:

- `__attribute__((NSObject))` on typedefs for ObjC-integrated retainable family types,
- ownership-transfer attributes on APIs returning/consuming family values:
  - `os_returns_retained` / `os_returns_not_retained` / `os_consumed`,
  - `cf_returns_retained` / `cf_returns_not_retained` / `cf_consumed`,
  - `ns_returns_retained` / `ns_returns_not_retained` / `ns_consumed`.

No existing single Clang attribute fully encodes the family declaration itself; `objc_retainable_family(FamilyName)` is the canonical Objective‑C 3.0 header/interface spelling.

### 8.4.4 ARC and non-ARC behavior (normative) {#part-8-4-4}

If a family is ObjC-integrated and ARC is enabled:

- values of that family shall participate in ARC ownership operations as retainable references,
- direct calls to functions annotated `objc_family_retain`, `objc_family_release`, or `objc_family_autorelease` are ill-formed and shall be diagnosed as errors.

If ARC is disabled:

- direct calls to family retain/release/autorelease functions are permitted,
- the implementation shall still preserve retainable-family and transfer annotations for static analysis and interface emission.

### 8.4.5 Analyzer and optimizer integration {#part-8-4-5}

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

### 8.7.3 Enforcement model (normative in strict-system) {#part-8-7-3}

Strict-system conformance requires a front-end borrowed-pointer escape checker that is:

- flow-sensitive and intra-procedural,
- executed during ordinary semantic analysis for each translation unit,
- capable of diagnosing all required escaping-use cases in [§8.7.4](#part-8-7-4) without requiring an opt-in whole-program analyzer run.

Additional interprocedural/static-analyzer checks are recommended, but they are additive and shall not replace required front-end enforcement.

### 8.7.4 Escaping-use trigger points and minimum diagnostics (normative) {#part-8-7-4}

In strict-system mode, a borrowed pointer value is treated as **non-escaping** unless an explicit unsafe escape marker is used.

The implementation shall diagnose, at minimum, the following _escaping uses_ as ill-formed (error in strict-system):

- storing the borrowed pointer into global/static/thread-local storage,
- storing it into heap-resident storage, including Objective‑C ivars/properties or memory reachable from dynamic allocation,
- capturing it by an escaping block (including passing that block to an escaping parameter),
- passing it as an argument to a parameter not proven non-escaping and not declared to preserve borrowed lifetime,
- returning it from a function/method unless the declaration uses `borrowed` return type plus `objc_returns_borrowed(owner_index=N)` naming a valid owner parameter,
- converting it to an untracked representation (`void *`, integer) when the resulting value may outlive the owner.

Each required diagnostic shall identify the escaping operation and provide at least one actionable fix direction:

- extend owner lifetime (`withLifetime` / `keepAlive`),
- copy data to owned storage,
- or mark the operation explicitly unsafe.

### 8.7.5 False-positive avoidance requirements (normative) {#part-8-7-5}

In strict-system mode, implementations shall accept the following without borrowed-escape diagnostics:

- uses fully contained in a lexical region dominated by the owner lifetime,
- assignment to automatic local storage proven not to outlive the owner,
- passing to parameters proven non-escaping (`noescape` or equivalent),
- capture by blocks proven non-escaping and invoked synchronously in the same full-expression,
- copying data out of the borrowed region into owned storage when the borrowed pointer itself does not escape.

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

No open issues are tracked in this part for v1.

