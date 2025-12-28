# Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors {#part-10}
_Working draft v0.10 — last updated 2025-12-28_

## 10.1 Purpose {#part-10-1}
Objective‑C has historically relied on:
- the C preprocessor,
- code generation scripts,
- and conventions.

Objective‑C 3.0 introduces structured mechanisms that reduce boilerplate while remaining **auditable** and **toolable**:

- **Derives** for common generated conformances and boilerplate members,
- **AST macros** with strong safety constraints,
- **Property behaviors/wrappers** for reusable property semantics.

This part is constrained by:
- interface emission requirements ([B.7](#b-7)),
- ABI/layout constraints ([C.9](#c-9)),
- and the principle that generated code must remain inspectable.

## 10.2 Derives {#part-10-2}

### 10.2.1 Canonical spelling {#part-10-2-1}
A derive request is attached using:

```c
__attribute__((objc_derive("TraitName")))
```

Multiple derives may be listed by repeating the attribute.

### 10.2.2 Semantics (normative) {#part-10-2-2}
A derive request causes the compiler (or derive implementation) to synthesize declarations as if the user had written them.

Rules:
1. Derives are **deterministic**: the same source must produce the same expanded declarations.
2. Derives are **pure** with respect to the compilation: they shall not depend on network, wall-clock time, or other nondeterministic inputs.
3. If a derive cannot be satisfied (missing requirements, ambiguous synthesis), compilation is ill-formed and shall produce diagnostics identifying the missing inputs.

### 10.2.3 ABI and interface emission (normative) {#part-10-2-3}
If a derive synthesizes declarations that affect:
- type layout,
- exported method sets,
- or protocol conformances,

then those synthesized declarations are part of the module’s API surface and:
- shall be recorded in module metadata, and
- shall appear in any emitted interface ([B.7](#b-7), [C.9](#c-9)).

### 10.2.4 Standard derives (non-normative) {#part-10-2-4}
A standard library may provide derives analogous to:
- equality/hash,
- debug formatting,
- coding/serialization.

This draft does not require a particular set in v1; it requires the *mechanism* and the interface emission guarantees.

## 10.3 AST macros {#part-10-3}

### 10.3.1 Concept {#part-10-3-1}
AST macros are compile-time programs that transform syntax/AST into expanded declarations or expressions.

### 10.3.2 Canonical marker attribute {#part-10-3-2}
Macro entry points may be annotated with:

```c
__attribute__((objc_macro))
```

The macro definition and packaging format are implementation-defined.

### 10.3.3 Safety constraints (normative) {#part-10-3-3}
A conforming implementation shall provide enforcement such that macros used in trusted builds are:
- deterministic (no nondeterministic inputs),
- sandboxable (no arbitrary filesystem/network unless explicitly permitted by the build system),
- and bounded (resource limits to keep builds predictable).

### 10.3.4 Expansion phases (normative) {#part-10-3-4}
Macro expansion (and derive expansion) that introduces declarations affecting layout/ABI shall occur before:
- type layout is finalized, and
- code generation.

This ensures ABI is computed from the *expanded* program ([C.9](#c-9)).

### 10.3.5 Interface emission (normative) {#part-10-3-5}
If macro expansion introduces exported declarations, the emitted interface shall include the expanded declarations (or an equivalent representation that reconstructs them), not merely the macro invocation.

## 10.4 Property behaviors / wrappers {#part-10-4}

### 10.4.1 Concept {#part-10-4-1}
A property behavior is a reusable specification of:
- storage strategy,
- accessors (get/set),
- synchronization or actor isolation constraints,
- and optional hooks (KVO/KVC integration, validation, etc.).

### 10.4.2 Exported ABI restrictions (normative) {#part-10-4-2}
Behaviors that affect layout or emitted ivars of exported types shall be constrained such that:
- layout is deterministic from the expanded declarations, and
- cross-module clients can rely on the same layout.

In v1, implementations may restrict behaviors on exported ABI surfaces to a “safe subset” (e.g., behaviors that desugar into explicit ivars/accessors).

### 10.4.3 Composition (normative) {#part-10-4-3}
If multiple behaviors are applied to a property, the composition order and conflict rules must be deterministic and diagnosable.
Ambiguous compositions are ill-formed.

### 10.4.4 Performance contracts {#part-10-4-4}
Behaviors must declare whether:
- accessors are direct-callable (eligible for inlining),
- they require dynamic dispatch hooks,
- they require executor/actor hops.

Compilers should surface these contracts in diagnostics to help developers choose appropriate behaviors.

## 10.5 Required diagnostics (minimum) {#part-10-5}
- Derive request cannot be satisfied: error with notes listing missing members/protocol requirements.
- Macro expansion introduces exported declarations but cannot be represented in emitted interface: error (or emit-only warning in permissive mode).
- Property behavior composition conflict: error with explanation of conflict.

## 10.6 Open issues {#part-10-6}
- Macro/derive packaging format (SwiftPM-like vs compiler-integrated vs build-system supplied).
- Which derives are standardized in core vs shipped in a standard library module.
- Whether to standardize a canonical surface syntax for behaviors beyond attributes in v1.
