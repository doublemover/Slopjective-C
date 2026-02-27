# Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors {#part-10}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

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

### 10.2.4 Derive tiers and conformance requirements (normative) {#part-10-2-4}

Objective-C 3.0 v1 defines derive support as tiered:

- **Core profile**: no individual derive is required for Core conformance.
- **Optional metaprogramming profile (`OPT-META`)**: derive support is required and shall expose a minimum portable set.

The minimum portable derive set for `OPT-META` claims is:

- `Equality`
- `Hash`
- `DebugDescription`
- `Codable`

Implementations may provide additional derives, but they shall not redefine the semantics of the portable set above.

### 10.2.5 Optional derive feature macros and metadata (normative) {#part-10-2-5}

When `OPT-META` is supported, implementations shall expose feature macros for portable derives:

- `__OBJC3_FEATURE_DERIVE_EQUALITY__`
- `__OBJC3_FEATURE_DERIVE_HASH__`
- `__OBJC3_FEATURE_DERIVE_DEBUGDESCRIPTION__`
- `__OBJC3_FEATURE_DERIVE_CODABLE__`

If a toolchain claims support for a derive in source mode, the same derive capability shall be representable in module metadata and textual interfaces so downstream builds can validate availability under separate compilation ([Part 2](#part-2), [D](#d)).

## 10.3 AST macros {#part-10-3}

### 10.3.1 Concept {#part-10-3-1}

AST macros are compile-time programs that transform syntax/AST into expanded declarations or expressions.

### 10.3.2 Canonical marker attribute {#part-10-3-2}

Macro entry points may be annotated with:

```c
__attribute__((objc_macro))
```

Macro entry points and derive implementations shall be distributed in a portable package model defined by [§10.3.6](#part-10-3-6).

### 10.3.3 Safety constraints (normative) {#part-10-3-3}

A conforming implementation shall provide enforcement such that macros used in trusted builds are:

- deterministic (no nondeterministic inputs),
- sandboxable (no arbitrary filesystem/network unless explicitly permitted by the build system),
- and bounded (resource limits to keep builds predictable).

### 10.3.4 Expansion phases (normative) {#part-10-3-4}

Macro expansion (and derive expansion) that introduces declarations affecting layout/ABI shall occur before:

- type layout is finalized, and
- code generation.

This ensures ABI is computed from the _expanded_ program ([C.9](#c-9)).

### 10.3.5 Interface emission (normative) {#part-10-3-5}

If macro expansion introduces exported declarations, the emitted interface shall include the expanded declarations (or an equivalent representation that reconstructs them), not merely the macro invocation.

### 10.3.6 Macro/derive packaging and discovery model (normative) {#part-10-3-6}

Objective-C 3.0 v1 standardizes a build-system supplied packaging model for macros/derives:

- A macro/derive package shall include a machine-readable manifest file named `objc3-meta.json`.
- The manifest shall include:
  - package identifier,
  - package semantic version,
  - deterministic content fingerprint,
  - declared macro/derive entry points,
  - declared required capabilities/toolchain revision constraints.
- Toolchains shall accept explicit package registration inputs equivalent to:
  - `-fobjc3-meta-package=<path>` (repeatable), and
  - a lockfile input equivalent to `-fobjc3-meta-lock=<path>`.

Discovery/load requirements:

- Package resolution and load order shall be deterministic.
- Builds shall not rely on ambient global package search paths for conformance claims; the package set must be explicit in build inputs.
- If a requested macro/derive cannot be resolved from declared packages, the compile is ill-formed and a diagnostic is required.

Reproducibility requirements:

- With identical source, package manifests, and lockfile, expansion results shall be byte-for-byte reproducible in emitted interface/module metadata.
- Implementations may support additional discovery mechanisms, but conforming behavior shall remain representable by the standardized explicit-package model above.

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

### 10.4.5 Property-behavior surface syntax decision (normative) {#part-10-4-5}

Objective-C 3.0 v1 standardizes **attribute-based behavior declaration/application only**.
No additional behavior-block surface syntax is part of v1.

Future-compat reservation:

- `behavior` is reserved as a contextual keyword in property-behavior positions for possible v2 syntax.
- Forms equivalent to `behavior { ... }` are reserved and shall be rejected in v1 mode with a diagnostic indicating the syntax is reserved for future revisions.
- Implementations may provide experimental syntax only behind non-conforming extension flags and shall not emit such syntax in canonical interfaces.

## 10.5 Required diagnostics (minimum) {#part-10-5}

- Derive request cannot be satisfied: error with notes listing missing members/protocol requirements.
- Macro expansion introduces exported declarations but cannot be represented in emitted interface: error (or emit-only warning in permissive mode).
- Property behavior composition conflict: error with explanation of conflict.
- Requested derive capability is not available in the active package set/profile claim: error with note naming required derive macro/capability.
- Unresolvable macro/derive package manifest or lockfile mismatch under conformance mode: error.
- Reserved property-behavior surface syntax used in v1 mode: error with note indicating attribute-based form.

## 10.6 Conformance tests (minimum) {#part-10-6}

Conforming suites shall include at least:

- `META-PKG-01`: package discovery via explicit `-fobjc3-meta-package` inputs is deterministic and independent of ambient search paths.
- `META-PKG-02`: lockfile mismatch or missing package produces required diagnostic and blocks expansion.
- `META-PKG-03`: identical source + manifests + lockfile yields reproducible expansion outputs in interfaces/metadata.
- `META-DRV-01`: Core claim passes without requiring individual derives.
- `META-DRV-02`: `OPT-META` claim requires and validates `Equality`, `Hash`, `DebugDescription`, and `Codable` derive availability.
- `META-DRV-03`: missing optional derive capability while source requests it yields required diagnostic with derive capability ID/macro guidance.
- `META-SYN-01`: attribute-based behavior form parses and lowers as specified.
- `META-SYN-02`: reserved behavior-block syntax in v1 mode is rejected with required reserved-syntax diagnostic.

## 10.7 Open issues {#part-10-7}

No open issues are tracked in this part for v0.11.

Resolved reference: macro/derive extension governance process
(`spec/governance/MACRO_DERIVE_EXTENSION_GOVERNANCE.md`).


