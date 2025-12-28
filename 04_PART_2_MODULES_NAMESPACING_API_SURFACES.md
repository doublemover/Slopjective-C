# Part 2 — Modules, Namespacing, and API Surfaces
_Working draft v0.9 — last updated 2025-12-28_

## 2.1 Purpose
This part defines:

- module import rules and their interaction with headers,
- a **module-qualified name** mechanism to reduce global-prefix collision pressure,
- API surface contracts: visibility, stability, and availability,
- requirements for interface extraction/emission so ObjC 3.0 semantics survive separate compilation.

This part is closely related to:
- **01B** (canonical spellings for emitted interfaces),
- **01C** (separate compilation and lowering contracts), and
- **01D** (the normative checklist of what must be preserved in module metadata across boundaries).

## 2.2 Modules

### 2.2.1 `@import`
Objective‑C 3.0 standardizes `@import` as the preferred import mechanism.

A module import shall:
- make the imported module’s public declarations visible,
- apply the module’s exported nullability metadata (Part 3),
- apply the module’s exported availability metadata (if any),
- make any exported “strictness recommendation” visible to tooling (Part 1).

### 2.2.2 `#import` and mixed-mode code (non-normative)
`#import` remains supported for compatibility with existing headers.
However, toolchains are encouraged to treat `@import` as the semantic source of truth for:
- nullability defaults,
- availability,
- and extracted interface emission.

### 2.2.3 Module maps and header ownership (implementation-defined)
The mechanism that maps headers to modules (module maps, build system metadata, etc.) is implementation-defined.
A conforming implementation shall behave **as if** each public declaration belongs to exactly one owning module for the purpose of:
- API visibility,
- interface emission,
- and cross-module diagnostics.

## 2.3 Module-qualified names

### 2.3.1 Motivation
Objective‑C historically uses global prefixes (e.g., `NS`, `UI`, `CG`) to avoid collisions.
As module ecosystems grow, prefixes are increasingly insufficient and noisy.

Objective‑C 3.0 introduces a lightweight module-qualified name form that:
- is unambiguous in ObjC and ObjC++,
- does not require a global namespace feature,
- is stable under interface emission.

### 2.3.2 Syntax (normative)
A *module-qualified name* is written:

```objc
@ModuleName.Identifier
@TopLevel.Submodule.Identifier
```

Grammar (informative):

- `module-qualified-name` → `'@' module-path '.' identifier`
- `module-path` → `identifier ('.' identifier)*`

### 2.3.3 Where module-qualified names may appear (normative)
Module-qualified names may appear in:
- type positions (class names, protocol names, typedef names),
- expression positions where an identifier would be valid (e.g., referring to a global function or global constant),
- attribute arguments where an identifier names a type or symbol.

They shall not appear in:
- selector spelling positions (the `:`-separated selector pieces),
- preprocessor directive names.

### 2.3.4 Name resolution (normative)
A module-qualified name resolves by:
1. Resolving the module path to a module (or submodule) that is visible in the current translation unit.
2. Looking up the identifier in that module’s exported declarations.

If no matching declaration exists, the program is ill-formed.

### 2.3.5 Interface emission (normative)
If a declaration is referenced using a module-qualified name in an emitted textual interface, the emitter shall preserve the module-qualified form **or** emit an equivalent unqualified name together with imports that make it unambiguous.

## 2.4 API surface contracts

### 2.4.0 Cross-module semantic preservation (normative)
A module’s public API contract includes not only names and types, but also ObjC 3.0 semantics that affect call legality and call lowering.
The minimum required set is enumerated in **01D.3.1 Table A**.


### 2.4.1 Public vs SPI vs private (normative)
A module’s API surface is partitioned into (at minimum):
- **Public API**: visible to all importers.
- **SPI** (system/private interface): visible only to privileged importers (mechanism implementation-defined).
- **Private**: not visible outside the owning module.

The mechanism used to mark partitions is implementation-defined (attributes, build system flags, header directory layout, etc.), but a conforming toolchain shall preserve partition metadata in module files and interface emission.

### 2.4.2 Effects and attributes are part of the API contract (normative)
For exported functions/methods, the following are API-significant and must be preserved across module boundaries:

- effects: `async`, `throws`
- executor affinity: `objc_executor(...)`
- task-spawn recognition attributes (when exporting concurrency entry points)
- nullability and ownership qualifiers
- direct/final/sealed annotations that affect call legality or dispatch (Part 9)

## 2.5 Extracted interfaces and semantic preservation

### 2.5.1 Requirement (normative)
If an implementation provides any facility that emits an interface description (textual or AST-based), then importing that interface must reconstruct:

- the same declarations and types,
- the same effects (`async`, `throws`),
- the same canonical attributes/pragmas needed for semantics.

Canonical spellings are defined in **01B**.

### 2.5.2 Format (implementation-defined)
This draft does not require a particular distribution format (text vs binary).
However, the format must be sufficient to support:

- strictness/migration tooling (Part 1),
- diagnostics for effect mismatches (01C.2),
- and cross-module concurrency checking (Part 7).

## 2.6 Required diagnostics (minimum)
- Using a module-qualified name for an unloaded/unimported module: error with fix-it to add `@import`.
- Resolving a module-qualified name to multiple candidates (ambiguous exports): error; recommend qualification or import narrowing.
- Redeclaring an imported API with mismatched effects/ABI-significant attributes: error (see 01C.2).
