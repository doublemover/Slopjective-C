# Objective‑C 3.0 — Attribute and Syntax Catalog {#b}

_Working draft v0.11 — last updated 2026-02-23_

## B.0 Purpose {#b-0}

Objective‑C 3.0 introduces new language **effects**, **annotations**, and a small amount of **surface syntax**.
To keep separate compilation reliable and to keep module interfaces stable, this document defines the **canonical spellings** that:

- all conforming implementations shall accept; and
- any interface-generation tool shall emit.

Implementations may accept additional “sugar” spellings (keywords, pragmas, macros), but those spellings are **not** required for conformance unless explicitly stated elsewhere.

## B.1 Canonical vs. optional spellings {#b-1}

### B.1.1 Canonical spellings {#b-1-1}

The canonical spellings use forms already widely supported by LLVM/Clang-family toolchains:

- C/ObjC pragmas: `#pragma ...`
- GNU/Clang attributes: `__attribute__((...))`
- Existing Objective‑C keywords where the feature is inherently grammatical (`async`, `await`, `try`, `throw`, etc.)

### B.1.2 Optional sugar spellings {#b-1-2}

Implementations may additionally provide sugar spellings such as:

- `@`-directives (e.g., `@assume_nonnull_begin`) as aliases for pragmas.
- Framework macros (e.g., `NS_ASSUME_NONNULL_BEGIN`) as aliases for pragmas.
- Alternative attribute syntaxes (e.g., C++11 `[[...]]`) when compiling as ObjC++.

Such sugar spellings shall not change semantics.

## B.2 Nullability defaults {#b-2}

### B.2.1 Nonnull-by-default region pragmas (canonical) {#b-2-1}

Objective‑C 3.0 defines a nonnull-by-default region using:

```c
#pragma objc assume_nonnull begin
// declarations
#pragma objc assume_nonnull end
```

Semantics are defined in [Part 3](#part-3) ([§3.2.4](#part-3-2-4)).

### B.2.2 Optional aliases (non-normative) {#b-2-2}

Implementations may treat the following as aliases with identical semantics:

- `#pragma clang assume_nonnull begin/end`
- `NS_ASSUME_NONNULL_BEGIN/NS_ASSUME_NONNULL_END` (macro-based)

## B.3 Concurrency and executors {#b-3}

### B.3.1 Executor affinity annotation (canonical) {#b-3-1}

Executor affinity is expressed with:

```c
__attribute__((objc_executor(main)))
__attribute__((objc_executor(global)))
__attribute__((objc_executor(named("com.example.myexecutor"))))
```

Semantics are defined in [Part 7](#part-7) ([§7.4](#part-7-4)).

### B.3.2 Task-spawn recognition attributes (canonical) {#b-3-2}

The compiler recognizes standard-library task entry points via attributes:

```c
__attribute__((objc_task_spawn))
__attribute__((objc_task_detached))
__attribute__((objc_task_group))
```

Semantics are defined in [Part 7](#part-7) ([§7.5](#part-7-5)).

> Note: These attributes are intended to attach to **functions/methods** that take an `async` block/callback and create tasks/groups, not to arbitrary user functions.

### B.3.3 Unsafe Sendable escape hatch (canonical) {#b-3-3}

For strict concurrency checking, implementations shall recognize an explicit escape hatch that marks a type as “Sendable-like” by programmer promise, even if the compiler cannot prove it.

Canonical attribute:

```c
__attribute__((objc_unsafe_sendable))
```

This attribute may be applied to:

- Objective‑C interface declarations (`@interface` / `@implementation`) to promise instances are safe to transfer across concurrency domains, and/or
- specific typedefs or wrapper types used for cross-task messaging.

Semantics are defined in [Part 7](#part-7) ([§7.8](#part-7-8)).

> Toolchains may also provide sugar spellings (e.g., `@unsafeSendable`), but emitted interfaces must use the canonical attribute.

### B.3.4 Actor isolation modifier: nonisolated (canonical) {#b-3-4}

For actor types, implementations shall support a way to mark specific members as **nonisolated**.

Canonical attribute:

```c
__attribute__((objc_nonisolated))
```

This attribute may be applied to:

- Objective‑C methods and properties declared within an `actor class`.

Semantics are defined in [Part 7](#part-7) ([§7.7.4](#part-7-7-4)).

> Toolchains may additionally support a contextual keyword spelling (`nonisolated`) as sugar, but emitted interfaces should preserve semantics and may prefer the canonical attribute spelling.

## B.4 Errors {#b-4}

### B.4.1 NSError bridging attribute (canonical) {#b-4-1}

For interop with NSError-out-parameter conventions, implementations shall recognize:

```c
__attribute__((objc_nserror))
```

Semantics are defined in [Part 6](#part-6) ([§6.9](#part-6-9)).

### B.4.2 Status-code bridging attribute (canonical) {#b-4-2}

For return-code APIs, implementations shall recognize:

```c
__attribute__((objc_status_code(/* parameters */)))
```

Semantics are defined in [Part 6](#part-6) ([§6.10](#part-6-10)).

## B.5 Performance and dynamism controls {#b-5}

### B.5.1 Direct methods and class defaults (canonical) {#b-5-1}

Direct dispatch controls use:

```c
__attribute__((objc_direct))          // methods
__attribute__((objc_direct_members))  // classes
```

`objc_direct` applies to methods. `objc_direct_members` applies to classes and makes members direct by default as defined in [Part 9](#part-9).

### B.5.2 Final and sealed (canonical) {#b-5-2}

Objective‑C 3.0 uses the following canonical spellings:

```c
__attribute__((objc_final))                  // methods or classes
__attribute__((objc_sealed))                 // classes (module-sealed)
```

If a toolchain already provides an equivalent attribute (e.g., `objc_subclassing_restricted`), it may treat that attribute as an alias.

Semantics are defined in [Part 9](#part-9).

### B.5.3 Force-dynamic dispatch (canonical) {#b-5-3}

Objective‑C 3.0 v1 defines an explicit dynamic-dispatch marker:

```c
__attribute__((objc_dynamic))  // methods
```

Semantics are defined in [Part 9](#part-9), including interaction with `objc_direct`, `objc_final`, and `objc_sealed`.

## B.6 Metaprogramming {#b-6}

### B.6.1 Derive / synthesize (canonical) {#b-6-1}

Derivation requests use:

```c
__attribute__((objc_derive("TraitName")))
```

Semantics are defined in [Part 10](#part-10).

### B.6.2 Macro expansion (canonical) {#b-6-2}

If AST macros are supported, macro entry points may be annotated with:

```c
__attribute__((objc_macro))
```

The actual macro declaration syntax is implementation-defined ([Part 10](#part-10)), but interface emission shall preserve the canonical attributes and any synthesized declarations.

## B.7 Module interface emission requirements (normative) {#b-7}

If an implementation provides any facility that emits a textual interface for a module (e.g., generated headers, module interface stubs, API dumps), then:

1. The emitted interface shall be **semantics-preserving**: importing it must reconstruct the same declarations, effects, and attributes.
2. The emitter shall use the **canonical spellings** from this catalog (or semantically equivalent spellings defined as canonical elsewhere in the spec).
3. The emitted interface shall not depend on user macros for semantics (macros may remain for documentation convenience, but the semantic attributes/pragmas must be explicit).

## B.8 System programming extensions {#b-8}

This section catalogs canonical spellings for “system programming extensions” described in **[Part 8](#part-8)**.
These spellings are intended to survive header/module interface emission and mixed ObjC/ObjC++ builds.

### B.8.1 Resource cleanup on scope exit (canonical) {#b-8-1}

**Variable attribute form:**

```c
__attribute__((objc_resource(close=CloseFn, invalid=InvalidExpr)))
```

- `CloseFn` is the identifier of a cleanup function.
- `InvalidExpr` is a constant expression representing the “invalid” sentinel value for the resource type.

**Semantics:** See [Part 8](#part-8) [§8.3.2](#part-8-3-2). The compiler shall treat a variable annotated with `objc_resource` as a
_single-owner handle_ for purposes of move/use-after-move diagnostics (where enabled).

**Optional sugar (non-normative):**

- `@resource(CloseFn, invalid: InvalidExpr)` as described in [Part 8](#part-8).

### B.8.2 Plain cleanup hook (canonical / existing) {#b-8-2}

**Variable attribute form (Clang existing):**

```c
__attribute__((cleanup(CleanupFn)))
```

**Optional sugar (non-normative):**

- `@cleanup(CleanupFn)` as described in [Part 8](#part-8).

### B.8.3 Borrowed interior pointers (canonical) {#b-8-3}

**Type qualifier form (canonical):**

```c
borrowed T *
```

The `borrowed` qualifier is a contextual keyword in type grammar ([Part 8](#part-8) [§8.7](#part-8-7)). It is not reserved as a general identifier.

### B.8.4 Function return is borrowed from an owner (canonical) {#b-8-4}

**Function/return attribute form:**

```c
__attribute__((objc_returns_borrowed(owner_index=N)))
```

- `N` is a 0-based index into the formal parameter list that identifies the “owner” parameter.

**Semantics:** See [Part 8](#part-8) [§8.7.2](#part-8-7-2). Toolchains shall preserve this attribute in module metadata ([D Table A](#d-3-1)).

### B.8.5 Block capture list contextual keywords (canonical) {#b-8-5}

The capture list feature in [Part 8](#part-8) uses contextual keywords:

- `weak`
- `unowned`
- `move`

These tokens are contextual within capture list grammar only and shall not be reserved globally.

### B.8.6 Retainable C family declarations (canonical) {#b-8-6}

**Type attribute form (canonical):**

```c
__attribute__((objc_retainable_family(FamilyName)))
```

- Applies to typedef declarations that define a retainable family identity.

**Family operation attributes (canonical):**

```c
__attribute__((objc_family_retain(FamilyName)))
__attribute__((objc_family_release(FamilyName)))
__attribute__((objc_family_autorelease(FamilyName))) // optional
```

- Apply to function declarations that define retain/release/autorelease operations for `FamilyName`.

**ObjC integration marker (canonical existing):**

```c
__attribute__((NSObject))
```

- When present on the family typedef under Objective‑C compilation, the family is treated as ObjC-integrated for ARC semantics per [Part 8](#part-8) [§8.4](#part-8-4).

**Compatibility aliases (accepted when semantically equivalent):**

- `os_returns_retained` / `os_returns_not_retained` / `os_consumed`
- `cf_returns_retained` / `cf_returns_not_retained` / `cf_consumed`
- `ns_returns_retained` / `ns_returns_not_retained` / `ns_consumed`

## B.9 Reserved tokens {#b-9}

Objective‑C 3.0 reserves (at minimum) the following tokens as keywords:

- `async`, `await`, `actor`
- `throws`, `try`, `throw`, `do`, `catch`
- `defer`, `guard`, `match`, `case`
- `let`, `var`

Additional reserved keywords may be added by other parts.

