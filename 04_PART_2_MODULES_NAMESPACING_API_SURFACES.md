# Part 2 — Modules, Namespacing, and API Surfaces
_Working draft v0.6 — last updated 2025-12-28_

## 2.1 Purpose
This part defines:
- module import rules and their interaction with headers,
- module-qualified names to reduce prefix-collision pressure,
- API surface contracts: visibility, stability, and availability,
- guidance for framework authors migrating to ObjC 3.0 APIs.

Objective‑C 3.0 treats module boundaries as the unit of “API truth”: nullability completeness, ownership/error annotations, and concurrency intent should be preservable across separate compilation and distribution.

---

## 2.2 Modules

### 2.2.1 `@import`
Objective‑C 3.0 standardizes `@import` as the preferred import mechanism for modules.

A module import shall:
- make the imported module’s **public** declarations visible,
- preserve the imported API’s declared **nullability** and other type modifiers (Part 3),
- avoid “textual include hazards” where possible (e.g., macro-order dependence) by using a compiler-managed module representation.

> Note: Toolchains may still support `#include`/`#import` for compatibility, but ObjC 3.0 frameworks should be distributable without requiring clients to rely on transitive textual includes.

### 2.2.2 Submodules
An implementation may support submodule imports such as:

```objc
@import Foundation.NSString;
```

If supported:
- submodule names are resolved in the module namespace (not the C preprocessor namespace),
- importing a submodule shall not silently change the meaning of previously-visible names without a diagnostic (at least a warning in permissive mode).

---

## 2.3 Module-qualified names

### 2.3.1 Motivation
Objective‑C’s traditional prefix conventions (e.g., `NS`, `UI`, `CG`) are not a robust namespacing mechanism.

Objective‑C 3.0 defines module qualification to:
- disambiguate conflicting type names across modules,
- allow frameworks to expose “clean” names without global prefix collisions,
- make interop layers (Swift overlays, system shims) more explicit.

### 2.3.2 Syntax (provisional)
This draft uses an illustrative syntax:

- `Module::TypeName`
- `Module::ProtocolName`

where `Module` may include submodule components (e.g., `Framework.Sub`).

> Open issue (surface spelling): choose a final syntax compatible with Objective‑C and ObjC++. Alternatives include `Module.Type`, `@qualified(Module, Type)`, or a keyworded form that cannot collide with C++ scope resolution.

### 2.3.3 Grammar (provisional)
```text
module-name:
    identifier ('.' identifier)*

module-qualified-identifier:
    module-name '::' identifier
```

### 2.3.4 Semantics
- **Unqualified lookup** searches the current module, then imported modules (implementation-defined import order), and shall diagnose ambiguity.
- **Qualified lookup** resolves directly to the named module’s exported declaration.
- If two modules export the same unqualified name, unqualified lookup shall be ambiguous and diagnosed; qualification resolves it.

---

## 2.4 API surface contracts

### 2.4.1 Visibility
Objective‑C 3.0 introduces a notion of *module-scoped* visibility for declarations.

This draft uses placeholder spellings:
- `@public`, `@internal`, `@private`

> Open issue (spelling): map these to a canonical attribute form that is compatible with C/ObjC headers and does not conflict with existing Objective‑C keywords.

Normative semantics:
- **public** declarations are exported from the module’s public interface.
- **internal** declarations are visible to other files within the module but not to importers.
- **private** declarations are visible only within the declaring file or lexical region (implementation-defined, but must be diagnosable).

Categories and extensions shall obey the same visibility rules: an internal category must not “leak” into the public interface.

### 2.4.2 Stability (ABI/source)
Objective‑C 3.0 distinguishes API stability intent:

- `@stable` declarations commit to the module’s declared compatibility policy (Part 1) with respect to source compatibility.
- `@unstable` declarations may change without source-compatibility promises.

> Open issue: map stability intent to existing “SPI” mechanisms on Apple toolchains and to cross-platform visibility systems.

### 2.4.3 Availability and versioning
Objective‑C 3.0 standardizes availability annotations, conceptually:

```objc
@available(platform, introduced: x.y, deprecated: a.b, obsoleted: c.d)
```

Compilers shall diagnose availability misuse in strict modes (e.g., calling unavailable APIs without guards).

---

## 2.5 Default nullability and strictness at module boundaries
A module may declare:
- a default nullability region for its public interface (Part 3), and
- a recommended strictness level for clients (Part 1).

Rules:
- Clients may override strictness, but the **declared nullability** of imported APIs shall be preserved in type checking.
- If an imported API omits nullability where completeness is required by the module’s policy, strict toolchains should diagnose the omission at the module boundary (Part 12).

---

## 2.6 Documentation and extracted interface files
A conforming toolchain should be able to emit an extracted interface description (analogous in spirit to Swift’s `.swiftinterface`) containing:
- module exports,
- nullability,
- ownership and error-model annotations,
- concurrency annotations (`async`, executor/actor isolation).

This enables resilient distribution of ObjC 3.0 frameworks without shipping raw headers.

> Open issue: define an “ObjC interface format” for distribution. Candidates include a stable text format or a stable serialized AST.
