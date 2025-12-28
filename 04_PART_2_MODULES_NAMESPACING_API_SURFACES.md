# Part 2 — Modules, Namespacing, and API Surfaces
_Working draft v0.6 — last updated 2025-12-28_

## 2.1 Purpose
This part defines:
- module import rules and their interaction with headers,
- module-qualified names to reduce prefix-collision pressure,
- API surface contracts: visibility, stability, and availability,
- how ObjC 3.0 semantics are preserved across module boundaries,
- guidance for framework authors migrating to ObjC 3.0 APIs.

## 2.2 Modules

### 2.2.1 `@import`
Objective‑C 3.0 standardizes `@import` as the preferred import mechanism for modules.

A module import shall:
- make the imported module’s public declarations visible,
- apply the module’s exported nullability/ownership/concurrency metadata (as present),
- avoid textual inclusion hazards where possible.

### 2.2.2 Textual includes and mixing
`#import` remains available for legacy compatibility. However, in ObjC 3.0 mode:
- toolchains should prefer `@import` where a module exists,
- and should warn in strict mode when public headers rely on textual include order for semantics (e.g., nullability macro regions).

### 2.2.3 Submodules
An implementation may support submodule imports:

```objc
@import Foundation.NSString;
```

If supported, name lookup rules shall be specified to avoid surprising shadowing. A submodule import shall not silently change meaning of names already imported, except by adding new declarations.

## 2.3 Module metadata (interface contracts)

### 2.3.1 Metadata categories
A module may publish metadata that affects how its API is checked when imported:

- **Nullability completeness**: whether public APIs are fully annotated (Part 3).
- **Ownership and transfer**: whether transfer annotations are present for unsafe APIs (Parts 4/8).
- **Concurrency intent**: executor affinity defaults, actor isolation availability, and “strict concurrency recommended” hints (Part 7).
- **Strictness recommendation**: a recommended client checking level (Part 1).

### 2.3.2 Exporting metadata
This draft does not mandate a single module metadata file format. A conforming implementation shall, however, preserve metadata across module boundaries such that importing a module yields the same type/ownership/effect information as compiling the headers directly.

> Decision D‑007: header-facing semantics should have canonical `__attribute__((...))` spellings so that metadata is not “macro folklore.”

## 2.4 Module-qualified names (namespacing)

### 2.4.1 Motivation
Objective‑C’s traditional prefix conventions (e.g., `NS`, `UI`, `CG`) are not a robust namespacing mechanism.

Objective‑C 3.0 defines a module qualification mechanism to:
- disambiguate conflicting type names,
- allow frameworks to expose “clean” names without collisions,
- and reduce pressure to keep long global prefixes forever.

### 2.4.2 Syntax (provisional but implementable)
This draft defines an *unambiguous* module-qualified identifier form for ObjC 3.0 mode:

```text
@Module.Name
```

Examples:
- `@Foundation.NSString`
- `@MyFramework.Widget`

Notes:
- The leading `@` makes the qualification unambiguous in ObjC++ translation units (it does not conflict with C++ `::` scope resolution).
- `.` is chosen because it is widely understood and does not conflict with existing type-name grammar.

> Alternative spellings (e.g., `Module::Name`) are possible, but this draft standardizes `@Module.Name` as the v1 spelling to avoid ObjC++ ambiguity.

### 2.4.3 Semantics
- **Unqualified lookup** searches the current module, then imported modules.
- **Qualified lookup** resolves directly to the named module’s exported declaration set.
- If two imported modules export the same unqualified name, unqualified lookup shall be ambiguous and diagnosed; qualification resolves it.

### 2.4.4 Qualification scope
Qualification applies to:
- types (classes, protocols, typedefs),
- top-level functions,
- global variables/constants.

This draft does not define qualification for selectors; selectors remain globally interned runtime entities.

## 2.5 API surface contracts

### 2.5.1 Visibility
Objective‑C 3.0 defines module-scoped visibility categories:

- `@public`
- `@internal`
- `@private`

**Normative intent:**
- `@public` declarations are part of the module’s public API surface.
- `@internal` declarations are visible to the module but not exported for external clients.
- `@private` declarations are visible only within the defining file/implementation region.

**Canonical attribute spelling (Decision D‑007):**
Implementations should expose these as attributes on declarations, e.g.:

- `__attribute__((objc_visibility(public)))`
- `__attribute__((objc_visibility(internal)))`
- `__attribute__((objc_visibility(private)))`

(Exact attribute name is a toolchain choice; the semantics above are normative.)

### 2.5.2 Stability (source/ABI)
The spec defines stability markers for declarations:

- `@stable` — the module commits to source compatibility (and, if applicable, ABI stability) under its published policy
- `@unstable` — the declaration may change without compatibility guarantees

In strict mode, consuming `@unstable` APIs from a different module should produce a warning unless the import is marked as “internal/SPI allowed.”

> Note: Apple toolchains have SPI and availability mechanisms; this draft specifies portable semantics and allows mapping onto platform-specific tooling.

### 2.5.3 Availability
Objective‑C 3.0 standardizes availability annotations:

```objc
@available(platform, introduced: 1.0, deprecated: 2.0, obsoleted: 3.0)
```

Compilers shall diagnose availability misuse in strict modes (calling unavailable APIs without guards).

### 2.5.4 API extraction / interface files
A conforming toolchain should be able to emit a “module interface” description containing:
- module exports and visibility,
- nullability and ownership metadata,
- error and status-code mapping metadata (Part 6),
- concurrency metadata (async/throws effects, executor affinity, actor isolation).

This enables resilient distribution without shipping raw, macro-heavy headers.

> Open issue: define a portable “ObjC interface format” (text or binary) analogous in intent to Swift’s `.swiftinterface`.

## 2.6 Default nullability and strictness at module boundaries
A module may declare:
- a default nullability regime for its public interface (e.g., “complete nullability”),
- a recommended strictness level for clients,
- and a recommended concurrency checking mode.

Clients may override strictness, but imported API metadata shall be preserved in type checking.

## 2.7 Open issues
- Confirm the final module-qualified syntax and its interaction with tooling; this draft standardizes `@Module.Name` for v1 but may accept synonyms.
- Map `@stable/@unstable` cleanly onto existing platform availability/SPI infrastructure.
- Define a portable interface file format and minimum required metadata fields.
