# Objective‑C 3.0 — Attribute and Surface Syntax Catalog (v0.7)
_Last updated: 2025-12-28_

This document defines the **canonical spellings** (especially for headers/module interfaces) for language and toolchain-visible features introduced by the Objective‑C 3.0 draft.

It exists to:
- keep the specification internally consistent,
- give toolchain implementers a single “what do we actually parse/emit” reference,
- and provide a stable mapping from “nice surface sugar” to **header-safe spellings**.

## 0. Principles (normative)

1. **Header-safe canonicalization.**  
   Where a feature needs to survive preprocessing, module interface generation, and cross-language tooling, the specification favors **C/Clang-style attributes** as the canonical spelling.

2. **Optional sugar.**  
   “Surface sugar” spellings (especially new `@`-prefixed modifiers) are **not required** unless explicitly stated. Toolchains may provide sugar as a source-level convenience, but module interfaces shall preserve canonical spellings.

3. **No silent semantic dependence on textual include order.**  
   If a feature affects type checking, it must be representable in module interfaces (attributes, metadata, or standardized pragmas).

These principles refine Decision D‑007 in the Decisions Log.

---

## 1. Reserved keywords (ObjC 3.0 mode)

Minimum reserved keywords are defined in Part 1. Toolchains should provide a raw-identifier escape hatch for compatibility.

---

## 2. Directives and pragmas

### 2.1 Nonnull-by-default regions (v1)
**Canonical spelling:**
```c
#pragma objc assume_nonnull begin
#pragma objc assume_nonnull end
```

**Accepted aliases (recommended for Clang-family toolchains):**
```c
#pragma clang assume_nonnull begin
#pragma clang assume_nonnull end
```

**Notes:**
- Foundation-style macros (e.g., `NS_ASSUME_NONNULL_BEGIN/END`) may expand to the pragma alias form via `_Pragma(...)`.
- Module interfaces shall preserve the *effective* nonnull-by-default behavior (Part 3).

### 2.2 Dispatch intent regions (optimization hint; optional)
**Canonical spelling (if implemented):**
```c
#pragma objc3 dispatch dynamic
#pragma objc3 dispatch static
```

These pragmas express intent to favor dynamic dispatch or static/direct dispatch in the following region. They are **non-normative optimization hints**: correctness shall not depend on them.

---

## 3. Module-qualified identifiers (ObjC / ObjC++)

**Canonical spelling:**
```text
@<module-path>.<TopLevelName>
```

Where `<module-path>` is one or more identifiers separated by `.` and `TopLevelName` is an identifier.

Examples:
- `@Foundation.NSString`
- `@Foundation.NSStringEncoding` (if in module scope)
- `@MyFramework.Submodule.Widget`

Parsing rule:
- The **final** identifier after the last `.` is the declaration name.
- Everything between `@` and that final `.` is the module path.

This spelling is chosen to remain unambiguous in ObjC++ translation units.

---

## 4. Canonical attributes

All attributes below use the canonical C spelling:

```c
__attribute__((<attribute-name>(<optional-args>)))
```

Toolchains may additionally support the C++11 attribute spelling `[[...]]` as a synonym, but module interfaces shall preserve at least one canonical form.

### 4.1 Concurrency

#### 4.1.1 Executors
```c
__attribute__((objc_executor(main)))
__attribute__((objc_executor(global)))
__attribute__((objc_executor(named("<name>"))))
```

Applies to: functions/methods, and actor declarations (where applicable).  
Semantics: Part 7.

#### 4.1.2 Task creation APIs
```c
__attribute__((objc_task_spawn))
__attribute__((objc_task_detached))
__attribute__((objc_task_group))
```

Applies to: functions/methods in the standard library (or equivalent).  
Semantics: Part 7.

#### 4.1.3 Sendable checking
```c
__attribute__((objc_sendable))
__attribute__((objc_unsafe_sendable))
```

Applies to: types and declarations as specified by Part 7.  
These attributes exist to support checking in pure ObjC codebases without requiring Swift-only annotations.

> Note: Part 7 also defines `@protocol Sendable` as the semantic marker in ObjC code. Toolchains may map between protocol conformance and these attributes.

### 4.2 Errors and bridging

#### 4.2.1 NSError out-parameter marking
```c
__attribute__((objc_error_out))
```

Applies to: an `NSError **` out parameter.  
Semantics: Part 6.

#### 4.2.2 Status-code APIs
```c
__attribute__((objc_status_code(success = <integer-literal>)))
__attribute__((objc_status_to_error(<mapping-function>)))
```

Applies to: a function.  
Semantics: Part 6.

### 4.3 Performance and dynamism

#### 4.3.1 Direct dispatch
```c
__attribute__((objc_direct))
__attribute__((objc_direct_members))
```

Applies to: methods and classes/interfaces.  
Semantics: Part 9.

#### 4.3.2 Final / sealed
```c
__attribute__((objc_final))
__attribute__((objc_sealed))
```

Applies to: methods (`objc_final`) and classes/interfaces (`objc_sealed`).  
Semantics: Part 9.

> Note: Some toolchains may choose to map these onto existing vendor attributes; this catalog defines the required *capability* and canonical spelling used in this draft.

### 4.4 Metaprogramming

#### 4.4.1 Derives
```c
__attribute__((objc_derive(<Derive1>, <Derive2>, ...)))
```

Applies to: declarations that support derives (interfaces, structs, enums as specified in Part 10).  
Semantics: Part 10.

---

## 5. Required sugar spellings (v1)

The following sugar spellings are **required** because they are core language syntax (not optional attributes):

- Optional types: `T?`, implicitly unwrapped optionals: `T!` (Part 3)
- Optional member access: `?.` and optional message send: `[receiver? selector]` (Part 3)
- Nil-coalescing: `??` (Part 3)
- Typed key path literal: `@keypath(Root, member.chain)` (Part 3)
- Module-qualified identifiers: `@Module.Symbol` (Part 2)
- Effects: `async`, `await`, `throws`, `try`, `throw`, `do`, `catch` (Parts 6–7)
- Bindings: `if let`, `guard let` with `let`/`var` (Part 3)

All other “nice” spellings (e.g., `@final`, `@sealed`, `@task_spawn`, `@derive`) are **not required** in v1 unless a future decision explicitly upgrades them.
