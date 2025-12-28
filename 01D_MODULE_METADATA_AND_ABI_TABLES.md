# Objective‑C 3.0 — Module Metadata and ABI Surface Tables
_Working draft v0.10 — last updated 2025-12-28_

## 01D.0 Purpose
Objective‑C 3.0 adds source-level semantics that **must survive module boundaries**:
effects (`async`, `throws`), nullability defaults, executor/actor isolation, and dispatch controls (`objc_direct`, `objc_sealed`, etc.).

This document is a **normative checklist** for what a conforming implementation must preserve in:
- module metadata (binary or AST-based), and
- any emitted textual interface (01B.7).

It also summarizes which features are **ABI-affecting** and what ABI stability constraints apply.

> This document does not mandate a specific on-disk format. It defines *required information*.

## 01D.1 Terminology
- **Module metadata**: any compiled representation that an importer uses instead of re-parsing raw headers (e.g., Clang module files, serialized AST, interface stubs).
- **Textual interface**: a tool-emitted, importable text representation intended to preserve semantics for distribution (01B.7).
- **ABI-affecting**: changes the calling convention, symbol shape, layout, or runtime dispatch surface such that mismatches across translation units could cause miscompilation or runtime faults.

## 01D.2 Module metadata requirements (normative)
A conforming implementation shall preserve, for all exported declarations:

1. **Full type information** including:
   - parameter/return types,
   - nullability qualifiers and defaults (Part 3),
   - ownership qualifiers and ARC-related attributes (Part 4),
   - pragmatic generics information (Part 3) sufficient for type checking (even if erased at ABI).

2. **Effect information**:
   - whether the callable is `throws`,
   - whether the callable is `async`,
   - and whether the callable is *potentially suspending at call sites* due to isolation (Part 7).

3. **Isolation and scheduling metadata**:
   - executor affinity (`objc_executor(...)`) (Part 7),
   - actor isolation (actor type, isolated vs nonisolated members) (Part 7),
   - Sendable-like constraints for cross-task/actor boundaries (Part 7).

4. **Dispatch and dynamism controls**:
   - `objc_direct`, `objc_final`, `objc_sealed` (Part 9),
   - any attributes that change call legality or override/subclass legality.

5. **Interop bridging markers**:
   - `objc_nserror` and `objc_status_code(...)` (Part 6),
   - any language-defined overlays/bridges that affect call lowering.

6. **Provenance**:
   - the owning module of each exported declaration (Part 2),
   - API partition membership (public/SPI/private) if the toolchain supports it (Part 2).

## 01D.3 Required metadata tables

### 01D.3.1 Table A — “must preserve across module boundaries”
| Feature | Applies to | Must be recorded in module metadata | Must be emitted in textual interface | Notes |
|---|---|---:|---:|---|
| Nullability qualifiers (`_Nullable`, `_Nonnull`) | types, params, returns, properties | ✅ | ✅ | Includes inferred defaults where part of the public contract. |
| Nonnull-by-default regions | headers / interface units | ✅ | ✅ | Canonical pragmas from 01B.2. |
| Pragmatic generics parameters | class/protocol types, collections | ✅ | ✅ | ABI may erase; type checking must not. |
| `T?`, `T!` (optional spellings) | type surface | ✅ | ✅ | Emitted form may choose canonical spellings but semantics must match. |
| `throws` effect | functions/methods/blocks | ✅ | ✅ | ABI-affecting (see Table B). |
| `async` effect | functions/methods/blocks | ✅ | ✅ | ABI-affecting (see Table B). |
| Executor affinity `objc_executor(...)` | funcs/methods/types | ✅ | ✅ | May imply call-site `await` when crossing executors (Part 7). |
| Actor type / actor isolation | actor classes + members | ✅ | ✅ | Includes nonisolated markings. |
| Sendable-like constraints | types, captures, params/returns | ✅ | ✅ | Importers must be able to enforce strict checks. |
| Borrowed pointer qualifier (`borrowed T *`) | types (params/returns) | ✅ | ✅ | Part 8; enables escape diagnostics in strict-system. Importers must preserve the qualifier. |
| Borrowed-return marker (`objc_returns_borrowed(owner_index=N)`) | functions/methods | ✅ | ✅ | Part 8; identifies the owner parameter for lifetime checking of interior pointers. |
| Task-spawn recognition (`objc_task_spawn` etc.) | stdlib entry points | ✅ | ✅ | Needed for compiler enforcement at call sites. |
| Direct method `objc_direct` | methods | ✅ | ✅ | Changes call legality and category interactions. |
| Final `objc_final` | classes/methods | ✅ | ✅ | Optimization + legality constraints. |
| Sealed `objc_sealed` | classes | ✅ | ✅ | Cross-module subclass legality. |
| Derive/macro synthesized declarations | types/members | ✅ | ✅ | Synthesized members must be visible to importers before layout/ABI decisions. |
| Error bridging markers (`objc_nserror`, `objc_status_code`) | funcs/methods | ✅ | ✅ | Affects lowering of `try` and wrappers. |
| Availability / platform gates | all exported decls | ✅ | ✅ | Not a new ObjC 3.0 feature, but required for correctness. |

### 01D.3.2 Table B — ABI boundary summary (v1)
| Feature | ABI impact | Required stability property | Canonical/recommended ABI shape |
|---|---|---|---|
| `throws` | **Yes** | Caller and callee must agree on calling convention across modules | Recommended: trailing `id<Error> * _Nullable outError` (01C.4). |
| `async` | **Yes** | Importers must call using the same coroutine/continuation ABI | Recommended: LLVM coroutine lowering (01C.5), with a task/executor context. |
| Executor/actor isolation | Usually **no** (call-site scheduling) | Metadata must exist so importer inserts hops and requires `await` | No ABI change required for sync bodies; hop is an `await`ed scheduling operation. |
| Optional chaining/sends | No | Semantics preserved in caller IR | Lower to conditional receiver check; args not evaluated on nil (01C.3.1). |
| Direct methods | Sometimes (dispatch surface) | Importers must know legality + dispatch mode | Recommended: direct symbol call + omit dynamic lookup where permitted (01C.8). |
| Generics (pragmatic) | No (erased) | Importers must type-check consistently | Erased in ABI; preserved in metadata/interface. |
| Key paths | Library ABI | Standard library must define stable representation | ABI governed by stdlib; metadata must preserve `KeyPath<Root,Value>` types. |

### 01D.3.3 Table C — Runtime hooks (minimum expectations)
This table names **conceptual hooks**. Implementations may use different symbol names.

| Area | Minimum required capability | Notes |
|---|---|---|
| Executors | Enqueue continuation onto an executor; query “current executor” | Needed for `objc_executor(...)` and for `await` resumption. |
| Tasks | Create child/detached tasks; join; cancellation state | Standard library provides API; compiler enforces via attributes. |
| Actors | Each actor instance maps to a serial executor; enqueue isolated work | Representation is implementation-defined (inline field or side table). |
| Autorelease pools | Push/pop implicit pools around execution slices | Normative on ObjC runtimes (01C.7). |
| Diagnostics metadata | Preserve enough source mapping for async/macro debugging | Part 12 requires debuggability. |

## 01D.4 Conformance tests (minimum)
A conforming implementation’s test suite should include:

- Importing a module and verifying that `async`/`throws` effects survive and mismatches are diagnosed.
- Importing executor-annotated APIs and verifying cross-executor calls require `await` in strict concurrency mode.
- Importing actor APIs and verifying cross-actor access requires `await` and preserves isolation.
- Verifying that emitted textual interfaces preserve canonical spellings (01B) and round-trip semantics.
