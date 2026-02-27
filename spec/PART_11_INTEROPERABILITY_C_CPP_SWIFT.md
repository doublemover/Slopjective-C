# Part 11 — Interoperability: C, C++, and Swift {#part-11}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 11.1 Purpose {#part-11-1}

Objective‑C’s success depends on interoperability:

- with **[C](#c)** (system APIs),
- with **C++** (ObjC++),
- and with **Swift** (modern app frameworks).

This part defines interoperability expectations so ObjC 3.0 features can be adopted without breaking the ecosystem.

This part is intentionally conservative about ABI: where a feature affects ABI or lowering, it is cross-referenced to **[C](#c)** and summarized in **[D](#d)**.

## 11.2 C interoperability {#part-11-2}

### 11.2.1 Baseline ABI compatibility {#part-11-2-1}

Objective‑C 3.0 preserves baseline C ABI calling conventions for declarations that do not use new effects.

For declarations that use ObjC 3.0 effects (`throws`, `async`), a conforming toolchain shall provide a stable, documented calling convention ([C.4](#c-4), [C.5](#c-5)).

### 11.2.2 Exporting `throws` to C (normative intent) {#part-11-2-2}

When a throwing function is made visible to C, it shall be representable using a C-callable surface.

Recommended representation:

- a trailing error-out parameter (`id<Error> * _Nullable outError`) as described in [C.4](#c-4).

This aligns with Cocoa NSError patterns and enables C callers to handle errors without language support for `try`.

### 11.2.3 Exporting `async` to C (normative for implementations) {#part-11-2-3}

When an `async` function is made visible to C, a conforming implementation shall expose a completion-handler thunk using the canonical ABI shape in this section.

For a source declaration:

```objc
R f(A1 a1, A2 a2) async throws;
```

the exported C-callable thunk shall be equivalent to:

```c
typedef enum objc_async_completion_status {
    OBJC_ASYNC_COMPLETION_SUCCESS = 0,
    OBJC_ASYNC_COMPLETION_ERROR = 1,
    OBJC_ASYNC_COMPLETION_CANCELLED = 2,
} objc_async_completion_status_t;

typedef void (*f_completion_t)(
    void * _Nullable context,
    objc_async_completion_status_t status,
    R result,
    id<Error> _Nullable error);

void f_async_c(
    A1 a1,
    A2 a2,
    void * _Nullable context,
    f_completion_t _Nonnull completion);
```

For `R == void`, the completion callback shall omit the `result` parameter.

The canonical parameter order is:

1. original source parameters (`A1`, `A2`, ...),
2. trailing `void * _Nullable context`,
3. trailing nonnull completion callback.

Completion delivery rules:

- The completion callback shall be invoked exactly once for each thunk invocation.
- `completion == NULL` is a contract violation; behavior is undefined unless a platform profile defines a checked trap.
- Completion may occur after `f_async_c` returns; callers shall not assume synchronous callback delivery.

Error and cancellation representation at the C boundary:

- On success: `status == OBJC_ASYNC_COMPLETION_SUCCESS`, `error == nil`, and `result` contains the function result.
- On thrown failure: `status == OBJC_ASYNC_COMPLETION_ERROR`, `error != nil`, and `result` is unspecified and shall be ignored by C callers.
- On cancellation: `status == OBJC_ASYNC_COMPLETION_CANCELLED`, `error != nil`, and `error` shall be the distinguished cancellation representation required by [Part 7](#part-7) [§7.6.4](#part-7-6-4). `result` is unspecified and shall be ignored by C callers.
- For declarations that are `async` but not `throws`, `OBJC_ASYNC_COMPLETION_ERROR` shall not be produced.

Ownership and lifetime rules:

- `context` is caller-owned opaque state. The implementation shall pass it through unchanged to `completion` and shall not free or retain it.
- The caller shall keep `context` valid until `completion` returns.
- After `completion` returns, the implementation shall not read from or write to `context`.
- Objective‑C object/block values delivered as `result` (when applicable) or `error` are passed at +0 ownership. If C/ObjC caller code needs them beyond callback return, it shall retain/copy them before returning from `completion`.
- The implementation shall keep delivered values alive for the dynamic extent of `completion`.

The thunk ABI shall be stable under separate compilation and recordable in module metadata ([C.2](#c-2), [C.5](#c-5)).

### 11.2.4 Required conformance tests for C callers of exported `async` APIs {#part-11-2-4}

A conforming implementation shall ship tests where a C translation unit invokes exported async thunks and validates all of:

- Separate-compilation ABI stability: export the thunk in one module/translation unit, call it from a separate C translation unit, and link successfully.
- Canonical signature shape: generated declarations include the trailing `context` parameter and trailing completion callback in the required order.
- `void`-result shape: for exported `async` APIs with `R == void`, the callback signature omits `result` and remains callable from C.
- Exactly-once completion: each invocation yields one and only one callback on success, thrown error, and cancellation paths.
- Context round-trip identity: callback observes the same `context` pointer bit-pattern passed by the caller.
- Status/error invariants: success sets `error == nil`; failure and cancellation set `error != nil`; callers ignore `result` unless status is success.
- Cancellation mapping: cancellation is reported with `OBJC_ASYNC_COMPLETION_CANCELLED` and a distinguished cancellation error object as required by [Part 7](#part-7).
- Lifetime transfer contract: values retained/copied inside completion remain valid after completion returns, while unretained +0 values are not required to outlive callback scope.

## 11.3 C++ / ObjC++ interoperability {#part-11-3}

### 11.3.1 Parsing and canonical spellings {#part-11-3-1}

Canonical spellings in [B](#b) are chosen to be compatible with ObjC++ translation units:

- `__attribute__((...))` is accepted in C++ mode,
- `#pragma ...` is available in C++ mode.

Implementations may additionally support C++11 attribute spellings (`[[...]]`) in ObjC++ as sugar, but interface emission shall use canonical spellings ([B.7](#b-7)).

### 11.3.2 Exceptions {#part-11-3-2}

Objective‑C exceptions (`@throw/@try/@catch`) remain distinct from ObjC 3.0 `throws`.
C++ exceptions remain distinct as well.

Interoperability guidance:

- Do not translate ObjC 3.0 `throws` into C++ exceptions implicitly.
- If an implementation provides bridging, it must be explicit and toolable.

## 11.4 Swift interoperability {#part-11-4}

### 11.4.1 Nullability and optionals {#part-11-4-1}

ObjC 3.0 nullability annotations and `T?` sugar import naturally as Swift optionals where applicable, consistent with existing Swift/ObjC interop rules.

### 11.4.2 Errors {#part-11-4-2}

- ObjC 3.0 `throws` imports as Swift `throws` when the declaration is visible through a module interface that preserves the effect.
- NSError-out parameter APIs annotated with `objc_nserror` ([Part 6](#part-6)) should import as Swift `throws` where possible.

### 11.4.3 Concurrency {#part-11-4-3}

- ObjC 3.0 `async` imports as Swift `async` when preserved through module metadata/interfaces.
- Completion-handler APIs may be imported/exported across languages using implementation-defined annotations; this draft encourages toolchains to support automated thunk generation.

### 11.4.4 Executors and isolation {#part-11-4-4}

Executor affinity (`objc_executor(main)`) is intended to map cleanly to Swift main-thread isolation concepts.
The exact imported attribute spelling is implementation-defined, but semantics must be preserved across the boundary.

Actor isolation metadata should be preserved such that Swift clients do not accidentally violate ObjC actor invariants (and vice versa).

### 11.4.5 Performance/dynamism controls {#part-11-4-5}

Direct/final/sealed intent should map, where possible, to Swift’s:

- `final`,
- non-overridable members,
- and restricted subclassing patterns.

The mapping is implementation-defined but should preserve the core constraints.

## 11.5 Required diagnostics (minimum) {#part-11-5}

- Importing an API where effects (`async`/`throws`) are lost due to missing interface metadata: warn and suggest enabling module/interface emission.
- Crossing a language boundary with non-Sendable-like values in strict concurrency mode: warn/error depending on policy ([Part 7](#part-7), [Part 12](#part-12)).

## 11.6 Open issues {#part-11-6}

None currently for the v1 portability baseline in this part.


