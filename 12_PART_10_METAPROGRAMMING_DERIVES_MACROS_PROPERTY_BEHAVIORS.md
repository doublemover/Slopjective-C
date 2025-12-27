# Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors

## 10.1 Purpose
Objective‑C has historically relied on:
- the C preprocessor,
- code generation scripts,
- and conventions.

Objective‑C 3.0 introduces structured mechanisms that reduce boilerplate while remaining auditable and toolable:
- derives for common conformances,
- AST macros with strong safety constraints,
- property behaviors/wrappers for reusable property semantics.

## 10.2 Derives
### 10.2.1 Syntax (provisional)
```objc
@derive(Equatable, Hashable, Codable)
@interface Person : NSObject
@property NSString* name;
@property NSInteger age;
@end
```

### 10.2.2 Semantics
A derive expands to compiler-synthesized members:
- `isEqual:` and `hash` for Equatable/Hashable
- encoding/decoding methods for Codable-like protocols
- copying methods for NSCopying-like protocols (if declared)

The expansion shall:
- be deterministic,
- be visible to tooling (expanded AST view),
- and be overridable only through explicit hooks.

### 10.2.3 Diagnostics
Derive shall fail with clear diagnostics if:
- required fields are not representable,
- cycles or unsupported types are present (for coding derives),
- user-defined methods conflict with synthesized ones without explicit override markers.

## 10.3 AST macros
### 10.3.1 Overview
An AST macro is a compile-time transformation that receives typed syntax/AST and produces additional AST nodes.

### 10.3.2 Safety and sandboxing
Objective‑C 3.0 requires that macro execution be:
- deterministic by default,
- hermetic (no network access),
- and either sandboxed or restricted to approved plugin contexts.

### 10.3.3 Macro kinds
- declaration macros (add members)
- expression macros (produce expressions)
- attribute macros (transform annotated declarations)

### 10.3.4 Hygiene
Macros shall be hygienic by default:
- generated identifiers do not capture or shadow user identifiers unless explicitly requested.

### 10.3.5 Tooling visibility
Toolchains shall provide:
- a way to view expanded code,
- stable source locations for diagnostics pointing through macro expansion.

## 10.4 Property behaviors / wrappers
### 10.4.1 Motivation
Reusable property semantics are common:
- observation
- synchronization
- clamping and validation
- lazy initialization

### 10.4.2 Syntax (provisional)
```objc
@property @observed NSString* title;
@property @atomic NSInteger count;
```

Or wrapper-style:
```objc
@property Wrapped<Atomic<int>> count;
```

### 10.4.3 Semantics
A behavior/wrapper may:
- introduce backing storage,
- synthesize accessors,
- attach observation hooks,
- impose thread-safety rules.

The spec shall define:
- how behaviors compose,
- which behaviors are allowed on exported ABI surfaces,
- and how they interact with KVC/KVO and key paths.

### 10.4.4 Performance contracts
Behaviors must declare whether:
- accessors are direct-callable (eligible for inlining),
- they require dynamic dispatch hooks.

## 10.5 Open issues
- Plugin packaging format (SwiftPM-like vs compiler-integrated).
- Which derives are standardized in the core language vs shipped in a standard library module.
- Final syntax spellings.
