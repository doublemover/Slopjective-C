# Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors
_Working draft v0.7 — last updated 2025-12-28_

## 10.1 Purpose

> See also: **01B_ATTRIBUTE_AND_SYNTAX_CATALOG.md** for canonical spellings.

Objective‑C has historically relied on:
- the C preprocessor,
- code generation scripts,
- and informal conventions.

Objective‑C 3.0 introduces structured metaprogramming mechanisms that are:
- **deterministic** (builds are reproducible),
- **auditable** (expanded form is inspectable),
- **toolable** (indexers, analyzers, and IDEs can reason about results),
- and **ABI-conscious** (public interfaces remain stable).

This part defines three mechanisms:
1. **Derives** — compiler-synthesized members for common patterns.
2. **AST Macros** — user-defined compile-time expansions with strong constraints.
3. **Property behaviors/wrappers** — reusable property semantics (observation, atomicity, validation, lazy storage).

## 10.2 Derives

### 10.2.1 Syntax (v1)
Derives are requested by applying an attribute to the declaration being derived.

**Canonical spelling (Decision D‑011):**
```objc
__attribute__((objc_derive(Equatable, Hashable)))
@interface Person : NSObject
@property (readonly) NSString *name;
@property (readonly) NSInteger age;
@end
```

Toolchains may provide additional sugar (e.g., an `@derive(...)` directive), but such sugar is not required in v1 (Decision D‑010). Module interfaces shall preserve the attribute form (directly or as equivalent metadata).

### 10.2.2 Standard derives (v1 set) (v1 set)
This draft defines a minimal v1 derive set (informative names; toolchains may ship as standard library macros initially):

- **Equatable / Hashable**: synthesize `-isEqual:` and `-hash`.
- **Codable-like**: synthesize encode/decode routines for supported field types.
- **MemberwiseInit** (optional): synthesize an initializer taking stored properties (where representable).

A derive shall be rejected with diagnostics if the type does not meet preconditions.

### 10.2.3 Semantics (normative)
A derive expands to compiler-synthesized declarations that:
- are deterministic,
- are visible to tooling as expanded AST,
- obey visibility and API surface rules (Part 2),
- and do not require runtime reflection beyond what baseline ObjC already provides.

### 10.2.4 Conflict and customization
If user code declares a member that conflicts with a derived member:
- In permissive mode: warn and choose user member.
- In strict mode: error unless an explicit `@derive_override` marker is present.

A future revision may standardize customization hooks (e.g., attributes marking which properties participate).

### 10.2.5 ABI considerations
For public types:
- derived members that contribute to ABI (e.g., method presence) shall be considered part of the public API surface if the type is `@public`.
- removing a derive is therefore an API-breaking change (diagnosed by tooling).

## 10.3 AST macros

### 10.3.1 Macro kinds
AST macros may be:
- **declaration macros** (add members or declarations),
- **attribute macros** (transform an annotated declaration),
- **expression macros** (expand to an expression).

### 10.3.2 Determinism and sandboxing (normative)
Macro execution shall be deterministic by default:
- no network access,
- no reading arbitrary files outside declared inputs,
- stable hashing of macro inputs for incremental builds.

A conforming implementation shall provide:
- either a sandboxed execution environment, or
- a trust/allowlist mechanism where untrusted macros are rejected.

### 10.3.3 Canonical packaging (open but required concept)
This draft does not fix the packaging format, but requires the concept of a *macro module*:
- a compiled artifact containing macro implementations,
- referenced explicitly by the build system and/or module metadata,
- versioned and cacheable.

### 10.3.4 Hygiene (normative intent)
Macros shall be hygienic by default:
- generated identifiers do not accidentally capture user identifiers,
- name collisions are prevented unless explicitly requested.

### 10.3.5 Tooling visibility (normative)
Toolchains shall provide:
- a way to view macro-expanded code,
- stable source locations for diagnostics that cross macro boundaries,
- and an exportable “expanded interface” for module interfaces (Part 2).

## 10.4 Property behaviors / wrappers

### 10.4.1 Motivation
Reusable property semantics are pervasive:
- observation (KVO-like, custom observation),
- synchronization/atomicity,
- validation/clamping,
- lazy initialization.

### 10.4.2 Two surface models
This draft allows two equivalent surface models:

1) **Attribute behavior model**
```objc
@property @observed NSString *title;
@property @atomic NSInteger count;
```

2) **Wrapper type model**
```objc
@property Wrapped<Atomic<int>> count;
```

A conforming implementation may choose one as the canonical surface syntax and treat the other as sugar.

### 10.4.3 Semantics (normative intent)
A behavior/wrapper may:
- introduce backing storage,
- synthesize accessors,
- attach observation hooks,
- impose thread-safety rules,
- and participate in key path typing (Part 3).

Behaviors must declare:
- whether accessors are eligible for direct calls/inlining (Part 9),
- whether they require dynamic dispatch hooks.

### 10.4.4 Composition
If multiple behaviors are applied, the composition order shall be defined and deterministic.
This draft proposes left-to-right composition as written in source.

### 10.4.5 KVC/KVO and key paths
For behaviors intended to interoperate with Foundation-style KVC/KVO:
- key path formation shall observe the behavior’s declared key-path exposure,
- observation hooks must not break typed key paths.

### 10.4.6 Restrictions for public ABI
In public API surfaces, behaviors must be ABI-stable:
- a behavior used in a public property shall have a stable, named identity,
- changing the behavior is an API-breaking change unless marked as resilient by module policy.

## 10.5 Required diagnostics
- Macro expansion failures must include clear diagnostics pointing to the expansion site and the macro definition.
- Derive failures must explain missing preconditions.
- Behavior composition conflicts must be diagnosed with suggested reorderings or explicit overrides.

## 10.6 Open issues
- Finalize macro packaging format and trust model (SwiftPM-like vs compiler-integrated).
- Decide which derives are core language vs standard-library macros.
- Finalize the canonical surface spelling for property behaviors (attributes vs wrapper types).
