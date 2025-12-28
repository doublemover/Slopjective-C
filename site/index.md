---
title: Objective-C 3.0 Draft Specification
layout: default
---
<!-- BEGIN TABLE_OF_CONTENTS.md -->
# Objective‑C 3.0 Draft Specification (Working Draft) — Table of Contents <a id="toc"></a>
_Last generated: 2025-12-28_
_Working draft: v0.10_

This is a working draft of an **Objective‑C 3.0** specification broken into numbered, implementable parts.  
Each part is a separate Markdown file intended to be read independently, but the parts are cross-referenced where needed.

## Front matter <a id="toc-front-matter"></a>
- **[TABLE_OF_CONTENTS.md](#toc)** — This table of contents (you are here)
- **[INTRODUCTION.md](#intro)** — Overview, goals, guiding principles, non‑goals, and cross‑cutting design choices
- **[DECISIONS_LOG.md](#decisions)** — Explicit design decisions (ship/no‑ship) made to keep v1 implementable
- **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)** — Canonical spellings for pragmas/attributes and module-interface emission requirements
- **[LOWERING_AND_RUNTIME_CONTRACTS.md](#c)** — Implementation contracts for lowering, ABI stability, and runtime hooks
- **[MODULE_METADATA_AND_ABI_TABLES.md](#d)** — Normative tables for required module metadata and ABI boundaries (implementer checklist)
- **[CONFORMANCE_PROFILE_CHECKLIST.md](#e)** — Conformance profiles and implementer checklists (compiler/toolchain + optional feature sets)

## Parts <a id="toc-parts"></a>
- **[PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md](#part-0)** — Baseline Objective‑C (as implemented by Clang) and the normative reference model used by this draft     
- **[PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md](#part-1)** — Language versioning, feature gating, conformance levels, and migration tooling requirements
- **[PART_2_MODULES_NAMESPACING_API_SURFACES.md](#part-2)** — Modules, imports, module-qualified names, visibility, and API surface contracts
- **[PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md](#part-3)** — Nullability, nonnull-by-default, optionals, pragmatic generics, and typed key paths
- **[PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md](#part-4)** — ARC, ownership qualifiers, transfer/consumption, and interaction with retainable families
- **[PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md](#part-5)** — `defer`, `guard`, `match`, patterns, and control-flow rules
- **[PART_6_ERRORS_RESULTS_THROWS.md](#part-6)** — `throws`, `try`, `do/catch`, `Result`, propagation, and interop with NSError/return codes
- **[PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md](#part-7)** — `async/await`, executors, cancellation, tasks, and actors
- **[PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md](#part-8)** — Library-defined subsets and system programming extensions (IOKit-style ergonomics, safety, performance)
- **[PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md](#part-9)** — Direct/final/sealed controls, dynamism boundaries, and performance-oriented constraints
- **[PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md](#part-10)** — Derives, AST macros, property behaviors/wrappers, and safety constraints
- **[PART_11_INTEROPERABILITY_C_CPP_SWIFT.md](#part-11)** — C/ObjC/ObjC++/Swift interop rules, overlays, and ABI mapping notes
- **[PART_12_DIAGNOSTICS_TOOLING_TESTS.md](#part-12)** — Required diagnostics, fix-its, migrators, analyzers, and conformance test suites

## How to read this draft <a id="toc-how-to-read-this-draft"></a>
1. Start with **[INTRODUCTION.md](#intro)** (goals + cross-cutting constraints).       
2. Read **[Part 1](#part-1)** next (**[PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md](#part-1)**) for conformance levels and migration strategy.
3. Then choose a “slice” (types/errors/concurrency/system/perf) depending on what you’re implementing.

## Status / scope note <a id="toc-status-scope-note"></a>
This draft is ambitious but implementable:
- It prefers features that can be implemented by a Clang/LLVM toolchain without rewriting the Objective‑C runtime.
- It keeps “strictness” opt-in via conformance levels, while providing a clear path to stricter defaults over time.
- Where a feature affects ABI or runtime, it is documented in **[C](#c)** and cross-referenced from the relevant part.

## Pass notes <a id="toc-pass-notes"></a>
- **v0.5:** locked major decisions for optionals/errors/concurrency ([D-001](#decisions-d-001)…[D-006](#decisions-d-006)).
- **v0.8:** adds canonical surface spellings ([B](#b)) and a lowering/ABI/runtime contract ([C](#c)), and begins tightening ABI-impacting features for separate compilation.
- **v0.9:** adds explicit module-metadata/ABI boundary tables ([D](#d)) and tightens `await` requirements for all potentially-suspending operations across executors/actors.
<!-- END TABLE_OF_CONTENTS.md -->

---

<!-- BEGIN INTRODUCTION.md -->
# Objective‑C 3.0 Draft Specification (Working Draft) — Introduction <a id="intro"></a>
_Last generated: 2025-12-28_
_Working draft: v0.10_

## 1. What Objective‑C 3.0 is trying to be <a id="intro-1"></a>
Objective‑C 3.0 is a modernization of Objective‑C focused on:

- **Safety by default (when provable)**: eliminate the most common crash classes and logic bugs without abandoning Objective‑C’s dynamic runtime model.
- **Composable concurrency**: upgrade from “blocks + conventions” to structured `async/await`, cancellation, and actor isolation.
- **Ergonomics without magic**: reduce boilerplate through language-supported patterns (optionals, Result/throws, key paths, derives/macros) that still lower to transparent, testable behavior.
- **System-library excellence**: make Apple platform “library-defined subsets” (CoreFoundation, libdispatch, XPC, IOKit/Mach handles) safer and more ergonomic through first-class annotations and diagnostics.
- **Incremental adoption**: preserve existing ABI and interoperability, and provide an opt-in path that does not require rewriting codebases.

This draft is written to be implementable primarily in **Clang/LLVM**, with minimal required runtime changes. Where runtime support is needed, it is specified as small libraries or well-defined hooks.

## 2. Guiding design principles <a id="intro-2"></a>
### 2.1 Compatibility boundaries are explicit <a id="intro-2-1"></a>
Objective‑C 3.0 uses a **language mode** (and optional conformance “strictness” levels) so that:
- existing Objective‑C remains valid without surprise behavior changes;
- new defaults (nonnull-by-default, stricter diagnostics) are only enabled when explicitly selected.

### 2.2 New syntax must map to existing concepts <a id="intro-2-2"></a>
New features should lower to:
- message sends and existing ObjC runtime rules where appropriate;
- compiler-inserted helper calls (ARC-style) where necessary;
- well-defined control-flow lowering (coroutines, defers, matches).

### 2.3 Safety is enforced where it can be proven; escape hatches are explicit <a id="intro-2-3"></a>
If the compiler can prove a property (nonnullness, non-escaping borrowed pointers, cleanup correctness), it shall enforce it in strict modes.
If it cannot, the language must provide explicit annotations/constructs (e.g., `unsafeEscapeBorrowed`, `@unsafeSendable`, `unowned`) that make risk obvious in code review.

### 2.4 “System programming” is a first-class audience <a id="intro-2-4"></a>
The language shall accommodate:
- handle-based APIs with strict cleanup requirements;
- “retainable C families” where ARC integration exists today through headers/macros;
- borrowed interior-pointer APIs that require careful lifetime management;
- performance-sensitive patterns where dynamic dispatch must be controlled.

### 2.5 Tooling is part of the spec <a id="intro-2-5"></a>
Objective‑C 3.0 is not just grammar: it includes required diagnostics, fix-its, and a conformance test suite model. A feature that cannot be audited and migrated is not “done”.

## 3. Non-goals (important) <a id="intro-3"></a>
Objective‑C 3.0 does not aim to:
- become Swift, Rust, or Go;
- introduce a global borrow checker over all code;
- remove dynamic dispatch, swizzling, forwarding, or the Objective‑C runtime;
- require a new ABI for existing frameworks.

## 4. The “big rocks” of ObjC 3.0 (high-level) <a id="intro-4"></a>
- **Types**: nonnull-by-default regions; optionals and binding; pragmatic generics; typed key paths.
- **Errors**: `Result<T, E>`, `throws`, and propagation (`?`) with bridging to NSError and return codes.
- **Concurrency**: `async/await`, structured tasks, cancellation, executors, actors, sendable-like checking.
- **System extensions**: `defer`, resource handles/cleanup contracts, retainable C families, borrowed pointers, capture lists, lifetime controls.
- **Performance**: direct methods, `final`/`sealed`, and explicit dynamic/static boundaries.
- **Boilerplate elimination**: derives, AST macros, property behaviors/wrappers.

## 5. Document organization <a id="intro-5"></a>
Each part is a separate Markdown file. The system-programming chapter ([Part 8](#part-8)) is intentionally detailed because it anchors “library-defined subset” accommodation, but it is only one part of a broader specification.

## 6. Open issues and expected iteration <a id="intro-6"></a>
This is a working draft. Each part includes “Open Issues” subsections for unresolved syntax bikeshedding and deeper ABI questions. The goal is to converge by:
- shipping implementable subsets early (nullability defaults, defer, Result/?, capture lists),
- then layering bigger features (async/await, actors, macros).

## v0.3 update <a id="intro-v0-3-update"></a>
This bundle expands [Part 3](#part-3) (types/optionals), [Part 6](#part-6) (errors), and [Part 7](#part-7) (concurrency) with concrete grammar and enforceable rules, including optional message sends (`[x? foo]`), `try`/`throw`/`do`-`catch`, and executor/actor isolation scaffolding.

## v0.4 update <a id="intro-v0-4-update"></a>
This revision makes three “ship/no‑ship” decisions:
1) Optional chaining (`?.`, `[x? foo]`) is **reference-only** in v1 (no scalar/struct chaining).
2) `throws` is **untyped** in v1 (always `id<Error>`); typed throws is deferred.
3) Task spawning remains **library-defined** in v1; no `task {}` keyword syntax (compiler recognition uses standardized attributes).


## v0.5 update <a id="intro-v0-5-update"></a>
This pass resolves:
- the exact surface spelling for executor annotations ([Part 7](#part-7)),
- the v1 policy for optional (`T?`) propagation with `?` ([Part 6](#part-6)), and
- the normative autorelease pool contract at suspension points ([Part 7](#part-7)).


## v0.8 pass focus <a id="intro-v0-8-pass-focus"></a>
This pass tightens the specification specifically for **separate compilation** and **implementability**:

- **Canonical spellings** are centralized in **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)** (attributes/pragmas that are stable for module interfaces).
- **Lowering / ABI / runtime contracts** are centralized in **[LOWERING_AND_RUNTIME_CONTRACTS.md](#c)**.
- Parts [6](#part-6)/[7](#part-7)/[9](#part-9)/[11](#part-11)/[12](#part-12) are updated to cross-reference [B](#b)/[C](#c) where semantics depend on lowering choices.

This is a deliberate step toward a spec that can be implemented in Clang/LLVM with predictable behavior across modules.

## v0.9 pass focus <a id="intro-v0-9-pass-focus"></a>
This pass makes the draft more “engineer-ready” by tightening the boundary between **source semantics** and **cross-module implementation reality**:

- Adds **[MODULE_METADATA_AND_ABI_TABLES.md](#d)**: explicit tables for what must be preserved in module metadata and emitted interfaces, and which features are ABI-affecting.
- Tightens the meaning of `await`: it is required for **any potentially suspending operation**, including cross-executor and cross-actor access (even when the callee is not explicitly `async`), aligning the surface model with implementable executor/actor hops.
- Expands C with more concrete lowering obligations where separate compilation would otherwise be ambiguous.
<!-- END INTRODUCTION.md -->

---

<!-- BEGIN DECISIONS_LOG.md -->
# Objective‑C 3.0 — Design Decisions Log (v0.10) <a id="decisions"></a>
_Last updated: 2025-12-28_

This log captures explicit “ship/no‑ship” decisions made to keep Objective‑C 3.0 **ambitious but implementable** (especially under separate compilation).

---

## D-001: Optional chaining is reference-only in v1 <a id="decisions-d-001"></a>
**Decision:** Optional member access (`?.`) and optional message sends (`[receiver? selector]`) are supported only when the accessed member/method returns:
- an Objective‑C object pointer type, or
- a block pointer type, or
- `void` (for optional message sends).

Optional chaining for scalar/struct returns is **not** supported in v1.

**Rationale:**
- Objective‑C’s historic “messaging `nil` returns 0” behavior for scalars is a major source of silent bugs.
- Supporting scalar/struct optionals well would require a value-optional ABI and conversion rules that are too large for v1.
- The safe alternative is explicit unwrapping/binding of the receiver (`if let` / `guard let`), which v1 supports ergonomically.

**Spec impact:** [Part 3](#part-3) [§3.4](#part-3-4).

---

## D-002: `throws` is untyped in v1; typed throws deferred <a id="decisions-d-002"></a>
**Decision:** The v1 `throws` effect is always **untyped**, with thrown values of type `id<Error>`.

Typed throws syntax (e.g., `throws(E)`) is reserved for future extension but is not part of v1 grammar/semantics.

**Rationale:**
- Objective‑C’s runtime dynamism and mixed-language interop (NSError, C return codes) favor a single error supertype.
- Typed throws adds significant complexity to generics, bridging, and ABI/lowering.

**Spec impact:** [Part 6](#part-6).

---

## D-003: Task spawning is library-defined in v1 (no `task {}` keyword) <a id="decisions-d-003"></a>
**Decision:** Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided via the **standard library**.

The compiler recognizes task entry points via attributes (see [D-007](#decisions-d-007) and [Part 7](#part-7)).

**Rationale:** Keeps parsing surface small; avoids freezing spawn semantics before runtime patterns stabilize.

**Spec impact:** [Part 7](#part-7) [§7.5](#part-7-5).

---

## D-004: Executor annotations — canonical spelling and meaning (v1) <a id="decisions-d-004"></a>
**Decision:** Executor affinity is expressed with the canonical spelling:

- `__attribute__((objc_executor(main)))`
- `__attribute__((objc_executor(global)))`
- `__attribute__((objc_executor(named("..."))))`

If a declaration is annotated `objc_executor(X)`, then:
- entering it from another executor requires an executor hop (typically by `await`ing the call), and
- the compiler enforces the hop requirement in strict concurrency checking mode.

**Rationale:** `__attribute__((...))` integrates into existing LLVM/Clang pipelines and is stable for module interface emission.

**Spec impact:** [Part 7](#part-7) and [B](#b)/[C](#c).

---

## D-005: Optional propagation (`T?` with postfix `?`) follows carrier rules (v1) <a id="decisions-d-005"></a>
**Decision:** Postfix propagation `e?` is allowed on an optional `e : T?` **only** when the enclosing function returns an optional type.

- In an optional-returning function, `e?` yields `T` when non-`nil`, otherwise performs `return nil;`.
- Using `e?` in a function returning `Result<…>` or `throws` is **ill‑formed** in v1 (no implicit nil→error mapping).

**Rationale:** Prevents accidental “nil becomes an error” magic; keeps control-flow explicit.

**Spec impact:** [Part 3](#part-3) and [Part 6](#part-6).

---

## D-006: Autorelease pool boundaries at suspension points (v1) <a id="decisions-d-006"></a>
**Decision:** On Objective‑C runtimes with autorelease semantics, each task *execution slice* (resume → next suspension or completion) runs inside an implicit autorelease pool that is drained:
- before suspending at an `await`, and
- when the task completes.

**Rationale:** Predictable memory behavior for Foundation-heavy code; avoids autorelease buildup across long async chains.

**Spec impact:** [Part 7](#part-7), [Part 4](#part-4), and [C](#c).

---

## D-007: Canonical spellings and interface emission are attribute/pragma-first (v1) <a id="decisions-d-007"></a>
**Decision:** Features that must survive **module interface emission** and **separate compilation** have canonical spellings defined in:

- **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)**

Sugar spellings (macros, `@`-directives, alternate attribute syntaxes) may exist, but the canonical emitted form shall use the catalog spellings.

**Rationale:** Keeps generated interfaces unambiguous and independent of user macro environments.

**Spec impact:** [B](#b), [Part 2](#part-2), [Part 12](#part-12).

---

## D-008: Generic methods are deferred in v1 <a id="decisions-d-008"></a>
**Decision:** v1 includes **generic types** (pragmatic, erased generics) but defers **generic methods/functions**.

**Rationale:**
- Generic methods create difficult interactions with Objective‑C selector syntax, method redeclaration/overload rules, and module interface printing.
- The majority of practical value on Apple platforms comes from generic container types and constrained protocols.

**Spec impact:** [Part 3](#part-3) [§3.5](#part-3-5) (generic methods moved to future extensions).

---

## D-009: `throws` uses a stable “error-out” calling convention (v1) <a id="decisions-d-009"></a>
**Decision:** v1 requires a stable ABI for `throws` that supports separate compilation. The recommended (and default) convention is a trailing error-out parameter (`outError`) of type `id<Error> _Nullable * _Nullable`.

**Rationale:** Matches long-standing Cocoa patterns (NSError-out) and is easy to lower in LLVM without stack unwinding.

**Spec impact:** C and [Part 6](#part-6).

---

## D-010: `async` lowers to coroutines scheduled by executors (v1) <a id="decisions-d-010"></a>
**Decision:** v1 `async` semantics are implemented as coroutine state machines with suspension at `await`. Resumption is scheduled by the active executor, and the runtime/stdlib exposes enough primitives to:
- create tasks,
- hop executors,
- enqueue actor-isolated work, and
- propagate cancellation.

**Rationale:** This is the most direct path to implement structured `async/await` in LLVM while preserving ARC and Objective‑C runtime behavior.

**Spec impact:** C and [Part 7](#part-7).


---

## D-011: `await` is required for any potentially suspending operation (v1) <a id="decisions-d-011"></a>
**Decision:** In v1, `await` is not restricted to “calling explicitly-async functions.”  
It is required for **any operation that may suspend**, including:

- calls to `async` functions,
- cross-executor entry into `objc_executor(X)` declarations when not proven already on `X`,
- cross-actor access to actor-isolated members when not already on the actor’s executor,
- joining tasks / iterating task-group results where suspension may occur.

`await` remains permitted only in `async` contexts.

**Rationale:** Executor and actor isolation are implemented by *hops* that may suspend even when the callee’s body is “logically synchronous.” Requiring `await` keeps suspension explicit at the call site while preserving ergonomic isolation.

**Spec impact:** [Part 7](#part-7), [C](#c), [D](#d).

---

## D-012: Required module metadata set is normative (v1) <a id="decisions-d-012"></a>
**Decision:** For ObjC 3.0 semantics to survive separate compilation, the required information enumerated in **[D](#d)** is normative: a conforming toolchain shall preserve that information in module metadata and in emitted textual interfaces.

**Rationale:** Without an explicit checklist, toolchains drift into “works in a single TU” but fails at module boundaries. [D](#d) makes the “separate compilation contract” testable.

**Spec impact:** [C](#c), [D](#d), [Part 2](#part-2), [Part 12](#part-12).
<!-- END DECISIONS_LOG.md -->

---

<!-- BEGIN ATTRIBUTE_AND_SYNTAX_CATALOG.md -->
# Objective‑C 3.0 — Attribute and Syntax Catalog <a id="b"></a>
_Working draft v0.10 — last updated 2025-12-28_

## B.0 Purpose <a id="b-0"></a>
Objective‑C 3.0 introduces new language **effects**, **annotations**, and a small amount of **surface syntax**.
To keep separate compilation reliable and to keep module interfaces stable, this document defines the **canonical spellings** that:

- all conforming implementations shall accept; and
- any interface-generation tool shall emit.

Implementations may accept additional “sugar” spellings (keywords, pragmas, macros), but those spellings are **not** required for conformance unless explicitly stated elsewhere.

## B.1 Canonical vs. optional spellings <a id="b-1"></a>

### B.1.1 Canonical spellings <a id="b-1-1"></a>
The canonical spellings use forms already widely supported by LLVM/Clang-family toolchains:

- C/ObjC pragmas: `#pragma ...`
- GNU/Clang attributes: `__attribute__((...))`
- Existing Objective‑C keywords where the feature is inherently grammatical (`async`, `await`, `try`, `throw`, etc.)

### B.1.2 Optional sugar spellings <a id="b-1-2"></a>
Implementations may additionally provide sugar spellings such as:

- `@`-directives (e.g., `@assume_nonnull_begin`) as aliases for pragmas.
- Framework macros (e.g., `NS_ASSUME_NONNULL_BEGIN`) as aliases for pragmas.
- Alternative attribute syntaxes (e.g., C++11 `[[...]]`) when compiling as ObjC++.

Such sugar spellings shall not change semantics.

## B.2 Nullability defaults <a id="b-2"></a>

### B.2.1 Nonnull-by-default region pragmas (canonical) <a id="b-2-1"></a>
Objective‑C 3.0 defines a nonnull-by-default region using:

```c
#pragma objc assume_nonnull begin
// declarations
#pragma objc assume_nonnull end
```

Semantics are defined in [Part 3](#part-3) ([§3.2.4](#part-3-2-4)).

### B.2.2 Optional aliases (non-normative) <a id="b-2-2"></a>
Implementations may treat the following as aliases with identical semantics:

- `#pragma clang assume_nonnull begin/end`
- `NS_ASSUME_NONNULL_BEGIN/NS_ASSUME_NONNULL_END` (macro-based)


## B.3 Concurrency and executors <a id="b-3"></a>

### B.3.1 Executor affinity annotation (canonical) <a id="b-3-1"></a>
Executor affinity is expressed with:

```c
__attribute__((objc_executor(main)))
__attribute__((objc_executor(global)))
__attribute__((objc_executor(named("com.example.myexecutor"))))
```

Semantics are defined in [Part 7](#part-7) ([§7.4](#part-7-4)).

### B.3.2 Task-spawn recognition attributes (canonical) <a id="b-3-2"></a>
The compiler recognizes standard-library task entry points via attributes:

```c
__attribute__((objc_task_spawn))
__attribute__((objc_task_detached))
__attribute__((objc_task_group))
```

Semantics are defined in [Part 7](#part-7) ([§7.5](#part-7-5)).

> Note: These attributes are intended to attach to **functions/methods** that take an `async` block/callback and create tasks/groups, not to arbitrary user functions.

### B.3.3 Unsafe Sendable escape hatch (canonical) <a id="b-3-3"></a>
For strict concurrency checking, implementations shall recognize an explicit escape hatch that marks a type as “Sendable-like” by programmer promise, even if the compiler cannot prove it.

Canonical attribute:

```c
__attribute__((objc_unsafe_sendable))
```

This attribute may be applied to:
- Objective‑C interface declarations (`@interface` / `@implementation`) to promise instances are safe to transfer across concurrency domains, and/or
- specific typedefs or wrapper types used for cross-task messaging.

Semantics are defined in [Part 7](#part-7) ([§7.8](#part-7-8)).

> Toolchains may also provide sugar spellings (e.g., `@unsafeSendable`), but emitted interfaces must use the canonical attribute.

### B.3.4 Actor isolation modifier: nonisolated (canonical) <a id="b-3-4"></a>
For actor types, implementations shall support a way to mark specific members as **nonisolated**.

Canonical attribute:

```c
__attribute__((objc_nonisolated))
```

This attribute may be applied to:
- Objective‑C methods and properties declared within an `actor class`.

Semantics are defined in [Part 7](#part-7) ([§7.7.4](#part-7-7-4)).

> Toolchains may additionally support a contextual keyword spelling (`nonisolated`) as sugar, but emitted interfaces should preserve semantics and may prefer the canonical attribute spelling.


## B.4 Errors <a id="b-4"></a>

### B.4.1 NSError bridging attribute (canonical) <a id="b-4-1"></a>
For interop with NSError-out-parameter conventions, implementations shall recognize:

```c
__attribute__((objc_nserror))
```

Semantics are defined in [Part 6](#part-6) ([§6.9](#part-6-9)).

### B.4.2 Status-code bridging attribute (canonical) <a id="b-4-2"></a>
For return-code APIs, implementations shall recognize:

```c
__attribute__((objc_status_code(/* parameters */)))
```

Semantics are defined in [Part 6](#part-6) ([§6.10](#part-6-10)).

## B.5 Performance and dynamism controls <a id="b-5"></a>

### B.5.1 Direct methods (canonical) <a id="b-5-1"></a>
Direct method intent is expressed with:

```c
__attribute__((objc_direct))
```

Applied to methods. (Class-wide defaults are defined in [Part 9](#part-9).)

### B.5.2 Final and sealed (canonical) <a id="b-5-2"></a>
Objective‑C 3.0 uses the following canonical spellings:

```c
__attribute__((objc_final))                  // methods or classes
__attribute__((objc_sealed))                 // classes (module-sealed)
```

If a toolchain already provides an equivalent attribute (e.g., `objc_subclassing_restricted`), it may treat that attribute as an alias.

Semantics are defined in [Part 9](#part-9).

## B.6 Metaprogramming <a id="b-6"></a>

### B.6.1 Derive / synthesize (canonical) <a id="b-6-1"></a>
Derivation requests use:

```c
__attribute__((objc_derive("TraitName")))
```

Semantics are defined in [Part 10](#part-10).

### B.6.2 Macro expansion (canonical) <a id="b-6-2"></a>
If AST macros are supported, macro entry points may be annotated with:

```c
__attribute__((objc_macro))
```

The actual macro declaration syntax is implementation-defined ([Part 10](#part-10)), but interface emission shall preserve the canonical attributes and any synthesized declarations.

## B.7 Module interface emission requirements (normative) <a id="b-7"></a>
If an implementation provides any facility that emits a textual interface for a module (e.g., generated headers, module interface stubs, API dumps), then:

1. The emitted interface shall be **semantics-preserving**: importing it must reconstruct the same declarations, effects, and attributes.
2. The emitter shall use the **canonical spellings** from this catalog (or semantically equivalent spellings defined as canonical elsewhere in the spec).
3. The emitted interface shall not depend on user macros for semantics (macros may remain for documentation convenience, but the semantic attributes/pragmas must be explicit).

## B.8 System programming extensions <a id="b-8"></a>

This section catalogs canonical spellings for “system programming extensions” described in **[Part 8](#part-8)**.
These spellings are intended to survive header/module interface emission and mixed ObjC/ObjC++ builds.

### B.8.1 Resource cleanup on scope exit (canonical) <a id="b-8-1"></a>

**Variable attribute form:**

```c
__attribute__((objc_resource(close=CloseFn, invalid=InvalidExpr)))
```

- `CloseFn` is the identifier of a cleanup function.
- `InvalidExpr` is a constant expression representing the “invalid” sentinel value for the resource type.

**Semantics:** See [Part 8](#part-8) [§8.3.2](#part-8-3-2). The compiler shall treat a variable annotated with `objc_resource` as a
*single-owner handle* for purposes of move/use-after-move diagnostics (where enabled).

**Optional sugar (non-normative):**
- `@resource(CloseFn, invalid: InvalidExpr)` as described in [Part 8](#part-8).

### B.8.2 Plain cleanup hook (canonical / existing) <a id="b-8-2"></a>

**Variable attribute form (Clang existing):**

```c
__attribute__((cleanup(CleanupFn)))
```

**Optional sugar (non-normative):**
- `@cleanup(CleanupFn)` as described in [Part 8](#part-8).

### B.8.3 Borrowed interior pointers (canonical) <a id="b-8-3"></a>

**Type qualifier form (canonical):**

```c
borrowed T *
```

The `borrowed` qualifier is a contextual keyword in type grammar ([Part 8](#part-8) [§8.7](#part-8-7)). It is not reserved as a general identifier.

### B.8.4 Function return is borrowed from an owner (canonical) <a id="b-8-4"></a>

**Function/return attribute form:**

```c
__attribute__((objc_returns_borrowed(owner_index=N)))
```

- `N` is a 0-based index into the formal parameter list that identifies the “owner” parameter.

**Semantics:** See [Part 8](#part-8) [§8.7.2](#part-8-7-2). Toolchains shall preserve this attribute in module metadata ([D Table A](#d-3-1)).

### B.8.5 Block capture list contextual keywords (canonical) <a id="b-8-5"></a>

The capture list feature in [Part 8](#part-8) uses contextual keywords:
- `weak`
- `unowned`
- `move`

These tokens are contextual within capture list grammar only and shall not be reserved globally.

## B.9 Reserved tokens <a id="b-9"></a>
Objective‑C 3.0 reserves (at minimum) the following tokens as keywords:

- `async`, `await`, `actor`
- `throws`, `try`, `throw`, `do`, `catch`
- `defer`, `guard`, `match`, `case`
- `let`, `var`

Additional reserved keywords may be added by other parts.
<!-- END ATTRIBUTE_AND_SYNTAX_CATALOG.md -->

---

<!-- BEGIN LOWERING_AND_RUNTIME_CONTRACTS.md -->
# Objective‑C 3.0 — Lowering, ABI, and Runtime Contracts <a id="c"></a>
_Working draft v0.10 — last updated 2025-12-28_

## C.0 Scope and audience <a id="c-0"></a>
This document is primarily **normative for implementations** (compiler + runtime + standard library).
It does not change source-level semantics defined elsewhere; it constrains how a conforming implementation supports those semantics under **separate compilation** and across **module boundaries**.

Where this document provides “canonical lowering” patterns, those are:
- **required** when explicitly labeled *normative*; and
- otherwise **recommended** as the default lowering for LLVM/Clang implementations.

## C.1 Definitions <a id="c-1"></a>
- **ABI**: the binary calling convention and data layout visible to linkers and other languages.
- **Runtime hooks**: functions/types provided by a support library needed to implement language semantics (e.g., executors, task scheduling).
- **Effect**: a property of a callable type that changes call/return behavior (`throws`, `async`).

## C.2 Separate compilation requirements (normative) <a id="c-2"></a>
A conforming implementation shall ensure:

1. **Effect and isolation consistency is recorded in module metadata.**  
   If a declaration is imported from a module, its:
   - `async`/`throws` effects,
   - executor affinity (`objc_executor(...)`),
   - actor isolation (actor type + isolated/nonisolated members), and
   - any dispatch-affecting attributes (`objc_direct`, `objc_sealed`, etc.)
   shall be part of the imported type information.

   The minimum required set is enumerated normatively in **[MODULE_METADATA_AND_ABI_TABLES.md](#d)**.

2. **Mismatched redeclarations are diagnosed.**  
   Redeclaring an imported function/method with a different effect set or incompatible lowering-affecting attributes is ill-formed.

3. **Interface emission is semantics-preserving.**  
   If a textual module interface is emitted, it shall preserve effects and attributes (see [B.7](#b-7)).

## C.2.1 Required module metadata (normative) <a id="c-2-1"></a>
A conforming implementation shall preserve in module metadata and interface emission the items listed in **[D.3.1](#d-3-1) [Table A](#d-3-1)**.

This requirement is intentionally testable: if importing a module can change whether a call requires `try`/`await`, or can change dispatch legality (`objc_direct`), the implementation is non-conforming.

## C.3 Canonical lowering patterns <a id="c-3"></a>

### C.3.1 Optional message send `[receiver? ...]` (normative) <a id="c-3-1"></a>
Optional message send semantics are defined in [Part 3](#part-3). Conforming implementations shall lower:

```objc
[receiver? sel:arg1 other:arg2]
```

as if by the following abstract steps:

1. Evaluate `receiver` exactly once into a temporary.
2. If the receiver temporary is `nil`, produce the “nil case” result:
   - object/block return: `nil`
   - `void` return: no effect
3. Otherwise, evaluate argument expressions in source order and perform the message send.

**Key requirement:** argument expressions shall **not** be evaluated in the `nil` case.

### C.3.2 Nil-coalescing `??` (recommended) <a id="c-3-2"></a>
`a ?? b` should lower to a single evaluation of `a` with a conditional branch.
Implementations should avoid duplicating retains/releases when `a` is an object pointer.

### C.3.3 Postfix propagation `?` (recommended) <a id="c-3-3"></a>
The postfix propagation operator in Parts 3 and 6 should lower to a structured early-exit:

- `Optional<T>` carrier: `if (!x) return nil; else use *x;`
- `Result<T,E>` carrier: `if (isErr(x)) return Err(e); else use t;`

Implementations should preserve left-to-right evaluation and should not introduce hidden temporaries with observable lifetimes beyond what ARC already requires.

## C.4 `throws` ABI and lowering <a id="c-4"></a>

### C.4.1 Canonical ABI shape for throwing functions (normative for implementations) <a id="c-4-1"></a>
For a function declared:

```c
R f(A1 a1, A2 a2) throws;
```

a conforming implementation shall provide a stable calling convention that allows the caller to receive either:
- a normal return of type `R`, or
- an error value of type `id<Error>`.

**Canonical ABI (recommended and permitted as normative):** the implementation uses an additional trailing *error-out parameter*:

```c
R f(A1 a1, A2 a2, id<Error> _Nullable * _Nullable outError);
```

Rules:
- On success: if `outError != NULL`, store `nil` into `*outError`, then return the value.
- On error: if `outError != NULL`, store the thrown error into `*outError`, then return an unspecified value of type `R` (or zero-initialized as QoI).

> Note: For `void` return, the return value is omitted and the outError store alone indicates failure.

An implementation may choose a different ABI shape, but only if it is:
- stable under separate compilation, and
- representable in module metadata so importers use the same ABI.

### C.4.2 Throwing Objective‑C methods (recommended) <a id="c-4-2"></a>
For Objective‑C methods, the same “trailing error-out parameter” approach is recommended: the selector’s parameter list includes the implicit trailing outError at the ABI level.
The language surface does not expose that parameter; it is introduced/consumed by the compiler as part of the `throws` effect.

### C.4.3 Lowering of `try` (recommended) <a id="c-4-3"></a>

### C.4.4 Objective‑C selectors vs ABI signatures (normative intent) <a id="c-4-4"></a>
For Objective‑C methods, the `throws` effect is **not** part of the selector spelling. The selector used for lookup remains the source-level selector.

However, the *call ABI* of the method’s implementation may include the canonical trailing error-out parameter described in [C.4.1](#c-4-1).

A conforming implementation shall ensure that:
- when a call site is type-checked as calling a `throws` method, the generated call passes the error-out parameter in the canonical position required by that implementation’s ABI; and
- when forming or calling an `IMP` for a `throws` method (e.g., via `methodForSelector:`), the compiler uses a function pointer type whose parameter list includes the error-out parameter.

**Reflection note (non-normative):** baseline Objective‑C runtime type-encoding strings do not represent `throws`. This draft does not require extending the runtime type encoding in v1. Toolchains may provide extended metadata for reflective invocation as an extension.


A `try` call should lower to:
- allocate a local `id<Error> err = nil;`
- perform the call with `&err`
- if `err != nil`, branch to the enclosing error propagation/handler path.

`try?` and `try!` should be implemented as specified in [Part 6](#part-6), using the same underlying error value.

## C.5 `async` ABI and lowering <a id="c-5"></a>

### C.5.1 Coroutine model (normative for implementations) <a id="c-5-1"></a>
A conforming implementation shall implement `async` functions such that:
- each `await` is a potential suspension point,
- local variables required after an `await` have stable storage across suspension,
- ARC + cleanup semantics are preserved across suspension (see Parts 4 and 7).

**Recommended lowering:** lower `async` functions to LLVM coroutine state machines (e.g., using LLVM’s coroutine intrinsics), with resumption scheduled by the active executor.

### C.5.2 Task context (normative) <a id="c-5-2"></a>
While executing an `async` function, there exists a *current task context* that provides at least:
- cancellation state,
- current executor identity,
- and (if actors are used) current actor isolation context.

The concrete representation is implementation-defined, but the values must be queryable by the standard library and usable by the runtime to enforce executor hops.

### C.5.3 Executor hops (recommended) <a id="c-5-3"></a>
When entering a declaration annotated with `objc_executor(X)`, implementations should:
- check whether the current executor is `X`;
- if not, suspend and schedule resumption on `X`.

Implementations should provide (directly or via the standard library) a primitive equivalent to:

- `await ExecutorHop(X)`

so that the compiler can lower hops in a uniform way.


### C.5.4 Call-site lowering for isolation boundaries (normative intent) <a id="c-5-4"></a>
[Part 7](#part-7) defines `await` as the marker for **potential suspension** ([Decision D-011](#decisions-d-011)), not merely “calling explicitly-async functions.”

A conforming implementation shall treat the following as potentially-suspending at call sites when the relevant metadata indicates an isolation boundary is being crossed:

- entering an `objc_executor(X)` declaration from code not proven to already be on executor `X`;
- entering an actor-isolated member from outside that actor’s executor.

**Recommended lowering pattern (informative):**
1. If already on the required executor, call directly.
2. Otherwise:
   - suspend the current task,
   - enqueue the continuation onto the required executor (or actor executor),
   - resume and perform the call,
   - then continue.

This may be implemented either as:
- an implicit compiler-inserted hop around the call (still requiring `await` in the source), or
- a lowering through an explicit hop primitive plus a direct call.

The observable requirement is: the call **may suspend**, and the work executes on the correct executor.


## C.6 Actors: runtime representation and dispatch <a id="c-6"></a>

### C.6.1 Actor executor (normative) <a id="c-6-1"></a>
Each actor instance shall be associated with a serial executor used to protect isolated mutable state.
Cross-actor access requires enqueueing work onto that executor.

### C.6.2 Actor method entry (recommended) <a id="c-6-2"></a>
A non-reentrant actor method entry should:
- hop to the actor’s executor (if not already on it),
- then execute until the next suspension point,
- preserving the actor’s invariants.

Reentrancy behavior is defined by [Part 7](#part-7); this document only constrains that runtime scheduling is consistent with that behavior.

## C.7 Autorelease pools at suspension points (normative for ObjC runtimes) <a id="c-7"></a>
On Objective‑C runtimes with autorelease semantics, implementations shall ensure:

- Each *task execution slice* (from resume to next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule exists to make async code’s memory behavior predictable for Foundation-heavy code.

## C.8 Direct/final/sealed lowering (recommended) <a id="c-8"></a>
- `objc_direct` methods should lower to direct function calls when statically referenced, and may omit dynamic method table entries as permitted by [Part 9](#part-9).
- `objc_final` should enable devirtualization and dispatch optimization but shall not change observable message lookup semantics unless paired with `objc_direct`.
- `objc_sealed` should enable whole-module reasoning about subclass sets; implementations should treat cross-module subclassing attempts as ill-formed or diagnose at link time where possible.

## C.9 Macro/derive expansion and ABI (normative for implementations) <a id="c-9"></a>
If macro expansion or derives synthesize declarations that affect layout or vtables/dispatch surfaces (including generated ivars, properties, methods), then:
- the synthesis must occur before ABI layout is finalized for the type;
- the synthesized declarations must appear in any emitted module interface or API dump.

## C.10 System programming analysis hooks (recommended) <a id="c-10"></a>
This section collects implementation guidance for **[Part 8](#part-8)** features that are primarily enforced by diagnostics and analysis rather than runtime hooks.

### C.10.1 Resources and cleanup <a id="c-10-1"></a>
`objc_resource(...)` ([Part 8](#part-8)) should lower equivalently to a scope-exit cleanup action (similar to `cleanup(...)`), but implementations should additionally:
- perform use-after-move and double-close diagnostics in strict-system profiles;
- treat the “invalid sentinel” as the moved-from / reset state.

### C.10.2 Borrowed pointers <a id="c-10-2"></a>
`borrowed T *` and `objc_returns_borrowed(owner_index=...)` ([Part 8](#part-8)) should be represented in the AST and module metadata ([D Table A](#d-3-1)) so that:
- call sites can enforce escape analysis in strict-system profiles;
- importers preserve the qualifier for downstream tooling.

No runtime support is required by these features in v1; the contract is primarily compile-time diagnostics and toolability.

## C.11 Conformance tests (non-normative) <a id="c-11"></a>
Implementations are encouraged to provide a conformance suite that includes:
- effect mismatch diagnostics across modules,
- correct argument evaluation semantics for optional sends,
- `throws` propagation behavior in nested `do/catch`,
- executor hop correctness and actor isolation enforcement across module boundaries.
<!-- END LOWERING_AND_RUNTIME_CONTRACTS.md -->

---

<!-- BEGIN MODULE_METADATA_AND_ABI_TABLES.md -->
# Objective‑C 3.0 — Module Metadata and ABI Surface Tables <a id="d"></a>
_Working draft v0.10 — last updated 2025-12-28_

## D.0 Purpose <a id="d-0"></a>
Objective‑C 3.0 adds source-level semantics that **must survive module boundaries**:
effects (`async`, `throws`), nullability defaults, executor/actor isolation, and dispatch controls (`objc_direct`, `objc_sealed`, etc.).

This document is a **normative checklist** for what a conforming implementation must preserve in:
- module metadata (binary or AST-based), and
- any emitted textual interface ([B.7](#b-7)).

It also summarizes which features are **ABI-affecting** and what ABI stability constraints apply.

> This document does not mandate a specific on-disk format. It defines *required information*.

## D.1 Terminology <a id="d-1"></a>
- **Module metadata**: any compiled representation that an importer uses instead of re-parsing raw headers (e.g., Clang module files, serialized AST, interface stubs).
- **Textual interface**: a tool-emitted, importable text representation intended to preserve semantics for distribution ([B.7](#b-7)).
- **ABI-affecting**: changes the calling convention, symbol shape, layout, or runtime dispatch surface such that mismatches across translation units could cause miscompilation or runtime faults.

## D.2 Module metadata requirements (normative) <a id="d-2"></a>
A conforming implementation shall preserve, for all exported declarations:

1. **Full type information** including:
   - parameter/return types,
   - nullability qualifiers and defaults ([Part 3](#part-3)),
   - ownership qualifiers and ARC-related attributes ([Part 4](#part-4)),
   - pragmatic generics information ([Part 3](#part-3)) sufficient for type checking (even if erased at ABI).

2. **Effect information**:
   - whether the callable is `throws`,
   - whether the callable is `async`,
   - and whether the callable is *potentially suspending at call sites* due to isolation ([Part 7](#part-7)).

3. **Isolation and scheduling metadata**:
   - executor affinity (`objc_executor(...)`) ([Part 7](#part-7)),
   - actor isolation (actor type, isolated vs nonisolated members) ([Part 7](#part-7)),
   - Sendable-like constraints for cross-task/actor boundaries ([Part 7](#part-7)).

4. **Dispatch and dynamism controls**:
   - `objc_direct`, `objc_final`, `objc_sealed` ([Part 9](#part-9)),
   - any attributes that change call legality or override/subclass legality.

5. **Interop bridging markers**:
   - `objc_nserror` and `objc_status_code(...)` ([Part 6](#part-6)),
   - any language-defined overlays/bridges that affect call lowering.

6. **Provenance**:
   - the owning module of each exported declaration ([Part 2](#part-2)),
   - API partition membership (public/SPI/private) if the toolchain supports it ([Part 2](#part-2)).

## D.3 Required metadata tables <a id="d-3"></a>

### D.3.1 Table A — “must preserve across module boundaries” <a id="d-3-1"></a>

| Feature | Applies to | Must be recorded in module metadata | Must be emitted in textual interface | Notes |
|---|---|---:|---:|---|
| Nullability qualifiers (`_Nullable`, `_Nonnull`) | types, params, returns, properties | ✅ | ✅ | Includes inferred defaults where part of the public contract. |
| Nonnull-by-default regions | headers / interface units | ✅ | ✅ | Canonical pragmas from [B.2](#b-2). |
| Pragmatic generics parameters | class/protocol types, collections | ✅ | ✅ | ABI may erase; type checking must not. |
| `T?`, `T!` (optional spellings) | type surface | ✅ | ✅ | Emitted form may choose canonical spellings but semantics must match. |
| `throws` effect | functions/methods/blocks | ✅ | ✅ | ABI-affecting (see [Table B](#d-3-2)). |
| `async` effect | functions/methods/blocks | ✅ | ✅ | ABI-affecting (see [Table B](#d-3-2)). |
| Executor affinity `objc_executor(...)` | funcs/methods/types | ✅ | ✅ | May imply call-site `await` when crossing executors ([Part 7](#part-7)). |
| Actor type / actor isolation | actor classes + members | ✅ | ✅ | Includes nonisolated markings. |
| Sendable-like constraints | types, captures, params/returns | ✅ | ✅ | Importers must be able to enforce strict checks. |
| Borrowed pointer qualifier (`borrowed T *`) | types (params/returns) | ✅ | ✅ | [Part 8](#part-8); enables escape diagnostics in strict-system. Importers must preserve the qualifier. |
| Borrowed-return marker (`objc_returns_borrowed(owner_index=N)`) | functions/methods | ✅ | ✅ | [Part 8](#part-8); identifies the owner parameter for lifetime checking of interior pointers. |
| Task-spawn recognition (`objc_task_spawn` etc.) | stdlib entry points | ✅ | ✅ | Needed for compiler enforcement at call sites. |
| Direct method `objc_direct` | methods | ✅ | ✅ | Changes call legality and category interactions. |
| Final `objc_final` | classes/methods | ✅ | ✅ | Optimization + legality constraints. |
| Sealed `objc_sealed` | classes | ✅ | ✅ | Cross-module subclass legality. |
| Derive/macro synthesized declarations | types/members | ✅ | ✅ | Synthesized members must be visible to importers before layout/ABI decisions. |
| Error bridging markers (`objc_nserror`, `objc_status_code`) | funcs/methods | ✅ | ✅ | Affects lowering of `try` and wrappers. |
| Availability / platform gates | all exported decls | ✅ | ✅ | Not a new ObjC 3.0 feature, but required for correctness. |

### D.3.2 Table B — ABI boundary summary (v1) <a id="d-3-2"></a>

| Feature | ABI impact | Required stability property | Canonical/recommended ABI shape |
|---|---|---|---|
| `throws` | **Yes** | Caller and callee must agree on calling convention across modules | Recommended: trailing `id<Error> * _Nullable outError` ([C.4](#c-4)). |
| `async` | **Yes** | Importers must call using the same coroutine/continuation ABI | Recommended: LLVM coroutine lowering ([C.5](#c-5)), with a task/executor context. |
| Executor/actor isolation | Usually **no** (call-site scheduling) | Metadata must exist so importer inserts hops and requires `await` | No ABI change required for sync bodies; hop is an `await`ed scheduling operation. |
| Optional chaining/sends | No | Semantics preserved in caller IR | Lower to conditional receiver check; args not evaluated on nil ([C.3.1](#c-3-1)). |
| Direct methods | Sometimes (dispatch surface) | Importers must know legality + dispatch mode | Recommended: direct symbol call + omit dynamic lookup where permitted ([C.8](#c-8)). |
| Generics (pragmatic) | No (erased) | Importers must type-check consistently | Erased in ABI; preserved in metadata/interface. |
| Key paths | Library ABI | Standard library must define stable representation | ABI governed by stdlib; metadata must preserve `KeyPath<Root,Value>` types. |

### D.3.3 Table C — Runtime hooks (minimum expectations) <a id="d-3-3"></a>
This table names **conceptual hooks**. Implementations may use different symbol names.

| Area | Minimum required capability | Notes |
|---|---|---|
| Executors | Enqueue continuation onto an executor; query “current executor” | Needed for `objc_executor(...)` and for `await` resumption. |
| Tasks | Create child/detached tasks; join; cancellation state | Standard library provides API; compiler enforces via attributes. |
| Actors | Each actor instance maps to a serial executor; enqueue isolated work | Representation is implementation-defined (inline field or side table). |
| Autorelease pools | Push/pop implicit pools around execution slices | Normative on ObjC runtimes ([C.7](#c-7)). |
| Diagnostics metadata | Preserve enough source mapping for async/macro debugging | [Part 12](#part-12) requires debuggability. |

## D.4 Conformance tests (minimum) <a id="d-4"></a>
A conforming implementation’s test suite should include:

- Importing a module and verifying that `async`/`throws` effects survive and mismatches are diagnosed.
- Importing executor-annotated APIs and verifying cross-executor calls require `await` in strict concurrency mode.
- Importing actor APIs and verifying cross-actor access requires `await` and preserves isolation.
- Verifying that emitted textual interfaces preserve canonical spellings ([B](#b)) and round-trip semantics.
<!-- END MODULE_METADATA_AND_ABI_TABLES.md -->

---

<!-- BEGIN CONFORMANCE_PROFILE_CHECKLIST.md -->
# Objective‑C 3.0 — Conformance Profile Checklist <a id="e"></a>
_Working draft v0.10 — last updated 2025-12-28_

This document defines **conformance profiles** for Objective‑C 3.0 v1 and provides a **checklist** for compiler/toolchain implementers (and, where relevant, SDK authors) to make conformance claims concrete and testable.

It is intentionally redundant with the rest of the specification: the goal is to let an implementer answer “Do we actually implement ObjC 3.0?” without reading every part end-to-end.

## E.1 How to use this checklist <a id="e-1"></a>

### E.1.1 Conformance is a claim about a toolchain, not just a compiler <a id="e-1-1"></a>
A conformance claim applies to the **toolchain bundle**:

- frontend (parser + type checker),
- code generator and optimizer,
- runtime support (Objective‑C runtime + concurrency runtime hooks where applicable),
- standard libraries/modules required by the language features,
- interface emission + module metadata support,
- diagnostics and migration tooling requirements.

### E.1.2 Tags <a id="e-1-2"></a>
Checklist items are annotated with tags indicating which conformance profile requires them.

- **[CORE]** required for **ObjC 3.0 v1 Core**
- **[STRICT]** required for **ObjC 3.0 v1 Strict** (in addition to CORE)
- **[CONC]** required for **ObjC 3.0 v1 Strict Concurrency** (in addition to STRICT)
- **[SYSTEM]** required for **ObjC 3.0 v1 Strict System** (in addition to STRICT, and typically CONC)

Optional feature sets:

- **[OPT-META]** metaprogramming feature set ([Part 10](#part-10))
- **[OPT-SWIFT]** Swift interop feature set ([Part 11](#part-11), Swift-facing portions)
- **[OPT-CXX]** C++-enhanced interop feature set ([Part 11](#part-11), C++-specific portions)

### E.1.3 What it means to “pass” <a id="e-1-3"></a>
A toolchain **passes** a profile when it satisfies *all* checklist items tagged for that profile, and when it can demonstrate (via tests) that required module metadata and runtime contracts behave correctly under **separate compilation**.

## E.2 Profile definitions (normative) <a id="e-2"></a>

### E.2.1 ObjC 3.0 v1 Core <a id="e-2-1"></a>
A toolchain claiming **ObjC 3.0 v1 Core** shall:

- implement the language mode selection and feature-test mechanisms ([Part 1](#part-1)),
- implement modules/interface emission and required metadata preservation ([Part 2](#part-2), [B](#b), [D](#d)),
- implement the v1 type safety surface: nullability defaults, optionals, pragmatic generics, key paths as specified ([Part 3](#part-3)),
- implement memory-management and ARC interactions required by the above ([Part 4](#part-4)),
- implement the control-flow constructs that are part of the v1 surface ([Part 5](#part-5)),
- implement the error model (`throws`, `try`, bridging) ([Part 6](#part-6), [C](#c)),
- implement the concurrency model (`async/await`, executors, cancellation, actors) sufficiently to compile and run correct programs ([Part 7](#part-7), [C](#c)).

“Core” does **not** require the optional feature sets (metaprogramming macros, Swift overlays) unless separately claimed.

### E.2.2 ObjC 3.0 v1 Strict <a id="e-2-2"></a>
A toolchain claiming **ObjC 3.0 v1 Strict** shall:

- support strictness selection ([Part 1](#part-1)) and treat “strict-ill‑formed” constructs as errors,
- provide the required diagnostics and fix-its for migration ([Part 12](#part-12)),
- ensure canonical spellings are emitted in textual interfaces ([B](#b), [Part 2](#part-2), [Part 12](#part-12)).

### E.2.3 ObjC 3.0 v1 Strict Concurrency <a id="e-2-3"></a>
A toolchain claiming **ObjC 3.0 v1 Strict Concurrency** shall:

- support strict concurrency checking selection ([Part 1](#part-1)),
- enforce actor isolation, executor hops, and Sendable-like rules as specified ([Part 7](#part-7)),
- provide diagnostics for common data-race hazards and isolation violations ([Part 12](#part-12)),
- preserve concurrency-relevant metadata across modules ([D Table A](#d-3-1)).

### E.2.4 ObjC 3.0 v1 Strict System <a id="e-2-4"></a>
A toolchain claiming **ObjC 3.0 v1 Strict System** shall:

- support strict-system strictness selection ([Part 1](#part-1)),
- implement the system-programming extensions in [Part 8](#part-8) (resources, borrowed pointers, capture lists) and their associated diagnostics,
- preserve borrowed/lifetime annotations across modules ([B.8](#b-8), [D Table A](#d-3-1)),
- provide at least intra-procedural enforcement for borrowed-pointer escape analysis and resource use-after-move patterns ([Part 8](#part-8), [Part 12](#part-12)).

“Strict System” is intended for SDKs and codebases like IOKit, DriverKit, and other “handle heavy” or ownership-sensitive domains.

## E.3 Checklist <a id="e-3"></a>

### E.3.1 Language mode selection, feature tests, and versioning <a id="e-3-1"></a>
- [ ] **[CORE]** Provide a language mode selection mechanism equivalent to `-fobjc-version=3`. ([Part 1](#part-1) [§1.2](#part-1-2))
- [ ] **[CORE]** Provide a source-level mechanism (pragma or directive) to set ObjC 3.0 mode for a translation unit, or document that only the flag form is supported. ([Part 1](#part-1))
- [ ] **[CORE]** Provide feature test macros for major feature groups (optionals, throws, async/await, actors, direct/final/sealed, derives/macros if implemented). ([Part 1](#part-1) [§1.4](#part-1-4))
- [ ] **[STRICT]** Provide strictness selection equivalent to `-fobjc3-strictness=permissive|strict|strict-system`. ([Part 1](#part-1) [§1.5](#part-1-5))
- [ ] **[CONC]** Provide concurrency checking selection equivalent to `-fobjc3-concurrency=strict|off`. ([Part 1](#part-1) [§1.6](#part-1-6))

### E.3.2 Modules, namespacing, and interface emission <a id="e-3-2"></a>
- [ ] **[CORE]** Implement module-aware compilation for Objective‑C declarations sufficient to support stable import, name lookup, and diagnostics. ([Part 2](#part-2))
- [ ] **[CORE]** Provide a textual interface emission mode (or equivalent) that can be used for verification in CI. ([Part 2](#part-2), [Part 12](#part-12))
- [ ] **[CORE]** Emit **canonical spellings** for ObjC 3.0 features in textual interfaces, per [B](#b). ([B.1](#b-1), [B.7](#b-7); [Part 2](#part-2))
- [ ] **[CORE]** Preserve and record all “must preserve” metadata in [D Table A](#d-3-1). ([D.3.1](#d-3-1))
- [ ] **[STRICT]** Provide an interface verification mode that detects mismatch between compiled module metadata and emitted textual interface. ([Part 12](#part-12))

### E.3.3 Type system: nullability defaults, optionals, generics, key paths <a id="e-3-3"></a>
- [ ] **[CORE]** Support nullability qualifiers and treat them as part of the type system in ObjC 3.0 mode. ([Part 3](#part-3))
- [ ] **[CORE]** Support nonnull-by-default regions with canonical pragma spellings ([B.2](#b-2)). ([Part 3](#part-3), [B.2](#b-2))
- [ ] **[STRICT]** Diagnose missing nullability where required by strictness level; provide fix-its. ([Part 12](#part-12))
- [ ] **[CORE]** Support optional types `T?` and IUO `T!` where specified, including:
  - optional binding (`if let`, `guard let`) ([Part 3](#part-3), [Part 5](#part-5)),
  - optional chaining / optional message send `[receiver? sel]` ([Part 3](#part-3), [C.3.1](#c-3-1)),
  - postfix propagation `expr?` per carrier rules ([Part 3](#part-3); [Part 6](#part-6); [C.3.3](#c-3-3)).
- [ ] **[CORE]** Enforce v1 restriction that optional chaining is reference-only for member returns ([D-001](#decisions-d-001); [Part 3](#part-3)).
- [ ] **[CORE]** Preserve optional and nullability information in module metadata and textual interfaces ([D Table A](#d-3-1)).
- [ ] **[CORE]** Support pragmatic generics on types (erased at runtime but checked by compiler) as specified. ([Part 3](#part-3))
- [ ] **[CORE]** Defer generic methods/functions (do not implement, or gate behind an extension flag) per [D-008](#decisions-d-008). ([Part 3](#part-3); Decisions Log)
- [ ] **[CORE]** Support key path literal and typing rules to the extent specified by [Part 3](#part-3), or clearly mark as unsupported if still provisional and do not claim the feature macro. ([Part 3](#part-3))

### E.3.4 Memory management and lifetime <a id="e-3-4"></a>
- [ ] **[CORE]** Preserve ObjC ARC semantics and ensure new language features lower without violating ARC rules. ([Part 4](#part-4))
- [ ] **[CORE]** Implement the suspension-point autorelease pool contract ([D-006](#decisions-d-006); [C.7](#c-7); [Part 7](#part-7)). This includes:
  - creating an implicit pool for each async “slice” as specified,
  - draining it at each suspension point.
- [ ] **[STRICT]** Provide diagnostics for suspicious lifetime extensions, escaping of stack-bound resources, and unsafe bridging where specified. ([Part 4](#part-4), [Part 12](#part-12))

### E.3.5 Control flow and safety constructs <a id="e-3-5"></a>
- [ ] **[CORE]** Implement `defer` with LIFO scope-exit semantics ([Part 5](#part-5); [Part 8](#part-8) [§8.2](#part-8-2)).
- [ ] **[CORE]** Ensure `defer` executes on normal exit and stack unwinding exits (where applicable) as specified. ([Part 5](#part-5)/8)
- [ ] **[CORE]** Implement `guard` and refinement rules for optionals/patterns. ([Part 5](#part-5))
- [ ] **[CORE]** Implement `match` (pattern matching) to the extent specified, or clearly mark as provisional and do not claim a feature macro if not implemented. ([Part 5](#part-5))
- [ ] **[STRICT]** Diagnose illegal non-local exits from `defer` bodies ([Part 5](#part-5)/8) and other ill-formed constructs.

### E.3.6 Errors and `throws` <a id="e-3-6"></a>
- [ ] **[CORE]** Implement untyped `throws` as the v1 model ([D-002](#decisions-d-002); [Part 6](#part-6)).
- [ ] **[CORE]** Provide a stable ABI/lowering model for `throws` ([D-009](#decisions-d-009); [C.4](#c-4); [D Table B](#d-3-2)).
- [ ] **[CORE]** Support `try`, `do/catch`, and propagation rules that integrate with optionals as specified. ([Part 6](#part-6))
- [ ] **[CORE]** Support NSError/status-code bridging attributes where specified ([B.4](#b-4); [Part 6](#part-6)), including module metadata preservation ([D Table A](#d-3-1)).
- [ ] **[STRICT]** Provide diagnostics for ignored errors, missing `try`, and invalid bridging patterns. ([Part 12](#part-12))

### E.3.7 Concurrency: `async/await`, executors, cancellation, actors <a id="e-3-7"></a>
- [ ] **[CORE]** Implement `async` and `await` grammar and typing rules as specified. ([Part 7](#part-7))
- [ ] **[CORE]** Implement a coroutine-based lowering model for `async` ([D-010](#decisions-d-010); [C.5](#c-5)) and preserve required ABI metadata ([D Table B](#d-3-2)).
- [ ] **[CORE]** Implement cancellation propagation and task-context behavior as specified. ([Part 7](#part-7); [C.5.2](#c-5-2))
- [ ] **[CORE]** Implement executor affinity annotations and call-site behavior:
  - accept canonical `objc_executor(...)` spellings ([B.3.1](#b-3-1)),
  - preserve in module metadata ([D Table A](#d-3-1)),
  - perform required hops as specified. ([Part 7](#part-7); [C.5.3](#c-5-3))
- [ ] **[CORE]** Implement actor declarations and actor isolation rules ([Part 7](#part-7); [C.6](#c-6)).
- [ ] **[STRICT]** Provide diagnostics for isolation violations, invalid executor annotations, and suspicious cross-actor calls. ([Part 12](#part-12))
- [ ] **[CONC]** Enforce Sendable-like constraints for:
  - captured values in `async` blocks/tasks,
  - parameters/returns across actor boundaries,
  - values crossing executor domains. ([Part 7](#part-7); [D Table A](#d-3-1))
- [ ] **[CONC]** Enforce [D-011](#decisions-d-011): require `await` for any potentially-suspending operation, not only explicit `async` calls. (Decisions Log; [Part 7](#part-7))

### E.3.8 System programming extensions (Part 8) <a id="e-3-8"></a>
- [ ] **[SYSTEM]** Support canonical attribute spellings for [Part 8](#part-8) features ([B.8](#b-8)), including:
  - `objc_resource(close=..., invalid=...)`,
  - `objc_returns_borrowed(owner_index=...)`,
  - `borrowed T *` type qualifier,
  - capture list contextual keywords.
- [ ] **[SYSTEM]** Implement resource cleanup semantics and associated diagnostics ([Part 8](#part-8) [§8.3](#part-8-3); [Part 12](#part-12) [§12.3.6](#part-12-3-6)).
- [ ] **[SYSTEM]** Implement `withLifetime` / `keepAlive` semantics and ensure they interact correctly with ARC. ([Part 8](#part-8) [§8.6](#part-8-6))
- [ ] **[SYSTEM]** Implement borrowed pointer rules and diagnostics ([Part 8](#part-8) [§8.7](#part-8-7)) at least intra-procedurally.
- [ ] **[SYSTEM]** Preserve borrowed/lifetime annotations in module metadata ([D Table A](#d-3-1)).
- [ ] **[SYSTEM]** Implement capture lists ([Part 8](#part-8) [§8.8](#part-8-8)) with required evaluation order and move/weak/unowned semantics.
- [ ] **[SYSTEM]** Provide diagnostics for:
  - borrowed escape to heap/global/ivar,
  - borrowed escape into `@escaping` blocks,
  - use-after-move for resources,
  - dangerous `unowned` captures. ([Part 12](#part-12))

### E.3.9 Performance and dynamism controls <a id="e-3-9"></a>
- [ ] **[CORE]** Accept and preserve canonical spellings for `objc_direct`, `objc_final`, `objc_sealed` ([B.5](#b-5); [Part 9](#part-9)).
- [ ] **[CORE]** Enforce legality rules across categories/extensions and module boundaries as specified. ([Part 9](#part-9); [C.8](#c-8); [D](#d))
- [ ] **[STRICT]** Provide diagnostics for calling direct methods via dynamic dispatch, illegal overrides of final/sealed, and related misuse. ([Part 12](#part-12))

### E.3.10 Metaprogramming (optional feature set) <a id="e-3-10"></a>
- [ ] **[OPT-META]** Implement derives (`objc_derive(...)`) with deterministic, tool-visible expansion. ([Part 10](#part-10); [B.6.1](#b-6-1))
- [ ] **[OPT-META]** Implement macros (`objc_macro(...)`) with sandboxing / safety constraints as specified. ([Part 10](#part-10); [B.6.2](#b-6-2))
- [ ] **[OPT-META]** Preserve macro/derive expansions in module metadata and textual interfaces as required by D. ([D Table A](#d-3-1))

### E.3.11 Interoperability (optional feature sets) <a id="e-3-11"></a>
- [ ] **[CORE]** Maintain full interop with C and Objective‑C runtime behavior. (Baseline + [Part 11](#part-11))
- [ ] **[OPT-CXX]** Document and test ObjC++ interactions for ownership, `throws`, and `async` lowering. ([Part 11](#part-11); C)
- [ ] **[OPT-SWIFT]** Provide a Swift interop story (import/export) for:
  - optionals/nullability,
  - `throws` bridging,
  - `async/await` bridging,
  - actors and isolation metadata. ([Part 11](#part-11))

### E.3.12 Diagnostics, tooling, and tests <a id="e-3-12"></a>
- [ ] **[CORE]** Implement the minimum diagnostic groups in [Part 12](#part-12):
  - nullability/optionals,
  - throws,
  - concurrency,
  - modules/interface emission,
  - performance controls. ([Part 12](#part-12) [§12.3](#part-12-3))
- [ ] **[STRICT]** Provide fix-its and migrator support to move legacy code toward canonical spellings and safer idioms. ([Part 12](#part-12) [§12.4](#part-12-4))
- [ ] **[CORE]** Provide or publish a conformance test suite covering:
  - parsing/grammar,
  - type system and diagnostics,
  - dynamic semantics,
  - runtime contracts (throws/async lowering),
  - metadata/interface preservation. ([Part 12](#part-12) [§12.5](#part-12-5))
- [ ] **[SYSTEM]** Include tests for borrowed-pointer escape diagnostics and resource cleanup semantics. ([Part 8](#part-8); [Part 12](#part-12))

## E.4 Recommended evidence for a conformance claim (non-normative) <a id="e-4"></a>
A serious conformance claim should ship with:

- a public “conformance manifest” listing enabled profiles and feature-test macros,
- CI proofs that:
  - module interfaces round-trip (emit → import) without semantic loss,
  - [D Table A](#d-3-1) metadata is preserved under separate compilation,
  - runtime contracts for `throws` and `async` behave correctly under optimization,
- migration tooling notes for large codebases (warning groups, fix-its, staged adoption).
<!-- END CONFORMANCE_PROFILE_CHECKLIST.md -->

---

<!-- BEGIN PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md -->
# Part 0 — Baseline and Normative References <a id="part-0"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 0.1 Purpose <a id="part-0-1"></a>
This part establishes:

1. The baseline language and de‑facto dialect that Objective‑C 3.0 builds upon.
2. How this specification relates to existing compiler specifications and platform ABIs.
3. Common terms and definitions used throughout the specification.
4. The interpretation of normative language (“shall”, “should”, etc.).

Objective‑C 3.0 is defined as a *language mode* that extends a stable baseline. The baseline is the Objective‑C language as implemented by a conforming compiler, including widely deployed features (ARC, blocks, modules, nullability annotations, direct methods, etc.).

## 0.2 Normative reference model <a id="part-0-2"></a>
A specification may reference external documents in two ways:

- **Normative reference**: the referenced document defines required behavior as if included here.
- **Informative reference**: explanatory or historical; does not define requirements.

This draft treats certain de‑facto specifications as normative because they are widely implemented and relied upon for ABI correctness:

- The Objective‑C ARC rules as implemented by Clang-family compilers (normative for ARC insertion, qualifiers, and ARC optimizations).
- The Blocks language specification / block ABI (normative for block representation and calling convention).
- Objective‑C runtime message sending semantics and object model (normative for dynamic dispatch behavior).

Where this draft extends or constrains baseline behavior, it does so only in ObjC 3.0 mode ([Part 1](#part-1)).

> Note: This draft intentionally avoids rewriting existing ARC/Blocks specs verbatim; it specifies extensions and constraints needed by ObjC 3.0 features.

## 0.3 Normative terms <a id="part-0-3"></a>
The terms **shall**, **must**, **shall not**, **must not**, **should**, and **may** are to be interpreted as in RFC-style standards documents.

## 0.4 Definitions and shared terminology <a id="part-0-4"></a>

### 0.4.1 Translation unit <a id="part-0-4-1"></a>
A *translation unit* is a single source file after preprocessing and module import processing, compiled as one unit.

### 0.4.2 Objective‑C 3.0 mode <a id="part-0-4-2"></a>
*Objective‑C 3.0 mode* is the compilation mode in which the grammar, defaults, and diagnostics of Objective‑C 3.0 are enabled ([Part 1](#part-1)).

### 0.4.3 Conformance level <a id="part-0-4-3"></a>
A *conformance level* is a set of additional requirements and diagnostics applied on top of ObjC 3.0 mode ([Part 1](#part-1)).

### 0.4.4 Retainable pointer type <a id="part-0-4-4"></a>
A *retainable pointer type* is a type for which ARC may insert retain/release operations. This includes:
- Objective‑C object pointers (`id`, `Class`, `NSObject *`, etc.),
- block pointers,
- additional “retainable families” declared by annotations ([Part 8](#part-8)).

### 0.4.5 Object pointer type <a id="part-0-4-5"></a>
An *object pointer type* is a pointer to an Objective‑C object, including `id` and `Class`. In this draft, “object pointer” excludes CoreFoundation “toll-free bridged” pointers unless explicitly annotated as retainable families ([Part 8](#part-8)).

### 0.4.6 Block pointer type <a id="part-0-4-6"></a>
A *block pointer type* is a pointer to a block literal/closure as defined by the Blocks ABI.

### 0.4.7 Nullability <a id="part-0-4-7"></a>
*Nullability* is a type property (for object and block pointer types) indicating whether `nil` is a valid value. Nullability kinds are defined in [Part 3](#part-3).

### 0.4.8 Optional type <a id="part-0-4-8"></a>
An *optional type* is written `T?` (sugar) in ObjC 3.0 mode. Optional types are a type-system feature for object and block pointer types (v1), distinct from Objective‑C’s “message to nil returns zero” runtime behavior ([Part 3](#part-3)).

### 0.4.9 IUO type <a id="part-0-4-9"></a>
An *implicitly unwrapped optional* type is written `T!` (sugar). In v1, IUO exists primarily as a migration aid and is discouraged in strict modes ([Part 3](#part-3)).

### 0.4.10 Carrier type <a id="part-0-4-10"></a>
A *carrier type* is a type that represents either a success value or an alternative early-exit/absence value and participates in propagation (`?`). In v1, carriers include:
- optional types (`T?`), and
- `Result<T, E>` ([Part 6](#part-6)).

### 0.4.11 Effects <a id="part-0-4-11"></a>
*Effects* are part of a function’s type and govern call-site obligations. In v1, effects include `throws` ([Part 6](#part-6)) and `async` ([Part 7](#part-7)).

### 0.4.12 Executor <a id="part-0-4-12"></a>
An *executor* is an abstract scheduler for task continuations ([Part 7](#part-7)). Executors may represent:
- the main/UI executor,
- a global background executor,
- or custom executors defined by the runtime/library.

### 0.4.13 Task <a id="part-0-4-13"></a>
A *task* is a unit of asynchronous work managed by the concurrency runtime/library. Tasks may be child tasks or detached tasks and may be cancellable ([Part 7](#part-7)).

### 0.4.14 Actor <a id="part-0-4-14"></a>
An *actor* is a concurrency-isolated reference type whose mutable state is protected by isolation rules ([Part 7](#part-7)). Cross-actor interactions are mediated by `await` and Sendable-like constraints.

## 0.5 Relationship to C and C++ <a id="part-0-5"></a>
Objective‑C 3.0 remains a superset of C (and compatible with Objective‑C++) as implemented by the toolchain. Where syntax is extended, it is designed to avoid changing meaning in non‑ObjC3 translation units.

## 0.6 Open issues <a id="part-0-6"></a>
- Finalize a public “normative references” index with stable URLs/identifiers once this draft is hosted in a canonical repository.
- Define a formal “abstract machine” section that unifies ordering rules across Parts [3](#part-3)/[6](#part-6)/[7](#part-7)/[8](#part-8).
<!-- END PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md -->

---

<!-- BEGIN PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md -->
# Part 1 — Versioning, Compatibility, and Conformance <a id="part-1"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 1.1 Purpose <a id="part-1-1"></a>
This part defines:
- how Objective‑C 3.0 is enabled,
- how source and behavior changes are contained,
- conformance levels and the meaning of “strictness,”
- required feature-test mechanisms,
- migration tooling requirements.

## 1.2 Language mode selection <a id="part-1-2"></a>

### 1.2.1 Compiler option <a id="part-1-2-1"></a>
A conforming implementation shall provide a command-line mechanism equivalent to:

- `-fobjc-version=3`

Selecting ObjC 3.0 mode shall:
- enable the ObjC 3.0 grammar additions,
- enable ObjC 3.0 default rules (nonnull-by-default regions, etc.),
- enable ObjC 3.0 diagnostics (at least warnings in permissive mode).

### 1.2.2 Translation-unit granularity <a id="part-1-2-2"></a>
ObjC 3.0 mode applies to a **translation unit**. Mixing ObjC 3.0 and non‑ObjC3 within a single translation unit is not required and is discouraged.

A conforming implementation may support per-file directives (e.g., for build systems), but the normative model is “one mode per translation unit.”

### 1.2.3 Source directive (optional) <a id="part-1-2-3"></a>
A conforming implementation may provide a source directive such as:

- `#pragma objc_language_version(3)`

If supported, the directive shall apply to the translation unit.

> Open issue: allow per-region switching only if it can be made unsurprising and compatible with module/import semantics. This draft assumes per‑translation‑unit switching.

## 1.3 Reserved keywords and conflict handling <a id="part-1-3"></a>

### 1.3.1 Reserved keywords <a id="part-1-3-1"></a>
### 1.3.1 Contextual keywords (non-normative guidance) <a id="part-1-3-1-2"></a>
Some tokens are treated as **contextual keywords** only within specific grammar positions, to reduce breakage in existing codebases.

Examples include:
- `borrowed` (type qualifier; [Part 8](#part-8), also cataloged in [B.8.3](#b-8-3))
- `move`, `weak`, `unowned` (block capture lists; [Part 8](#part-8), also cataloged in [B.8.5](#b-8-5))

Conforming implementations should avoid reserving these tokens globally.
In ObjC 3.0 mode, at minimum the following tokens are reserved as keywords:

- Control flow: `defer`, `guard`, `match`, `case`
- Effects/concurrency: `async`, `await`, `actor`
- Errors: `try`, `throw`, `do`, `catch`, `throws`
- Bindings: `let`, `var`

### 1.3.2 Raw identifiers <a id="part-1-3-2"></a>
A conforming implementation shall provide a compatibility escape hatch for identifiers that collide with reserved keywords.

Two acceptable designs:
- a raw identifier syntax (e.g., `@identifier(defer)`), or
- a backtick escape (e.g., `` `defer` ``) in ObjC 3.0 mode only.

The toolchain shall also provide fix-its to mechanically rewrite collisions.

### 1.3.3 Backward compatibility note <a id="part-1-3-3"></a>
This specification does not require keyword reservation to apply outside ObjC 3.0 mode.

## 1.4 Feature test macros <a id="part-1-4"></a>

### 1.4.1 Required macros <a id="part-1-4-1"></a>
A conforming implementation shall provide predefined macros to allow conditional compilation:

- `__OBJC_VERSION__` (integer; at least `3` in ObjC 3.0 mode)
- `__OBJC3__` (defined to `1` in ObjC 3.0 mode)

### 1.4.2 Per-feature macros <a id="part-1-4-2"></a>
A conforming implementation shall provide per-feature macros:

- `__OBJC3_FEATURE_<NAME>__`

Examples:
- `__OBJC3_FEATURE_DEFER__`
- `__OBJC3_FEATURE_OPTIONALS__`
- `__OBJC3_FEATURE_THROWS__`
- `__OBJC3_FEATURE_ASYNC_AWAIT__`
- `__OBJC3_FEATURE_ACTORS__`

Feature macros shall be defined in ObjC 3.0 mode even if a feature is disabled by a profile, to allow feature probing (`0` meaning “recognized but disabled”).

### 1.4.3 `__has_feature` integration (optional) <a id="part-1-4-3"></a>
A conforming implementation may expose ObjC 3.0 features through a `__has_feature`-style mechanism. If present, it should align with the per-feature macros.

## 1.5 Conformance levels (strictness) <a id="part-1-5"></a>

### 1.5.1 Levels <a id="part-1-5-1"></a>
A translation unit in ObjC 3.0 mode may additionally declare a conformance level.

- **Permissive**: language features enabled; safety checks default to warnings; legacy patterns allowed.
- **Strict**: key safety checks are errors; opt-outs require explicit unsafe spellings.
- **Strict-system**: strict + additional system-API safety checks (resource cleanup correctness, borrowed pointer escape analysis, etc.).

### 1.5.2 Selecting a level <a id="part-1-5-2"></a>
A conforming implementation shall provide an option equivalent to:

- `-fobjc3-strictness=permissive|strict|strict-system`

### 1.5.3 Diagnostic escalation rule <a id="part-1-5-3"></a>
If a construct is ill‑formed in strict mode, it may still be accepted in permissive mode with a warning *only* if:
- the compiler can preserve baseline behavior, and
- the behavior is not undefined.

## 1.6 Orthogonal checking modes (submodes) <a id="part-1-6"></a>

### 1.6.1 Strict concurrency checking <a id="part-1-6-1"></a>
Concurrency checking is orthogonal: a translation unit may enable additional checking beyond the strictness level.

A conforming implementation shall provide an option equivalent to:

- `-fobjc3-concurrency=strict|off`

Strict concurrency checking enables additional diagnostics defined in [Part 7](#part-7)/12 (Sendable, actor isolation misuse, executor affinity misuse, etc.).

### 1.6.2 Strict performance checking (optional) <a id="part-1-6-2"></a>
Implementations may provide a “strict performance” mode enabling additional diagnostics and/or runtime assertions for [Part 9](#part-9) features (static regions, direct methods). If provided, the option shall be orthogonal to strictness levels.

> Note: This draft treats strict performance checking as an optional extension because runtime enforcement strategies differ by platform.

## 1.7 Source compatibility principles <a id="part-1-7"></a>

### 1.7.1 No silent semantic changes in non‑ObjC3 code <a id="part-1-7-1"></a>
Compiling code not in ObjC 3.0 mode shall not change meaning due to this specification.

### 1.7.2 Contained default changes <a id="part-1-7-2"></a>
Default changes (e.g., nonnull-by-default) apply only inside ObjC 3.0 translation units and/or explicitly marked module boundaries ([Part 2](#part-2)/3).

### 1.7.3 Header compatibility <a id="part-1-7-3"></a>
Headers intended to be consumed by non‑ObjC3 translation units shall:
- avoid ObjC 3.0‑only keywords in public API unless guarded by feature macros, or
- use canonical attribute spellings that are syntactically valid in baseline compilers ([Decision D-007](#decisions-d-007)).

## 1.8 Migration tooling requirements <a id="part-1-8"></a>
A conforming implementation shall provide:
- diagnostics with fix-its for common migrations,
- a batch migrator capable of applying safe transformations.

Minimum migrator capabilities:
1. Insert nullability annotations based on static inference and usage ([Part 3](#part-3)).
2. Convert “weak‑strong dance” patterns to capture lists ([Part 8](#part-8)).
3. Convert common manual cleanup patterns to `defer` or `@resource` ([Part 8](#part-8)).
4. Suggest `withLifetime/keepAlive` when borrowed-pointer diagnostics trigger ([Part 8](#part-8)).
5. Provide stubs for `throws` and `Result` bridging from NSError patterns ([Part 6](#part-6)).
6. Where possible, annotate completion-handler C/ObjC APIs for `async` bridging overlays ([Part 11](#part-11)).

## 1.9 Profiles (hook point) <a id="part-1-9"></a>

Objective‑C 3.0 defines optional **profiles**: named bundles of additional restrictions, defaults, and required library/runtime surfaces aimed at specific domains (e.g., “system”, “app”, “freestanding”).

Profiles are selected by **toolchain configuration**, not by source-level directives.
A conforming implementation shall provide a mechanism equivalent to:

- `-fobjc-profile=<name>`

Profiles may:
- enable additional diagnostics as errors ([Part 12](#part-12)),
- require particular standard modules (e.g., Concurrency; [Part 7](#part-7)),
- require additional module metadata preservation ([D](#d)),
- restrict unsafe constructs (e.g., borrowed pointer escaping; [Part 8](#part-8)).

This part defines the *hook point* and selection mechanism; profile contents are specified in **[CONFORMANCE_PROFILE_CHECKLIST.md](#e)**.
<!-- END PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md -->

---

<!-- BEGIN PART_2_MODULES_NAMESPACING_API_SURFACES.md -->
# Part 2 — Modules, Namespacing, and API Surfaces <a id="part-2"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 2.1 Purpose <a id="part-2-1"></a>
This part defines:

- module import rules and their interaction with headers,
- a **module-qualified name** mechanism to reduce global-prefix collision pressure,
- API surface contracts: visibility, stability, and availability,
- requirements for interface extraction/emission so ObjC 3.0 semantics survive separate compilation.

This part is closely related to:
- **[B](#b)** (canonical spellings for emitted interfaces),
- **[C](#c)** (separate compilation and lowering contracts), and
- **[D](#d)** (the normative checklist of what must be preserved in module metadata across boundaries).

## 2.2 Modules <a id="part-2-2"></a>

### 2.2.1 `@import` <a id="part-2-2-1"></a>
Objective‑C 3.0 standardizes `@import` as the preferred import mechanism.

A module import shall:
- make the imported module’s public declarations visible,
- apply the module’s exported nullability metadata ([Part 3](#part-3)),
- apply the module’s exported availability metadata (if any),
- make any exported “strictness recommendation” visible to tooling ([Part 1](#part-1)).

### 2.2.2 `#import` and mixed-mode code (non-normative) <a id="part-2-2-2"></a>
`#import` remains supported for compatibility with existing headers.
However, toolchains are encouraged to treat `@import` as the semantic source of truth for:
- nullability defaults,
- availability,
- and extracted interface emission.

### 2.2.3 Module maps and header ownership (implementation-defined) <a id="part-2-2-3"></a>
The mechanism that maps headers to modules (module maps, build system metadata, etc.) is implementation-defined.
A conforming implementation shall behave **as if** each public declaration belongs to exactly one owning module for the purpose of:
- API visibility,
- interface emission,
- and cross-module diagnostics.

## 2.3 Module-qualified names <a id="part-2-3"></a>

### 2.3.1 Motivation <a id="part-2-3-1"></a>
Objective‑C historically uses global prefixes (e.g., `NS`, `UI`, `CG`) to avoid collisions.
As module ecosystems grow, prefixes are increasingly insufficient and noisy.

Objective‑C 3.0 introduces a lightweight module-qualified name form that:
- is unambiguous in ObjC and ObjC++,
- does not require a global namespace feature,
- is stable under interface emission.

### 2.3.2 Syntax (normative) <a id="part-2-3-2"></a>
A *module-qualified name* is written:

```objc
@ModuleName.Identifier
@TopLevel.Submodule.Identifier
```

Grammar (informative):

- `module-qualified-name` → `'@' module-path '.' identifier`
- `module-path` → `identifier ('.' identifier)*`

### 2.3.3 Where module-qualified names may appear (normative) <a id="part-2-3-3"></a>
Module-qualified names may appear in:
- type positions (class names, protocol names, typedef names),
- expression positions where an identifier would be valid (e.g., referring to a global function or global constant),
- attribute arguments where an identifier names a type or symbol.

They shall not appear in:
- selector spelling positions (the `:`-separated selector pieces),
- preprocessor directive names.

### 2.3.4 Name resolution (normative) <a id="part-2-3-4"></a>
A module-qualified name resolves by:
1. Resolving the module path to a module (or submodule) that is visible in the current translation unit.
2. Looking up the identifier in that module’s exported declarations.

If no matching declaration exists, the program is ill-formed.

### 2.3.5 Interface emission (normative) <a id="part-2-3-5"></a>
If a declaration is referenced using a module-qualified name in an emitted textual interface, the emitter shall preserve the module-qualified form **or** emit an equivalent unqualified name together with imports that make it unambiguous.

## 2.4 API surface contracts <a id="part-2-4"></a>

### 2.4.0 Cross-module semantic preservation (normative) <a id="part-2-4-0"></a>
A module’s public API contract includes not only names and types, but also ObjC 3.0 semantics that affect call legality and call lowering.
The minimum required set is enumerated in **[D.3.1](#d-3-1) [Table A](#d-3-1)**.


### 2.4.1 Public vs SPI vs private (normative) <a id="part-2-4-1"></a>
A module’s API surface is partitioned into (at minimum):
- **Public API**: visible to all importers.
- **SPI** (system/private interface): visible only to privileged importers (mechanism implementation-defined).
- **Private**: not visible outside the owning module.

The mechanism used to mark partitions is implementation-defined (attributes, build system flags, header directory layout, etc.), but a conforming toolchain shall preserve partition metadata in module files and interface emission.

### 2.4.2 Effects and attributes are part of the API contract (normative) <a id="part-2-4-2"></a>
For exported functions/methods, the following are API-significant and must be preserved across module boundaries:

- effects: `async`, `throws`
- executor affinity: `objc_executor(...)`
- task-spawn recognition attributes (when exporting concurrency entry points)
- nullability and ownership qualifiers
- direct/final/sealed annotations that affect call legality or dispatch ([Part 9](#part-9))

## 2.5 Extracted interfaces and semantic preservation <a id="part-2-5"></a>

### 2.5.1 Requirement (normative) <a id="part-2-5-1"></a>
If an implementation provides any facility that emits an interface description (textual or AST-based), then importing that interface must reconstruct:

- the same declarations and types,
- the same effects (`async`, `throws`),
- the same canonical attributes/pragmas needed for semantics.

Canonical spellings are defined in **[B](#b)**.

### 2.5.2 Format (implementation-defined) <a id="part-2-5-2"></a>
This draft does not require a particular distribution format (text vs binary).
However, the format must be sufficient to support:

- strictness/migration tooling ([Part 1](#part-1)),
- diagnostics for effect mismatches ([C.2](#c-2)),
- and cross-module concurrency checking ([Part 7](#part-7)).

## 2.6 Required diagnostics (minimum) <a id="part-2-6"></a>
- Using a module-qualified name for an unloaded/unimported module: error with fix-it to add `@import`.
- Resolving a module-qualified name to multiple candidates (ambiguous exports): error; recommend qualification or import narrowing.
- Redeclaring an imported API with mismatched effects/ABI-significant attributes: error (see [C.2](#c-2)).
<!-- END PART_2_MODULES_NAMESPACING_API_SURFACES.md -->

---

<!-- BEGIN PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md -->
# Part 3 — Types: Nullability, Optionals, Pragmatic Generics, and Typed Key Paths <a id="part-3"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 3.0 Overview <a id="part-3-0"></a>

### v0.8 resolved decisions <a id="part-3-v0-8-resolved-decisions"></a>
- Optional chaining and optional message sends are **reference-only** in v1 (object/block/void only). Scalar/struct optional chaining is ill-formed.
- Optional message sends are **conditional calls**: argument expressions are evaluated only if the receiver is non-`nil`.
- Generic methods/functions are **deferred** in v1; generic *types* remain supported.
- Canonical nullability default regions use `#pragma objc assume_nonnull begin/end` ([B](#b)).

Objective‑C 3.0 improves type safety without abandoning Objective‑C’s model:

- **Nullability** becomes a core, enforceable type property, with module/region defaults.
- **Optionals** provide explicit “maybe” semantics with ergonomic binding (`if let`, `guard let`) and explicit optional access.
- **Pragmatic generics** extend today’s lightweight generics into a more uniform system that can express generic APIs and constraints while remaining ABI-compatible.
- **Typed key paths** provide compiler-checked alternatives to stringly typed KVC/KVO-style access.

This part defines **syntax**, **static semantics**, **dynamic semantics**, and **required diagnostics** for these features.

---

## 3.1 Lexical and grammar additions <a id="part-3-1"></a>

### 3.1.1 New keywords <a id="part-3-1-1"></a>
In Objective‑C 3.0 mode, the following keywords are reserved by this part:

- `guard`, `let`, `var`

(Additional reserved keywords are specified in [Part 1](#part-1).)

### 3.1.2 New punctuators <a id="part-3-1-2"></a>
This part introduces new punctuators in specific grammatical contexts:

- `?` and `!` as **type suffixes** in declarations (optional sugar)
- `?.` as an **optional member access** token in expressions
- `??` as a **nil-coalescing operator** in expressions

The tokens shall be recognized only where specified by the grammar; in all other contexts, existing C/Objective‑C meaning applies.

---

## 3.2 Nullability <a id="part-3-2"></a>

### 3.2.1 Nullability kinds <a id="part-3-2-1"></a>
Objective‑C 3.0 recognizes the following nullability kinds for Objective‑C object pointer types and block pointer types:

- **Nonnull**: value is not `nil`.
- **Nullable**: value may be `nil`.
- **Unspecified**: nullability is unknown (legacy).

The **unspecified** kind exists only for compatibility and shall be treated as “potentially nil” for safety diagnostics.

### 3.2.2 Where nullability applies <a id="part-3-2-2"></a>
Nullability applies to:
- Objective‑C object pointer types (`id`, `Class`, `NSObject *`, `NSString *`, `id<Proto>`, etc.)
- block pointer types (e.g., `void (^)(void)`)

Nullability does **not** apply directly to:
- scalar types (`int`, `BOOL`, `NSInteger`, etc.)
- pointers that are not object/block pointers (`int *`, `void *`, etc.)

(Those may be covered by separate annotations; out of scope for this part.)

### 3.2.3 Nullability spelling forms <a id="part-3-2-3"></a>
Objective‑C 3.0 supports three equivalent spelling forms for object/block pointer nullability:

1. **Qualifier form** (baseline spelling):
   - `_Nonnull`, `_Nullable`, `_Null_unspecified` (or implementation-provided equivalents)
2. **Objective‑C contextual keywords** (when supported by implementation):
   - `nonnull`, `nullable`
3. **Optional sugar** (ObjC 3.0):
   - `T?` (nullable)
   - `T!` (implicitly unwrapped optional; see [§3.3.3](#part-3-3-3))

#### 3.2.3.1 Canonicalization rule <a id="part-3-2-3-1"></a>
For type-checking purposes, implementations shall canonicalize:
- `T?` → `T _Nullable`
- `T!` → `T _Nullable` **plus** “implicitly-unwrapped” marker

If a type already includes an explicit nullability qualifier, the sugar form shall be rejected as redundant.

### 3.2.4 Nonnull-by-default regions <a id="part-3-2-4"></a>

#### 3.2.4.1 Region introduction <a id="part-3-2-4-1"></a>
A translation unit in ObjC 3.0 mode may establish a nonnull-by-default region using one of:

- a module-level default (recommended for frameworks),
- a pragma region, or
- an explicit language directive.

The canonical spelling is the pragma form ([B](#b)):

```c
#pragma objc assume_nonnull begin
// declarations
#pragma objc assume_nonnull end
```

Implementations may additionally support aliases such as `#pragma clang assume_nonnull begin/end` or `NS_ASSUME_NONNULL_BEGIN/END`, but emitted interfaces shall use the canonical spelling.

#### 3.2.4.2 Region semantics <a id="part-3-2-4-2"></a>
Inside a nonnull-by-default region:

- Any unannotated object/block pointer type in a **declaration** is treated as **nonnull**.
- `nullable` must be explicitly spelled.

Outside such a region:
- unannotated object/block pointer types are treated as **unspecified**.

> Rationale: this allows headers to become “safe by default” while preserving compatibility with legacy code.

### 3.2.5 Nullability completeness <a id="part-3-2-5"></a>

#### 3.2.5.1 Public interface completeness <a id="part-3-2-5-1"></a>
A module’s exported interface shall be considered **nullability-complete** if every exported object/block pointer type has an effective nullability (explicit or default-region-derived) that is not “unspecified”.

In **strict** mode ([Part 1](#part-1)):
- exporting “unspecified” nullability is ill-formed.

In **permissive** mode:
- it is permitted but diagnosed.

#### 3.2.5.2 Local completeness <a id="part-3-2-5-2"></a>
Within a translation unit, incomplete nullability is permitted but should be diagnosed when it impacts type safety (e.g., assigning “unspecified” to “nonnull”).

---

## 3.3 Optionals <a id="part-3-3"></a>

### 3.3.1 Optional type sugar (`T?`) <a id="part-3-3-1"></a>
A type written with the suffix `?` denotes a nullable object/block pointer type.

Examples:

```objc
NSString*? maybeName;
id<NSCopying>? maybeKey;
void (^? callback)(int);
```

### 3.3.2 Optional binding: `if let` and `guard let` <a id="part-3-3-2"></a>

#### 3.3.2.1 Grammar <a id="part-3-3-2-1"></a>
```text
if-let-statement:
    'if' let-binding-list compound-statement ('else' compound-statement)?

guard-let-statement:
    'guard' let-binding-list 'else' compound-statement

let-binding-list:
    'let' let-binding (',' let-binding)*
  | 'var' let-binding (',' let-binding)*

let-binding:
    identifier '=' expression
```

#### 3.3.2.2 Semantics <a id="part-3-3-2-2"></a>
For `if let`:

- Each binding expression is evaluated in source order.
- If any bound value is `nil`, the `then` block is skipped and the `else` block (if present) executes.
- On success, each bound identifier is introduced in the then-scope with **nonnull** type.

For `guard let`:

- Each binding expression is evaluated in source order.
- If any bound value is `nil`, the else block executes.
- The else block **must exit** the current scope by one of:
  - `return`, `throw`, `break`, `continue`, or other scope-exiting statement defined by this spec.
- After the guard, each bound identifier is in scope and has **nonnull** type.

#### 3.3.2.3 `let` vs `var` <a id="part-3-3-2-3"></a>
- `let` bindings are immutable.
- `var` bindings are mutable.

Implementations shall diagnose mutation of `let` bindings as an error.

### 3.3.3 Implicitly unwrapped optionals (`T!`) <a id="part-3-3-3"></a>
`T!` denotes a nullable value that is **implicitly unwrapped** at use sites in certain contexts.

#### 3.3.3.1 Intended use <a id="part-3-3-3-1"></a>
`T!` exists primarily for interop boundaries where the API is “logically nonnull” but not yet fully annotated, or where legacy behavior is relied upon.

#### 3.3.3.2 Static semantics <a id="part-3-3-3-2"></a>
In ObjC 3.0 strict mode:
- implicit unwrapping of `T!` without an explicit proof is diagnosed.
- implementations should offer fix-its to convert to `T?` + binding or add nullability to the API.

#### 3.3.3.3 Dynamic behavior (optional) <a id="part-3-3-3-3"></a>
An implementation may provide a runtime trap when an implicitly unwrapped optional is observed to be `nil` at a use site, but this is not required by this draft.

> Recommendation: treat IUO primarily as a static-typing aid and migration tool, not as a runtime “bomb”.

### 3.3.4 Nil-coalescing operator (`??`) <a id="part-3-3-4"></a>

#### 3.3.4.1 Grammar <a id="part-3-3-4-1"></a>
```text
nil-coalescing-expression:
    logical-or-expression
    nil-coalescing-expression '??' nil-coalescing-expression
```

`??` is right-associative.

#### 3.3.4.2 Semantics <a id="part-3-3-4-2"></a>
`a ?? b` evaluates `a`:
- if `a` is nonnull, yields `a` and does not evaluate `b`;
- otherwise evaluates and yields `b`.

#### 3.3.4.3 Typing <a id="part-3-3-4-3"></a>
If `a` has type `T?` and `b` has type `T` (nonnull), then `a ?? b` has type `T` (nonnull).

More generally:
- The result type is the least upper bound of the operand types, with nullability resolved to nonnull if the right operand is nonnull and `a` is tested.

In strict mode, `b` must be type-compatible with the unwrapped type of `a`; otherwise error.

---

## 3.4 Nullable receivers and optional access <a id="part-3-4"></a>

Objective‑C historically allows messaging `nil` and returns zero/`nil`. ObjC 3.0 preserves this behavior for compatibility but provides **explicit, typed optional access** and enables strict diagnostics.

### 3.4.1 Optional member access: `?.` <a id="part-3-4-1"></a>

#### 3.4.1.1 Grammar <a id="part-3-4-1-1"></a>
```text
postfix-expression:
    ...
    postfix-expression '?.' identifier
```

#### 3.4.1.2 Semantics <a id="part-3-4-1-2"></a>
For `x?.p` where `p` resolves to a property access:

- Evaluate `x` once.
- If `x` is `nil`, yield `nil`.
- Otherwise perform the normal property access and yield the value.

#### 3.4.1.3 Typing restrictions <a id="part-3-4-1-3"></a>
`?.` is permitted only when the property type is an **object/block pointer type**.
- The result type is the property type made nullable (i.e., `T?`).

If the property type is scalar/struct:
- `?.` is ill-formed in strict mode and diagnosed in permissive mode (because there is no scalar optional type in this draft).

### 3.4.2 Optional message send: `[receiver? selector]` <a id="part-3-4-2"></a>

#### 3.4.2.1 Grammar <a id="part-3-4-2-1"></a>
```text
message-expression:
    '[' receiver-expression optional-send-indicator? message-selector ']'

optional-send-indicator:
    '?'
```

The optional-send indicator applies to the receiver position only.

Example:
```objc
id? x = ...;
NSString*? s = [x? description];
```

#### 3.4.2.2 Semantics <a id="part-3-4-2-2"></a>
- Evaluate the receiver expression once.
- If receiver is `nil`, the optional send yields:
  - `nil` for object/block pointer returns,
  - no effect for `void` returns.

#### 3.4.2.3 Typing restrictions <a id="part-3-4-2-3"></a>

#### 3.4.2.4 Argument evaluation (normative) <a id="part-3-4-2-4"></a>
For an optional message send of the form:

```objc
[receiver? method:arg1 other:arg2]
```

the implementation shall:
1. Evaluate `receiver` exactly once.
2. If `receiver` is `nil`, the expression yields the “nil case” result ([§3.4.2.2](#part-3-4-2-2)) and the argument expressions `arg1`, `arg2`, … shall **not** be evaluated.
3. If `receiver` is non-`nil`, evaluate argument expressions in the same order as ordinary message sends, then perform the message send.

This differs intentionally from ordinary Objective‑C message sends, where arguments are evaluated even if the receiver is `nil`.


Optional message send is permitted only if the method return type is:
- `void`, or
- an object/block pointer type.

If the method returns scalar/struct, optional send is **ill-formed** in v1 (all conformance levels).

> Rationale: nil-messaging returning 0 for scalars is a major source of silent logic bugs; ObjC 3 strictness is allowed to reject it.

### 3.4.3 Ordinary sends on nullable receivers <a id="part-3-4-3"></a>
In **permissive** mode:
- sending a message to a nullable receiver using ordinary syntax (`[x foo]` where `x` is nullable) is permitted but diagnosed.

In **strict** mode:
- ordinary sends require the receiver type be nonnull, *unless* the send is written as an optional send (`[x? foo]`).

This rule is the cornerstone of “nonnull actually means something” in ObjC 3.

### 3.4.4 Flow-sensitive receiver refinement <a id="part-3-4-4"></a>
If control flow proves a receiver nonnull, ordinary sends are permitted within that scope:

```objc
if (x != nil) {
  [x foo]; // OK: x refined to nonnull
}
```

This refinement also applies to `if let` / `guard let`.

---

### 3.4.5 v1 restriction: no scalar/struct optional chaining (normative) <a id="part-3-4-5"></a>
In Objective‑C 3.0 v1, optional chaining constructs in [§3.4.1](#part-3-4-1) and [§3.4.2](#part-3-4-2) are defined **only** for:

- members/methods that return an Objective‑C object pointer type;
- members/methods that return a block pointer type; and
- `void` return type for optional message send only.

If the selected property or method returns any scalar, vector, enum, or struct/union type, use of `?.` or `[receiver? ...]` is **ill‑formed**.

**Note:** Ordinary Objective‑C message sends to `nil` receivers that yield scalar zero values remain part of the baseline language behavior. However, in ObjC 3.0 **strict** mode, ordinary sends on nullable receivers are rejected (see [§3.4.3](#part-3-4-3)), pushing scalar-return calls into explicitly nonnull contexts and eliminating silent fallbacks.



## 3.5 Pragmatic generics <a id="part-3-5"></a>

### 3.5.1 Goals and constraints <a id="part-3-5-1"></a>
Objective‑C 3.0 generics shall:
- improve compile-time checking,
- preserve ABI via type erasure unless explicitly reified,
- interact predictably with covariance/contravariance where declared.

### 3.5.2 Generic type declarations <a id="part-3-5-2"></a>

#### 3.5.2.1 Grammar (provisional) <a id="part-3-5-2-1"></a>
```text
generic-parameter-clause:
    '<' generic-parameter (',' generic-parameter)* '>'

generic-parameter:
    variance? identifier generic-constraints?

variance:
    '__covariant'
  | '__contravariant'

generic-constraints:
    ':' type-constraint (',' type-constraint)*
```

Example:
```objc
@interface Box<__covariant T> : NSObject
@end
```

#### 3.5.2.2 Constraints <a id="part-3-5-2-2"></a>
A `type-constraint` may be:
- an Objective‑C protocol type: `id<NSCopying>`
- a class bound: `NSObject`
- a composition: `id<Proto1, Proto2>`

The constraint grammar is intentionally limited for implementability.

### 3.5.3 Generic method/function declarations (future extension) <a id="part-3-5-3"></a>

Objective‑C 3.0 v1 defers generic methods/functions.
Toolchains may reserve the syntax, but in v1 it is ill‑formed.

Rationale: integrating generic method syntax with Objective‑C selector grammar and redeclaration rules introduces significant complexity for separate compilation and interface emission.

### 3.5.4 Type argument application <a id="part-3-5-4"></a>
Type arguments may be applied to generic types using angle brackets:
```objc
Box<NSString*>* b;
```

### 3.5.5 Erasure and runtime behavior <a id="part-3-5-5"></a>
Unless a declaration is explicitly marked `@reify_generics` (future extension), generic arguments are erased at runtime:
- they do not affect object layout,
- they do not affect message dispatch,
- they exist for type checking and tooling.

### 3.5.6 Variance <a id="part-3-5-6"></a>
If a generic parameter is declared covariant:
- `Box<NSString*>*` is a subtype of `Box<NSObject*>*`.

If invariant (default):
- no such subtyping is allowed.

Variance is enforced purely statically.

### 3.5.7 Diagnostics <a id="part-3-5-7"></a>
In strict mode, the compiler shall diagnose:
- passing a `Box<NSObject*>*` where `Box<NSString*>*` is required (covariance direction),
- using a generic parameter without satisfying constraints,
- downcasting across generic arguments without explicit unsafe spelling.

---

## 3.6 Typed key paths <a id="part-3-6"></a>

### 3.6.1 Key path literal <a id="part-3-6-1"></a>

#### 3.6.1.1 Syntax <a id="part-3-6-1-1"></a>
Objective‑C 3.0 introduces a key path literal:

```objc
@keypath(RootType, member.chain)
```

Examples:
```objc
auto kp1 = @keypath(Person, name);
auto kp2 = @keypath(Person, address.street);
```

Within instance methods, `self` may be used as root:
```objc
auto kp = @keypath(self, title);
```

#### 3.6.1.2 Grammar <a id="part-3-6-1-2"></a>
```text
keypath-literal:
    '@keypath' '(' keypath-root ',' keypath-components ')'

keypath-root:
    type-name
  | 'self'

keypath-components:
    identifier ('.' identifier)*
```

### 3.6.2 KeyPath types <a id="part-3-6-2"></a>
The standard library shall define:

- `KeyPath<Root, Value>`
- `WritableKeyPath<Root, Value>` (if writable)

A key path literal has type `KeyPath<Root, Value>` where:
- `Root` is the specified root type,
- `Value` is the type of the final member.

If any component is writable (settable) and the entire chain is writable, the literal may be typed as `WritableKeyPath`.

### 3.6.3 Static checking <a id="part-3-6-3"></a>
The compiler shall validate at compile time:
- each member exists,
- accessibility rules allow access,
- each step’s type matches the next component’s root.

If validation fails, the program is ill-formed.

### 3.6.4 Application and bridging <a id="part-3-6-4"></a>
The standard library shall provide:
- `Value KeyPathGet(KeyPath<Root, Value> kp, Root root);`
- `void KeyPathSet(WritableKeyPath<Root, Value> kp, Root root, Value v);`

The spec permits (but does not require) syntactic sugar such as:
- `root[kp]` for get and set.

Key paths may be explicitly converted to string keys for KVC where safe:
- `NSString* KeyPathToKVCString(KeyPath kp);`

Such conversions shall be explicit and diagnosed in strict mode if they may lose safety.

### 3.6.5 KVO/Observation integration <a id="part-3-6-5"></a>
Where an observation API accepts key paths, it shall prefer typed key paths over strings.
(Actual observation APIs are library-defined; this spec defines the language value representation and checking.)

---

## 3.7 Required diagnostics and fix-its <a id="part-3-7"></a>

### 3.7.1 Nullability <a id="part-3-7-1"></a>
Required diagnostics (at least warnings; errors in strict):
- assigning nullable/unspecified to nonnull without check,
- returning nullable from a function declared nonnull,
- exporting unspecified nullability in strict mode.

### 3.7.2 Optional misuse <a id="part-3-7-2"></a>
Required diagnostics:
- using a `T?` value where `T` is required without binding/unwrapping,
- optional member access used on scalar members,
- ordinary message sends on nullable receivers in strict mode (suggest optional send or binding).

### 3.7.3 Generics <a id="part-3-7-3"></a>
Required diagnostics:
- constraint violations,
- unsafe generic downcasts requiring explicit spelling.

### 3.7.4 Key paths <a id="part-3-7-4"></a>
Required diagnostics:
- invalid key path components,
- converting key paths to strings without explicit conversion.

---

## 3.8 Examples <a id="part-3-8"></a>

### 3.8.1 Optional binding and optional send <a id="part-3-8-1"></a>
```objc
NSString*? maybeName = [user? name];   // optional send returns nullable
NSString* name = maybeName ?? @"(none)";

guard let u = user else { return; }
NSString* n = [u name];               // safe: u is nonnull
```

### 3.8.2 Key paths <a id="part-3-8-2"></a>
```objc
auto kp = @keypath(Person, address.street);
NSString* s = KeyPathGet(kp, person);
```

---

## 3.9 Open issues <a id="part-3-9"></a>
1. Whether to standardize `id`/`Class` optional sugar and how it maps to nullability qualifiers.
2. Whether to introduce a first-class value optional ABI (`Optional<T>`) in a future revision.
3. Whether and how to add generic methods/functions in a future revision without breaking selector grammar.
<!-- END PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md -->

---

<!-- BEGIN PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md -->
# Part 4 — Memory Management and Ownership <a id="part-4"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 4.1 Purpose <a id="part-4-1"></a>
This part defines how ObjC 3.0:
- builds on ARC as the baseline ownership system,
- standardizes ownership qualifiers and lifetime guarantees,
- introduces transfer/consumption annotations that improve correctness and performance,
- integrates “retainable families” beyond ObjC objects (delegated to [Part 8](#part-8) for system libraries).

## 4.2 ARC as normative baseline <a id="part-4-2"></a>
In ObjC 3.0 mode with ARC enabled:
- the compiler shall insert retains/releases/autoreleases according to ARC rules.
- ownership qualifiers (`__strong`, `__weak`, `__autoreleasing`, `__unsafe_unretained`) shall be honored.

ObjC 3.0 does not change ARC’s fundamental model: it remains reference counting without cycle collection.

## 4.3 Ownership qualifiers (standardized) <a id="part-4-3"></a>
### 4.3.1 `__strong` <a id="part-4-3-1"></a>
Strong references keep objects alive.

### 4.3.2 `__weak` <a id="part-4-3-2"></a>
Weak references are zeroing and do not keep objects alive.
- Reading a weak reference yields a value that may be nil.
- In strict nullability mode, weak reads are treated as nullable unless proven.

### 4.3.3 `__unsafe_unretained` <a id="part-4-3-3"></a>
Non-owning and non-zeroing; use-after-free is possible.
- Its usage is discouraged and diagnosed in strict modes unless in explicitly marked unsafe contexts.

### 4.3.4 `__autoreleasing` <a id="part-4-3-4"></a>
Used primarily for NSError-style out parameters and bridging to autorelease pools.

## 4.4 Precise lifetime and lifetime shortening <a id="part-4-4"></a>
Optimizing compilers may shorten object lifetimes relative to source-level scope when it is safe under ARC rules.

Objective‑C 3.0 standardizes a precise lifetime mechanism:
- variables annotated with precise lifetime shall not be released before the end of their lexical scope.

This is foundational for borrowed interior pointers ([Part 8](#part-8)).

## 4.5 Transfer and consumption annotations <a id="part-4-5"></a>
Objective‑C 3.0 introduces standardized attributes for ownership transfer in APIs.

### 4.5.1 `@consumes` parameter <a id="part-4-5-1"></a>
A `@consumes` parameter indicates that:
- the callee takes over responsibility for releasing/disposing the argument.
- the caller must not use or dispose of it after the call unless it retains/copies explicitly.

### 4.5.2 `@returns_owned` / `@returns_unowned` <a id="part-4-5-2"></a>
For return values:
- `@returns_owned` means the caller receives ownership (must dispose under the appropriate rules).
- `@returns_unowned` means the caller borrows (must not outlive the owner; see [Part 8](#part-8) for borrowed pointers).

### 4.5.3 Diagnostics <a id="part-4-5-3"></a>
In strict mode:
- using a consumed argument after the call is an error (use-after-move).
- failing to dispose an owned return before it becomes unreachable is an error when the type is annotated as requiring cleanup ([Part 8](#part-8)).

## 4.6 Move and “one-time” values <a id="part-4-6"></a>
Objective‑C 3.0 introduces the concept of move for:
- resource handles ([Part 8](#part-8)),
- and optionally for certain object wrappers.

Move semantics are explicit:
- `take(x)` and `move x` capture list items ([Part 8](#part-8)) are the standardized spellings.

## 4.7 Autorelease pools and bridging <a id="part-4-7"></a>
ObjC 3.0 retains autorelease pool semantics and requires that new language constructs (`defer`, `async`) preserve correctness:
- defers execute before local releases in the same scope ([Part 8](#part-8)).
- `async` suspension points shall preserve pool semantics according to the concurrency runtime ([Part 7](#part-7)).

Per-task autorelease pool behavior at suspension points is defined normatively in [Part 7](#part-7) and [C](#c) ([Decision D-006](#decisions-d-006)).



## 4.7 Async suspension and autorelease pools <a id="part-4-7-2"></a>

### 4.7.1 Normative rule (Objective‑C runtimes) <a id="part-4-7-1"></a>
On Objective‑C runtimes with autorelease semantics, a conforming implementation shall ensure:

- Each *task execution slice* (resume → next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule is required to avoid unbounded autorelease growth across long async chains and to make memory behavior predictable for Foundation-heavy code.

See [Part 7](#part-7) [§7.9](#part-7-9) and [C.7](#c-7).


## 4.8 Interactions with retainable C families <a id="part-4-8"></a>
Retainable families (CoreFoundation-like, dispatch/xpc integration, custom refcounted C objects) are specified in [Part 8](#part-8). [Part 4](#part-4) defines their participation in the ownership model:
- retainable family types may be treated as retainable under ARC.
- transfer annotations may apply to them.

## 4.9 Open issues <a id="part-4-9"></a>
- Whether to introduce a first-class “unmanaged” wrapper type (like Swift’s `Unmanaged`) for explicit retain/release in non-ARC contexts.
- How to specify and test lifetime shortening behavior precisely across optimizations (requires conformance test suite guidance, [Part 12](#part-12)).
<!-- END PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md -->

---

<!-- BEGIN PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md -->
# Part 5 — Control Flow and Safety Constructs <a id="part-5"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 5.1 Purpose <a id="part-5-1"></a>
This part defines:
- `defer` as a core control-flow primitive,
- `guard` as a scope-exit conditional with refinement,
- `match` and pattern matching,
- rules that enable compilers to generate reliable cleanup and diagnostics.

This part specifies syntax and high-level semantics. Precise cleanup ordering (especially for system resources) is specified in [Part 8](#part-8) and is normative.

## 5.2 `defer` <a id="part-5-2"></a>

### 5.2.1 Syntax <a id="part-5-2-1"></a>
```text
defer compound-statement
```

Example:
```objc
FILE *f = fopen(path, "r");
defer { if (f) fclose(f); }

// ... use f ...
```

### 5.2.2 Semantics (normative) <a id="part-5-2-2"></a>
Executing a `defer` statement registers its body as a *scope-exit action* in the innermost enclosing lexical scope.

- Scope-exit actions execute in **LIFO** order at scope exit.
- Scope exit includes normal exit and stack unwinding (where the ABI runs cleanups).
- `defer` bodies execute **before** implicit ARC releases of strong locals in the same scope ([Part 8](#part-8)).

### 5.2.3 Restrictions (normative) <a id="part-5-2-3"></a>
A deferred body shall not perform a non-local exit from the enclosing scope. The following are ill‑formed inside a `defer` body:
- `return`, `break`, `continue`, `goto` that exits the enclosing scope,
- `throw` that exits the enclosing scope,
- C++ `throw` that exits the scope.

Rationale: defers must be reliably executed as cleanups; permitting non-local exits leads to un-auditable control flow.

### 5.2.4 Interaction with `async` <a id="part-5-2-4"></a>
A `defer` body in an `async` function executes when the scope exits, even if the function suspends/resumes multiple times ([Part 7](#part-7) [§7.9.2](#part-7-9-2)).

## 5.3 `guard` <a id="part-5-3"></a>

### 5.3.1 Syntax <a id="part-5-3-1"></a>
```text
guard-statement:
    'guard' guard-condition-list 'else' compound-statement
```

A `guard-condition-list` is a comma-separated list of:
- boolean expressions, and/or
- optional bindings (`let`/`var`) as defined in [Part 3](#part-3).

Example:
```objc
guard let user = maybeUser, user.isActive else {
  return nil;
}
```

### 5.3.2 Semantics (normative) <a id="part-5-3-2"></a>
- Conditions are evaluated left-to-right.
- If all conditions succeed, execution continues after the `guard`.
- If any condition fails, the `else` block executes.

### 5.3.3 Mandatory exit (normative) <a id="part-5-3-3"></a>
The `else` block of a `guard` statement shall perform a control-flow exit from the current scope. Conforming exits include:
- `return` (from the current function),
- `throw` (from the current `throws` function),
- `break` / `continue` (from the current loop),
- `goto` to a label outside the current scope.

If the compiler cannot prove that all control-flow paths in the `else` block exit, the program is ill‑formed.

### 5.3.4 Refinement <a id="part-5-3-4"></a>
Names proven by guard conditions are *refined* after the guard:
- Optional bindings introduce nonnull values in the following scope.
- Null checks refine nullable variables to nonnull within the post-guard region ([Part 3](#part-3)).

## 5.4 `match` <a id="part-5-4"></a>

### 5.4.1 Motivation <a id="part-5-4-1"></a>
Objective‑C has `switch`, but it is limited:
- it does not compose well with `Result`/error carriers,
- it does not bind values safely,
- it does not support exhaustiveness checking.

`match` is designed to be:
- explicit,
- analyzable,
- and lowerable to a switch/if-chain without allocations.

### 5.4.2 Syntax (normative) <a id="part-5-4-2"></a>
```text
match-statement:
    'match' '(' expression ')' ' match-case+ match-default? '

match-case:
    'case' pattern ':' compound-statement

match-default:
    'default' ':' compound-statement
```

Notes:
- `match` is a statement (not an expression) in v1.
- Fallthrough is not permitted.

### 5.4.3 Evaluation order (normative) <a id="part-5-4-3"></a>
- The scrutinee expression in `match (expr)` shall be evaluated exactly once.
- Case patterns are tested top-to-bottom.
- The first matching case is selected and its body executes.

### 5.4.4 Scoping (normative) <a id="part-5-4-4"></a>
Bindings introduced by a case pattern are scoped to that case body only.

### 5.4.5 Exhaustiveness (normative) <a id="part-5-4-5"></a>
In strict mode, `match` must be exhaustive when the compiler can prove the matched type is a *closed set*.

The following are treated as closed sets in v1:
- `Result<T, E>` (cases `Ok` and `Err`),
- other library-defined closed sum types explicitly marked as closed (future extension point).

For closed sets:
- Either all cases must be present, or a `default` must be present.
- Missing cases are an error in strict mode (warning in permissive mode).

### 5.4.6 Lowering (normative) <a id="part-5-4-6"></a>
`match` shall lower to:
- a switch/if-chain that preserves evaluation order and side effects,
- with no heap allocation required by `match` itself.

## 5.5 Patterns (v1) <a id="part-5-5"></a>

### 5.5.1 Wildcard <a id="part-5-5-1"></a>
`_` matches anything and binds nothing.

### 5.5.2 Literal constants <a id="part-5-5-2"></a>
Literal constants match values of compatible type:
- integers, characters,
- `@"string"` for Objective‑C string objects (pointer equality is not required; matching is defined for `Result` and closed types only in v1),
- `nil` for nullable pointer types.

> Note: Rich structural matching for arbitrary objects is intentionally not part of v1.

### 5.5.3 Binding pattern <a id="part-5-5-3"></a>
A binding pattern introduces a name:

- `let name`
- `var name`

In v1:
- `let` bindings are immutable within the case body.
- `var` bindings are mutable within the case body.

### 5.5.4 `Result` case patterns <a id="part-5-5-4"></a>
`Result<T, E>` participates in pattern matching ([Part 6](#part-6)).

The following case patterns are defined:
- `.Ok(let value)`
- `.Err(let error)`

Example:
```objc
match (r) {
  case .Ok(let v): { use(v); }
  case .Err(let e): { log(e); }
}
```

### 5.5.5 Type-test patterns (optional in v1) <a id="part-5-5-5"></a>
Implementations may provide a type-test pattern form:

- `is Type`
- `is Type let name` (bind as refined type)

If provided:
- For class types, it matches when the dynamic type is `Type` or a subclass.
- For protocol types, it matches when the value conforms to the protocol.

> This feature is optional in v1 because it intersects with Objective‑C runtime reflection and optimization.

## 5.6 Required diagnostics <a id="part-5-6"></a>
Minimum diagnostics include:
- non-exiting `guard else` blocks (error),
- `defer` bodies containing non-local exits (error),
- unreachable `match` cases (warning),
- non-exhaustive `match` over `Result` in strict mode (error; fix-it: add missing case or `default`).

## 5.7 Open issues <a id="part-5-7"></a>
- Whether `match` should also exist as an expression form (returning a value) in a future revision.
- Whether to standardize type-test patterns and how they should lower (runtime checks vs static reasoning).
- Whether to add guarded patterns (e.g., `case pat where condition:`) once keyword reservation is settled.
<!-- END PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md -->

---

<!-- BEGIN PART_6_ERRORS_RESULTS_THROWS.md -->
# Part 6 — Errors: Result, throws, try, and Propagation <a id="part-6"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 6.0 Overview <a id="part-6-0"></a>

### v0.9 resolved decisions <a id="part-6-v0-9-resolved-decisions"></a>
- The required module metadata set for preserving `throws` across module boundaries is enumerated in D.
- `try`/`throw`/`throws` are explicitly specified to be compile-time effects; runtime reflection of `throws` is not required in v1.


### v0.8 resolved decisions <a id="part-6-v0-8-resolved-decisions"></a>
- `throws` uses an untyped error value: `id<Error>`.
- Postfix optional propagation `e?` is carrier-preserving and is only valid in optional-returning functions (no implicit nil→error mapping).
- Conforming implementations provide a stable calling convention for `throws` across module boundaries ([C.4](#c-4)).


### v0.5 resolved decisions <a id="part-6-v0-5-resolved-decisions"></a>
- Optional propagation `e?` for `e : T?` is valid **only** in optional-returning functions (carrier-preserving). It is ill‑formed in `throws` or `Result` contexts (no implicit nil→error mapping).


### v0.4 resolved decisions <a id="part-6-v0-4-resolved-decisions"></a>
- `throws` is **untyped** in v1: thrown values are `id<Error>`.
- Typed throws syntax `throws(E)` is reserved for future extension and is not part of v1 grammar.

Objective‑C 3.0 standardizes a modern, explicit error model that can be used in new code while interoperating with existing Cocoa and system APIs.

This part defines:

- A standard error protocol `Error`.
- A `throws` effect for methods/functions (typed at least as `id<Error>`).
- `throw` statement and `try` expressions.
- A `do`/`catch` statement for handling thrown errors.
- A standard `Result<T, E>` carrier type.
- A postfix **propagation** operator `?` for `Result` and optionals (with disambiguation rules suitable for C/ObjC syntax).
- Interoperability rules for NSError-out-parameter patterns and return-code APIs.

This error system is designed to be implementable without relying on Objective‑C exceptions (`@throw/@try/@catch`), which remain separate.

---

## 6.1 Lexical and grammar additions <a id="part-6-1"></a>

### 6.1.1 New keywords <a id="part-6-1-1"></a>
In ObjC 3.0 mode, the following are reserved by this part:

- `throws`, `throw`, `try`, `do`, `catch`

### 6.1.2 Grammar summary (high level) <a id="part-6-1-2"></a>
This part introduces:
- `throws` as a function/method specifier.
- `throw` as a statement.
- `try`, `try?`, `try!` as expressions.
- `do { ... } catch ...` as a statement.
- `?` as a postfix propagation operator in limited contexts (see [§6.6](#part-6-6)).

---

## 6.2 The `Error` protocol and error values <a id="part-6-2"></a>

### 6.2.1 Definition <a id="part-6-2-1"></a>
Objective‑C 3.0 defines a standard protocol:

```objc
@protocol Error
@end
```

An *error value* is any Objective‑C object value that conforms to `Error`.

### 6.2.2 Bridging to NSError <a id="part-6-2-2"></a>
For platforms with Foundation:
- `NSError` shall be treated as conforming to `Error`.

Implementations may provide:
- implicit bridging from common error representations (e.g., status codes) into error objects via library hooks.

> Note: The language does not require Foundation, but defines the protocol so multiple ecosystems can conform.

---

## 6.3 `throws` effect <a id="part-6-3"></a>

### 6.3.1 Grammar <a id="part-6-3-1"></a>
A function or method declaration may include a `throws` specifier.

Provisional grammar (illustrative):
```text
function-declaration:
    declaration-specifiers declarator throws-specifier? function-body

throws-specifier:
    'throws'
```


### 6.3.2 Semantics <a id="part-6-3-2"></a>
A `throws` function may either:
- return normally with its declared return value, or
- exit by throwing an error value.

The thrown value type is `id<Error>`.

### 6.3.3 Call-site requirements <a id="part-6-3-3"></a>
A call to a throwing function is ill-formed unless it appears:
- within a `try` expression, or
- within a context that explicitly handles the error (e.g., bridging to Result).

### 6.3.4 Function and block types <a id="part-6-3-4"></a>


A throwing function’s type is distinct from a non-throwing function’s type.

Examples (illustrative):
- `R (^)(Args) throws` is a throwing block type.
- `R (^)(Args)` is a non-throwing block type.

**Conversion rules (normative intent):**
- A non-throwing function/block value may be implicitly converted to a throwing type (it never throws).
- A throwing function/block value shall not be implicitly converted to a non-throwing type.
  Toolchains may provide an explicit adapter helper that converts a throwing callable into a non-throwing callable by handling errors (e.g., by trapping, by mapping to `Result`, or by returning an optional), but such adapters must be explicit at the call site.

`async` and `throws` compose: `async throws` is a distinct combined effect set.

### 6.3.5 ABI and lowering (normative for implementations) <a id="part-6-3-5"></a>
Source-level semantics for `throws` are defined in this part.
In addition, conforming implementations shall ensure `throws` is stable under separate compilation ([C.2](#c-2)).

A conforming implementation shall:
- record the `throws` effect in module metadata (see also [D.3.1](#d-3-1) [Table A](#d-3-1)),
- diagnose effect mismatches on redeclaration/import ([C.2](#c-2)),
- and provide a stable calling convention for throwing functions/methods.

The recommended calling convention is the *trailing error-out parameter* described in [C.4](#c-4).

> Note: This draft intentionally chooses an error-out convention

**Reflection note (non-normative):** v1 does not require encoding `throws` in Objective‑C runtime type encodings. Toolchains may provide extended metadata as an extension.
 rather than stack unwinding, to keep interoperability with NSError/return-code APIs straightforward and to align with existing Objective‑C runtime practices.


Throwing is part of a function’s type.

- `R (^)(Args) throws` is a throwing block type.
- `R (^)(Args)` is non-throwing.

A throwing function value cannot be assigned to a non-throwing function type without an explicit adapter.


---

## 6.4 `throw` statement <a id="part-6-4"></a>

### 6.4.1 Grammar <a id="part-6-4-1"></a>
```text
throw-statement:
    'throw' expression ';'
```

### 6.4.2 Static semantics <a id="part-6-4-2"></a>
A `throw` statement is permitted only within a `throws` function or within a `catch` block.

The thrown expression must be convertible to:
- `id<Error>` for untyped throws, or

### 6.4.3 Dynamic semantics <a id="part-6-4-3"></a>
Executing `throw e;`:
- evaluates `e`,
- exits the current function by the error path,
- executes scope-exit actions (`defer`, resource cleanups) as specified in [Part 8](#part-8).

> Note: This is not Objective‑C exception throwing. It is a structured error return path.

---

## 6.5 `try` expressions <a id="part-6-5"></a>

### 6.5.1 Grammar <a id="part-6-5-1"></a>
```text
try-expression:
    'try' expression
  | 'try' '?' expression
  | 'try' '!' expression
```

### 6.5.2 `try e` <a id="part-6-5-2"></a>
`try e` evaluates `e` in a context that may throw.
- If `e` completes normally, `try e` yields its value.
- If `e` throws, the error is propagated to the caller, and the enclosing function must be `throws`.

### 6.5.3 `try? e` <a id="part-6-5-3"></a>
`try? e` converts a throwing evaluation into an optional result:
- If `e` succeeds, yields the value.
- If `e` throws, yields `nil` and discards the error.

In strict mode, discarding errors should produce a warning unless explicitly marked as intentional.

### 6.5.4 `try! e` <a id="part-6-5-4"></a>
`try! e` forces a throwing evaluation:
- If `e` succeeds, yields the value.
- If `e` throws, the program traps.

### 6.5.5 Typing <a id="part-6-5-5"></a>
- `try e` has the type of `e`.
- `try? e` has optional type of `e` (i.e., `T?`).
- `try! e` has the type of `e` but is unsafe in strict mode unless proven not to throw.

---

## 6.6 Postfix propagation operator `?` (Result / Optional) <a id="part-6-6"></a>

### 6.6.1 Purpose <a id="part-6-6-1"></a>
The postfix propagation operator provides Rust-like ergonomics for **carrier types** without relying on exceptions:

- `Result<T, E>`: unwrap or early-return error.
- `T?`: unwrap or early-return `nil` (or throw) depending on surrounding function’s declared return/effects.

This operator is intentionally limited to avoid conflict with C’s conditional operator `?:`.

### 6.6.2 Grammar and disambiguation <a id="part-6-6-2"></a>
The propagation operator is a postfix `?` applied to a **postfix-expression**, with a syntactic restriction:

- It may only appear when the next token is one of:
  - `')'`, `']'`, `'}'`, `','`, `';'`

Grammar:
```text
propagate-expression:
    postfix-expression '?'
```

> Note: This means `foo()? + 1` must be written as `(foo()?) + 1`.

### 6.6.3 Semantics for `Result` <a id="part-6-6-3"></a>
If `e` has type `Result<T, E>` then `e?` behaves as:
- if `Ok(t)`, yield `t`,
- if `Err(err)`, early-exit from the innermost function using that function’s error model:

Mapping:
- returns `Result<_, E>` → `return Err(err);`
- `throws` → `throw err;`
- otherwise ill-formed.

### 6.6.4 Semantics for optionals <a id="part-6-6-4"></a>
If `e` has type `T?` then `e?` yields `T` if nonnull, else early-exits.

Carrier rule (normative):
- `e?` is permitted only if the innermost enclosing function’s return type is an optional type.
- In that case, if `e` is `nil`, execution performs `return nil;` from that function.
- Otherwise `e?` yields the unwrapped `T`.

If the enclosing function is `throws` or returns `Result<…>`, use of `e?` is **ill‑formed** in v1.
Convert explicitly using `guard let … else { throw … }` or an explicit helper such as `ok_or(...)`.

### 6.6.5 Diagnostics <a id="part-6-6-5"></a>
- Using `?` outside the follow-token restriction is ill-formed (fix-it: parenthesize).
- Using `?` without compatible early-exit carrier is ill-formed.

---

## 6.7 `do` / `catch` <a id="part-6-7"></a>

### 6.7.1 Grammar <a id="part-6-7-1"></a>
```text
do-statement:
    'do' compound-statement catch-clauses

catch-clauses:
    catch-clause+
catch-clause:
    'catch' catch-pattern? compound-statement

catch-pattern:
    '(' type-name identifier? ')'
```

### 6.7.2 Semantics <a id="part-6-7-2"></a>
- The `do` block establishes a handler for thrown errors.
- On throw, transfer to first matching catch; otherwise propagate outward.

### 6.7.3 Catch matching <a id="part-6-7-3"></a>
A catch pattern `(T e)` matches if the thrown value:
- is an instance of `T` (class),
- conforms to `T` (protocol),
- or bridges to `T` via defined conversions.

A bare `catch { ... }` matches any error.

---

## 6.8 `Result<T, E>` standard type <a id="part-6-8"></a>

### 6.8.1 Definition <a id="part-6-8-1"></a>
The standard library shall provide a closed two-case Result type:

```text
Result<T, E> = Ok(T) | Err(E)
```

### 6.8.2 Requirements <a id="part-6-8-2"></a>
- Construction and inspection shall be possible without allocation where possible.
- `Result` participates in pattern matching ([Part 5](#part-5)) and propagation (`?`).

---

## 6.9 Interoperability: NSError-out-parameter conventions <a id="part-6-9"></a>

### 6.9.1 Recognizing an NSError-throwing signature <a id="part-6-9-1"></a>
Eligible if:
- final parameter is `NSError **` (or equivalent),
- annotated as error-out parameter (attribute or convention),
- return type is `BOOL` or object pointer.

### 6.9.2 Standard attribute <a id="part-6-9-2"></a>
Canonical attribute: `__attribute__((objc_nserror))` (see [B.4.1](#b-4-1)).

### 6.9.3 `try` lowering for NSError-bridged calls <a id="part-6-9-3"></a>
Compiler shall:
- synthesize temporary error out parameter,
- call,
- on failure, throw error or generic error if missing,
- on success, return value.

---

## 6.10 Interoperability: return-code APIs <a id="part-6-10"></a>

### 6.10.1 Status attribute <a id="part-6-10-1"></a>
Canonical attribute: `__attribute__((objc_status_code(success: constant, error_type: Type, mapping: Function)))` (see [B.4.2](#b-4-2)).

### 6.10.2 Bridging <a id="part-6-10-2"></a>
- Under `try`: throw on non-success.
- Under Result overlay: return `Err(mapped)`.

---

## 6.11 Required diagnostics <a id="part-6-11"></a>
Minimum diagnostics:
- calling throwing function without `try` (error),
- `throw` outside throws (error),
- misuse of postfix `?` (error with fix-it), including using `T?` propagation in `throws`/`Result` contexts,
- `try!` without proof (warning/error policy in strict).

---

## 6.12 Open issues <a id="part-6-12"></a>
- Whether to introduce a standard attribute or syntax for “never throws” in generic contexts (mostly redundant with absence of `throws`).
- Whether to standardize a nil→error mapping helper (e.g., `orThrow(...)`) in the standard library rather than as language sugar.
- Whether to add typed throws in a future revision (future extension).


## 6.13 Future extensions (non-normative) <a id="part-6-13"></a>

### 6.13.1 Typed throws <a id="part-6-13-1"></a>
A future revision may introduce typed throws syntax (e.g., `throws(E)`) to restrict the set of throwable error types.
This is explicitly **not** part of Objective‑C 3.0 v1 to keep the core error model simple and interoperable with NSError and return-code APIs.
<!-- END PART_6_ERRORS_RESULTS_THROWS.md -->

---

<!-- BEGIN PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md -->
# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors <a id="part-7"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 7.0 Overview <a id="part-7-0"></a>

### v0.9 resolved decisions <a id="part-7-v0-9-resolved-decisions"></a>
- `await` is required for any potentially suspending operation, including cross-executor and cross-actor entry ([Decision D-011](#decisions-d-011)).
- Required module metadata for concurrency/isolation is enumerated in D.


### v0.8 resolved decisions <a id="part-7-v0-8-resolved-decisions"></a>
- Canonical attribute spellings for concurrency/executor features are defined in B.
- Lowering/runtime contracts for `async/await`, executors, and actors are defined in C.


### v0.5 resolved decisions <a id="part-7-v0-5-resolved-decisions"></a>
- Executor annotations use a canonical Clang-style spelling `__attribute__((objc_executor(...)))`.
- Autorelease pool behavior at suspension points is normative: each task execution slice runs inside an implicit pool drained on suspend and completion.


### v0.4 resolved decisions <a id="part-7-v0-4-resolved-decisions"></a>
- v1 does **not** add `task {}` keyword syntax. Task spawning is provided by the standard library and recognized by the compiler via standardized attributes.

Objective‑C 3.0 introduces structured concurrency that is composable, auditable, and implementable.

This part defines:
- the `async` effect and `await` expression,
- executor model and annotations,
- structured tasks and cancellation (standard library contract),
- actor isolation,
- sendable-like checking as an opt-in safety ladder.

---

## 7.1 Lexical and grammar additions <a id="part-7-1"></a>

### 7.1.1 New keywords <a id="part-7-1-1"></a>
In ObjC 3.0 mode, the following are reserved by this part:

- `async`, `await`, `actor`

### 7.1.2 Effects and isolation <a id="part-7-1-2"></a>
`async` and `throws` are effects that become part of a function’s type.

Executor affinity (`objc_executor(...)`) and actor isolation are **isolation annotations** that may cause a call to **suspend at the call site** even when the callee is not explicitly `async` ([Decision D-011](#decisions-d-011)).

---

## 7.2 Async declarations <a id="part-7-2"></a>

### 7.2.1 Grammar (illustrative) <a id="part-7-2-1"></a>
```text
function-effect-specifier:
    'async'
  | 'throws'
  | 'async' 'throws'
  | 'throws' 'async'
```

### 7.2.2 Static semantics <a id="part-7-2-2"></a>
- `await` is permitted only in async contexts.
- Any **potentially suspending** operation (defined in [§7.3.2](#part-7-3-2)) shall appear within an `await` expression.

### 7.2.3 Block types <a id="part-7-2-3"></a>
Async is part of a block’s type:
- `R (^)(Args) async`
- `R (^)(Args)` (sync)

---

## 7.3 `await` expressions <a id="part-7-3"></a>

### 7.3.1 Grammar <a id="part-7-3-1"></a>
```text
await-expression:
    'await' expression
```

### 7.3.2 Static semantics (normative) <a id="part-7-3-2"></a>
A translation unit is ill-formed if it contains a **potentially suspending operation** that is not within the operand of an `await` expression.

The following are potentially suspending operations:
1. A call to a declaration whose type includes the `async` effect.
2. A cross-executor entry into a declaration annotated `objc_executor(X)` when the compiler cannot prove the current executor is `X`.
3. A cross-actor entry to an actor-isolated member when the compiler cannot prove it is already executing on that actor’s executor.
4. A task-join / task-group operation that is specified by the standard library contract to be `async`.

`await` applied to an expression that is not potentially suspending is permitted, but a conforming implementation should warn in strict modes (“unnecessary await”).

### 7.3.3 Dynamic semantics <a id="part-7-3-3"></a>
`await e` may suspend:
- suspension returns control to the current executor,
- later resumes on an executor consistent with isolation rules.

In particular, `await` may represent:
- suspending to call an `async` function,
- an executor hop to satisfy `objc_executor(...)`,
- or scheduling onto an actor’s serial executor.

---

## 7.4 Executors <a id="part-7-4"></a>

### 7.4.1 Concept <a id="part-7-4-1"></a>
An executor schedules task continuations.

### 7.4.2 Default executors <a id="part-7-4-2"></a>
Runtime shall provide:
- main executor
- global executor


### 7.4.3 Executor annotations (normative) <a id="part-7-4-3"></a>

Objective‑C 3.0 defines executor affinity using a standardized attribute spelling.

#### 7.4.3.1 Canonical spelling <a id="part-7-4-3-1"></a>
Executor affinity shall be expressed using a Clang-style attribute:

- `__attribute__((objc_executor(main)))`
- `__attribute__((objc_executor(global)))`
- `__attribute__((objc_executor(named("..."))))`

The attribute may be applied to:
- functions,
- Objective‑C methods,
- Objective‑C types (including actor classes), establishing a default executor for isolated members unless overridden.

#### 7.4.3.2 Static semantics (normative) <a id="part-7-4-3-2"></a>
If a declaration is annotated `objc_executor(X)`, then entering that declaration from a context not proven to already be on executor `X` is a **potentially suspending operation** ([§7.3.2](#part-7-3-2)) and therefore requires `await`.

In strict concurrency checking mode, the compiler shall reject a call to an executor-annotated declaration unless either:
- the call is within an `await` operand, or
- the compiler can prove the current execution is already on executor `X`.

#### 7.4.3.3 Dynamic semantics (model) <a id="part-7-4-3-3"></a>
An implementation shall ensure that execution of an executor-annotated declaration occurs on the specified executor.

When a call is type-checked as crossing an executor boundary, the implementation may insert an implicit hop **provided the call site is explicitly marked with `await`**.
The observable effect is that executor-affine code never runs on an incorrect executor, and the call is treated as potentially suspending.

#### 7.4.3.4 Notes <a id="part-7-4-3-4"></a>
- `main` is intended for UI-thread-affine work.
- `global` is intended for background execution.
- `named("...")` is for custom executors defined by the runtime/standard library.


---


## 7.5 Tasks and structured concurrency (standard library contract) <a id="part-7-5"></a>

### 7.5.1 Design decision (v1) <a id="part-7-5-1"></a>
Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided by the **standard library**, but the compiler shall recognize task-related APIs via attributes so that:
- Sendable-like checking can be enforced for concurrent captures,
- cancellation inheritance and “child vs detached” semantics are predictable,
- diagnostics can be issued for misuse (e.g., discarding a Task handle).

### 7.5.2 Standard attributes (normative) <a id="part-7-5-2"></a>

#### 7.5.2.1 `__attribute__((objc_task_spawn))` <a id="part-7-5-2-1"></a>
Applied to a function/method that creates a **child task**.

Requirements:
- The spawned task shall inherit:
  - the current cancellation state,
  - executor context (unless the API explicitly overrides executor),
  - and (optionally) priority metadata.
- The task is considered a child of the current task for cancellation propagation.

#### 7.5.2.2 `__attribute__((objc_task_detached))` <a id="part-7-5-2-2"></a>
Applied to a function/method that creates a **detached task**.

Requirements:
- Detached tasks do **not** inherit cancellation by default.
- Executor selection is implementation-defined unless explicitly specified.

#### 7.5.2.3 `__attribute__((objc_task_group))` <a id="part-7-5-2-3"></a>
Applied to a function/method that establishes a structured task group scope.

Requirements:
- All tasks added to the group shall complete (or be cancelled) before the group scope returns.
- If the group scope exits by throwing, the group shall cancel remaining tasks before unwinding completes.

### 7.5.3 Required standard library surface (abstract) <a id="part-7-5-3"></a>

A conforming implementation shall provide, in a standard concurrency module, APIs equivalent in expressive power to:

- **Spawn child task**
  - input: an `async` block (possibly throwing)
  - output: a task handle representing the child task

- **Spawn detached task**
  - same, but detached semantics

- **Join**
  - `await`-join a task handle to retrieve its result (or throw its error)

- **Task groups**
  - create group, add tasks, iterate results, join implicitly at scope end

The exact naming and Objective‑C surface syntax are implementation-defined, but the compiler shall be able to identify the spawn/group APIs via the attributes above.

### 7.5.4 Compiler checking hooks <a id="part-7-5-4"></a>

In strict concurrency checking mode:
- The compiler shall enforce Sendable-like requirements for values captured by `__attribute__((objc_task_spawn))` / `__attribute__((objc_task_detached))` async blocks.
- The compiler should warn when a returned task handle is unused (likely “fire and forget”); suggest using the detached API explicitly or awaiting/joining.

> Note: A future revision may add `task { ... }` as sugar (possibly via macros) once patterns stabilize.


## 7.6 Cancellation <a id="part-7-6"></a>

- cooperative cancellation
- propagation to child tasks
- cancellation check function(s)
- cancellation must still run defers/cleanups

---

## 7.7 Actors <a id="part-7-7"></a>

### 7.7.1 Actor types <a id="part-7-7-1"></a>
```objc
actor class Name : NSObject
@end
```

### 7.7.2 Isolation <a id="part-7-7-2"></a>
Actors provide data-race safety by associating each actor instance with a **serial executor** and treating the actor’s isolated mutable state as accessible only while running on that executor.

Informally: *outside code must hop onto the actor to touch its state.*

### 7.7.2.1 Actor-isolated members (normative) <a id="part-7-7-2-1"></a>
Unless otherwise specified, instance methods and instance properties of an `actor class` are **actor-isolated**.

A reference to an actor-isolated member from outside the actor’s isolation domain is a **potentially suspending operation** ([§7.3.2](#part-7-3-2)) and therefore requires `await`.

Example (illustrative):
```objc
actor class Counter : NSObject
@property (atomic) NSInteger value;
- (void)increment;
@end

async void f(Counter *c) {
  await [c increment];         // may hop to c's executor
  NSInteger v = await c.value; // may hop; read is isolated
}
```

Within actor-isolated code already executing on the actor’s executor, `await` is not required for isolated member access unless the member is explicitly `async`.



### 7.7.3 Reentrancy <a id="part-7-7-3"></a>
An `await` inside actor-isolated code may suspend the current task while still preserving the actor’s serial executor.
While suspended, other enqueued work on the same actor may run (i.e., **reentrancy** is possible).

Guidance (normative intent):
- Actor-isolated invariants must hold across `await` points.
- If code requires a non-reentrant critical region, it should avoid `await` while holding that invariant, or use explicit design patterns (e.g., copy state to locals before awaiting).


### 7.7.4 Nonisolated members (normative) <a id="part-7-7-4"></a>
A member may be declared **nonisolated** to indicate it is safe to call without actor isolation.

Canonical spelling (attribute):
```objc
actor class Logger : NSObject
- (void)log:(NSString *)message __attribute__((objc_nonisolated));
@end
```

Rules:
- A `nonisolated` member shall not access actor-isolated mutable state without an explicit hop.
- Parameters and return types of `nonisolated` members that cross concurrency domains should satisfy Sendable-like checking in strict mode.

> Note: Toolchains may additionally accept a contextual keyword spelling (`nonisolated`) as sugar, but emitted interfaces should use the canonical attribute spelling ([B.3.4](#b-3-4)).



---

## 7.8 Sendable-like checking <a id="part-7-8"></a>

### 7.8.1 Marker protocol <a id="part-7-8-1"></a>
```objc
@protocol Sendable
@end
```

### 7.8.2 Enforcement sites <a id="part-7-8-2"></a>
- captures into concurrent tasks
- cross-actor arguments/returns

Provide `__attribute__((objc_unsafe_sendable))` escape hatch ([B.3.3](#b-3-3)).

---


## 7.9 Interactions <a id="part-7-9"></a>

### 7.9.1 `try await` composition <a id="part-7-9-1"></a>
When calling an `async throws` function, both effects must be handled:
- `await` handles the async effect (may suspend),
- `try` handles the throws effect (may throw).

A conforming implementation shall accept `try await f(...)` as the canonical order.

### 7.9.2 `defer` and scope cleanup across suspension <a id="part-7-9-2"></a>
Defers registered in an async function scope execute when that scope exits, even if the function has suspended and resumed multiple times.

### 7.9.3 ARC lifetimes across suspension <a id="part-7-9-3"></a>
Objects referenced across an `await` must remain alive until no longer needed. Implementations shall retain values captured into an async frame and release them when the frame is destroyed (normal completion, throw, or cancellation unwind).

### 7.9.4 Autorelease pools at suspension points (normative on ObjC runtimes) <a id="part-7-9-4"></a>
On platforms that support Objective‑C autorelease pools:

- Each **task execution slice** (from a resume point until the next suspension or completion) shall execute within an **implicit autorelease pool**.
- If an `await` suspends the current task, the implementation shall drain the implicit autorelease pool **before** the task suspends and control returns to the executor.
- The implementation shall drain the implicit pool at task completion (normal return, thrown error, or cancellation unwind).

If an `await` completes synchronously without suspending, draining is permitted but not required.

Implementations may drain at additional safe points, but shall not permit unbounded autorelease pool growth across long-lived tasks that repeatedly suspend.


---



## 7.12 Lowering, ABI, and runtime hooks (normative for implementations) <a id="part-7-12"></a>

### 7.12.1 Separate compilation requirements <a id="part-7-12-1"></a>
Conforming implementations shall satisfy the separate compilation requirements in [C.2](#c-2) and the required metadata set in [D](#d) for all concurrency-relevant declarations, including:
- `async` effects,
- executor annotations (`objc_executor(...)`),
- actor isolation metadata,
- and task-spawn recognition attributes ([B.3.2](#b-3-2)).

### 7.12.2 Coroutine lowering model (informative summary) <a id="part-7-12-2"></a>
The recommended lowering model is:
- `async` functions lower to coroutine state machines ([C.5](#c-5)),
- `await` lowers to a potential suspension + resumption scheduled by the current executor,
- values that cross suspension are retained/lowered in a way that preserves ARC semantics.

### 7.12.3 Executor hop primitive <a id="part-7-12-3"></a>
Implementations should provide (directly or via the standard library) a primitive that represents:
- “suspend here and resume on executor X”

so that the compiler can uniformly lower:
- calls into `objc_executor(main)` declarations, and
- actor isolation hops.

### 7.12.4 Actor runtime representation (minimum) <a id="part-7-12-4"></a>
Each actor instance shall be associated with a serial executor that:
- enqueues actor-isolated work items, and
- ensures mutual exclusion for isolated mutable state.

The concrete queue representation is implementation-defined, but the association must be observable by the runtime/stdlib to implement `await`ed cross-actor calls.

### 7.12.5 Autorelease pools at suspension points <a id="part-7-12-5"></a>
Autorelease pool boundaries at suspension points are required by [Decision D-006](#decisions-d-006) and defined normatively in [C.7](#c-7) and [§7.9.4](#part-7-9-4).


## 7.10 Diagnostics <a id="part-7-10"></a>
Minimum diagnostics:
- `await` outside async (error)
- calling async without await (error)
- crossing an executor boundary into `objc_executor(X)` without `await` when a hop may be required (error in strict concurrency mode)
- cross-actor isolated access without `await` when a hop may be required (error in strict)
- Sendable violations in strict concurrency mode (error)
- unused task handles returned from `__attribute__((objc_task_spawn))` APIs (warning in strict concurrency mode)



## 7.11 Open issues <a id="part-7-11"></a>
- Whether to standardize a macro-based sugar for task spawning (keeping the core library-defined) without freezing semantics too early.
- Whether to make actor reentrancy controls explicit in the type system (e.g., `nonreentrant` markers) in a future revision.
- Whether to standardize a portable interface format for concurrency metadata beyond module files (see [Part 2](#part-2)).
<!-- END PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md -->

---

<!-- BEGIN PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md -->
# Part 8 — System Programming Extensions <a id="part-8"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 8.0 Scope and goals <a id="part-8-0"></a>
This part defines language features and attributes designed to accommodate “library-defined subsets” and system APIs, with improved safety, ergonomics, and performance predictability:

- Handle-based resources with strict cleanup requirements (IOKit, Mach).
- C libraries whose objects become ARC-managed under Objective‑C compilation (dispatch, XPC).
- CoreFoundation-style retainable APIs and ownership transfer attributes.
- Borrowed interior-pointer APIs that require explicit lifetime extension.
- Block capture patterns that commonly cause retain cycles (event handlers, callbacks).

This part intentionally provides *detailed normative semantics* because system-library correctness depends on precise ordering.



## 8.0.1 Canonical attribute spellings (header interoperability) <a id="part-8-0-1"></a>

**Normative note:** The canonical spellings for all attributes and pragmas used in this part are cataloged in **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)**. Inline spellings here are illustrative; emitted interfaces and module metadata shall use the catalog spellings.
Many system-facing features in this part are intended to be expressed in headers and preserved by module interfaces. Per [Decision D-007](#decisions-d-007), a conforming implementation shall support canonical `__attribute__((...))` spellings for these features, so that:
- APIs can be declared without macro folklore,
- analyzers can reason about semantics,
- and mixed ObjC/ObjC++ builds remain source-compatible.

This part uses ergonomic `@resource(...)`/`@cleanup(...)`/`borrowed` spellings in examples; implementations may accept these as ObjC 3.0 sugar. The canonical header spellings are described inline where relevant.

---

## 8.1 Unified scope-exit action model <a id="part-8-1"></a>
Within a lexical scope, the abstract machine maintains a stack of scope-exit actions. Actions are registered when:
- a `defer` statement executes, or
- a resource variable finishes successful initialization.

On scope exit, actions execute in reverse order of registration (LIFO).

Scope exit includes:
- fallthrough end of scope
- `return`, `break`, `continue`, `goto` leaving the scope
- stack unwinding due to ObjC/C++ exceptions (if applicable)

Non-local unwinding mechanisms that do not run cleanups (e.g., `longjmp`) are outside the guarantees of this part; crossing a cleanup scope using such mechanisms is undefined unless the platform ABI states otherwise.

---

## 8.2 `defer` (normative) <a id="part-8-2"></a>
### 8.2.1 Syntax <a id="part-8-2-1"></a>
```text
defer compound-statement
```

### 8.2.2 Semantics <a id="part-8-2-2"></a>
Executing a `defer` registers its body as a scope-exit action in the innermost enclosing lexical scope.

### 8.2.3 Ordering with ARC releases <a id="part-8-2-3"></a>
In a scope:
- `defer` actions shall execute **before** implicit ARC releases of strong locals in that scope.

Rationale: ensures cleanup code can safely reference locals without surprising early releases.

### 8.2.4 Restrictions <a id="part-8-2-4"></a>
A deferred body shall not contain a statement that performs a non-local exit from the enclosing scope. Such code is ill-formed.

---

## 8.3 Resource values and cleanup <a id="part-8-3"></a>

### 8.3.1 `@cleanup(F)` <a id="part-8-3-1"></a>
A local variable may be annotated with a cleanup function.

**Canonical header spelling (illustrative):**
- `__attribute__((cleanup(F)))` on the local variable.

A local variable may be annotated with a cleanup function:

```objc
@cleanup(F) Type name = initializer;
```

Semantics:
- after successful initialization, register a scope-exit action that calls `F` with the variable’s value (or address, if applicable) at scope exit.

### 8.3.2 `@resource(F, invalid: X)` <a id="part-8-3-2"></a>
A `@resource` variable has a cleanup function and an invalid sentinel.

**Canonical header spelling (illustrative):**
- `__attribute__((objc_resource(close=F, invalid=X)))` on the local variable.

A `@resource` variable has a cleanup function and an invalid sentinel:

```objc
@resource(CloseFn, invalid: 0) io_connect_t conn = 0;
```

At scope exit:
- if `conn != 0`, call `CloseFn(conn)`;
- set `conn` to invalid after cleanup (conceptually).

### 8.3.3 Cleanup signature matching <a id="part-8-3-3"></a>
Cleanup function `F` is applicable if it can be called as:
- `F(value)` or `F(&value)`.

### 8.3.4 Move support for resources <a id="part-8-3-4"></a>
#### 8.3.4.1 `take(x)` <a id="part-8-3-4-1"></a>
`take(x)` yields the current value of resource variable `x` and sets `x` to its invalid sentinel.

Using `x` after `take` yields the invalid value; in strict-system mode, using it as if valid without reinitialization is diagnosed.

#### 8.3.4.2 `reset(x, v)` <a id="part-8-3-4-2"></a>
`reset(x, v)`:
1. cleans up the current value of `x` if valid,
2. assigns `v` to `x`.

#### 8.3.4.3 `discard(x)` <a id="part-8-3-4-3"></a>
`discard(x)` cleans up and invalidates `x` immediately.

### 8.3.5 Type-directed cleanup diagnostics <a id="part-8-3-5"></a>
Objective‑C 3.0 introduces type annotations usable on typedefs:

- `@requires_cleanup(F)`
- `@resource_family(Name)`
- `@invalid(X)`

In strict-system mode, the compiler shall diagnose:
- forgetting to cleanup values of types with `@requires_cleanup`,
- calling a cleanup function not associated with the resource family,
- double-cleanup and use-after-take patterns.

> Note: This is explicitly intended to prevent “wrong close vs release” bugs in handle-heavy APIs.

---

## 8.4 Retainable C families and ObjC-integrated objects <a id="part-8-4"></a>

### 8.4.1 Motivation <a id="part-8-4-1"></a>
Some system libraries declare their objects as Objective‑C object types when compiling as Objective‑C so they can participate in ARC, blocks, and analyzer tooling. This behavior exists today in dispatch and XPC ecosystems.

Objective‑C 3.0 defines a language-level model so this is not “macro folklore.”

### 8.4.2 `@retainable_family` <a id="part-8-4-2"></a>
A library may declare a retainable family type by specifying:
- retain function
- release function
- optional autorelease function
- whether the family is ObjC-integrated under ObjC compilation

If ObjC-integrated:
- under ARC, manual retain/release functions for that family are ill-formed (diagnosed as errors).

### 8.4.3 Analyzer integration <a id="part-8-4-3"></a>
A conforming implementation shall expose retainable family semantics to:
- the ARC optimizer,
- the static analyzer.

---

## 8.5 CoreFoundation ownership attributes (normative) <a id="part-8-5"></a>
Objective‑C 3.0 standardizes ownership transfer attributes commonly used with ARC and analyzer:
- returns retained / not retained
- consumed parameters

The compiler shall use these to:
- insert correct ownership operations under ARC,
- and diagnose leaks and over-releases in strict modes.

---

## 8.6 Explicit lifetime control <a id="part-8-6"></a>

### 8.6.1 `withLifetime(expr) stmt` <a id="part-8-6-1"></a>
Syntax:
```objc
withLifetime(expr) { ... }
```

Semantics:
- evaluate `expr` once,
- bind it to an implicit strong precise-lifetime temporary,
- execute the statement,
- release the temporary after the statement.

### 8.6.2 `keepAlive(expr);` <a id="part-8-6-2"></a>
Extends the lifetime of `expr` to at least the end of the current lexical scope via an implicit precise-lifetime binding.

### 8.6.3 `objc_precise_lifetime` <a id="part-8-6-3"></a>
The implementation shall support a precise lifetime annotation for locals and define its effect: prevent lifetime shortening below lexical scope.

---

## 8.7 Borrowed interior pointers <a id="part-8-7"></a>

### 8.7.1 `borrowed T *` <a id="part-8-7-1"></a>
A borrowed pointer type indicates “valid only while some owner stays alive.”

Rules in strict-system mode:
- borrowed pointers shall not be stored into globals, heap storage, or escaping blocks without explicit unsafe escape.
- borrowed pointers derived from a non-stable owner require explicit lifetime control (`withLifetime`/`keepAlive`) or automatic owner materialization with a warning.

### 8.7.2 `@returns_borrowed(owner: N)` <a id="part-8-7-2"></a>
Functions may declare borrowed return relationships, enabling the compiler to enforce owner lifetime.

**Canonical header spelling (illustrative):**
- `__attribute__((objc_returns_borrowed(owner_index=N)))` on the function/return.

### 8.7.3 Escape analysis (normative in strict-system) <a id="part-8-7-3"></a>
In strict-system mode, a borrowed pointer value shall be treated as **non-escaping** unless an explicit unsafe escape is used.

The following are considered *escaping uses* and are ill-formed in strict-system mode:
- storing the borrowed pointer into a global/static variable,
- storing it into heap storage (including Objective‑C ivars/properties) without an explicit copy/retain of the owner,
- capturing it by an escaping block or passing it to a parameter not proven non-escaping.

The compiler shall provide diagnostics that suggest:
- extending the owner’s lifetime (`withLifetime` / `keepAlive`), or
- copying data out of the borrowed region into an owned buffer, or
- marking the operation explicitly unsafe.

A conforming implementation may rely on a combination of front-end rules and static analysis to enforce this.

---


## 8.8 Block capture lists <a id="part-8-8"></a>

### 8.8.1 Syntax <a id="part-8-8-1"></a>
Blocks may include an explicit capture list:

```objc
^[weak self, move handle] { ... }
```

A capture list is a comma-separated list of *capture items*:

```text
capture-list:
    '[' capture-item (',' capture-item)* ']'

capture-item:
    capture-modifier? identifier
```

Capture modifiers (v1):
- `strong` — capture strongly (default if omitted)
- `weak` — capture as zeroing weak under ARC
- `unowned` — capture as unsafe unretained (explicitly unsafe)
- `move` — move a one-time value into the block capture storage

### 8.8.2 Semantics (normative) <a id="part-8-8-2"></a>
Capture list items are evaluated at **block creation time**, not at block invocation time.

For a capture item `m x`:
- the current value of `x` in the enclosing scope is read,
- and stored into capture storage according to modifier `m`.

Modifier semantics:
- `strong x` stores a strong reference (ARC retain) to `x`.
- `weak x` stores a zeroing weak reference to `x`.
- `unowned x` stores an unsafe non-zeroing reference to `x` (may dangle).
- `move x` transfers the current value of `x` into the block’s capture storage and treats `x` as moved-from afterward (see [§8.8.4](#part-8-8-4)).

### 8.8.3 Capture evaluation order (normative) <a id="part-8-8-3"></a>
Capture list entries are evaluated **left-to-right**.
This ordering matters when later captures depend on earlier ones (or when move semantics are involved).

### 8.8.4 Move capture rules <a id="part-8-8-4"></a>
If `x` is captured with `move`:
- the capture stores the previous value of `x`,
- and `x` is set to a moved-from state.

For ordinary Objective‑C object pointers, “moved-from” is a safe state (typically `nil`) and use-after-move diagnostics are optional.
For resource/handle types annotated as requiring cleanup ([Part 8](#part-8).3/8.3.5), strict-system mode shall diagnose:
- use-after-move as an error, and
- double-cleanup hazards (because a moved-from handle must not be cleaned up twice).

### 8.8.5 Interaction with retain cycles <a id="part-8-8-5"></a>
Capture lists provide a standard spelling for the “weak‑strong dance” pattern.

Example:
```objc
__weak typeof(self) weakSelf = self;
[obj setHandler:^{
  typeof(self) self = weakSelf;
  if (!self) return;
  [self doThing];
}];
```

can be expressed as:

```objc
[obj setHandler:^[weak self] {
  guard let self = self else { return; }
  [self doThing];
}];
```

### 8.8.6 Diagnostics <a id="part-8-8-6"></a>
In strict-system mode, toolchains should:
- warn on likely retain cycles (e.g., capturing `self` strongly in long-lived handler registrations),
- warn/error when capturing a borrowed pointer into an escaping block ([Part 8](#part-8).7),
- diagnose use-after-move for resource/handle captures.

---


## 8.9 Conformance and migration <a id="part-8-9"></a>
- In permissive mode, most violations are warnings with fix-its.
- In strict-system mode, key violations are errors:
  - escaping borrowed pointers,
  - missing required cleanup,
  - wrong cleanup function usage.

---

## 8.10 Open issues <a id="part-8-10"></a>
- Final spelling for `@retainable_family` and whether it maps to existing Clang attributes.
- Whether borrowed-pointer enforcement is front-end only or requires analyzer for complete coverage.
- Whether resource annotations can be extended to struct fields (RAII-like aggregates).
<!-- END PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md -->

---

<!-- BEGIN PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md -->
# Part 9 — Performance and Dynamism Controls <a id="part-9"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 9.1 Purpose <a id="part-9-1"></a>
Objective‑C’s dynamic dispatch is a strength, but “open world” dynamism can inhibit optimization and complicate reasoning about safety.

Objective‑C 3.0 provides **opt-in** controls that:
- enable direct calls and devirtualization where safe,
- communicate intent to compilers and reviewers,
- preserve dynamic behavior by default,
- remain safe under separate compilation and across module boundaries ([C.2](#c-2)).

This part is designed to accommodate both Cocoa-style dynamic patterns *and* system/library subsets where predictability and performance matter.

## 9.2 Direct methods <a id="part-9-2"></a>

### 9.2.1 Concept <a id="part-9-2-1"></a>
A *direct method* is a method that:
- is not dynamically dispatched by selector lookup,
- cannot be overridden,
- and is intended to be invoked via a direct call when statically referenced.

### 9.2.2 Canonical spelling <a id="part-9-2-2"></a>
A direct method is declared with:

```c
__attribute__((objc_direct))
```

applied to an Objective‑C method declaration.

Toolchains may additionally support class-level defaults (e.g., “all members direct”) as an extension, but such defaults must be representable in emitted interfaces ([B.7](#b-7)).

### 9.2.3 Semantics (normative) <a id="part-9-2-3"></a>
1. A direct method shall not be overridden in any subclass.
2. A direct method shall not be introduced or replaced by a category with the same selector.
3. A direct method shall not participate in message forwarding semantics (`forwardInvocation:`) as part of selector lookup.

Calling model:
- A direct method call shall be formed only by a *statically-resolved* method reference (i.e., the compiler must know it targets a direct method declaration).
- Dynamic message sends (e.g., `objc_msgSend`, `performSelector:`) shall not be relied upon to find a direct method.

**Programmer model:** “If you want dynamic, do not mark it direct.”

### 9.2.4 Separate compilation notes (normative) <a id="part-9-2-4"></a>
Because direct methods change call legality and dispatch surfaces, the `objc_direct` attribute shall be:
- recorded in module metadata (see [D.3.1](#d-3-1) [Table A](#d-3-1)), and
- preserved in emitted interfaces.

Effect/attribute mismatches across modules are ill-formed ([C.2](#c-2)).

### 9.2.5 Recommended lowering (informative) <a id="part-9-2-5"></a>
Recommended lowering is described in [C.8](#c-8).
At a high level:
- the compiler emits a callable symbol for the method implementation, and
- calls to the method use a direct call rather than `objc_msgSend`.

## 9.3 Final methods and classes <a id="part-9-3"></a>

### 9.3.1 Concept <a id="part-9-3-1"></a>
A *final* declaration prohibits overriding/subclassing but does not necessarily remove the declaration from dynamic dispatch.

Final is primarily a **reasoning and optimization** tool:
- the compiler may devirtualize calls where it can prove the receiver type,
- but dynamic lookup semantics remain observable unless paired with `objc_direct`.

### 9.3.2 Canonical spelling <a id="part-9-3-2"></a>
Final declarations use:

```c
__attribute__((objc_final))
```

applied to:
- classes, to prohibit subclassing (subject to module sealing rules), and/or
- methods, to prohibit overriding.

### 9.3.3 Semantics (normative) <a id="part-9-3-3"></a>
- A `objc_final` class shall not be subclassed in any translation unit that sees the declaration.
- A `objc_final` method shall not be overridden by any subclass declaration.

Violations are ill-formed.

## 9.4 Sealed classes <a id="part-9-4"></a>

### 9.4.1 Concept <a id="part-9-4-1"></a>
A *sealed* class prohibits subclassing **outside** the defining module, but may allow subclassing **inside** the module.

This enables:
- whole-module reasoning about the subclass set, and
- optimization across module boundaries while still supporting internal extension points.

### 9.4.2 Canonical spelling <a id="part-9-4-2"></a>
Sealed classes use:

```c
__attribute__((objc_sealed))
```

Toolchains may treat existing equivalent attributes (e.g., “subclassing restricted”) as aliases.

### 9.4.3 Semantics (normative) <a id="part-9-4-3"></a>
- Subclassing a sealed class outside the owning module is ill-formed.
- Whether same-module subclassing is permitted is implementation-defined unless the attribute explicitly specifies (future extension).

At minimum, sealed shall be strong enough to support the “no external subclassing” contract.

## 9.5 Interaction with categories, swizzling, and reflection <a id="part-9-5"></a>

### 9.5.1 Categories <a id="part-9-5-1"></a>
- Categories may add methods to any class by default (baseline Objective‑C behavior).
- Categories shall not add or replace a `objc_direct` method selector; doing so is ill-formed.

### 9.5.2 Swizzling and IMP replacement (non-normative guidance) <a id="part-9-5-2"></a>
Swizzling is a common dynamic technique. Objective‑C 3.0 does not outlaw it, but it makes the tradeoff explicit:
- Do not mark APIs `objc_direct` if swizzling/forwarding must be supported.
- `objc_final` and `objc_sealed` do not prevent swizzling by themselves; they primarily constrain subclassing/overriding.

## 9.6 ABI and visibility rules (normative for implementations) <a id="part-9-6"></a>
Implementations shall ensure that direct/final/sealed remain safe under:
- separate compilation,
- LTO and non-LTO builds,
- and module boundaries.

Minimum requirements:
1. Attributes that affect dispatch legality (`objc_direct`) must be recorded in module metadata.
2. Interface emission must preserve the attributes using canonical spellings ([B.7](#b-7)).
3. Calling a method with incompatible assumptions about directness/finality across module boundaries must be diagnosed when possible ([Part 12](#part-12)).

## 9.7 Required diagnostics (minimum) <a id="part-9-7"></a>
- Declaring `objc_direct` on a method that is declared in an Objective‑C protocol and implemented dynamically: error (direct methods are not dispatchable by selector).
- Overriding a `objc_direct` or `objc_final` method: error.
- Declaring a category method with the same selector as a `objc_direct` method: error.
- Attempting to subclass an `objc_final` class: error.
- Attempting to subclass an `objc_sealed` class from another module: error.
- In strict performance mode: warn when a direct-call-capable reference is forced through dynamic dispatch (e.g., via `performSelector:`), with note explaining semantics.

## 9.8 Open issues <a id="part-9-8"></a>
- Whether to standardize a class-level “all members direct” attribute as part of v1 (or leave as implementation extension).
- Whether to standardize an attribute that *forces* dynamic dispatch (useful for debugging and swizzling-friendly APIs).
<!-- END PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md -->

---

<!-- BEGIN PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md -->
# Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors <a id="part-10"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 10.1 Purpose <a id="part-10-1"></a>
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

## 10.2 Derives <a id="part-10-2"></a>

### 10.2.1 Canonical spelling <a id="part-10-2-1"></a>
A derive request is attached using:

```c
__attribute__((objc_derive("TraitName")))
```

Multiple derives may be listed by repeating the attribute.

### 10.2.2 Semantics (normative) <a id="part-10-2-2"></a>
A derive request causes the compiler (or derive implementation) to synthesize declarations as if the user had written them.

Rules:
1. Derives are **deterministic**: the same source must produce the same expanded declarations.
2. Derives are **pure** with respect to the compilation: they shall not depend on network, wall-clock time, or other nondeterministic inputs.
3. If a derive cannot be satisfied (missing requirements, ambiguous synthesis), compilation is ill-formed and shall produce diagnostics identifying the missing inputs.

### 10.2.3 ABI and interface emission (normative) <a id="part-10-2-3"></a>
If a derive synthesizes declarations that affect:
- type layout,
- exported method sets,
- or protocol conformances,

then those synthesized declarations are part of the module’s API surface and:
- shall be recorded in module metadata, and
- shall appear in any emitted interface ([B.7](#b-7), [C.9](#c-9)).

### 10.2.4 Standard derives (non-normative) <a id="part-10-2-4"></a>
A standard library may provide derives analogous to:
- equality/hash,
- debug formatting,
- coding/serialization.

This draft does not require a particular set in v1; it requires the *mechanism* and the interface emission guarantees.

## 10.3 AST macros <a id="part-10-3"></a>

### 10.3.1 Concept <a id="part-10-3-1"></a>
AST macros are compile-time programs that transform syntax/AST into expanded declarations or expressions.

### 10.3.2 Canonical marker attribute <a id="part-10-3-2"></a>
Macro entry points may be annotated with:

```c
__attribute__((objc_macro))
```

The macro definition and packaging format are implementation-defined.

### 10.3.3 Safety constraints (normative) <a id="part-10-3-3"></a>
A conforming implementation shall provide enforcement such that macros used in trusted builds are:
- deterministic (no nondeterministic inputs),
- sandboxable (no arbitrary filesystem/network unless explicitly permitted by the build system),
- and bounded (resource limits to keep builds predictable).

### 10.3.4 Expansion phases (normative) <a id="part-10-3-4"></a>
Macro expansion (and derive expansion) that introduces declarations affecting layout/ABI shall occur before:
- type layout is finalized, and
- code generation.

This ensures ABI is computed from the *expanded* program ([C.9](#c-9)).

### 10.3.5 Interface emission (normative) <a id="part-10-3-5"></a>
If macro expansion introduces exported declarations, the emitted interface shall include the expanded declarations (or an equivalent representation that reconstructs them), not merely the macro invocation.

## 10.4 Property behaviors / wrappers <a id="part-10-4"></a>

### 10.4.1 Concept <a id="part-10-4-1"></a>
A property behavior is a reusable specification of:
- storage strategy,
- accessors (get/set),
- synchronization or actor isolation constraints,
- and optional hooks (KVO/KVC integration, validation, etc.).

### 10.4.2 Exported ABI restrictions (normative) <a id="part-10-4-2"></a>
Behaviors that affect layout or emitted ivars of exported types shall be constrained such that:
- layout is deterministic from the expanded declarations, and
- cross-module clients can rely on the same layout.

In v1, implementations may restrict behaviors on exported ABI surfaces to a “safe subset” (e.g., behaviors that desugar into explicit ivars/accessors).

### 10.4.3 Composition (normative) <a id="part-10-4-3"></a>
If multiple behaviors are applied to a property, the composition order and conflict rules must be deterministic and diagnosable.
Ambiguous compositions are ill-formed.

### 10.4.4 Performance contracts <a id="part-10-4-4"></a>
Behaviors must declare whether:
- accessors are direct-callable (eligible for inlining),
- they require dynamic dispatch hooks,
- they require executor/actor hops.

Compilers should surface these contracts in diagnostics to help developers choose appropriate behaviors.

## 10.5 Required diagnostics (minimum) <a id="part-10-5"></a>
- Derive request cannot be satisfied: error with notes listing missing members/protocol requirements.
- Macro expansion introduces exported declarations but cannot be represented in emitted interface: error (or emit-only warning in permissive mode).
- Property behavior composition conflict: error with explanation of conflict.

## 10.6 Open issues <a id="part-10-6"></a>
- Macro/derive packaging format (SwiftPM-like vs compiler-integrated vs build-system supplied).
- Which derives are standardized in core vs shipped in a standard library module.
- Whether to standardize a canonical surface syntax for behaviors beyond attributes in v1.
<!-- END PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md -->

---

<!-- BEGIN PART_11_INTEROPERABILITY_C_CPP_SWIFT.md -->
# Part 11 — Interoperability: C, C++, and Swift <a id="part-11"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 11.1 Purpose <a id="part-11-1"></a>
Objective‑C’s success depends on interoperability:
- with **[C](#c)** (system APIs),
- with **C++** (ObjC++),
- and with **Swift** (modern app frameworks).

This part defines interoperability expectations so ObjC 3.0 features can be adopted without breaking the ecosystem.

This part is intentionally conservative about ABI: where a feature affects ABI or lowering, it is cross-referenced to **[C](#c)** and summarized in **[D](#d)**.

## 11.2 C interoperability <a id="part-11-2"></a>

### 11.2.1 Baseline ABI compatibility <a id="part-11-2-1"></a>
Objective‑C 3.0 preserves baseline C ABI calling conventions for declarations that do not use new effects.

For declarations that use ObjC 3.0 effects (`throws`, `async`), a conforming toolchain shall provide a stable, documented calling convention ([C.4](#c-4), [C.5](#c-5)).

### 11.2.2 Exporting `throws` to C (normative intent) <a id="part-11-2-2"></a>
When a throwing function is made visible to C, it shall be representable using a C-callable surface.

Recommended representation:
- a trailing error-out parameter (`id<Error> * _Nullable outError`) as described in [C.4](#c-4).

This aligns with Cocoa NSError patterns and enables C callers to handle errors without language support for `try`.

### 11.2.3 Exporting `async` to C (normative intent) <a id="part-11-2-3"></a>
When an `async` function is made visible to C, it shall be representable using a C-callable surface.

This draft does not require a single canonical signature, but a conforming implementation shall provide at least one of:
- an automatically-generated completion-handler thunk, or
- a runtime entry point that can schedule the async function and invoke a completion callback when finished.

The chosen representation must be stable under separate compilation and recordable in module metadata ([C.2](#c-2)).

## 11.3 C++ / ObjC++ interoperability <a id="part-11-3"></a>

### 11.3.1 Parsing and canonical spellings <a id="part-11-3-1"></a>
Canonical spellings in [B](#b) are chosen to be compatible with ObjC++ translation units:
- `__attribute__((...))` is accepted in C++ mode,
- `#pragma ...` is available in C++ mode.

Implementations may additionally support C++11 attribute spellings (`[[...]]`) in ObjC++ as sugar, but interface emission shall use canonical spellings ([B.7](#b-7)).

### 11.3.2 Exceptions <a id="part-11-3-2"></a>
Objective‑C exceptions (`@throw/@try/@catch`) remain distinct from ObjC 3.0 `throws`.
C++ exceptions remain distinct as well.

Interoperability guidance:
- Do not translate ObjC 3.0 `throws` into C++ exceptions implicitly.
- If an implementation provides bridging, it must be explicit and toolable.

## 11.4 Swift interoperability <a id="part-11-4"></a>

### 11.4.1 Nullability and optionals <a id="part-11-4-1"></a>
ObjC 3.0 nullability annotations and `T?` sugar import naturally as Swift optionals where applicable, consistent with existing Swift/ObjC interop rules.

### 11.4.2 Errors <a id="part-11-4-2"></a>
- ObjC 3.0 `throws` imports as Swift `throws` when the declaration is visible through a module interface that preserves the effect.
- NSError-out parameter APIs annotated with `objc_nserror` ([Part 6](#part-6)) should import as Swift `throws` where possible.

### 11.4.3 Concurrency <a id="part-11-4-3"></a>
- ObjC 3.0 `async` imports as Swift `async` when preserved through module metadata/interfaces.
- Completion-handler APIs may be imported/exported across languages using implementation-defined annotations; this draft encourages toolchains to support automated thunk generation.

### 11.4.4 Executors and isolation <a id="part-11-4-4"></a>
Executor affinity (`objc_executor(main)`) is intended to map cleanly to Swift main-thread isolation concepts.
The exact imported attribute spelling is implementation-defined, but semantics must be preserved across the boundary.

Actor isolation metadata should be preserved such that Swift clients do not accidentally violate ObjC actor invariants (and vice versa).

### 11.4.5 Performance/dynamism controls <a id="part-11-4-5"></a>
Direct/final/sealed intent should map, where possible, to Swift’s:
- `final`,
- non-overridable members,
- and restricted subclassing patterns.

The mapping is implementation-defined but should preserve the core constraints.

## 11.5 Required diagnostics (minimum) <a id="part-11-5"></a>
- Importing an API where effects (`async`/`throws`) are lost due to missing interface metadata: warn and suggest enabling module/interface emission.
- Crossing a language boundary with non-Sendable-like values in strict concurrency mode: warn/error depending on policy ([Part 7](#part-7), [Part 12](#part-12)).

## 11.6 Open issues <a id="part-11-6"></a>
- A precise, portable “async-to-C” completion thunk signature (this may be a platform profile item).
- Exact mapping rules for strict nullability completeness in mixed-language modules.
<!-- END PART_11_INTEROPERABILITY_C_CPP_SWIFT.md -->

---

<!-- BEGIN PART_12_DIAGNOSTICS_TOOLING_TESTS.md -->
# Part 12 — Diagnostics, Tooling, and Test Suites <a id="part-12"></a>
_Working draft v0.10 — last updated 2025-12-28_

## 12.1 Purpose <a id="part-12-1"></a>
Objective‑C 3.0 treats tooling requirements as part of conformance:

- diagnostics must be specific and actionable,
- fix-its must exist for common mechanical migrations,
- and a conformance test suite must exist to prevent “whatever the compiler happens to do.”

This part is cross-cutting and references:
- **[B](#b)** for canonical spellings and interface emission requirements,
- **[C](#c)** for separate compilation and lowering contracts.
- **[D](#d)** for the normative checklist of required module metadata and ABI boundaries.

## 12.2 Diagnostic principles (normative) <a id="part-12-2"></a>
A conforming implementation shall provide diagnostics that:
1. Identify the precise source range of the problem.
2. Explain the rule being violated in terms of Objective‑C 3.0 concepts (nonnull, optional send, executor hop, etc.).
3. Provide at least one actionable suggestion.
4. Provide a fix-it when the transformation is mechanical and semantics-preserving.

## 12.3 Required diagnostics (minimum set) <a id="part-12-3"></a>

### 12.3.1 Nullability and optionals <a id="part-12-3-1"></a>
- Incomplete nullability in exported interfaces (strict mode: error).
- Passing nullable to nonnull without check/unwrap (strict: error; permissive: warning).
- Dereferencing nullable without unwrap (strict: error).
- Optional message send or optional member access used on scalar/struct returns (error; v1 restriction).
- Ordinary message send on nullable receiver in strict mode (error; fix-it suggests optional send or `guard let`).

### 12.3.2 Errors and `throws` <a id="part-12-3-2"></a>
- Calling a `throws` function/method without `try` (error).
- Redeclaring a `throws` declaration without `throws` (or vice versa) across module boundaries (error; see [C.2](#c-2)).
- Using postfix propagation `e?` on `T?` in a non-optional-returning function (error; fix-it suggests `guard let` or explicit conditional).
- Using postfix propagation `e?` expecting it to map to `throws`/`Result` (error; explain “carrier preserving” rule).

### 12.3.3 Concurrency (`async/await`, executors, actors) <a id="part-12-3-3"></a>
- Any potentially suspending operation without `await` (error), including calling an `async` function and crossing executor/actor isolation boundaries ([Part 7](#part-7) / [D-011](#decisions-d-011)).
- Calling into an `objc_executor(X)` declaration from a different executor without an `await` hop (strict concurrency: error; permissive: warning).
- Capturing non-Sendable-like values into a task-spawned async closure (strict concurrency: error; permissive: warning).
- Actor-isolated member access from outside the actor without `await` when a hop may be required (strict concurrency: error).
- `await` applied to an expression that cannot suspend (strict: warning) (diagnostic: “unnecessary await”).

### 12.3.4 Modules and interface emission <a id="part-12-3-4"></a>
- Missing required semantic metadata on imported declarations (effects/isolation/directness): error in strict modes; see [D.3.1](#d-3-1) [Table A](#d-3-1).
- Using a module-qualified name with a non-imported module: error with fix-it to add `@import`.
- Importing an API through a mechanism that loses effects/attributes (e.g., textual header without metadata) when strictness requires them: warning with suggestion to enable modules/interface emission.
- Emitted interface does not use canonical spellings from [B](#b): tooling warning; in “interface verification” mode, error.

### 12.3.5 Performance/dynamism controls <a id="part-12-3-5"></a>
- Overriding a `objc_final` method/class: error.
- Subclassing an `objc_final` class: error.
- Subclassing an `objc_sealed` class from another module: error.
- Declaring a category method that collides with a `objc_direct` selector: error.
- Calling a `objc_direct` method via dynamic facilities (e.g., `performSelector:`) when it can be statically diagnosed: warning (or error under strict performance profile).
### 12.3.6 System programming extensions <a id="part-12-3-6"></a>
A conforming implementation shall provide diagnostics (at least warnings, and errors in strict-system profiles) for:

- **Resource cleanup correctness** ([Part 8](#part-8) [§8.3](#part-8-3)):
  - missing required cleanup for `objc_resource(...)` or `@resource(...)` declarations (if required by the selected profile),
  - double-close patterns detectable intra-procedurally,
  - use-after-move / use-after-discard for moved-from resources (where move checking is enabled).

- **Borrowed pointer escaping** ([Part 8](#part-8) [§8.7](#part-8-7)):
  - returning a `borrowed` pointer without the required `objc_returns_borrowed(owner_index=...)` marker,
  - storing a borrowed pointer into a longer-lived location (global, ivar, heap) in strict-system profiles,
  - passing a borrowed pointer to an `@escaping` block in strict-system profiles.

- **Capture list safety** ([Part 8](#part-8) [§8.8](#part-8-8)):
  - suspicious strong captures of `self` in `async` contexts (if enabled by profile),
  - `unowned` capture of potentially-nil objects without an explicit check,
  - mixed `weak` + `move` captures that create use-after-move hazards.

## 12.4 Required tooling capabilities (minimum) <a id="part-12-4"></a>

### 12.4.1 Migrator <a id="part-12-4-1"></a>
A conforming toolchain shall provide (or ship) a migrator that can:
- insert nullability annotations based on inference and usage,
- rewrite common nullable-send patterns into optional sends or `guard let`,
- generate `throws` wrappers for NSError-out patterns ([Part 6](#part-6)),
- suggest executor/actor hop fixes where statically determinable ([Part 7](#part-7)).

### 12.4.2 Interface emission / verification <a id="part-12-4-2"></a>
If the toolchain supports emitting an interface description for distribution, it shall also support a verification mode that checks:
- semantic equivalence between the original module and the emitted interface,
- and the use of canonical spellings from [B](#b).

### 12.4.3 Static analysis hooks <a id="part-12-4-3"></a>
Implementations shall expose enough information for analyzers to:
- reason about nullability flow,
- reason about `throws` propagation,
- and enforce Sendable-like constraints under strict concurrency.
- expose `borrowed` and `objc_returns_borrowed(owner_index=...)` annotations in AST dumps and module metadata for external analyzers ([Part 8](#part-8), [D](#d)).
- expose resource annotations (`objc_resource(...)`) and move-state tracking hooks where enabled by profile ([Part 8](#part-8)).

## 12.5 Conformance test suite (minimum expectations) <a id="part-12-5"></a>
A conforming implementation shall ship or publish a test suite that covers at least:

### 12.5.1 Parsing/grammar <a id="part-12-5-1"></a>
- `async`/`await`/`throws` grammar interactions (`try await`, `await try`, etc.).
- Module-qualified name parsing (`@A.B.C`).

### 12.5.2 Type system and diagnostics <a id="part-12-5-2"></a>
- Module metadata preservation tests for each [Table A](#d-3-1) item in [D](#d) (import module; verify semantics survive; verify mismatch diagnostics).
- Strict vs permissive nullability behavior.
- Optional send restrictions (reference-only).
- Postfix propagation carrier-preserving rules.
- Effect mismatch diagnostics across module imports ([C.2](#c-2)).

### 12.5.3 Dynamic semantics <a id="part-12-5-3"></a>
- Optional send argument evaluation does not occur when receiver is `nil`.
- `throws` propagation through nested `do/catch`.
- Cancellation propagation in structured tasks.

### 12.5.4 Runtime contracts <a id="part-12-5-4"></a>
- Autorelease pool draining at suspension points (Objective‑C runtimes) ([D-006](#decisions-d-006) / [C.7](#c-7)).
- Executor/actor hops preserve ordering and do not deadlock in basic scenarios.
- Calling executor-annotated or actor-isolated *synchronous* members from outside requires `await` and schedules onto the correct executor ([D-011](#decisions-d-011)).

## 12.6 Debuggability requirements (minimum) <a id="part-12-6"></a>
For features like macros and async:
- stack traces must preserve source locations,
- debugging tools must be able to map generated code back to user code (macro expansion mapping),
- async tasks should be nameable/inspectable at least in debug builds.

## 12.7 Open issues <a id="part-12-7"></a>
- Standard format for reporting conformance results across toolchains.
- Whether “strict concurrency checking” is a separate conformance level or a strictness sub-mode.
<!-- END PART_12_DIAGNOSTICS_TOOLING_TESTS.md -->
