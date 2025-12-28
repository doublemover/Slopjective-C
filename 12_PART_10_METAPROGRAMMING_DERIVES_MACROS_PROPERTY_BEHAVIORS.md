# Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors
_Working draft v0.6 — last updated 2025-12-28_

## 10.1 Purpose
Objective‑C has historically relied on:
- the C preprocessor,
- code generation scripts,
- and conventions.

Objective‑C 3.0 introduces structured mechanisms that reduce boilerplate while remaining **auditable** and **toolable**:
- derives for common conformances and synthesized members,
- AST macros with strong safety/determinism constraints,
- property behaviors/wrappers for reusable property semantics.

This part is intentionally conservative: metaprogramming is permitted only where tooling can preserve source locations and where expansion is deterministic and cacheable.

---

## 10.2 Derives

### 10.2.1 Concept
A **derive** is a declaration-level request for synthesized code with well-defined semantics, similar in spirit to “derive traits” in Rust and compiler-synthesized conformances in Swift.

Derives are intended for:
- equality/hash boilerplate,
- Codable-like coding adapters,
- debug/description synthesis,
- small, predictable patterns that should not require external code generation.

### 10.2.2 Syntax (provisional)
This draft uses a lightweight attribute-like spelling:

```objc
__attribute__((objc_derive(Equatable, Hashable)))
@interface Point : NSObject
@property (nonatomic) int x;
@property (nonatomic) int y;
@end
```

> Open issue: final surface spelling (`@derive(...)` sugar vs pure attributes).

### 10.2.3 Semantics (normative)
- Derive expansion is a compile-time transformation that produces additional declarations as if written by the user.
- Expansion shall be **deterministic** with respect to:
  - the declaring type’s visible members,
  - the derive arguments,
  - and the imported modules’ stable interfaces.

A toolchain shall make the expanded declarations available to:
- type checking,
- code completion,
- and debugging (via source-location mapping; Part 12).

### 10.2.4 Conflict resolution (normative)
If a derive would synthesize a declaration that conflicts with a user declaration:
- in strict mode, the program is ill‑formed unless the user explicitly opts in to overriding/suppression,
- in permissive mode, the toolchain may accept but shall diagnose and prefer the user declaration.

### 10.2.5 Diagnostics
Derives shall fail with clear diagnostics if:
- required fields are not representable,
- cycles or unsupported types are present (for coding derives),
- user-defined methods conflict with synthesized ones without an explicit resolution marker.

---

## 10.3 AST macros

### 10.3.1 Concept
An **AST macro** is a compile-time transformation that receives typed syntax/AST and produces additional AST nodes.

Macros are intended for:
- repetitive declaration patterns that are too niche for built-in derives,
- safe wrappers around common “unsafe” APIs (e.g., checked conversions),
- performance-oriented boilerplate that should still be inspectable.

### 10.3.2 Safety and sandboxing (normative intent)
Objective‑C 3.0 requires that macro execution be constrained:

- Macros shall be **pure**: no network access; no reading arbitrary files; no environment-dependent output.
- Macro output shall be deterministic given the macro inputs and the module interfaces it depends on.
- Toolchains should execute macros in a sandboxed process and cache results.

> Open issue: the exact sandboxing contract is toolchain/runtime-specific; this draft specifies the semantic requirement (deterministic, dependency-tracked expansion).

### 10.3.3 Expansion phases (provisional)
A typical pipeline (informative but intended):
1. Parse source into syntax trees.
2. Expand declaration macros that do not require type information.
3. Type-check the module.
4. Expand typed macros that depend on type information.
5. Re-type-check affected declarations as needed.

### 10.3.4 Debuggability (normative)
- Macro-generated code shall preserve source locations such that stack traces and debugger stepping can attribute execution to either:
  - the macro expansion result, and/or
  - the macro call site.

Toolchains shall provide at least one mechanism to inspect macro expansions (Part 12).

---

## 10.4 Property behaviors / wrappers

### 10.4.1 Concept
A **property behavior** (a.k.a. wrapper) is a reusable pattern for property storage and accessors (e.g., atomicity, synchronization, validation, memoization) expressed as a first-class, toolable construct.

### 10.4.2 Syntax (provisional)
This draft uses an attribute-like spelling:

```objc
__attribute__((objc_property_behavior(Atomic)))
@property (nonatomic) int counter;
```

> Open issue: whether to align with Swift-like `@Wrapper` syntax for properties in ObjC source, while retaining an attribute spelling for headers/module interfaces.

### 10.4.3 Lowering model (normative intent)
Applying a property behavior conceptually rewrites the property into:
- backing storage (possibly hidden),
- synthesized getter/setter bodies that implement the behavior,
- and any required helper declarations.

The rewriting must:
- be deterministic,
- be inspectable by tooling,
- and preserve Objective‑C runtime expectations where applicable (KVC/KVO hooks, selector names).

### 10.4.4 Interactions (normative intent)
- Behaviors must not silently change ARC ownership semantics without explicit annotation (Part 4).
- Behaviors that introduce concurrency or locking must interact safely with `async`/actors (Part 7). Toolchains should diagnose behaviors that are unsafe under strict concurrency checking.

---

## 10.5 Open issues
- Final surface spelling for derives/macros/behaviors and compatibility with ObjC++ parsing.
- Module distribution of macros: stable interface representation and versioning.
- Policy for allowing user-defined macro plugins vs standard-library-only macros (security model).
