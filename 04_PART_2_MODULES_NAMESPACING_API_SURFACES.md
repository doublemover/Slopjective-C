# Part 2 — Modules, Namespacing, and API Surfaces

## 2.1 Purpose
This part defines:
- module import rules and their interaction with headers,
- module-qualified names to reduce prefix-collision pressure,
- API surface contracts: visibility, stability, and availability,
- guidance for framework authors migrating to ObjC 3.0 APIs.

## 2.2 Modules
### 2.2.1 `@import`
Objective‑C 3.0 standardizes `@import` as the preferred import mechanism for modules.

A module import shall:
- make the imported module’s public declarations visible,
- apply the module’s default nullability and strictness metadata (if declared),
- avoid textual inclusion hazards where possible.

### 2.2.2 Submodules
An implementation may support submodule imports:
- `@import Foundation.NSString;`

If supported, name lookup rules shall be specified to avoid surprising shadowing.

## 2.3 Module-qualified names
### 2.3.1 Motivation
Objective‑C’s traditional prefix conventions (e.g., `NS`, `UI`, `CG`) are not a robust namespacing mechanism.

Objective‑C 3.0 defines a module qualification mechanism to:
- disambiguate conflicting type names,
- allow frameworks to expose “clean” names without collisions.

### 2.3.2 Syntax (provisional)
This draft uses an illustrative syntax:

- `Module::TypeName`
- `Module::ProtocolName`

> Open issue: choose final syntax compatible with Objective‑C’s grammar and existing C++ interop. Alternatives include `Module.Type` or `@qualified(Module, Type)`.

### 2.3.3 Semantics
- Unqualified lookup searches the current module, then imported modules.
- Qualified lookup resolves directly to the named module’s declaration.
- If two modules export the same unqualified name, unqualified lookup shall be ambiguous and diagnosed; qualification resolves it.

## 2.4 API surface contracts
Objective‑C 3.0 adds standard attributes for API surfaces.

### 2.4.1 Visibility
- `@public`, `@internal`, `@private` (module-scoped visibility, not file-scoped)
- Categories and extensions must obey visibility rules.

### 2.4.2 Stability (ABI/source)
- `@stable` declarations commit to source stability guarantees in the module’s declared compatibility policy.
- `@unstable` declarations may change without source-compatibility promises.

> Open issue: map these to existing availability and SPI mechanisms in Apple toolchains; ensure cross-platform portability.

### 2.4.3 Availability and versioning
Objective‑C 3.0 standardizes availability annotations:
- `@available(platform, introduced: x.y, deprecated: a.b, obsoleted: c.d)`

Compilers shall diagnose availability misuse in strict modes (e.g., calling unavailable APIs without guards).

## 2.5 Default nullability and strictness at module boundaries
A module may declare:
- a default nullability region for its public interface,
- a recommended strictness level for clients.

Clients may override strictness, but the default nullability of imported APIs shall be preserved in type checking.

## 2.6 Documentation and extracted interface files
A conforming toolchain should be able to emit an interface description (AST/Swift-like `.swiftinterface` analogue) containing:
- module exports,
- nullability,
- ownership and error model annotations,
- concurrency annotations (`async`, actor isolation).

This enables resilient distribution of ObjC 3.0 frameworks without shipping raw headers.

> Open issue: define an “ObjC interface format” for distribution. Could be text-based or binary AST.
