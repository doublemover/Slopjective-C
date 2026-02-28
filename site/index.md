---
title: Objective-C 3.0 Draft Specification
layout: default
---

<!-- BEGIN TABLE_OF_CONTENTS.md -->

# Objective‑C 3.0 Draft Specification (Working Draft) — Table of Contents <a id="toc"></a>

_Last generated: 2026-02-27_
_Working draft: v0.11_

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
- **[STANDARD_LIBRARY_CONTRACT.md](#s)** — Minimum required standard-library modules, APIs, semantics, and versioning contracts
- **[ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md](#am)** — Unified abstract-machine semantics for evaluation order, lifetimes, cleanup, and suspension
- **[FORMAL_GRAMMAR_AND_PRECEDENCE.md](#f)** — Integrated ObjC 3.0 grammar appendix and operator precedence/disambiguation rules

## Parts <a id="toc-parts"></a>

- **[PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md](#part-0)** — Baseline definition, stable `NR-*` normative reference index, and conflict/override model
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

_Last generated: 2026-02-27_
_Working draft: v0.11_

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

1. Optional chaining (`?.`, `[x? foo]`) is **reference-only** in v1 (no scalar/struct chaining).
2. `throws` is **untyped** in v1 (always `id<Error>`); typed throws is deferred.
3. Task spawning remains **library-defined** in v1; no `task {}` keyword syntax (compiler recognition uses standardized attributes).

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

# Objective‑C 3.0 — Design Decisions Log (v0.11) <a id="decisions"></a>

_Last updated: 2026-02-23_

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

**Decision:** On Objective‑C runtimes with autorelease semantics, each task _execution slice_ (resume → next suspension or completion) runs inside an implicit autorelease pool that is drained:

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

**Rationale:** Executor and actor isolation are implemented by _hops_ that may suspend even when the callee’s body is “logically synchronous.” Requiring `await` keeps suspension explicit at the call site while preserving ergonomic isolation.

**Spec impact:** [Part 7](#part-7), [C](#c), [D](#d).

---

## D-012: Required module metadata set is normative (v1) <a id="decisions-d-012"></a>

**Decision:** For ObjC 3.0 semantics to survive separate compilation, the required information enumerated in **[D](#d)** is normative: a conforming toolchain shall preserve that information in module metadata and in emitted textual interfaces.

**Rationale:** Without an explicit checklist, toolchains drift into “works in a single TU” but fails at module boundaries. [D](#d) makes the “separate compilation contract” testable.

**Spec impact:** [C](#c), [D](#d), [Part 2](#part-2), [Part 12](#part-12).

---

## D-013: Future value-optionals use canonical `Optional<T>` spelling <a id="decisions-d-013"></a>

**Decision:** If a future value-optional feature is standardized, its canonical source spelling is `Optional<T>`.
`optional<T>` is not canonical and remains a reserved compatibility surface.

In conforming modes:

- parsers and interface emitters shall treat `Optional<T>` as the canonical spelling,
- textual interfaces shall emit `Optional<T>` when value-optionals are represented,
- any compatibility acceptance of `optional<T>` shall diagnose and offer a migration fix-it to `Optional<T>`.

**Rationale:** A single canonical spelling avoids dual-surface drift in tooling, formatting, metadata round-trips, and diagnostics while preserving a migration path for compatibility aliases.

**Spec impact:** [Part 3](#part-3) [§3.3.5](#part-3-3-5) and [§3.9](#part-3-9).

---

## D-014: Generic free-function mangling standardizes invariants, not one universal symbol string <a id="decisions-d-014"></a>

**Decision:** v0.11 does not require one byte-for-byte generic free-function mangling string across all toolchains.
Instead, conformance standardizes:

- semantic mangling invariants (base name, generic arity, normalized constraints),
- deterministic reproduction within a toolchain/policy,
- stable publication of a mangling policy identifier,
- preserved semantic signature data in metadata/interfaces for cross-tool verification.

Direct symbol-string equality is only required within the same declared mangling policy.

**Rationale:** This preserves portability and testability without freezing all toolchains onto one encoding format, while still preventing semantic ambiguity.

**Spec impact:** [Part 3](#part-3) [§3.5.3.3](#part-3-5-3-3) and [§3.9](#part-3-9), plus metadata validation surfaces in [D](#d).

---

## D-015: Future generic reification control is declaration-scoped <a id="decisions-d-015"></a>

**Decision:** Any future explicit generic reification mode applies per declaration via `@reify_generics` (or equivalent canonical declaration-level form).
Module/profile switches may gate whether declaration-level syntax is allowed, but shall not implicitly reify declarations that omit explicit markers.

Conforming metadata/interface behavior shall preserve whether a declaration is erased or explicitly reified.

**Rationale:** Declaration-scoped control supports gradual adoption, avoids module-wide semantic surprises, and keeps mixed erased/reified codebases tractable.

**Spec impact:** [Part 3](#part-3) [§3.5.5](#part-3-5-5) and [§3.9](#part-3-9).

<!-- END DECISIONS_LOG.md -->

---

<!-- BEGIN ATTRIBUTE_AND_SYNTAX_CATALOG.md -->

# Objective‑C 3.0 — Attribute and Syntax Catalog <a id="b"></a>

_Working draft v0.11 — last updated 2026-02-23_

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

### B.5.1 Direct methods and class defaults (canonical) <a id="b-5-1"></a>

Direct dispatch controls use:

```c
__attribute__((objc_direct))          // methods
__attribute__((objc_direct_members))  // classes
```

`objc_direct` applies to methods. `objc_direct_members` applies to classes and makes members direct by default as defined in [Part 9](#part-9).

### B.5.2 Final and sealed (canonical) <a id="b-5-2"></a>

Objective‑C 3.0 uses the following canonical spellings:

```c
__attribute__((objc_final))                  // methods or classes
__attribute__((objc_sealed))                 // classes (module-sealed)
```

If a toolchain already provides an equivalent attribute (e.g., `objc_subclassing_restricted`), it may treat that attribute as an alias.

Semantics are defined in [Part 9](#part-9).

### B.5.3 Force-dynamic dispatch (canonical) <a id="b-5-3"></a>

Objective‑C 3.0 v1 defines an explicit dynamic-dispatch marker:

```c
__attribute__((objc_dynamic))  // methods
```

Semantics are defined in [Part 9](#part-9), including interaction with `objc_direct`, `objc_final`, and `objc_sealed`.

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
_single-owner handle_ for purposes of move/use-after-move diagnostics (where enabled).

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

### B.8.6 Retainable C family declarations (canonical) <a id="b-8-6"></a>

**Type attribute form (canonical):**

```c
__attribute__((objc_retainable_family(FamilyName)))
```

- Applies to typedef declarations that define a retainable family identity.

**Family operation attributes (canonical):**

```c
__attribute__((objc_family_retain(FamilyName)))
__attribute__((objc_family_release(FamilyName)))
__attribute__((objc_family_autorelease(FamilyName))) // optional
```

- Apply to function declarations that define retain/release/autorelease operations for `FamilyName`.

**ObjC integration marker (canonical existing):**

```c
__attribute__((NSObject))
```

- When present on the family typedef under Objective‑C compilation, the family is treated as ObjC-integrated for ARC semantics per [Part 8](#part-8) [§8.4](#part-8-4).

**Compatibility aliases (accepted when semantically equivalent):**

- `os_returns_retained` / `os_returns_not_retained` / `os_consumed`
- `cf_returns_retained` / `cf_returns_not_retained` / `cf_consumed`
- `ns_returns_retained` / `ns_returns_not_retained` / `ns_consumed`

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

_Working draft v0.11 — last updated 2026-02-23_

## C.0 Scope and audience <a id="c-0"></a>

This document is primarily **normative for implementations** (compiler + runtime + standard library).
It does not change source-level semantics defined elsewhere; it constrains how a conforming implementation supports those semantics under **separate compilation** and across **module boundaries**.

Where this document provides “canonical lowering” patterns, those are:

- **required** when explicitly labeled _normative_; and
- otherwise **recommended** as the default lowering for LLVM/Clang implementations.

### C.0.1 Status tags used in this document <a id="c-0-1"></a>

Status labels in this document follow [Part 0](#part-0) [§0.3.2](#part-0-3-2):

- **normative** / **minimum** / **normative intent**: contributes to conformance requirements,
- **recommended**: QoI guidance,
- **informative** / **non-normative**: explanatory material.

## C.1 Definitions (informative) <a id="c-1"></a>

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
   - any dispatch-affecting attributes (`objc_direct`, `objc_direct_members`, `objc_dynamic`, `objc_sealed`, etc.)
     shall be part of the imported type information.

   The minimum required set is enumerated normatively in **[MODULE_METADATA_AND_ABI_TABLES.md](#d)**.

2. **Mismatched redeclarations are diagnosed.**  
   Redeclaring an imported function/method with a different effect set or incompatible lowering-affecting attributes is ill-formed.

3. **Interface emission is semantics-preserving.**  
   If a textual module interface is emitted, it shall preserve effects and attributes (see [B.7](#b-7)).

## C.2.1 Required module metadata (normative) <a id="c-2-1"></a>

A conforming implementation shall preserve in module metadata and interface emission the items listed in **[D.3.1](#d-3-1) [Table A](#d-3-1)**.

This requirement is intentionally testable: if importing a module can change whether a call requires `try`/`await`, or can change dispatch legality (`objc_direct` / `objc_direct_members` / `objc_dynamic`), the implementation is non-conforming.

## C.3 Canonical lowering patterns (mixed; see subsection tags) <a id="c-3"></a>

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

## C.4 `throws` ABI and lowering (normative for implementations) <a id="c-4"></a>

### C.4.1 Canonical ABI shape for throwing functions (normative for implementations) <a id="c-4-1"></a>

For a function declared:

```c
R f(A1 a1, A2 a2) throws;
```

a conforming implementation shall provide a stable calling convention that allows the caller to receive either:

- a normal return of type `R`, or
- an error value of type `id<Error>`.

**Canonical ABI (recommended and permitted as normative):** the implementation uses an additional trailing _error-out parameter_:

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

However, the _call ABI_ of the method’s implementation may include the canonical trailing error-out parameter described in [C.4.1](#c-4-1).

A conforming implementation shall ensure that:

- when a call site is type-checked as calling a `throws` method, the generated call passes the error-out parameter in the canonical position required by that implementation’s ABI; and
- when forming or calling an `IMP` for a `throws` method (e.g., via `methodForSelector:`), the compiler uses a function pointer type whose parameter list includes the error-out parameter.

**Reflection note (non-normative):** baseline Objective‑C runtime type-encoding strings do not represent `throws`. This draft does not require extending the runtime type encoding in v1. Toolchains may provide extended metadata for reflective invocation as an extension.

A `try` call should lower to:

- allocate a local `id<Error> err = nil;`
- perform the call with `&err`
- if `err != nil`, branch to the enclosing error propagation/handler path.

`try?` and `try!` should be implemented as specified in [Part 6](#part-6), using the same underlying error value.

## C.5 `async` ABI and lowering (normative for implementations) <a id="c-5"></a>

### C.5.1 Coroutine model (normative for implementations) <a id="c-5-1"></a>

A conforming implementation shall implement `async` functions such that:

- each `await` is a potential suspension point,
- local variables required after an `await` have stable storage across suspension,
- ARC + cleanup semantics are preserved across suspension (see Parts 4 and 7).

**Recommended lowering:** lower `async` functions to LLVM coroutine state machines (e.g., using LLVM’s coroutine intrinsics), with resumption scheduled by the active executor.

### C.5.2 Task context (normative) <a id="c-5-2"></a>

While executing an `async` function, there exists a _current task context_ that provides at least:

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

## C.6 Actors: runtime representation and dispatch (normative for implementations) <a id="c-6"></a>

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

- Each _task execution slice_ (from resume to next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule exists to make async code’s memory behavior predictable for Foundation-heavy code.

## C.8 Direct/final/sealed/dynamic lowering (recommended) <a id="c-8"></a>

- `objc_direct` methods should lower to direct function calls when statically referenced, and may omit dynamic method table entries as permitted by [Part 9](#part-9).
- Methods made effectively direct by `objc_direct_members` should lower identically to explicit `objc_direct`, except for members explicitly marked `objc_dynamic`.
- `objc_dynamic` methods should lower through selector-based dynamic dispatch even when surrounding declarations are eligible for direct/devirtualized lowering.
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

_Working draft v0.11 — last updated 2026-02-27_

## D.0 Purpose <a id="d-0"></a>

Objective‑C 3.0 adds source-level semantics that **must survive module boundaries**:
effects (`async`, `throws`), nullability defaults, executor/actor isolation, and dispatch controls (`objc_direct`, `objc_sealed`, etc.).

This document is a **normative checklist** for what a conforming implementation must preserve in:

- module metadata (binary or AST-based), and
- any emitted textual interface ([B.7](#b-7)).

It also summarizes which features are **ABI-affecting** and what ABI stability constraints apply.

> This document does not mandate a specific on-disk format. It defines _required information_.

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
   - and whether the callable is _potentially suspending at call sites_ due to isolation ([Part 7](#part-7)).

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

### D.2.1 Metadata encoding/version header (normative) <a id="d-2-1"></a>

Each module metadata payload shall carry a version header with:

- `schema_major` (non-negative integer),
- `schema_minor` (non-negative integer),
- a `required_capabilities` set naming non-ignorable semantic extensions used by the payload.

In addition, each encoded metadata field shall be classified as one of:

- **required semantic field**: importer must understand and validate it to preserve [Table A](#d-3-1) semantics,
- **ignorable extension field**: importer may ignore it without changing required language semantics.

Unknown required semantic fields (or unknown `required_capabilities`) shall be treated as incompatibilities.
Unknown ignorable extension fields are forward-compatible and may be skipped.

### D.2.2 Versioning rules (normative) <a id="d-2-2"></a>

`schema_major` shall be incremented for any incompatible encoding or semantic interpretation change, including:

- changing the meaning of an existing required field,
- deleting a required field without an equivalent replacement,
- reinterpreting a required field such that an older importer would miscompile accepted code.

`schema_minor` shall be incremented for compatible additive changes, including:

- adding new ignorable extension fields,
- adding new required capabilities that can be detected and rejected by older importers.

Within one `schema_major` line, a change is compatible only if an importer that does not implement the new minor revision will either:

- ignore only ignorable extension fields, or
- reject import deterministically before semantic use because a required field/capability is unknown.

Within one `schema_major` line:

- changing a required semantic field to ignorable, or changing semantic defaults for omitted required metadata, is incompatible and requires `schema_major` increment,
- changing an ignorable extension field to required semantic is allowed only with a new required capability and `schema_minor` increment so older importers hard-error instead of miscompiling,
- editorial-only/spec-clarification updates with no encoding or semantic change do not require a schema version change.

### D.2.3 Importer validation outcomes (normative) <a id="d-2-3"></a>

Importer validation distinguishes:

- **hard error**: import is rejected for that module/interface unit,
- **recoverable diagnostic**: import may continue with conservative assumptions, as allowed by profile rules in [Table E](#d-3-5).

A conforming importer shall not silently drop missing, unknown, or mismatched required semantic metadata.

Importer validation shall, at minimum:

1. validate `schema_major` support (mismatch is a hard error),
2. validate `required_capabilities` and required-field recognition (unknown required data is a hard error),
3. classify absent/invalid known-required semantic metadata as a missing-metadata condition and diagnose per [Table E](#d-3-5),
4. accept unknown ignorable extension fields without changing required [Table A](#d-3-1) semantics.

### D.2.4 Portable concurrency interface metadata (OCI-1) <a id="d-2-4"></a>

To support cross-toolchain interchange beyond native module files, implementations shall support OCI-1 metadata export/import for concurrency-relevant declarations.

OCI-1 requirements:

- schema header: `oci_schema_major`, `oci_schema_minor`,
- declaration identity key stable across emit/import within a release line,
- required concurrency fields listed in [Table F](#d-3-6).

OCI-1 versioning and compatibility follow the same rules as [D.2.2](#d-2-2) and [D.3.4](#d-3-4).
Unknown required OCI-1 fields/capabilities are hard errors.

## D.3 Required metadata tables <a id="d-3"></a>

### D.3.1 Table A — “must preserve across module boundaries” <a id="d-3-1"></a>

| Feature                                                         | Applies to                         | Must be recorded in module metadata | Must be emitted in textual interface | Notes                                                                                                  |
| --------------------------------------------------------------- | ---------------------------------- | ----------------------------------: | -----------------------------------: | ------------------------------------------------------------------------------------------------------ |
| Nullability qualifiers (`_Nullable`, `_Nonnull`)                | types, params, returns, properties |                                  ✅ |                                   ✅ | Includes inferred defaults where part of the public contract.                                          |
| Nonnull-by-default regions                                      | headers / interface units          |                                  ✅ |                                   ✅ | Canonical pragmas from [B.2](#b-2).                                                                    |
| Pragmatic generics parameters                                   | class/protocol types, collections  |                                  ✅ |                                   ✅ | ABI may erase; type checking must not.                                                                 |
| `T?`, `T!` (optional spellings)                                 | type surface                       |                                  ✅ |                                   ✅ | Emitted form may choose canonical spellings but semantics must match.                                  |
| `throws` effect                                                 | functions/methods/blocks           |                                  ✅ |                                   ✅ | ABI-affecting (see [Table B](#d-3-2)).                                                                 |
| `async` effect                                                  | functions/methods/blocks           |                                  ✅ |                                   ✅ | ABI-affecting (see [Table B](#d-3-2)).                                                                 |
| Executor affinity `objc_executor(...)`                          | funcs/methods/types                |                                  ✅ |                                   ✅ | May imply call-site `await` when crossing executors ([Part 7](#part-7)).                               |
| Actor type / actor isolation                                    | actor classes + members            |                                  ✅ |                                   ✅ | Includes nonisolated markings.                                                                         |
| Sendable-like constraints                                       | types, captures, params/returns    |                                  ✅ |                                   ✅ | Importers must be able to enforce strict checks.                                                       |
| Borrowed pointer qualifier (`borrowed T *`)                     | types (params/returns)             |                                  ✅ |                                   ✅ | [Part 8](#part-8); enables escape diagnostics in strict-system. Importers must preserve the qualifier. |
| Borrowed-return marker (`objc_returns_borrowed(owner_index=N)`) | functions/methods                  |                                  ✅ |                                   ✅ | [Part 8](#part-8); identifies the owner parameter for lifetime checking of interior pointers.          |
| Task-spawn recognition (`objc_task_spawn` etc.)                 | stdlib entry points                |                                  ✅ |                                   ✅ | Needed for compiler enforcement at call sites.                                                         |
| Direct method `objc_direct`                                     | methods                            |                                  ✅ |                                   ✅ | Changes call legality and category interactions.                                                       |
| Final `objc_final`                                              | classes/methods                    |                                  ✅ |                                   ✅ | Optimization + legality constraints.                                                                   |
| Sealed `objc_sealed`                                            | classes                            |                                  ✅ |                                   ✅ | Cross-module subclass legality.                                                                        |
| Derive/macro synthesized declarations                           | types/members                      |                                  ✅ |                                   ✅ | Synthesized members must be visible to importers before layout/ABI decisions.                          |
| Error bridging markers (`objc_nserror`, `objc_status_code`)     | funcs/methods                      |                                  ✅ |                                   ✅ | Affects lowering of `try` and wrappers.                                                                |
| Availability / platform gates                                   | all exported decls                 |                                  ✅ |                                   ✅ | Not a new ObjC 3.0 feature, but required for correctness.                                              |

### D.3.2 Table B — ABI boundary summary (v1) <a id="d-3-2"></a>

| Feature                  | ABI impact                            | Required stability property                                       | Canonical/recommended ABI shape                                                      |
| ------------------------ | ------------------------------------- | ----------------------------------------------------------------- | ------------------------------------------------------------------------------------ |
| `throws`                 | **Yes**                               | Caller and callee must agree on calling convention across modules | Recommended: trailing `id<Error> * _Nullable outError` ([C.4](#c-4)).                |
| `async`                  | **Yes**                               | Importers must call using the same coroutine/continuation ABI     | Recommended: LLVM coroutine lowering ([C.5](#c-5)), with a task/executor context.    |
| Executor/actor isolation | Usually **no** (call-site scheduling) | Metadata must exist so importer inserts hops and requires `await` | No ABI change required for sync bodies; hop is an `await`ed scheduling operation.    |
| Optional chaining/sends  | No                                    | Semantics preserved in caller IR                                  | Lower to conditional receiver check; args not evaluated on nil ([C.3.1](#c-3-1)).    |
| Direct methods           | Sometimes (dispatch surface)          | Importers must know legality + dispatch mode                      | Recommended: direct symbol call + omit dynamic lookup where permitted ([C.8](#c-8)). |
| Generics (pragmatic)     | No (erased)                           | Importers must type-check consistently                            | Erased in ABI; preserved in metadata/interface.                                      |
| Key paths                | Library ABI                           | Standard library must define stable representation                | ABI governed by stdlib; metadata must preserve `KeyPath<Root,Value>` types.          |

### D.3.3 Table C — Runtime hooks (minimum expectations) <a id="d-3-3"></a>

This table names **conceptual hooks**. Implementations may use different symbol names.

| Area                 | Minimum required capability                                          | Notes                                                                  |
| -------------------- | -------------------------------------------------------------------- | ---------------------------------------------------------------------- |
| Executors            | Enqueue continuation onto an executor; query “current executor”      | Needed for `objc_executor(...)` and for `await` resumption.            |
| Tasks                | Create child/detached tasks; join; cancellation state                | Standard library provides API; compiler enforces via attributes.       |
| Actors               | Each actor instance maps to a serial executor; enqueue isolated work | Representation is implementation-defined (inline field or side table). |
| Autorelease pools    | Push/pop implicit pools around execution slices                      | Normative on ObjC runtimes ([C.7](#c-7)).                              |
| Diagnostics metadata | Preserve enough source mapping for async/macro debugging             | [Part 12](#part-12) requires debuggability.                            |

### D.3.4 Table D — Metadata version compatibility matrix (normative) <a id="d-3-4"></a>

| Producer metadata vs importer support / payload condition             | Compatibility direction                      | Required importer behavior                                                                                    |
| --------------------------------------------------------------------- | -------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| `schema_major` equal; producer `schema_minor` <= importer max minor   | backward (new importer reads older payload)  | Accept; treat absent newer fields as unavailable.                                                             |
| `schema_major` equal; producer `schema_minor` > importer max minor    | forward (older importer reads newer payload) | Accept only if all unknown elements are ignorable extension fields and all `required_capabilities` are known. |
| `schema_major` equal; producer minor newer with unknown required data | forward                                      | Hard error: reject import; report unknown required field/capability.                                          |
| `schema_major` equal; known-required field missing/invalid in payload | both                                         | Diagnose per [Table E](#d-3-5); ABI-significant/effect-lowering omissions remain hard errors in all profiles. |
| `schema_major` differs                                                | both                                         | Hard error: reject import; report producer and importer major versions.                                       |

For forward compatibility, ignorable extension fields are explicitly non-semantic for [Table A](#d-3-1) conformance and may be skipped.

### D.3.5 Table E — Importer validation by conformance profile (normative) <a id="d-3-5"></a>

Profiles are defined in [Part 1](#part-1) and the checklist in [E.2](#e-2).

| Validation condition                                                                                                                          | Core                                                                   | Strict                  | Strict Concurrency      | Strict System           |
| --------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------- | ----------------------- | ----------------------- | ----------------------- |
| Missing or mismatched ABI-significant/effect-lowering metadata (`throws`, `async`, `objc_nserror`, `objc_status_code(...)`, dispatch markers) | Hard error                                                             | Hard error              | Hard error              | Hard error              |
| Missing or mismatched isolation metadata (`objc_executor(...)`, actor isolation/nonisolated markers)                                          | Recoverable diagnostic; conservative import                            | Hard error              | Hard error              | Hard error              |
| Missing or mismatched Sendable/task-spawn metadata                                                                                            | Recoverable diagnostic                                                 | Recoverable diagnostic  | Hard error              | Hard error              |
| Missing or mismatched borrowed/lifetime system metadata                                                                                       | Recoverable diagnostic                                                 | Recoverable diagnostic  | Recoverable diagnostic  | Hard error              |
| Unknown ignorable extension field                                                                                                             | Ignored (may emit note)                                                | Ignored (may emit note) | Ignored (may emit note) | Ignored (may emit note) |
| Unknown required field/capability                                                                                                             | Hard error                                                             | Hard error              | Hard error              | Hard error              |
| Textual interface vs module metadata mismatch for any [Table A](#d-3-1) item                                                                  | Hard error for ABI-significant items; otherwise recoverable diagnostic | Hard error              | Hard error              | Hard error              |

Missing-metadata severity by profile shall follow [Table E](#d-3-5) exactly:

- **Core**: non-ABI missing metadata may be recoverable, but diagnostics are mandatory and import must remain conservative.
- **Strict**: missing isolation metadata is a hard error; sendability/task-spawn and borrowed/lifetime metadata remain recoverable unless another row upgrades severity.
- **Strict Concurrency**: missing concurrency metadata required for isolation/sendability/task-spawn is a hard error.
- **Strict System**: missing concurrency metadata and borrowed/lifetime metadata is a hard error.

When OCI-1 is the active interchange path, missing OCI-1 fields map into these same rows by category (`effects.*`, `isolation.*`, `sendable.*`).

### D.3.6 Table F — OCI-1 required concurrency metadata fields (normative) <a id="d-3-6"></a>

| OCI-1 field              | Meaning                                                                  | Required for profile                                                      |
| ------------------------ | ------------------------------------------------------------------------ | ------------------------------------------------------------------------- |
| `decl_id`                | Stable declaration identity key for merge/join across interface payloads | Core and above                                                            |
| `effects.async`          | Whether declaration is async                                             | Core and above                                                            |
| `effects.throws`         | Whether declaration is throwing                                          | Core and above                                                            |
| `isolation.executor`     | Executor affinity (`objc_executor(...)` equivalent)                      | Core and above                                                            |
| `isolation.actor`        | Actor isolation binding for declarations/members                         | Core and above                                                            |
| `isolation.nonisolated`  | Explicit nonisolated marker                                              | Core and above                                                            |
| `sendable.boundary`      | Whether Sendable-like checking applies at this boundary                  | Strict Concurrency and Strict System (diagnostic optional in Core/Strict) |
| `sendable.unsafe_marker` | Unsafe-sendable escape marker metadata                                   | Strict Concurrency and Strict System                                      |

## D.4 Conformance tests (minimum) <a id="d-4"></a>

A conforming implementation’s test suite shall include the following module/interface tests.

### D.4.1 Normative round-trip interface test flow <a id="d-4-1"></a>

For each representative API set, tests shall perform all of:

1. Build module `M` from source declarations.
2. Emit an interface representation `I` for `M` (textual or equivalent).
3. Import `I` in a clean translation unit and separately import `M`.
4. Verify that imported declarations from step 3 are semantically equivalent for [Table A](#d-3-1) items.
5. Compile call sites that exercise the imported declarations and verify diagnostics/typing behavior are unchanged.

Minimum required declaration coverage for round-trip tests:

- `throws` declarations (function + method), including bridging markers (`objc_nserror`, `objc_status_code(...)`) and unchanged `try` obligations.
- `async` declarations with `await` call sites.
- combined `async throws` declarations with `try await` call sites.
- Executor-isolated declarations (`objc_executor(...)`) requiring a hop.
- Actor-isolated declarations, including at least one `nonisolated` member.
- Dispatch legality attributes (`objc_direct`, `objc_final`, `objc_sealed`).
- Profile-gated metadata: Sendable/task-spawn metadata and borrowed/lifetime metadata where claimed.

Round-trip verification shall compare, for each covered declaration, the effect/attribute tuple seen by importers:

- effects (`throws`, `async`) and error-bridging markers,
- isolation/executor/nonisolated metadata,
- task-spawn/sendability metadata where profile-claimed,
- dispatch and borrowed/lifetime markers where applicable.

### D.4.2 Compatibility and diagnostics tests <a id="d-4-2"></a>

A conforming test suite shall include positive and negative import tests for [Table D](#d-3-4) and [Table E](#d-3-5), including:

- importing older minor metadata with a newer importer (accepted),
- importing newer minor metadata containing only ignorable extension fields (accepted),
- importing newer metadata with unknown required fields/capabilities (hard error),
- importing metadata with different `schema_major` (hard error),
- intentionally removing or altering metadata for `throws`/`async`/error-bridging/isolation and asserting profile-appropriate diagnostics,
- profile-matrix checks for missing metadata severity (Core recoverable for non-ABI metadata; Strict isolation hard error; Strict Concurrency hard error for concurrency metadata; Strict System hard error for concurrency plus borrowed/lifetime metadata).

### D.4.3 OCI-1 export/import tests (normative minimum) <a id="d-4-3"></a>

A conforming test suite shall include OCI-1-specific tests that validate:

- exporting OCI-1 from an interface and importing it in a clean build preserves concurrency behavior (`await`/hop/isolation diagnostics),
- missing required OCI-1 fields are diagnosed per [D.3.5](#d-3-5),
- unknown required OCI-1 fields/capabilities trigger hard errors,
- additive OCI-1 minor-version fields marked ignorable are accepted,
- OCI-1 round-trip preserves `effects.async`, `effects.throws`, and required isolation/sendability fields from [Table F](#d-3-6).
<!-- END MODULE_METADATA_AND_ABI_TABLES.md -->

---

<!-- BEGIN CONFORMANCE_PROFILE_CHECKLIST.md -->

# Objective‑C 3.0 — Conformance Profile Checklist <a id="e"></a>

_Working draft v0.11 — last updated 2026-02-23_

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
- **[CONC]** required for **ObjC 3.0 v1 Strict Concurrency** (strictness `strict` + concurrency sub-mode `strict`)
- **[SYSTEM]** required for **ObjC 3.0 v1 Strict System** (strictness `strict-system` + concurrency sub-mode `strict`)

Optional feature sets:

- **[OPT-META]** metaprogramming feature set ([Part 10](#part-10))
- **[OPT-SWIFT]** Swift interop feature set ([Part 11](#part-11), Swift-facing portions)
- **[OPT-CXX]** C++-enhanced interop feature set ([Part 11](#part-11), C++-specific portions)

### E.1.3 What it means to “pass” <a id="e-1-3"></a>

A toolchain **passes** a profile when it satisfies _all_ checklist items tagged for that profile, and when it can demonstrate (via tests) that required module metadata and runtime contracts behave correctly under **separate compilation**.

### E.1.4 Normative vs QoI separation <a id="e-1-4"></a>

- Sections **E.2** and **E.3** are normative and define conformance requirements.
- Section **E.4** is non-normative quality-of-implementation (QoI) guidance and is not required to claim conformance.
- Checklist items in **E.3** shall reference normative requirements only; recommendations belong in **E.4**.

### E.1.5 v0.13 Profile-Gate Delta Binding <a id="e-1-5"></a>

For v0.13 planning-cycle closeout for issue `#792` in tranche
`BATCH-20260223-11S`, profile-gate delta evidence is anchored in:

- `spec/planning/v013_profile_gate_delta.md` (seed `V013-SPEC-04`, acceptance gate `AC-V013-SPEC-04`).
- `spec/planning/evidence/lane_d/v013_seed_spec04_validation_20260223.md` (lane D validation transcript and acceptance mapping continuity).

This anchor records post-v0.12 gate deltas for `core`, `strict`,
`strict-concurrency`, and `strict-system`, and binds the dependency chain
`EDGE-V013-002`, `EDGE-V013-003`, and transitive `EDGE-V013-018` for release
handoff consumers while preserving `AC-V013-SPEC-04-01` through
`AC-V013-SPEC-04-07` continuity.

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

- select strictness `strict` and strict concurrency sub-mode `strict`,
- treat strict concurrency as a strictness sub-mode, not as a fourth strictness level ([Part 1](#part-1)),
- enforce actor isolation, executor hops, and Sendable-like rules as specified ([Part 7](#part-7)),
- provide diagnostics for common data-race hazards and isolation violations ([Part 12](#part-12)),
- preserve concurrency-relevant metadata across modules ([D Table A](#d-3-1)).

### E.2.4 ObjC 3.0 v1 Strict System <a id="e-2-4"></a>

A toolchain claiming **ObjC 3.0 v1 Strict System** shall:

- support strict-system strictness selection ([Part 1](#part-1)),
- enable strict concurrency sub-mode (`-fobjc3-concurrency=strict`) for profile-conforming runs,
- implement the system-programming extensions in [Part 8](#part-8) (resources, borrowed pointers, capture lists) and their associated diagnostics,
- preserve borrowed/lifetime annotations across modules ([B.8](#b-8), [D Table A](#d-3-1)),
- provide at least intra-procedural enforcement for borrowed-pointer escape analysis and resource use-after-move patterns ([Part 8](#part-8), [Part 12](#part-12)).

“Strict System” is intended for SDKs and codebases like IOKit, DriverKit, and other “handle heavy” or ownership-sensitive domains.

## E.3 Checklist <a id="e-3"></a>

### E.3.1 Language mode selection, feature tests, and versioning <a id="e-3-1"></a>

- [x] SPT-0024 **[CORE]** Provide a language mode selection mechanism equivalent to `-fobjc-version=3`. ([Part 1](#part-1) [§1.2](#part-1-2)) ([Issue #48](https://github.com/doublemover/Slopjective-C/issues/48)) Evidence: `tests/conformance/parser/TUV-01.json`, `tests/conformance/parser/LMV-48-NEG-01.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_48_cli_mode_selection`.
- [x] SPT-0025 **[CORE]** Provide a source-level mechanism (pragma or directive) to set ObjC 3.0 mode for a translation unit, or document that only the flag form is supported. ([Part 1](#part-1)) ([Issue #49](https://github.com/doublemover/Slopjective-C/issues/49)) Evidence: `tests/conformance/parser/TUV-02.json`, `tests/conformance/parser/TUV-05.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_49_source_mode_selection`.
- [x] SPT-0026 **[CORE]** Enforce the v1 translation-unit-only language-version model and reject per-region language-version switching forms. ([Part 1](#part-1) [§1.2.2](#part-1-2-2), [§1.2.3](#part-1-2-3)) ([Issue #21](https://github.com/doublemover/Slopjective-C/issues/21)) Evidence: `tests/conformance/parser/TUV-03.json`, `tests/conformance/parser/TUV-04.json`, `tests/conformance/parser/TUV-05.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_49_source_mode_selection`.
- [x] SPT-0027 **[CORE]** Include conformance tests for accepted translation-unit selection forms and rejected region/late/conflicting forms (`TUV-01`..`TUV-05`). ([Part 1](#part-1) [§1.2.4](#part-1-2-4), [Part 12](#part-12) [§12.5.8](#part-12-5-8)) ([Issue #21](https://github.com/doublemover/Slopjective-C/issues/21)) Evidence: `tests/conformance/parser/TUV-03.json`, `tests/conformance/parser/TUV-04.json`, `tests/conformance/parser/TUV-05.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_49_source_mode_selection`.
- [x] SPT-0028 **[CORE]** Provide feature test macros for major feature groups (optionals, throws, async/await, actors, direct/final/sealed, derives/macros if implemented). ([Part 1](#part-1) [§1.4](#part-1-4)) ([Issue #50](https://github.com/doublemover/Slopjective-C/issues/50)) Evidence: `tests/conformance/parser/FTM-50-01.json`, `tests/conformance/parser/FTM-50-03.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_50_feature_test_macros`.
- [x] SPT-0029 **[STRICT]** Provide strictness selection equivalent to `-fobjc3-strictness=permissive|strict|strict-system`. ([Part 1](#part-1) [§1.5](#part-1-5)) ([Issue #51](https://github.com/doublemover/Slopjective-C/issues/51)) Evidence: `tests/conformance/diagnostics/SCM-01.json`, `tests/conformance/diagnostics/SCM-04.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_51_strictness_matrix`.
- [x] SPT-0030 **[STRICT]** Provide mode-selection macros `__OBJC3_STRICTNESS_LEVEL__`, `__OBJC3_CONCURRENCY_MODE__`, and `__OBJC3_CONCURRENCY_STRICT__`. ([Part 1](#part-1) [§1.4.3](#part-1-4-3)) ([Issue #44](https://github.com/doublemover/Slopjective-C/issues/44)) Evidence: `tests/conformance/diagnostics/SCM-01.json`, `tests/conformance/diagnostics/SCM-06.json`, `tests/conformance/parser/FTM-50-03.json`. Validation: `tests/conformance/diagnostics/manifest.json` groups `issue_51_strictness_matrix` and `issue_52_concurrency_submode_matrix`.
- [x] SPT-0031 **[CONC]** Provide concurrency sub-mode selection equivalent to `-fobjc3-concurrency=strict|off`; do not model strict concurrency as a fourth strictness level. ([Part 1](#part-1) [§1.6](#part-1-6), [§1.6.2](#part-1-6-2)) ([Issue #44](https://github.com/doublemover/Slopjective-C/issues/44)) Evidence: `tests/conformance/diagnostics/SCM-01.json`, `tests/conformance/diagnostics/SCM-06.json`, `tests/conformance/parser/FTM-50-03.json`. Validation: `tests/conformance/diagnostics/manifest.json` groups `issue_51_strictness_matrix` and `issue_52_concurrency_submode_matrix`.

### E.3.2 Modules, namespacing, and interface emission <a id="e-3-2"></a>

- [x] SPT-0032 **[CORE]** Implement module-aware compilation for Objective‑C declarations sufficient to support stable import, name lookup, and diagnostics. ([Part 2](#part-2)) ([Issue #53](https://github.com/doublemover/Slopjective-C/issues/53)) Evidence: `tests/conformance/module_roundtrip/MOD-53-01.json`, `tests/conformance/module_roundtrip/MOD-53-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_53_module_aware_compilation`.
- [x] SPT-0033 **[CORE]** Provide a textual interface emission mode (or equivalent) that can be used for verification in CI. ([Part 2](#part-2), [Part 12](#part-12)) ([Issue #54](https://github.com/doublemover/Slopjective-C/issues/54)) Evidence: `tests/conformance/module_roundtrip/IFC-54-01.json`, `tests/conformance/module_roundtrip/IFC-54-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_54_textual_interface_emission`.
- [x] SPT-0034 **[CORE]** Emit **canonical spellings** for ObjC 3.0 features in textual interfaces, per [B](#b). ([B.1](#b-1), [B.7](#b-7); [Part 2](#part-2)) ([Issue #55](https://github.com/doublemover/Slopjective-C/issues/55)) Evidence: `tests/conformance/module_roundtrip/IFC-55-01.json`, `tests/conformance/module_roundtrip/IFC-55-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_55_canonical_interface_spellings`.
- [x] SPT-0035 **[CORE]** Preserve and record all “must preserve” metadata in [D Table A](#d-3-1). ([D.3.1](#d-3-1)) ([Issue #56](https://github.com/doublemover/Slopjective-C/issues/56)) Evidence: `tests/conformance/module_roundtrip/META-56-01.json`, `tests/conformance/module_roundtrip/META-56-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_56_metadata_preservation`.
- [x] SPT-0036 **[STRICT]** Provide an interface verification mode that detects mismatch between compiled module metadata and emitted textual interface. ([Part 12](#part-12)) ([Issue #57](https://github.com/doublemover/Slopjective-C/issues/57)) Evidence: `tests/conformance/module_roundtrip/IFV-57-01.json`, `tests/conformance/module_roundtrip/IFV-57-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_57_interface_verification_mode`.

### E.3.3 Type system: nullability defaults, optionals, generics, key paths <a id="e-3-3"></a>

- [x] SPT-0037 **[CORE]** Support nullability qualifiers and treat them as part of the type system in ObjC 3.0 mode. ([Part 3](#part-3)) ([Issue #58](https://github.com/doublemover/Slopjective-C/issues/58)) Evidence: `tests/conformance/semantic/TYP-58-01.json`, `tests/conformance/semantic/TYP-58-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_58_nullability_qualifiers`.
- [x] SPT-0038 **[CORE]** Support nonnull-by-default regions with canonical pragma spellings ([B.2](#b-2)). ([Part 3](#part-3), [B.2](#b-2)) ([Issue #59](https://github.com/doublemover/Slopjective-C/issues/59)) Evidence: `tests/conformance/semantic/NNB-59-01.json`, `tests/conformance/semantic/NNB-59-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_59_nonnull_default_regions`.
- [x] SPT-0039 **[STRICT]** Diagnose missing nullability where required by strictness level; provide fix-its. ([Part 12](#part-12)) ([Issue #60](https://github.com/doublemover/Slopjective-C/issues/60)) Evidence: `tests/conformance/diagnostics/NUL-60-NEG-01.json`, `tests/conformance/diagnostics/NUL-60-FIX-01.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_60_strict_missing_nullability`.
- [x] SPT-0040 **[CORE]** Support optional types `T?` and IUO `T!` where specified, including: ([Issue #61](https://github.com/doublemover/Slopjective-C/issues/61)) Evidence: `tests/conformance/semantic/OPT-61-01.json`, `tests/conformance/semantic/OPT-61-03.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_61_optional_and_iuo_types`.
  - optional binding (`if let`, `guard let`) ([Part 3](#part-3), [Part 5](#part-5)),
  - optional chaining / optional message send `[receiver? sel]` ([Part 3](#part-3), [C.3.1](#c-3-1)),
  - postfix propagation `expr?` per carrier rules ([Part 3](#part-3); [Part 6](#part-6); [C.3.3](#c-3-3)).
- [x] SPT-0041 **[CORE]** Enforce v1 restriction that optional chaining is reference-only for member returns ([D-001](#decisions-d-001); [Part 3](#part-3)). ([Issue #62](https://github.com/doublemover/Slopjective-C/issues/62)) Evidence: `tests/conformance/semantic/OPT-62-NEG-01.json`, `tests/conformance/semantic/OPT-62-POS-01.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_62_reference_only_optional_chaining`.
- [x] SPT-0042 **[CORE]** Preserve optional and nullability information in module metadata and textual interfaces ([D Table A](#d-3-1)). ([Issue #63](https://github.com/doublemover/Slopjective-C/issues/63)) Evidence: `tests/conformance/module_roundtrip/META-63-01.json`, `tests/conformance/module_roundtrip/IFC-63-01.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_63_optional_nullability_preservation`.
- [x] SPT-0043 **[CORE]** Support pragmatic generics on types (erased at runtime but checked by compiler) as specified. ([Part 3](#part-3)) ([Issue #64](https://github.com/doublemover/Slopjective-C/issues/64)) Evidence: `tests/conformance/semantic/GEN-64-01.json`, `tests/conformance/semantic/GEN-64-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_64_pragmatic_generic_types`.
- [x] SPT-0044 **[CORE]** Defer generic methods/functions (do not implement, or gate behind an extension flag) per [D-008](#decisions-d-008). ([Part 3](#part-3); Decisions Log) ([Issue #65](https://github.com/doublemover/Slopjective-C/issues/65)) Evidence: `tests/conformance/semantic/GEN-65-01.json`, `tests/conformance/semantic/GEN-65-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_65_generic_methods_deferral_gate`.
- [x] SPT-0045 **[CORE]** Support key path literal and typing rules to the extent specified by [Part 3](#part-3), or clearly mark as unsupported if still provisional and do not claim the feature macro. ([Part 3](#part-3)) ([Issue #66](https://github.com/doublemover/Slopjective-C/issues/66)) Evidence: `tests/conformance/semantic/KPATH-66-01.json`, `tests/conformance/semantic/KPATH-66-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_66_key_path_literal_support`.

### E.3.4 Memory management and lifetime <a id="e-3-4"></a>

- [x] SPT-0046 **[CORE]** Preserve ObjC ARC semantics and ensure new language features lower without violating ARC rules. ([Part 4](#part-4)) ([Issue #67](https://github.com/doublemover/Slopjective-C/issues/67)) Evidence: `tests/conformance/lowering_abi/ARC-67-01.json`, `tests/conformance/lowering_abi/ARC-67-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_67_arc_semantic_preservation`.
- [x] SPT-0047 **[CORE]** Implement the suspension-point autorelease pool contract ([D-006](#decisions-d-006); [C.7](#c-7); [Part 7](#part-7)). This includes: ([Issue #68](https://github.com/doublemover/Slopjective-C/issues/68)) Evidence: `tests/conformance/lowering_abi/ARP-68-RT-01.json`, `tests/conformance/lowering_abi/ARP-68-RT-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_68_suspension_autorelease_pool_contract`.
  - creating an implicit pool for each async “slice” as specified,
  - draining it at each suspension point.
- [x] SPT-0048 **[STRICT]** Provide diagnostics for suspicious lifetime extensions, escaping of stack-bound resources, and unsafe bridging where specified. ([Part 4](#part-4), [Part 12](#part-12)) ([Issue #69](https://github.com/doublemover/Slopjective-C/issues/69)) Evidence: `tests/conformance/diagnostics/LFT-69-NEG-01.json`, `tests/conformance/diagnostics/LFT-69-FIX-01.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_69_lifetime_escape_diagnostics`.

### E.3.5 Control flow and safety constructs <a id="e-3-5"></a>

- [x] SPT-0049 **[CORE]** Implement `defer` with LIFO scope-exit semantics ([Part 5](#part-5); [Part 8](#part-8) [§8.2](#part-8-2)). ([Issue #70](https://github.com/doublemover/Slopjective-C/issues/70)) Evidence: `tests/conformance/semantic/DEF-70-01.json`, `tests/conformance/semantic/DEF-70-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_70_defer_lifo_semantics`.
- [x] SPT-0050 **[CORE]** Ensure `defer` executes on normal exit and stack unwinding exits (where applicable) as specified. ([Part 5](#part-5)/8) ([Issue #71](https://github.com/doublemover/Slopjective-C/issues/71)) Evidence: `tests/conformance/lowering_abi/DEF-71-RT-01.json`, `tests/conformance/lowering_abi/DEF-71-RT-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_71_defer_normal_and_unwind`.
- [x] SPT-0051 **[CORE]** Implement `guard` and refinement rules for optionals/patterns. ([Part 5](#part-5)) ([Issue #72](https://github.com/doublemover/Slopjective-C/issues/72)) Evidence: `tests/conformance/semantic/GRD-72-01.json`, `tests/conformance/semantic/GRD-72-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_72_guard_refinement_rules`.
- [x] SPT-0052 **[CORE]** Implement `match` (pattern matching) to the extent specified, or clearly mark as provisional and do not claim a feature macro if not implemented. ([Part 5](#part-5)) ([Issue #73](https://github.com/doublemover/Slopjective-C/issues/73)) Evidence: `tests/conformance/semantic/MTC-73-01.json`, `tests/conformance/semantic/MTC-73-GATE-01.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_73_match_or_gating`.
- [x] SPT-0053 **[STRICT]** Diagnose illegal non-local exits from `defer` bodies ([Part 5](#part-5)/8) and other ill-formed constructs. ([Issue #74](https://github.com/doublemover/Slopjective-C/issues/74)) Evidence: `tests/conformance/diagnostics/NLE-74-NEG-01.json`, `tests/conformance/diagnostics/NLE-74-NEG-02.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_74_defer_non_local_exit_diagnostics`.

### E.3.6 Errors and `throws` <a id="e-3-6"></a>

- [x] SPT-0054 **[CORE]** Implement untyped `throws` as the v1 model ([D-002](#decisions-d-002); [Part 6](#part-6)). ([Issue #75](https://github.com/doublemover/Slopjective-C/issues/75)) Evidence: `tests/conformance/semantic/THR-75-01.json`, `tests/conformance/semantic/THR-75-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_75_untyped_throws_v1`.
- [x] SPT-0055 **[CORE]** Provide a stable ABI/lowering model for `throws` ([D-009](#decisions-d-009); [C.4](#c-4); [D Table B](#d-3-2)). ([Issue #76](https://github.com/doublemover/Slopjective-C/issues/76)) Evidence: `tests/conformance/lowering_abi/THR-76-ABI-01.json`, `tests/conformance/lowering_abi/THR-76-ABI-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_76_throws_abi_stability`.
- [x] SPT-0056 **[CORE]** Support `try`, `do/catch`, and propagation rules that integrate with optionals as specified. ([Part 6](#part-6)) ([Issue #77](https://github.com/doublemover/Slopjective-C/issues/77)) Evidence: `tests/conformance/semantic/TRY-77-01.json`, `tests/conformance/semantic/TRY-77-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_77_try_do_catch_propagation`.
- [x] SPT-0057 **[CORE]** Support NSError/status-code bridging attributes where specified ([B.4](#b-4); [Part 6](#part-6)), including module metadata preservation ([D Table A](#d-3-1)). ([Issue #78](https://github.com/doublemover/Slopjective-C/issues/78)) Evidence: `tests/conformance/semantic/BRG-78-01.json`, `tests/conformance/module_roundtrip/BRG-78-MOD-01.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_78_bridging_attributes_semantic`.
- [x] SPT-0058 **[STRICT]** Provide diagnostics for ignored errors, missing `try`, and invalid bridging patterns. ([Part 12](#part-12)) ([Issue #79](https://github.com/doublemover/Slopjective-C/issues/79)) Evidence: `tests/conformance/diagnostics/ERR-79-NEG-01.json`, `tests/conformance/diagnostics/ERR-79-FIX-01.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_79_throws_misuse_diagnostics`.

### E.3.7 Concurrency: `async/await`, executors, cancellation, actors <a id="e-3-7"></a>

- [x] SPT-0059 **[CORE]** Implement `async` and `await` grammar and typing rules as specified. ([Part 7](#part-7)) ([Issue #80](https://github.com/doublemover/Slopjective-C/issues/80)) Evidence: `tests/conformance/parser/ASY-01.json`, `tests/conformance/semantic/ASY-04.json`, `tests/conformance/diagnostics/ASY-06.json`. Validation: parser/semantic/diagnostics manifests for `issue_80_async_await_*` groups.
- [x] SPT-0060 **[CORE]** Implement a coroutine-based lowering model for `async` ([D-010](#decisions-d-010); [C.5](#c-5)) and preserve required ABI metadata ([D Table B](#d-3-2)). ([Issue #81](https://github.com/doublemover/Slopjective-C/issues/81)) Evidence: `tests/conformance/lowering_abi/CORO-01.json`, `tests/conformance/lowering_abi/CORO-03.json`, `tests/conformance/module_roundtrip/CORO-04.json`. Validation: lowering/module manifests for `issue_81_async_coroutine_*` groups.
- [x] SPT-0061 **[CORE]** Implement cancellation propagation and task-context behavior as specified. ([Part 7](#part-7); [C.5.2](#c-5-2)) ([Issue #82](https://github.com/doublemover/Slopjective-C/issues/82)) Evidence: `tests/conformance/semantic/CAN-01.json`, `tests/conformance/lowering_abi/CAN-07.json`. Validation: semantic and lowering manifests for `issue_82_cancellation_*` groups.
- [x] SPT-0062 **[CORE]** Implement executor affinity annotations and call-site behavior: ([Issue #83](https://github.com/doublemover/Slopjective-C/issues/83)) Evidence: `tests/conformance/semantic/EXEC-ATTR-01.json`, `tests/conformance/lowering_abi/EXE-04.json`, `tests/conformance/module_roundtrip/EXEC-ATTR-03.json`. Validation: semantic/lowering/module manifests for `issue_83_executor_*` groups.
  - accept canonical `objc_executor(...)` spellings ([B.3.1](#b-3-1)),
  - preserve in module metadata ([D Table A](#d-3-1)),
  - perform required hops as specified. ([Part 7](#part-7); [C.5.3](#c-5-3))
- [x] SPT-0063 **[CORE]** Implement actor declarations and actor isolation rules ([Part 7](#part-7); [C.6](#c-6)). ([Issue #84](https://github.com/doublemover/Slopjective-C/issues/84)) Evidence: `tests/conformance/semantic/ACT-01.json`, `tests/conformance/lowering_abi/ACT-07.json`, `tests/conformance/module_roundtrip/ACT-09.json`. Validation: semantic/lowering/module manifests for `issue_84_actor_*` groups.
- [x] SPT-0064 **[STRICT]** Provide diagnostics for isolation violations, invalid executor annotations, and suspicious cross-actor calls. ([Part 12](#part-12)) ([Issue #85](https://github.com/doublemover/Slopjective-C/issues/85)) Evidence: `tests/conformance/diagnostics/CONC-DIAG-01.json`, `tests/conformance/diagnostics/CONC-DIAG-06.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_85_isolation_executor_diagnostics`.
- [x] SPT-0065 **[CONC]** Enforce Sendable-like constraints for: ([Issue #86](https://github.com/doublemover/Slopjective-C/issues/86)) Evidence: `tests/conformance/semantic/SND-01.json`, `tests/conformance/module_roundtrip/SND-XM-02.json`, `tests/conformance/diagnostics/SND-08.json`. Validation: semantic/module/diagnostics manifests for `issue_86_sendable_*` groups.
  - captured values in `async` blocks/tasks,
  - parameters/returns across actor boundaries,
  - values crossing executor domains. ([Part 7](#part-7); [D Table A](#d-3-1))
- [x] SPT-0066 **[CONC]** Enforce [D-011](#decisions-d-011): require `await` for any potentially-suspending operation, not only explicit `async` calls. (Decisions Log; [Part 7](#part-7)) ([Issue #87](https://github.com/doublemover/Slopjective-C/issues/87)) Evidence: `tests/conformance/parser/AWT-01.json`, `tests/conformance/semantic/AWT-04.json`, `tests/conformance/diagnostics/AWT-06.json`. Validation: parser/semantic/diagnostics manifests for `issue_87_d011_await_requirement_*` groups.

### E.3.8 System programming extensions (Part 8) <a id="e-3-8"></a>

- [x] SPT-0067 **[SYSTEM]** Support canonical attribute spellings for [Part 8](#part-8) features ([B.8](#b-8)), including: ([Issue #88](https://github.com/doublemover/Slopjective-C/issues/88)) Evidence: `tests/conformance/parser/SYS-ATTR-01.json`, `tests/conformance/parser/SYS-ATTR-04.json`, `tests/conformance/module_roundtrip/SYS-ATTR-08.json`. Validation: parser/module manifests for `issue_88_*` groups.
  - `objc_resource(close=..., invalid=...)`,
  - `objc_returns_borrowed(owner_index=...)`,
  - `borrowed T *` type qualifier,
  - capture list contextual keywords.
- [x] SPT-0068 **[SYSTEM]** Implement resource cleanup semantics and associated diagnostics ([Part 8](#part-8) [§8.3](#part-8-3); [Part 12](#part-12) [§12.3.6](#part-12-3-6)). ([Issue #89](https://github.com/doublemover/Slopjective-C/issues/89)) Evidence: `tests/conformance/semantic/RES-01.json`, `tests/conformance/lowering_abi/RES-04.json`, `tests/conformance/diagnostics/RES-06.json`. Validation: semantic/lowering/diagnostics manifests for `issue_89_resource_cleanup_*` groups.
- [x] SPT-0069 **[SYSTEM]** Implement `withLifetime` / `keepAlive` semantics and ensure they interact correctly with ARC. ([Part 8](#part-8) [§8.6](#part-8-6)) ([Issue #90](https://github.com/doublemover/Slopjective-C/issues/90)) Evidence: `tests/conformance/semantic/LIFE-01.json`, `tests/conformance/lowering_abi/LIFE-05.json`. Validation: semantic/lowering manifests for `issue_90_lifetime_extension_*` groups.
- [x] SPT-0070 **[SYSTEM]** Implement borrowed pointer rules and diagnostics ([Part 8](#part-8) [§8.7](#part-8-7)) at least intra-procedurally. ([Issue #91](https://github.com/doublemover/Slopjective-C/issues/91)) Evidence: `tests/conformance/semantic/BRW-NEG-01.json`, `tests/conformance/semantic/BRW-POS-04.json`, `tests/conformance/diagnostics/BRW-NEG-05.json`. Validation: semantic/diagnostics manifests for `issue_91_borrowed_pointer_*` groups.
- [x] SPT-0071 **[SYSTEM]** Preserve borrowed/lifetime annotations in module metadata ([D Table A](#d-3-1)). ([Issue #92](https://github.com/doublemover/Slopjective-C/issues/92)) Evidence: `tests/conformance/module_roundtrip/BRW-META-01.json`, `tests/conformance/module_roundtrip/BRW-META-03.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_92_borrowed_lifetime_metadata_roundtrip`.
- [x] SPT-0072 **[SYSTEM]** Implement capture lists ([Part 8](#part-8) [§8.8](#part-8-8)) with required evaluation order and move/weak/unowned semantics. ([Issue #93](https://github.com/doublemover/Slopjective-C/issues/93)) Evidence: `tests/conformance/parser/CAP-01.json`, `tests/conformance/semantic/CAP-05.json`, `tests/conformance/diagnostics/CAP-08.json`. Validation: parser/semantic/diagnostics manifests for `issue_93_capture_list_*` groups.
- [x] SPT-0073 **[SYSTEM]** Provide diagnostics for: ([Issue #94](https://github.com/doublemover/Slopjective-C/issues/94)) Evidence: `tests/conformance/diagnostics/SYS-DIAG-01.json`, `tests/conformance/diagnostics/SYS-DIAG-08.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_94_system_hazard_diagnostics`.
  - borrowed escape to heap/global/ivar,
  - borrowed escape into `@escaping` blocks,
  - use-after-move for resources,
  - dangerous `unowned` captures. ([Part 12](#part-12))

### E.3.9 Performance and dynamism controls <a id="e-3-9"></a>

- [x] SPT-0074 **[CORE]** Accept and preserve canonical spellings for `objc_direct`, `objc_final`, `objc_sealed` ([B.5](#b-5); [Part 9](#part-9)). ([Issue #95](https://github.com/doublemover/Slopjective-C/issues/95)) Evidence: `tests/conformance/parser/PERF-ATTR-01.json`, `tests/conformance/module_roundtrip/PERF-ATTR-04.json`. Validation: parser/module manifests for `issue_95_performance_attribute_*` groups.
- [x] SPT-0075 **[CORE]** Enforce legality rules across categories/extensions and module boundaries as specified. ([Part 9](#part-9); [C.8](#c-8); [D](#d)) ([Issue #96](https://github.com/doublemover/Slopjective-C/issues/96)) Evidence: `tests/conformance/semantic/PERF-DIRMEM-01.json`, `tests/conformance/semantic/PERF-LEG-02.json`, `tests/conformance/module_roundtrip/PERF-LEG-03.json`. Validation: semantic/module manifests for `issue_96_performance_legality_*` groups.
- [x] SPT-0076 **[STRICT]** Provide diagnostics for calling direct methods via dynamic dispatch, illegal overrides of final/sealed, and related misuse. ([Part 12](#part-12)) ([Issue #97](https://github.com/doublemover/Slopjective-C/issues/97)) Evidence: `tests/conformance/diagnostics/PERF-DYN-01.json`, `tests/conformance/diagnostics/PERF-DIAG-04.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_97_performance_misuse_diagnostics`.

### E.3.10 Metaprogramming (optional feature set) <a id="e-3-10"></a>

- [x] SPT-0077 **[OPT-META]** Implement derives (`objc_derive(...)`) with deterministic, tool-visible expansion. ([Part 10](#part-10); [B.6.1](#b-6-1)) ([Issue #98](https://github.com/doublemover/Slopjective-C/issues/98)) Evidence: `tests/conformance/semantic/META-DRV-01.json`, `tests/conformance/module_roundtrip/META-EXP-02.json`. Validation: semantic/module manifests for `issue_98_*` groups.
- [x] SPT-0078 **[OPT-META]** Implement macros (`objc_macro(...)`) with sandboxing / safety constraints as specified. ([Part 10](#part-10); [B.6.2](#b-6-2)) ([Issue #99](https://github.com/doublemover/Slopjective-C/issues/99)) Evidence: `tests/conformance/semantic/META-PKG-01.json`, `tests/conformance/diagnostics/META-MAC-04.json`. Validation: semantic/diagnostics manifests for `issue_99_*` groups.
- [x] SPT-0079 **[OPT-META]** Preserve macro/derive expansions in module metadata and textual interfaces as required by D. ([D Table A](#d-3-1)) ([Issue #100](https://github.com/doublemover/Slopjective-C/issues/100)) Evidence: `tests/conformance/module_roundtrip/META-XM-01.json`, `tests/conformance/module_roundtrip/META-XM-04.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_100_macro_derive_roundtrip_preservation`.

### E.3.11 Interoperability (optional feature sets) <a id="e-3-11"></a>

- [x] SPT-0080 **[CORE]** Maintain full interop with C and Objective‑C runtime behavior. (Baseline + [Part 11](#part-11)) ([Issue #101](https://github.com/doublemover/Slopjective-C/issues/101)) Evidence: `tests/conformance/semantic/INT-C-01.json`, `tests/conformance/lowering_abi/INT-C-12.json`. Validation: semantic/lowering manifests for `issue_101_c_interop_*` groups.
- [x] SPT-0081 **[OPT-CXX]** Document and test ObjC++ interactions for ownership, `throws`, and `async` lowering. ([Part 11](#part-11); C) ([Issue #102](https://github.com/doublemover/Slopjective-C/issues/102)) Evidence: `tests/conformance/semantic/INT-CXX-01.json`, `tests/conformance/lowering_abi/INT-CXX-08.json`. Validation: semantic/lowering manifests for `issue_102_objcxx_interop_*` groups.
- [x] SPT-0082 **[OPT-SWIFT]** Provide a Swift interop story (import/export) for: ([Issue #103](https://github.com/doublemover/Slopjective-C/issues/103)) Evidence: `tests/conformance/semantic/INT-SWIFT-01.json`, `tests/conformance/module_roundtrip/INT-SWIFT-08.json`. Validation: semantic/module manifests for `issue_103_swift_interop_*` groups.
  - optionals/nullability,
  - `throws` bridging,
  - `async/await` bridging,
  - actors and isolation metadata. ([Part 11](#part-11))

### E.3.12 Diagnostics, tooling, and tests <a id="e-3-12"></a>

- [x] SPT-0083 **[CORE]** Implement the minimum diagnostic groups in [Part 12](#part-12): ([Issue #104](https://github.com/doublemover/Slopjective-C/issues/104)) Evidence: `tests/conformance/diagnostics/DIAG-GRP-01.json`, `tests/conformance/diagnostics/DIAG-GRP-10.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_104_diagnostic_group_matrix`.
  - nullability/optionals,
  - throws,
  - concurrency,
  - modules/interface emission,
  - performance controls. ([Part 12](#part-12) [§12.3](#part-12-3))
- [x] SPT-0084 **[STRICT]** Provide fix-its and migrator support to move legacy code toward canonical spellings and safer idioms. ([Part 12](#part-12) [§12.4](#part-12-4)) ([Issue #105](https://github.com/doublemover/Slopjective-C/issues/105)) Evidence: `tests/conformance/diagnostics/MIG-01.json`, `tests/conformance/diagnostics/MIG-08.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_105_migrator_fixit_suite`.
- [x] SPT-0085 **[CORE]** Provide machine-readable conformance report emission and validation equivalent to `--emit-objc3-conformance`, `--emit-objc3-conformance-format`, and `--validate-objc3-conformance`. ([Part 12](#part-12) [§12.4.4](#part-12-4-4), [§12.4.5](#part-12-4-5)) ([Issue #43](https://github.com/doublemover/Slopjective-C/issues/43)) Evidence: `tests/conformance/diagnostics/CRPT-01.json`, `tests/conformance/diagnostics/CRPT-06.json`, `schemas/objc3-conformance-dashboard-status-v1.schema.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_106_conformance_report_validation` and `scripts/check_release_evidence.py`.
- [x] SPT-0086 **[CORE]** Provide or publish a conformance test suite covering: ([Issue #106](https://github.com/doublemover/Slopjective-C/issues/106)) Evidence: `tests/conformance/README.md`, `tests/conformance/COVERAGE_MAP.md`, `tests/conformance/diagnostics/CRPT-01.json`. Validation: bucket manifests `tests/conformance/*/manifest.json` and diagnostics group `issue_106_conformance_report_validation`.
  - required repository buckets (`parser`, `semantic`, `lowering_abi`, `module_roundtrip`, `diagnostics`) and mapped responsibilities, ([Part 12](#part-12) [§12.5.0](#part-12-5-0))
  - parsing/grammar, type system/diagnostics, dynamic semantics, runtime contracts, and metadata/interface preservation coverage, ([Part 12](#part-12) [§12.5](#part-12-5))
  - cross-module effects/attributes preservation checks (`throws`, `async`, isolation, executor affinity), ([Part 12](#part-12) [§12.5.2](#part-12-5-2))
  - profile minimum counts (Core/Strict/Strict Concurrency/Strict System), ([Part 12](#part-12) [§12.5.6](#part-12-5-6))
  - portable diagnostic assertions (`code`, `severity`, `span`, required fix-its). ([Part 12](#part-12) [§12.5.7](#part-12-5-7))
- [x] SPT-0087 **[CORE]** Include conformance-report schema/content tests (`CRPT-01`..`CRPT-06`) covering emission, required keys, enum validity, and known-deviation shape checks. ([Part 12](#part-12) [§12.5.9](#part-12-5-9)) ([Issue #43](https://github.com/doublemover/Slopjective-C/issues/43)) Evidence: `tests/conformance/diagnostics/CRPT-01.json`, `tests/conformance/diagnostics/CRPT-06.json`, `schemas/objc3-conformance-dashboard-status-v1.schema.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_106_conformance_report_validation` and `scripts/check_release_evidence.py`.
- [x] SPT-0088 **[CONC]** Include strictness/sub-mode consistency tests (`SCM-01`..`SCM-06`) covering diagnostics, macro values, and profile mapping behavior. ([Part 12](#part-12) [§12.5.10](#part-12-5-10), [Part 1](#part-1) [§1.6.2](#part-1-6-2)) ([Issue #44](https://github.com/doublemover/Slopjective-C/issues/44)) Evidence: `tests/conformance/diagnostics/SCM-01.json`, `tests/conformance/diagnostics/SCM-06.json`, `tests/conformance/parser/FTM-50-03.json`. Validation: `tests/conformance/diagnostics/manifest.json` groups `issue_51_strictness_matrix` and `issue_52_concurrency_submode_matrix`.
- [x] SPT-0089 **[SYSTEM]** Include tests for borrowed-pointer escape diagnostics and resource cleanup semantics. ([Part 8](#part-8); [Part 12](#part-12)) ([Issue #107](https://github.com/doublemover/Slopjective-C/issues/107)) Evidence: `tests/conformance/diagnostics/BRW-NEG-03.json`, `tests/conformance/diagnostics/RES-06.json`, `tests/conformance/lowering_abi/RES-04.json`. Validation: diagnostics manifests groups `issue_89_resource_cleanup_diagnostics`, `issue_91_borrowed_pointer_diagnostics`, and `issue_94_system_hazard_diagnostics`.

## E.4 Recommended evidence for a conformance claim (non-normative) <a id="e-4"></a>

A serious conformance claim should ship with:

- a public machine-readable conformance report (JSON required; YAML optional) listing claimed profiles, optional feature sets, versions, and known deviations,
- CI proofs that:
  - module interfaces round-trip (emit → import) without semantic loss,
  - [D Table A](#d-3-1) metadata is preserved under separate compilation,
  - runtime contracts for `throws` and `async` behave correctly under optimization,
- migration tooling notes for large codebases (warning groups, fix-its, staged adoption).
<!-- END CONFORMANCE_PROFILE_CHECKLIST.md -->

---

<!-- BEGIN STANDARD_LIBRARY_CONTRACT.md -->

# Objective-C 3.0 - Standard Library Contract (Minimum) <a id="s"></a>

_Working draft v0.11 - last updated 2026-02-23_

## S.0 Purpose and scope (normative) <a id="s-0"></a>

This document defines the minimum standard-library surface required for Objective-C 3.0 conformance claims.

It specifies:

- required module names and capability identifiers,
- minimum exported types/functions for each required module,
- semantic contracts needed by language rules in [Part 3](#part-3), [Part 6](#part-6), [Part 7](#part-7), and [Part 12](#part-12),
- versioning and ABI-stability expectations,
- conformance test obligations for required library APIs.

This contract is normative for toolchains claiming ObjC 3.0 Core/Strict/Strict Concurrency/Strict System profiles.

## S.1 Required modules and capability identifiers (normative) <a id="s-1"></a>

### S.1.1 Canonical module names <a id="s-1-1"></a>

A conforming implementation shall provide the following modules (or aliases mapped to these canonical capability IDs):

| Canonical module    | Capability ID           | Required for profile |
| ------------------- | ----------------------- | -------------------- |
| `objc3.core`        | `objc3.cap.core`        | Core and above       |
| `objc3.errors`      | `objc3.cap.errors`      | Core and above       |
| `objc3.concurrency` | `objc3.cap.concurrency` | Core and above       |
| `objc3.keypath`     | `objc3.cap.keypath`     | Core and above       |
| `objc3.system`      | `objc3.cap.system`      | Strict System        |

If implementation-specific names are used, module metadata and conformance reports shall publish the canonical capability-ID mapping.

### S.1.2 Discovery and import requirements <a id="s-1-2"></a>

- Importing required modules shall preserve effects/isolation metadata required by [D Table A](#d-3-1).
- Missing required modules for a claimed profile is a conformance failure and shall be diagnosed.
- Capability IDs shall be queryable from emitted interface/module metadata.

## S.2 Minimum API surface by module (normative) <a id="s-2"></a>

### S.2.1 `objc3.core` <a id="s-2-1"></a>

`objc3.core` shall provide:

- baseline protocol/type aliases needed by this draft to name core contracts,
- feature/capability query hooks sufficient for profile-aware diagnostics,
- public constants or metadata exposing language/profile revision identifiers.

### S.2.2 `objc3.errors` <a id="s-2-2"></a>

`objc3.errors` shall provide at least:

- protocol `Error` (or equivalent) matching [Part 6](#part-6),
- carrier `Result<T, E>` with `Ok`/`Err` construction and inspection,
- helpers required to bridge status-code/NSError-style APIs to `throws`/`Result` flows.

Canonical nil-to-error helpers (required):

- `orThrow<T>(value: T?, makeError: () -> id<Error>) throws -> T`
- `okOr<T, E: Error>(value: T?, makeError: () -> E) -> Result<T, E>`

Minimum semantic guarantees:

- `Error` values are stably representable across module boundaries.
- `Result` case identity and payload layout are stable enough for separate compilation.
- Conversions used by language-defined lowering preserve error payloads without silent loss.
- `orThrow` and `okOr` shall evaluate `value` exactly once.
- `orThrow`/`okOr` shall evaluate `makeError` lazily and at most once, only when `value` is `nil`.
- `orThrow` shall throw exactly the produced `Error` value on `nil`; `okOr` shall return `Err(producedError)` on `nil`.

### S.2.3 `objc3.concurrency` <a id="s-2-3"></a>

`objc3.concurrency` shall provide at least:

- child-task spawn primitive (recognized as structured spawn),
- detached-task spawn primitive,
- task-handle join/wait APIs (throwing and/or status-returning forms),
- task-group scope APIs,
- cancellation query and cancellation checkpoint APIs,
- executor hop primitive or equivalent runtime hook.

Minimum semantic guarantees:

- child/detached cancellation behavior matches [Part 7](#part-7),
- join/wait APIs reflect cancellation/error outcomes without reporting false success,
- executor/actor boundaries preserve required suspension semantics.

### S.2.4 `objc3.keypath` <a id="s-2-4"></a>

`objc3.keypath` shall provide at least:

- key-path value type(s) for typed key-path literals,
- lookup/application APIs sufficient to evaluate key paths against supported roots,
- metadata representation that preserves type components across module boundaries.

### S.2.5 `objc3.system` (profile-gated) <a id="s-2-5"></a>

For Strict System claims, `objc3.system` shall provide:

- resource cleanup helpers needed by [Part 8](#part-8),
- borrowed/lifetime helper intrinsics where language rules require runtime cooperation,
- diagnostics/tooling hooks required by [Part 12](#part-12) strict-system diagnostics.

## S.3 Semantic contracts for required primitives (normative) <a id="s-3"></a>

### S.3.1 Errors and propagation <a id="s-3-1"></a>

- `throws` lowering and `Result` adapters shall be interoperable under separate compilation.
- `try`/`try?`/propagation behavior shall remain consistent whether declarations are imported textually or via module metadata.
- `orThrow` shall participate in `try` as an ordinary throwing call; `okOr` shall participate in postfix `?` as an ordinary `Result` value per [Part 6](#part-6).

### S.3.2 Concurrency primitives <a id="s-3-2"></a>

- Spawn APIs must preserve structured-parent relationships when marked as child spawn.
- Detached APIs must not inherit cancellation unless explicitly documented.
- Cancellation checkpoints must be observable and testable.
- Executor hop APIs must preserve current-task context and required isolation metadata.

### S.3.3 Key-path behavior <a id="s-3-3"></a>

- Key-path application must preserve nullability/type constraints defined in [Part 3](#part-3).
- Invalid key-path application shall diagnose per profile requirements.

## S.4 Versioning and ABI expectations (normative) <a id="s-4"></a>

### S.4.1 Module versioning model <a id="s-4-1"></a>

Each required module shall expose semantic version metadata:

- `module_semver_major`,
- `module_semver_minor`,
- `module_semver_patch`,
- and supported capability IDs.

Major-version changes indicate potentially incompatible API/ABI changes.
Minor-version changes are additive and backward compatible.

### S.4.2 ABI stability requirements <a id="s-4-2"></a>

For claimed profile conformance:

- exported signatures for required APIs shall be stable within a major version,
- metadata describing effects/isolation/capabilities shall satisfy [Part 2](#part-2) and [D](#d) compatibility rules,
- mixing module binaries/interfaces with incompatible major versions shall be diagnosed.

### S.4.3 Profile-specific stability requirements <a id="s-4-3"></a>

- Core/Strict claims require stable `objc3.core`, `objc3.errors`, `objc3.concurrency`, and `objc3.keypath` contracts.
- Strict System claims additionally require stable `objc3.system` contracts.
- Optional feature-set modules are out of scope unless that feature-set conformance is claimed.

## S.5 Conformance test obligations (normative minimum) <a id="s-5"></a>

A conforming implementation shall include tests equivalent in coverage to:

1. Module presence/discovery:
   - required module import succeeds for claimed profile;
   - missing required module is diagnosed.
2. Error model:
   - `Error` and `Result` APIs interoperate with `throws` lowering across module boundaries.
   - `orThrow` with non-`nil` input returns payload and does not evaluate `makeError`.
   - `orThrow` with `nil` input throws the value produced by `makeError` (single evaluation).
   - `okOr` with non-`nil` input returns `Ok(payload)` and does not evaluate `makeError`.
   - `okOr` with `nil` input returns `Err(producedError)` from a single `makeError` evaluation.
   - `try orThrow(...)` and `okOr(...)?` compile and behave as specified in [Part 6](#part-6).
3. Concurrency:
   - child vs detached semantics, cancellation propagation, join/wait outcomes, executor hops.
4. Key paths:
   - typed key-path metadata round-trip and runtime application behavior.
5. Versioning:
   - incompatibility diagnostics for major-version mismatch;
   - compatibility for additive minor updates.

## S.6 Relationship to conformance checklist (normative) <a id="s-6"></a>

- [E.3](#e-3) profile checklist items that reference standard-library requirements shall be interpreted using this document.
- Toolchains shall cite this contract in conformance manifests when claiming Core/Strict/Strict Concurrency/Strict System profiles.
<!-- END STANDARD_LIBRARY_CONTRACT.md -->

---

<!-- BEGIN ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md -->

# Objective-C 3.0 - Abstract Machine and Semantic Core <a id="am"></a>

_Working draft v0.11 - last updated 2026-02-23_

## AM.0 Purpose and scope <a id="am-0"></a>

This document defines the unified abstract-machine rules that span:

- [Part 3](#part-3) (optionals and optional chaining),
- [Part 5](#part-5) (control-flow exits and `defer`),
- [Part 6](#part-6) (throws/try/propagation),
- [Part 7](#part-7) (async/await),
- [Part 8](#part-8) (defer and resource cleanup).

This document is normative for cross-part behavior. Part-specific rules remain normative for construct-local typing and syntax.

If two rules appear to overlap, the more specific construct rule applies, and this document defines how the rules compose.

### AM.0.1 Normative anchor map <a id="am-0-1"></a>

This document composes construct-local rules from the following sections:

- [Part 3](#part-3): [§3.3.2](#part-3-3-2), [§3.3.4](#part-3-3-4), [§3.4.1](#part-3-4-1), [§3.4.2](#part-3-4-2), [§3.4.2.4](#part-3-4-2-4).
- [Part 5](#part-5): [§5.2](#part-5-2), [§5.2.4](#part-5-2-4), [§5.3.2](#part-5-3-2), [§5.4.3](#part-5-4-3).
- [Part 6](#part-6): [§6.5.3](#part-6-5-3), [§6.5.4](#part-6-5-4), [§6.6](#part-6-6), [§6.6.4](#part-6-6-4).
- [Part 7](#part-7): [§7.3](#part-7-3), [§7.6.5](#part-7-6-5), [§7.9.1](#part-7-9-1), [§7.9.2](#part-7-9-2), [§7.9.3](#part-7-9-3), [§7.9.4](#part-7-9-4).
- [Part 8](#part-8): [§8.1](#part-8-1), [§8.2.3](#part-8-2-3), [§8.3](#part-8-3), [§8.6](#part-8-6), [§8.8.3](#part-8-8-3).

## AM.1 Abstract machine state <a id="am-1"></a>

For a running function/task, the abstract machine tracks:

- a lexical scope stack;
- for each scope, a scope-exit action stack (cleanup stack);
- current evaluation state for the active full-expression;
- for `async` functions, an async frame that stores values live across suspension;
- for Objective-C runtimes with autorelease semantics, the implicit autorelease pool for the current task execution slice.

### AM.1.1 Full-expression boundary <a id="am-1-1"></a>

A full-expression boundary is the point where temporaries that are not lifetime-extended may be destroyed.

### AM.1.2 Scope-exit action stack <a id="am-1-2"></a>

A scope-exit action stack contains actions registered by:

- executed `defer` statements ([Part 5](#part-5) [§5.2](#part-5-2));
- successful initialization of cleanup/resource locals ([Part 8](#part-8) [§8.3](#part-8-3)).

Actions execute only when the scope exits; mere suspension at `await` is not scope exit.

## AM.2 Expression evaluation order contract <a id="am-2"></a>

### AM.2.1 Baseline preserved <a id="am-2-1"></a>

Objective-C 3.0 does not globally replace baseline C/Objective-C evaluation-order rules.

Unless a rule in this specification explicitly adds ordering, ordinary C/Objective-C expression ordering remains baseline behavior.

In particular, Objective-C 3.0 does not add a universal left-to-right argument evaluation guarantee for ordinary C calls or ordinary Objective-C message sends.

### AM.2.2 Additional ObjC 3.0 guarantees (normative delta) <a id="am-2-2"></a>

Objective-C 3.0 adds the following ordering/single-evaluation guarantees relative to the baseline:

| Construct                              | Added guarantee                                                                                                                   | Primary source                              |
| -------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------- |
| `x?.p`                                 | `x` shall be evaluated exactly once; if `x == nil`, result is `nil` and property access is not performed.                         | [Part 3](#part-3) [§3.4.1.2](#part-3-4-1-2) |
| `[receiver? sel:arg1 other:arg2]`      | `receiver` shall be evaluated exactly once; if `receiver == nil`, argument expressions shall not be evaluated and no send occurs. | [Part 3](#part-3) [§3.4.2.4](#part-3-4-2-4) |
| `a ?? b`                               | `a` shall be evaluated first and exactly once; `b` shall be evaluated only if `a` is `nil`.                                       | [Part 3](#part-3) [§3.3.4.2](#part-3-3-4-2) |
| `e?` (postfix propagation)             | `e` shall be evaluated exactly once before deciding unwrap vs early exit.                                                         | [Part 6](#part-6) [§6.6](#part-6-6)         |
| `if let` / `guard let` binding lists   | Binding expressions shall be evaluated left-to-right.                                                                             | [Part 3](#part-3) [§3.3.2.2](#part-3-3-2-2) |
| `guard` condition lists                | Conditions shall be evaluated left-to-right.                                                                                      | [Part 5](#part-5) [§5.3.2](#part-5-3-2)     |
| `match (expr)`                         | `expr` shall be evaluated exactly once; case tests are top-to-bottom.                                                             | [Part 5](#part-5) [§5.4.3](#part-5-4-3)     |
| Block capture list `[cap1, cap2, ...]` | Capture items shall be evaluated left-to-right at block creation time.                                                            | [Part 8](#part-8) [§8.8.3](#part-8-8-3)     |

No other new global expression-order guarantees are introduced in v1.

### AM.2.3 Suspension sequencing boundary <a id="am-2-3"></a>

For `await e`:

- user-visible side effects sequenced before the suspension point shall occur before suspension;
- evaluation that is sequenced after `await` shall not occur until resumption;
- lowering shall not duplicate user subexpression evaluation solely due to suspend/resume transformation.

### AM.2.4 Where ObjC 3.0 changes baseline guarantees <a id="am-2-4"></a>

Relative to baseline C/Objective-C, ObjC 3.0 adds only the explicit guarantees listed in [AM.2.2](#am-2-2), plus cross-construct composition guarantees in [AM.4.4](#am-4-4), [AM.5](#am-5), and [AM.6](#am-6).

Outside those sections, evaluation order and sequencing remain baseline/implementation-defined as in ordinary C/Objective-C.

## AM.3 Temporaries and lifetime rules <a id="am-3"></a>

### AM.3.1 Base lifetime rule <a id="am-3-1"></a>

A temporary created during expression evaluation shall remain valid until at least the end of its containing full-expression, unless a rule below extends it further.

### AM.3.2 Lifetime extension points <a id="am-3-2"></a>

A temporary or local lifetime is extended when required by:

- explicit lifetime controls (`withLifetime`, `keepAlive`, precise-lifetime annotations in [Part 8](#part-8));
- capture into storage that outlives the full-expression (for example block captures);
- async suspension requirements in [AM.3.3](#am-3-3).

### AM.3.3 Values live across `await` <a id="am-3-3"></a>

Any value needed after a potentially suspending `await` shall be materialized in async-frame storage and kept alive across suspension.

For ARC-managed values:

- the implementation shall retain frame-stored strong values as needed before suspension; and
- shall release them when they become dead (or when the async frame is destroyed by return, throw, or cancellation unwind).

Values proven dead before suspension may be released before suspension unless prohibited by precise-lifetime rules.

### AM.3.4 Hidden temporaries in short-circuit forms <a id="am-3-4"></a>

Hidden temporaries used to implement `?.`, optional send, `??`, `try?`, and postfix `?` shall:

- preserve exactly-once evaluation guarantees; and
- not outlive the enclosing full-expression unless required by [AM.3.3](#am-3-3) or explicit lifetime controls.

## AM.4 Cleanup stack behavior <a id="am-4"></a>

### AM.4.1 Registration <a id="am-4-1"></a>

Within a lexical scope, scope-exit actions are registered when:

- a `defer` statement executes; or
- a cleanup/resource local completes successful initialization.

Registration is dynamic: code paths not executed do not register actions.

### AM.4.2 Exit triggers <a id="am-4-2"></a>

Scope-exit actions run when leaving the scope via:

- fallthrough to scope end;
- `return`, `break`, `continue`, or `goto` leaving the scope;
- structured error propagation (`throw`, `try` propagation, postfix `?` early exit);
- cancellation unwind paths defined by [Part 7](#part-7);
- exception unwinding paths that run language cleanups.

Non-local transfers that bypass cleanups (for example `longjmp` across cleanup scopes) are outside guarantees and are undefined unless the platform ABI explicitly guarantees cleanup execution.

### AM.4.3 Per-scope execution order <a id="am-4-3"></a>

For a scope `S`, registered scope-exit actions shall execute in reverse registration order (LIFO).

For exits that leave multiple nested scopes, scopes are processed from innermost to outermost, applying LIFO within each scope.

### AM.4.4 Ordering with implicit ARC releases <a id="am-4-4"></a>

In each exiting scope:

1. registered scope-exit actions execute first (LIFO);
2. implicit ARC releases for strong locals in that scope execute after all scope-exit actions of that scope.

Relative order among implicit ARC releases is baseline/implementation-defined unless specified elsewhere, but all such releases are sequenced after scope-exit actions in the same scope.

## AM.5 Unified scope-exit algorithm <a id="am-5"></a>

When control leaves scope `S`, a conforming implementation shall behave as if by:

1. Pop and execute each registered scope-exit action in `S` (LIFO).
2. Perform implicit scope-final ARC releases for `S`.
3. Continue unwinding to the next enclosing scope if control is still exiting outward.

This algorithm applies uniformly for normal return, throw paths, postfix-propagation early exits, and cancellation unwind.

## AM.6 `await` interaction rules <a id="am-6"></a>

### AM.6.1 `await` and scope-exit actions <a id="am-6-1"></a>

Encountering `await` is not scope exit.

Therefore, suspension at `await` shall not by itself execute:

- `defer` actions;
- resource cleanup actions;
- scope-final implicit ARC releases.

Those actions execute only when their lexical scope actually exits.

### AM.6.2 `await` and ARC lifetime boundaries <a id="am-6-2"></a>

At a potentially suspending `await`:

- values needed after resumption shall be preserved in the async frame ([AM.3.3](#am-3-3));
- values dead before suspension may be released before suspension;
- precise-lifetime rules still prohibit early release when such annotations/constructs apply.

When a scope eventually exits, defer/resource cleanup ordering relative to ARC remains governed by [AM.4.4](#am-4-4).

### AM.6.3 `await` and autorelease pools <a id="am-6-3"></a>

On Objective-C runtimes with autorelease semantics:

- each task execution slice (resume -> next suspension or completion) shall execute inside an implicit autorelease pool;
- if `await` actually suspends, the current slice pool shall be drained before suspension;
- pool drain shall also occur at task completion (normal return, thrown error, or cancellation unwind).

If an `await` completes synchronously without suspension, draining at that point is permitted but not required.

Autorelease-pool draining is not scope exit and shall not trigger scope-exit actions by itself.

### AM.6.4 `await` with `try`/`throws` <a id="am-6-4"></a>

`try await e` is the canonical composed form for `async throws` calls.

If the awaited operation throws:

- the throw is observed at the `await` expression;
- propagation follows [Part 6](#part-6) rules; and
- all exited scopes run scope-exit actions and ARC releases per [AM.5](#am-5).

For `try? await e`:

- success yields the value;
- throw yields `nil`;
- cleanup ordering remains unchanged.

For `try! await e`:

- success yields the value;
- throw traps as specified in [Part 6](#part-6); no additional post-trap ordering guarantees are required.

### AM.6.5 `await` with postfix propagation `?` <a id="am-6-5"></a>

For forms equivalent to `(await e)?`:

1. evaluate `await e` first (including any suspension/resumption);
2. apply postfix propagation semantics to the resulting carrier value.

If propagation triggers early exit (`return nil`, `return Err(...)`, or `throw ...`), scope exit proceeds under [AM.5](#am-5).

Carrier restrictions from [Part 6](#part-6) remain in force:

- optional propagation is valid only in optional-returning functions;
- using optional propagation to implicitly map into `throws` or `Result` contexts is ill-formed in v1.

### AM.6.6 `await` with optional chaining and optional send <a id="am-6-6"></a>

For `await x?.p`:

- `x` is evaluated exactly once;
- if `x == nil`, result is `nil` and no member access occurs, therefore no suspension occurs on that path;
- if `x != nil`, normal member access proceeds; suspension is possible only if that access is potentially suspending.

For `await [receiver? sel:arg1 other:arg2]`:

- `receiver` is evaluated exactly once;
- if `receiver == nil`, arguments are not evaluated, no send occurs, and no suspension occurs on that path;
- if `receiver != nil`, arguments are evaluated using ordinary-send ordering, then the send occurs; suspension is possible only on this non-`nil` path.

Applying `await` to a path proven non-suspending is permitted and may be diagnosed as unnecessary in strict modes.

### AM.6.7 Composed `await`/`try`/propagation ordering <a id="am-6-7"></a>

For composed forms such as `return (try? await x?.f())?;` in an optional-returning `async` function, implementations shall preserve the following abstract order:

1. Evaluate `x` exactly once.
2. Apply optional chaining/send short-circuit rules ([AM.2.2](#am-2-2)):
   - if `x == nil`, yield `nil` for the chained/send expression, skip chained argument/member evaluation, and do not suspend on that path;
   - if `x != nil`, evaluate the non-`nil` path normally (including any potentially suspending operation).
3. Apply `await` to the selected non-`nil` path evaluation (if any), with possible suspension/resumption.
4. Apply `try`/`try?`/`try!` semantics ([AM.6.4](#am-6-4)).
5. Apply postfix propagation `?` to the resulting carrier value ([AM.6.5](#am-6-5)).
6. If propagation early-exits, run scope-exit and ARC ordering per [AM.5](#am-5).

Replacing `try?` with `try` preserves steps 1-3; if a throw occurs at step 4, propagation at step 5 is not reached and unwind follows [AM.5](#am-5).

## AM.7 Conformance test matrix (normative minimum) <a id="am-7"></a>

A conforming implementation shall provide tests equivalent in coverage to the following matrix.

### AM.7.1 Matrix <a id="am-7-1"></a>

| ID     | Combination under test                                                     | Required outcome                                                                                                                                                    |
| ------ | -------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| AM-T01 | `defer` + normal return                                                    | Defers execute once, in LIFO order, before scope-final ARC releases.                                                                                                |
| AM-T02 | `defer` + cleanup/resource local + return                                  | Defer/resource actions follow per-scope LIFO registration order; all run before scope-final ARC releases.                                                           |
| AM-T03 | `defer` + `throw`                                                          | Throw path runs scope-exit actions and ARC releases for all exited scopes (inner to outer).                                                                         |
| AM-T04 | `defer` + `try await` where awaited call throws                            | Error propagates from `await`; defer/cleanup actions still run exactly once during unwind.                                                                          |
| AM-T05 | `try? await`                                                               | Throwing awaited call yields `nil`; no thrown error escapes; cleanup ordering remains per AM.5.                                                                     |
| AM-T06 | `try! await` throwing path                                                 | Program traps on thrown error; implementation is not required to provide post-trap cleanup guarantees.                                                              |
| AM-T07 | `(await optionalProducer())?` in optional-returning function               | Operand is evaluated once; on `nil`, function early-returns `nil` and runs scope-exit actions once.                                                                 |
| AM-T08 | `(await optionalProducer())?` in `throws` function                         | Compile-time error: optional postfix propagation is not a valid carrier in `throws` context.                                                                        |
| AM-T09 | `await x?.p` with `x == nil` path                                          | `x` evaluated once; result `nil`; no member access and no suspension on nil path.                                                                                   |
| AM-T10 | `await [r? m:sideEffect()]` with `r == nil` path                           | `r` evaluated once; `sideEffect()` not evaluated; no send and no suspension on nil path.                                                                            |
| AM-T11 | `await [r? m:sideEffect()]` with `r != nil` path                           | `sideEffect()` evaluated in ordinary-send order; send occurs; suspension permitted only on this path.                                                               |
| AM-T12 | `defer` around await cancellation point                                    | If cancellation causes unwind/error at `await`, defer/cleanup actions execute exactly once before task/frame teardown completes.                                    |
| AM-T13 | Nested scopes with `defer` + postfix `?` early exit                        | Early exit unwinds innermost-to-outermost scopes; each scope uses LIFO action order.                                                                                |
| AM-T14 | `await` in scope without exit                                              | Suspend/resume alone does not run scope-exit actions or scope-final ARC releases.                                                                                   |
| AM-T15 | `defer` + `return (try? await x?.f())?;` with `x == nil` path              | `x` evaluated once; `f`/arguments not evaluated; no suspension on nil path; postfix propagation early-exits and runs cleanup ordering per AM.5.                     |
| AM-T16 | `defer` + `return (try? await x?.f())?;` with `x != nil`, `f` throws       | Non-`nil` path may suspend; throw is converted to `nil` by `try?`; postfix propagation early-exits; defer/cleanup actions run once before ARC scope-final releases. |
| AM-T17 | `defer` + `return (try await x?.f())?;` with `x != nil`, `f` throws        | Throw propagates from `try await`; postfix propagation is not applied on throwing path; exited scopes still run cleanup ordering per AM.5.                          |
| AM-T18 | `defer` + `return (try await x?.f())?;` with `x == nil` path               | Nil short-circuit occurs before suspension; no throw occurs on nil path; postfix propagation early-exits `nil` and executes scope cleanups once.                    |
| AM-T19 | `defer` + `return (try? await [r? m:sideEffect()])?;` with `r == nil` path | `r` evaluated once; `sideEffect()` not evaluated; no send and no suspension on nil path; early exit still executes defer/cleanup in AM.5 order.                     |

### AM.7.2 Static diagnostics required by matrix <a id="am-7-2"></a>

At minimum, matrix coverage shall include diagnostics for:

- potentially suspending operations used without `await`;
- invalid optional postfix propagation carrier context;
- optional chaining/send restrictions from [Part 3](#part-3) (including scalar/struct restriction).
<!-- END ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md -->

---

<!-- BEGIN FORMAL_GRAMMAR_AND_PRECEDENCE.md -->

# Appendix F - Formal Grammar and Precedence for ObjC 3.0 Additions <a id="f"></a>

This appendix integrates the grammar additions defined across [Part 2](#part-2), [Part 3](#part-3), [Part 5](#part-5), [Part 6](#part-6), [Part 7](#part-7), and [Part 8](#part-8) into a single EBNF reference.

Unlisted productions are inherited from baseline C/Objective-C grammar as defined in [Part 0](#part-0).

## F.1 Scope and Normative Status <a id="f-1"></a>

This appendix is normative for:

- parse shape for ObjC 3.0 syntax additions,
- operator precedence/associativity for the integrated expression grammar,
- syntactic disambiguation rules where ObjC 3.0 additions overlap existing C/Objective-C forms.

Static and dynamic semantics remain defined in the feature parts (especially [Part 3](#part-3), [Part 6](#part-6), [Part 7](#part-7), and [Part 12](#part-12)).

## F.2 Notation and Lexical Conventions <a id="f-2"></a>

- EBNF operators: `[ ... ]` optional, `{ ... }` repetition (zero or more), `|` alternative.
- Quoted terminals use exact token spelling.
- Nonterminals ending in `-base` are inherited unchanged from the baseline grammar.

Tokenization conventions used by this appendix:

- `?.` and `??` are distinct punctuators.
- `?.` takes precedence over `?` followed by `.` when contiguous.
- `??` takes precedence over two consecutive `?` tokens when contiguous.
- `try?` and `try!` are `try` forms in prefix-expression context.
- `PropagationFollowSet = { ')', ']', '}', ',', ';' }`.
- Whitespace breaks multi-character punctuators; for example, `a ? .b` is not `a?.b`.

```ebnf
reserved-keyword-objc3 =
      "defer" | "guard" | "match" | "case" | "let" | "var"
    | "throws" | "throw" | "try" | "do" | "catch"
    | "async" | "await" | "actor" ;

contextual-keyword-objc3 =
      "borrowed" | "move" | "weak" | "unowned" | "strong"
    | "is" | "where" ;

propagation-follow-token = ")" | "]" | "}" | "," | ";" ;
```

## F.3 Integrated EBNF Grammar (ObjC 3.0 Additions) <a id="f-3"></a>

### F.3.1 Names, Declarations, and Type-Surface Additions <a id="f-3-1"></a>

```ebnf
module-qualified-name = "@" module-path "." identifier ;
module-path = identifier { "." identifier } ;

optional-type-suffix = "?" | "!" ;

generic-parameter-clause = "<" generic-parameter { "," generic-parameter } ">" ;
generic-parameter = [ variance ] identifier [ generic-constraints ] ;
variance = "__covariant" | "__contravariant" ;
generic-constraints = ":" type-constraint { "," type-constraint } ;

function-effect-specifier =
      "async"
    | "throws"
    | "async" "throws"
    | "throws" "async" ;

throws-specifier = "throws" ;

actor-class-declaration =
    "actor" "class" identifier [ ":" objc-superclass-name ] objc-interface-body ;

keypath-literal = "@keypath" "(" keypath-root "," keypath-components ")" ;
keypath-root = type-name | "self" ;
keypath-components = identifier { "." identifier } ;

resource-local-annotation =
      "@cleanup" "(" identifier ")"
    | "@resource" "(" identifier "," "invalid" ":" constant-expression ")" ;

borrowed-type-qualifier = "borrowed" ;

objc3-attribute-specifier = "__attribute__" "(" "(" objc3-attribute ")" ")" ;
objc3-attribute =
      executor-affinity-attribute
    | task-recognition-attribute
    | resource-attribute
    | cleanup-attribute
    | returns-borrowed-attribute ;

executor-affinity-attribute =
    "objc_executor" "(" executor-affinity-argument ")" ;
executor-affinity-argument =
      "main"
    | "global"
    | "named" "(" string-literal ")" ;

task-recognition-attribute =
      "objc_task_spawn"
    | "objc_task_detached"
    | "objc_task_group" ;

resource-attribute =
    "objc_resource" "(" "close" "=" identifier "," "invalid" "=" constant-expression ")" ;
cleanup-attribute = "cleanup" "(" identifier ")" ;
returns-borrowed-attribute =
    "objc_returns_borrowed" "(" "owner_index" "=" integer-constant ")" ;
```

### F.3.2 Statement and Pattern Additions <a id="f-3-2"></a>

```ebnf
let-binding-list = ( "let" | "var" ) let-binding { "," let-binding } ;
let-binding = identifier "=" expression ;

if-let-statement =
    "if" let-binding-list compound-statement [ "else" compound-statement ] ;

guard-let-statement =
    "guard" let-binding-list "else" compound-statement ;

guard-condition = expression | let-binding ;
guard-condition-list = guard-condition { "," guard-condition } ;

guard-statement =
    "guard" guard-condition-list "else" compound-statement ;

defer-statement = "defer" compound-statement ;

match-statement =
    "match" "(" expression ")" "{" match-case { match-case } [ match-default ] "}" ;
match-case = "case" pattern ":" compound-statement ;
match-case-guarded-reserved = "case" pattern "where" expression ":" compound-statement ;
(* reserved for future guarded patterns; rejected in ObjC 3.0 v1 *)
match-default = "default" ":" compound-statement ;

match-expression-reserved =
    "match" "(" expression ")" "{" match-expression-case { match-expression-case } [ match-expression-default ] "}" ;
match-expression-case = "case" pattern "=>" expression ";" ;
match-expression-default = "default" "=>" expression ";" ;
(* reserved for ObjC >= 3.1; ObjC 3.0 v1 parsers shall reject with a targeted diagnostic *)

pattern =
      "_"
    | literal
    | binding-pattern
    | result-pattern
    | type-test-pattern ;

binding-pattern = ( "let" | "var" ) identifier ;
result-pattern =
      "." "Ok" "(" binding-pattern ")"
    | "." "Err" "(" binding-pattern ")" ;

type-test-pattern =
      "is" type-name
    | "is" type-name ( "let" | "var" ) identifier ;
(* enabled only when __OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 1 *)

throw-statement = "throw" expression ";" ;

do-statement = "do" compound-statement catch-clause { catch-clause } ;
catch-clause = "catch" [ catch-pattern ] compound-statement ;
catch-pattern = "(" type-name [ identifier ] ")" ;

with-lifetime-statement = "withLifetime" "(" expression ")" statement ;
keep-alive-statement = "keepAlive" "(" expression ")" ";" ;
```

### F.3.3 Expression Additions (Integrated) <a id="f-3-3"></a>

```ebnf
primary-expression =
      primary-expression-base
    | module-qualified-name
    | keypath-literal
    | message-expression
    | block-literal ;

block-literal =
    "^" [ capture-list ] [ block-parameter-clause ] compound-statement ;

capture-list = "[" capture-item { "," capture-item } "]" ;
capture-item = [ capture-modifier ] identifier ;
capture-modifier = "strong" | "weak" | "unowned" | "move" ;

message-expression =
    "[" receiver-expression [ optional-send-indicator ] message-selector "]" ;
optional-send-indicator = "?" ;

postfix-expression = primary-expression { postfix-operator } ;
postfix-operator =
      "[" expression "]"
    | "(" [ argument-expression-list ] ")"
    | "." identifier
    | "->" identifier
    | "?." identifier
    | "++"
    | "--" ;

postfix-propagation-expression =
    postfix-expression [ postfix-propagation-operator ] ;

postfix-propagation-operator = "?" ;
(* valid only when the next token is in PropagationFollowSet *)

await-expression = "await" cast-expression ;
try-expression =
      "try" cast-expression
    | "try?" cast-expression
    | "try!" cast-expression ;

unary-expression =
      postfix-propagation-expression
    | await-expression
    | try-expression
    | unary-operator cast-expression
    | "sizeof" unary-expression
    | "sizeof" "(" type-name ")" ;

cast-expression =
      "(" type-name ")" cast-expression
    | unary-expression ;

logical-or-expression = logical-or-expression-base ;

nil-coalescing-expression =
      logical-or-expression
    | logical-or-expression "??" nil-coalescing-expression ;

conditional-expression =
      nil-coalescing-expression
    | nil-coalescing-expression "?" expression ":" conditional-expression ;

assignment-expression =
      conditional-expression
    | unary-expression assignment-operator assignment-expression ;

expression = assignment-expression { "," assignment-expression } ;
```

### F.3.4 Canonical Attribute Integration Notes <a id="f-3-4"></a>

The productions above include canonical header spellings from [ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b), while preserving ergonomic sugar (`@cleanup`, `@resource`) used in [Part 8](#part-8).

Attribute placement constraints (declaration vs type-position legality) are inherited from baseline Objective-C attribute grammar and from part-specific rules.

## F.4 Operator Precedence and Associativity <a id="f-4"></a>

Precedence is listed from highest (binds tightest) to lowest.

| Level | Operators / forms                                                                    | Associativity           | Notes                                                                                             |
| ----- | ------------------------------------------------------------------------------------ | ----------------------- | ------------------------------------------------------------------------------------------------- |
| 1     | Postfix selectors and calls: `x.y`, `x->y`, `x?.y`, `x(...)`, `x[...]`, `x++`, `x--` | Left-to-right           | Includes ordinary member access `.`, pointer member access `->`, and optional member access `?.`. |
| 2     | Postfix propagation: `x?`                                                            | Postfix (single suffix) | Allowed only when next token is in `PropagationFollowSet`; see [F.5](#f-5).                       |
| 3     | Prefix and cast forms: `(T)x`, `await x`, `try x`, `try? x`, `try! x`, unary ops     | Right-to-left           | `await`/`try` forms compose with casts and unary operators.                                       |
| 4     | Multiplicative: `*`, `/`, `%`                                                        | Left-to-right           | Baseline C tier.                                                                                  |
| 5     | Additive: `+`, `-`                                                                   | Left-to-right           | Baseline C tier.                                                                                  |
| 6     | Shift: `<<`, `>>`                                                                    | Left-to-right           | Baseline C tier.                                                                                  |
| 7     | Relational: `<`, `<=`, `>`, `>=`                                                     | Left-to-right           | Baseline C tier.                                                                                  |
| 8     | Equality: `==`, `!=`                                                                 | Left-to-right           | Baseline C tier.                                                                                  |
| 9     | Bitwise AND: `&`                                                                     | Left-to-right           | Baseline C tier.                                                                                  |
| 10    | Bitwise XOR: `^`                                                                     | Left-to-right           | Baseline C tier.                                                                                  |
| 11    | Bitwise OR: `\|`                                                                     | Left-to-right           | Baseline C tier.                                                                                  |
| 12    | Logical AND: `&&`                                                                    | Left-to-right           | Baseline C tier.                                                                                  |
| 13    | Logical OR: `\|\|`                                                                   | Left-to-right           | Baseline C tier.                                                                                  |
| 14    | Nil-coalescing: `??`                                                                 | Right-to-left           | As defined in [Part 3](#part-3).                                                                  |
| 15    | Conditional: `?:`                                                                    | Right-to-left           | Ternary conditional.                                                                              |
| 16    | Assignment: `=`, `+=`, `-=`, ...                                                     | Right-to-left           | Baseline C tier.                                                                                  |
| 17    | Comma: `,`                                                                           | Left-to-right           | Baseline C tier.                                                                                  |

### F.4.1 Required Binding Examples <a id="f-4-1"></a>

| Source form      | Required grouping  | Rationale                                                                     |
| ---------------- | ------------------ | ----------------------------------------------------------------------------- |
| `a?.b.c`         | `(a?.b).c`         | `?.` is a postfix selector at the same precedence tier as `.`.                |
| `a->b?.c`        | `(a->b)?.c`        | `->` and `?.` are both postfix selectors with left-to-right grouping.         |
| `a?.b ? c : d`   | `(a?.b) ? c : d`   | Postfix selectors bind tighter than ternary `?:`.                             |
| `a ?? b ? c : d` | `(a ?? b) ? c : d` | `??` binds tighter than `?:`.                                                 |
| `a ? b ?? c : d` | `a ? (b ?? c) : d` | `??` in ternary arms still binds tighter than `?:`.                           |
| `(T)a?.b`        | `(T)(a?.b)`        | Cast consumes a `cast-expression`; postfix selectors are inside that operand. |
| `((T)a)?`        | `((T)a)?`          | Propagation on a cast result requires parenthesized expression result.        |
| `(T)a?`          | `(T)a ? ... : ...` | Disambiguates as ternary introducer, not propagation; see [F.5.2](#f-5-2).    |

## F.5 Disambiguation Rules (Normative) <a id="f-5"></a>

### F.5.1 Tokenization priority <a id="f-5-1"></a>

A conforming lexer shall use maximal munch for overlapping punctuators.

- `?.` is tokenized as one punctuator when contiguous.
- `??` is tokenized as one punctuator when contiguous.
- `a ? .b` tokenizes as `?` then `.`, not as `?.`.
- `a ? ? b` tokenizes as two `?` tokens, not as `??`.
- In prefix-expression context, `try?` and `try!` are parsed as `try` forms, not as `try` followed by ternary or logical-not parsing.

### F.5.2 Cast vs parenthesized expression <a id="f-5-2"></a>

Cast disambiguation follows baseline C/Objective-C rules:

- If `(` ... `)` forms a valid `type-name` in the current scope, parse as cast.
- Otherwise parse as parenthesized expression.

Because postfix propagation applies to a completed `postfix-expression` node, not through an enclosing cast production, propagation on a cast result shall parenthesize the cast as an expression first:

- valid: `((T)x)?`
- not propagation: `(T)x?` (parsed as ternary `?` start, then diagnosed if incomplete)

### F.5.3 Postfix propagation `?` vs ternary `?:` <a id="f-5-3"></a>

When `?` follows a `postfix-expression`, parser behavior is:

1. If lookahead token after `?` is in `PropagationFollowSet = { ')', ']', '}', ',', ';' }`, parse postfix propagation.
2. Otherwise, parse `?` as ternary conditional introducer.

Consequences:

- `foo()?;` is propagation.
- `foo()? + 1` is not propagation and shall be diagnosed with the follow-token rule.
- `a ? b : c` remains ternary.

### F.5.4 Interaction with `??` <a id="f-5-4"></a>

Because `??` is a distinct punctuator, `a??b` is parsed as `a ?? b`, not as `a? ?b`.

If parsing later fails in a way consistent with likely ternary intent (for example `a??b:c`), the implementation shall provide a targeted diagnostic that explains `??` tokenization and offer fix-it `a ? b : c`.

### F.5.5 `await` and `try` composition <a id="f-5-5"></a>

Parsers shall accept both orders in prefix chains:

- `try await f()`
- `await try f()`

Canonical order is `try await ...` per [Part 7](#part-7). Implementations should warn on non-canonical order and offer a mechanical reorder fix-it when safe.

### F.5.6 Optional send indicator vs conditional expressions <a id="f-5-6"></a>

Inside message expressions, `[` receiver `?` selector `]` uses `?` as `optional-send-indicator`.

If a conditional expression is intended for the receiver, it shall be parenthesized, for example:

- `[(cond ? a : b) sel]`

### F.5.7 Required diagnostics and fix-its for ambiguous forms <a id="f-5-7"></a>

For the ambiguous/token-overlap forms below, diagnostics and fix-its are required:

| Pattern                                                                   | Required diagnostic                                                                                                                 | Required fix-it                                                                                                      |
| ------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------- |
| `expr? tok` where `tok` is not in `PropagationFollowSet`                  | Explain that postfix propagation `?` is disallowed by follow-token restriction and that parse falls back to ternary interpretation. | Parenthesize propagation subexpression when that rewrite is mechanical (for example `foo()? + 1` -> `(foo()?) + 1`). |
| `(T)x?` with missing `:`/third operand                                    | Explain cast-vs-propagation disambiguation and that `?` starts ternary in this form.                                                | `((T)x)?` when propagation intent is inferred from local context.                                                    |
| `a??b:c`                                                                  | Explain maximal-munch tokenization to `a ?? b : c`.                                                                                 | `a ? b : c`.                                                                                                         |
| `await try f()`                                                           | Explain non-canonical effect ordering, while parse remains valid.                                                                   | Reorder to `try await f()` when no comments/macros would be reordered unsafely.                                      |
| `[cond ? a : b ? sel]`                                                    | Explain optional-send/conditional-receiver ambiguity and required receiver parenthesization.                                        | `[(cond ? a : b) ? sel]`.                                                                                            |
| `match (...) { case pat => expr; ... }` in ObjC 3.0 v1 mode               | Explain that `=>` arms are reserved for future `match`-expression syntax and are not enabled in v1.                                 | Rewrite to statement form `case pat: { ... }` or rewrite as `if`/`switch`.                                           |
| `x = match (...) { case pat: { ... } ... };` in ObjC 3.0 v1 mode          | Explain that `match` is statement-only in v1 and cannot appear in expression position.                                              | Hoist into a statement `match` that assigns to a temporary, or rewrite as `if`/`switch`.                             |
| `case is Type ...` when `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0` | Explain that type-test patterns are optional and disabled in the current mode.                                                      | Guard with `#if __OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__` or rewrite to `default` + explicit `if`/cast chain.      |
| `case pattern where condition: ...` in ObjC 3.0 v1 mode                   | Explain that guarded patterns are deferred and this grammar slot is reserved for a future revision.                                 | Drop the guard and perform the condition inside the case body, or rewrite as nested `if`.                            |

### F.5.8 `match` Statement vs Reserved Expression Form <a id="f-5-8"></a>

In ObjC 3.0 v1:

- `match-statement` is accepted with `case ... : compound-statement`,
- `match-expression-reserved` is never accepted (even if parsed as a candidate),
- encountering `=>` after `case pattern` inside `match` shall produce a targeted “reserved for future expression form” diagnostic.

Parsers shall not silently reinterpret `case ... => ...` as statement syntax.

### F.5.9 Type-Test Pattern Feature Gate <a id="f-5-9"></a>

The token `is` is a contextual keyword in pattern position only.

- When `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 1`, `type-test-pattern` is enabled.
- When `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0`, `case is ...` shall be rejected with a targeted diagnostic.
- Outside pattern positions, `is` remains available as an identifier for source-compatibility.

If an imported metadata payload declares required capability `objc3.pattern.type_test.v1` and the importer does not support that capability, import shall fail as a hard error per [D.2.3](#d-2-3).

### F.5.10 Guarded Pattern Reservation (`where`) <a id="f-5-10"></a>

In ObjC 3.0 v1, guarded patterns are not part of accepted `match` syntax.

- `match-case` remains `case pattern : compound-statement`.
- `match-case-guarded-reserved` is a reserved future slot and shall be rejected in v1.
- `where` is contextual: it is only treated specially after a complete `case pattern` candidate.

Parsers shall emit a targeted deferred-feature diagnostic for `case pattern where condition:`.

## F.6 Parser Conformance Test Matrix (Ambiguity and Edge Cases) <a id="f-6"></a>

Each row is required for parser conformance. Diagnostics and fix-its are required when a transformation is mechanical, consistent with [Part 12](#part-12).

| ID   | Snippet                                                                                                                             | Expected parse/result                                                                                              | Required diagnostic / fix-it                                                                                                                           |
| ---- | ----------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| P-01 | `a?.b`                                                                                                                              | Parse as optional member access.                                                                                   | If `b` is scalar/struct return in v1: error (Part 3 restriction), suggest binding + ordinary access.                                                   |
| P-02 | `a?b:c`                                                                                                                             | Parse as ternary conditional.                                                                                      | No parse diagnostic.                                                                                                                                   |
| P-03 | `a ?? b ?? c`                                                                                                                       | Parse as `a ?? (b ?? c)` (right-associative).                                                                      | No parse diagnostic.                                                                                                                                   |
| P-04 | `a ?? b ? c : d`                                                                                                                    | Parse as `(a ?? b) ? c : d`.                                                                                       | No parse diagnostic.                                                                                                                                   |
| P-05 | `a ? b ?? c : d`                                                                                                                    | Parse as `a ? (b ?? c) : d`.                                                                                       | No parse diagnostic.                                                                                                                                   |
| P-06 | `foo()?;`                                                                                                                           | Parse as postfix propagation.                                                                                      | If carrier/type rules fail: error per Part 6 with carrier-preserving explanation.                                                                      |
| P-07 | `foo()? + 1;`                                                                                                                       | Not propagation (fails follow-token rule).                                                                         | Error: postfix `?` follow-token restriction; fix-it: `(foo()?) + 1`.                                                                                   |
| P-08 | `((T)x)?;`                                                                                                                          | Parse as propagation of parenthesized cast result.                                                                 | No parse diagnostic.                                                                                                                                   |
| P-09 | `(T)x?;`                                                                                                                            | Parse `?` as ternary start, then incomplete conditional.                                                           | Targeted diagnostic for cast/propagation ambiguity; fix-it: `((T)x)?` when propagation intent is inferred.                                             |
| P-10 | `try await f();`                                                                                                                    | Parse and type-check as canonical combined effects.                                                                | No ordering diagnostic.                                                                                                                                |
| P-11 | `await try f();`                                                                                                                    | Parse successfully.                                                                                                | Warning: non-canonical `await try` order; fix-it: `try await f();`.                                                                                    |
| P-12 | `try? await f();`                                                                                                                   | Parse as optionalizing `try` over awaited call.                                                                    | No parse diagnostic; apply Part 6 semantics.                                                                                                           |
| P-13 | `fThrows();`                                                                                                                        | Parse call expression.                                                                                             | Error: calling `throws` declaration without `try`; fix-it: insert `try`.                                                                               |
| P-14 | `await fAsync();` in non-`async` function                                                                                           | Parse prefix expression.                                                                                           | Error: `await` outside async context; suggest adding `async` to enclosing declaration or restructuring call site.                                      |
| P-15 | `opt?;` in non-optional-returning function                                                                                          | Parse propagation form.                                                                                            | Error: optional propagation requires optional-returning function; fix-it: `guard let` or explicit conditional return path.                             |
| P-16 | `opt?;` in `throws` or `Result` context expecting nil->error mapping                                                                | Parse propagation form.                                                                                            | Error: carrier-preserving rule forbids implicit nil->error conversion; fix-it: explicit `guard let ... else { throw ... }` or explicit adapter helper. |
| P-17 | `[obj? scalarValue]` where method returns scalar/struct                                                                             | Parse optional send.                                                                                               | Error (v1 restriction); fix-it: bind/unwrap receiver and use ordinary send in proven-nonnull path.                                                     |
| P-18 | `obj?.count` where member type is scalar/struct                                                                                     | Parse optional member access.                                                                                      | Error (v1 restriction); fix-it: bind/unwrap then access `count`.                                                                                       |
| P-19 | `a??b:c`                                                                                                                            | Tokenize as `a ?? b : c`; parse fails at `:` in this context.                                                      | Diagnostic should explain `??` tokenization; fix-it for likely ternary intent: `a ? b : c`.                                                            |
| P-20 | `f(x?, y)`                                                                                                                          | Parse `x?` as propagation (`,` is follow token).                                                                   | If carrier mismatch: error per Part 6; otherwise no parse diagnostic.                                                                                  |
| P-21 | `a?.b.c`                                                                                                                            | Parse as `(a?.b).c`.                                                                                               | No parse diagnostic.                                                                                                                                   |
| P-22 | `a->b?.c`                                                                                                                           | Parse as `(a->b)?.c`.                                                                                              | If typing later fails for selected members, issue semantic diagnostic only.                                                                            |
| P-23 | `a?.b?;`                                                                                                                            | Parse as propagation applied to `a?.b` (`;` is follow token).                                                      | If carrier mismatch: error per Part 6; otherwise no parse diagnostic.                                                                                  |
| P-24 | `a?.b? + c;`                                                                                                                        | Not propagation (fails follow-token rule at `+`); ternary path then fails if incomplete.                           | Error: follow-token restriction; fix-it: `(a?.b?) + c`.                                                                                                |
| P-25 | `(T)a?.b`                                                                                                                           | Parse as cast over optional member expression: `(T)(a?.b)`.                                                        | No parse diagnostic.                                                                                                                                   |
| P-26 | `a?.b ?? c ? d : e`                                                                                                                 | Parse as `((a?.b) ?? c) ? d : e`.                                                                                  | No parse diagnostic.                                                                                                                                   |
| P-27 | `try! await f() ?? g`                                                                                                               | Parse as `(try! (await f())) ?? g`.                                                                                | No parse diagnostic.                                                                                                                                   |
| P-28 | `a ? .b : c`                                                                                                                        | Tokenization is `?` then `.` (not `?.`); parse fails near `.`.                                                     | Diagnostic: optional-member punctuator requires contiguous `?.`; fix-it: `a?.b` when context supports optional member access intent.                   |
| P-29 | `a ? ? b : c`                                                                                                                       | Tokenization is two `?` tokens (not `??`); parse fails.                                                            | Diagnostic: `??` must be contiguous; fix-it: remove intervening whitespace.                                                                            |
| P-30 | `[cond ? a : b ? sel]`                                                                                                              | Parse as optional send with conditional receiver candidate; receiver parenthesization required by [F.5.6](#f-5-6). | Fix-it: `[(cond ? a : b) ? sel]`.                                                                                                                      |
| P-31 | `match (r) { case .Ok(let v): { use(v); } default: { useDefault(); } }`                                                             | Parse as `match-statement` (statement position).                                                                   | No parse diagnostic.                                                                                                                                   |
| P-32 | `x = match (r) { case .Ok(let v): { use(v); } default: { useDefault(); } };`                                                        | Reject in ObjC 3.0 v1: `match` in expression position.                                                             | Error: `match` is statement-only in v1; suggest statement rewrite with temporary assignment.                                                           |
| P-33 | `x = match (r) { case .Ok(let v) => v; default => 0; };`                                                                            | Recognize reserved expression-form candidate and reject in ObjC 3.0 v1.                                            | Error: `=>` arm syntax reserved for future `match` expression form; suggest statement-form rewrite.                                                    |
| P-34 | `match (obj) { case is MyType let t: { use(t); } default: { fallback(); } }` with `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 1` | Parse with `type-test-pattern` + binding.                                                                          | No parse diagnostic.                                                                                                                                   |
| P-35 | `match (obj) { case is MyType let t: { use(t); } default: { fallback(); } }` with `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0` | Reject in parser/semantic gate check.                                                                              | Error: type-test patterns disabled; suggest feature guard or explicit `if`/cast chain.                                                                 |
| P-36 | `int is = 1;`                                                                                                                       | Parse `is` as ordinary identifier outside pattern position.                                                        | No parse diagnostic.                                                                                                                                   |
| P-37 | `match (x) { case let v where v > 0: { use(v); } default: { fallback(); } }`                                                        | Reject in ObjC 3.0 v1 (reserved guarded pattern).                                                                  | Error: guarded patterns are deferred in v1; suggest moving condition into case body.                                                                   |
| P-38 | `match (x) { case .Ok(let v) where v > 0: { use(v); } default: { fallback(); } }`                                                   | Reject in ObjC 3.0 v1 (reserved guarded pattern), including after composite pattern.                               | Error: reserved `where` guard slot not enabled in v1; suggest nested `if` rewrite.                                                                     |
| P-39 | `match (x) { case let where: { use(where); } default: { fallback(); } }`                                                            | Parse as binding pattern where identifier is `where`.                                                              | No parse diagnostic (contextual keyword does not reserve identifier slot).                                                                             |

### F.6.1 Minimum diagnostic quality for matrix failures <a id="f-6-1"></a>

For rows requiring rejection, a conforming implementation shall:

- identify the `?`/operator token that triggered ambiguity,
- state which rule failed (follow-token, carrier-preserving, tokenization, async context, optional-chain restriction, v1 `match` form restriction, feature-gate restriction, or guarded-pattern reservation),
- provide at least one actionable fix-it when the rewrite is mechanical.
<!-- END FORMAL_GRAMMAR_AND_PRECEDENCE.md -->

---

<!-- BEGIN PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md -->

# Part 0 — Baseline and Normative References <a id="part-0"></a>

_Working draft v0.11 — last updated 2026-02-27_

## 0.1 Purpose <a id="part-0-1"></a>

This part establishes:

1. The baseline language and de-facto dialect that Objective-C 3.0 builds upon.
2. The stable external references that are normative for conformance.
3. Priority rules when this draft and an external reference conflict.
4. Shared normative terminology used across all parts.

Objective-C 3.0 is a language mode layered on top of a stable baseline. That baseline is pinned by the normative reference identifiers in [Section 0.2.1](#part-0-2-1).
Cross-part composition rules for ordering/lifetime/cleanup/suspension are defined in [Abstract Machine and Semantic Core](#am).

## 0.2 Normative reference model <a id="part-0-2"></a>

### 0.2.1 Stable normative reference index <a id="part-0-2-1"></a>

The table below is the complete normative reference index for this draft.
No external source is normative unless it appears here with an `NR-*` identifier.
For each identifier, a conforming toolchain release shall pin an immutable tuple: `(version or edition, date or release stamp, commit or artifact hash)`.

| ID                  | Canonical reference                                                         | Version / edition      | Date / release stamp               | Commit / artifact hash                                       | Exact normative scope in this draft                                                                                                             |
| ------------------- | --------------------------------------------------------------------------- | ---------------------- | ---------------------------------- | ------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| **NR-C18**          | ISO/IEC 9899:2018 (C18)                                                     | ISO/IEC 9899:2018      | 2018 edition                       | N/A (published standard text)                                | Core C lexical grammar, preprocessing model, declarators, expression semantics, and inherited C UB categories unless explicitly overridden.     |
| **NR-CPP20**        | ISO/IEC 14882:2020 (C++20)                                                  | ISO/IEC 14882:2020     | 2020 edition                       | N/A (published standard text)                                | C++ core-language rules that apply in mixed Objective-C++ translation units where this draft allows C++ interop.                                |
| **NR-LLVM-OBJC**    | LLVM/Clang Objective-C language + ARC behavior                              | `llvmorg-18.1.8`       | Release stamp for `llvmorg-18.1.8` | Git commit resolved by tag `llvmorg-18.1.8`                  | Objective-C front-end baseline for ARC qualifier semantics, retain/release insertion, method-family inference, and legacy ObjC diagnostics.     |
| **NR-BLOCKS-ABI**   | Clang Blocks specification and block ABI model                              | `llvmorg-18.1.8`       | Release stamp for `llvmorg-18.1.8` | Git commit resolved by tag `llvmorg-18.1.8`                  | Block object representation, capture semantics, calling convention, and copy/dispose helper contracts used by Parts 4/7/8.                      |
| **NR-OBJC-RUNTIME** | Objective-C runtime profile bundle for selected platform family             | `objc3-runtime-2025Q4` | Profile stamp `2025Q4`             | Artifact digest(s) listed in `objc3-runtime-2025Q4` manifest | Runtime class/metaclass model, selector identity, dispatch and category attachment semantics, and runtime constraints referenced by Parts 9/11. |
| **NR-ABI-PLATFORM** | Platform C/ObjC ABI profile bundle selected by the toolchain conformance ID | `objc3-abi-2025Q4`     | Profile stamp `2025Q4`             | Artifact digest(s) listed in `objc3-abi-2025Q4` manifest     | Calling conventions, data layout assumptions, symbol/linkage contracts, and ABI boundaries where this draft does not define a replacement ABI.  |

Conforming implementations shall publish concrete URLs and resolved hashes for `NR-LLVM-OBJC`, `NR-BLOCKS-ABI`, `NR-OBJC-RUNTIME`, and `NR-ABI-PLATFORM` in release notes or conformance reports.
For published language standards (`NR-C18`, `NR-CPP20`), the cited edition text is the immutable artifact.

### 0.2.2 Reference precedence and conflict rules <a id="part-0-2-2"></a>

If sources disagree, conformance shall be determined by this priority order:

1. Explicit normative rules in this draft (Parts 0-12, plus B/C/D/E).
2. Normative references listed in [Section 0.2.1](#part-0-2-1), interpreted by their pinned tuples.
3. Implementation-defined behavior explicitly permitted by this draft.

Conflict/override rules:

- If an ObjC 3.0 rule and a normative reference conflict, the ObjC 3.0 rule overrides the reference for conformance.
- If this draft forbids a construct that an external reference would otherwise allow, the construct is ill-formed; diagnostic required.
- If this draft is silent and two external references conflict, the implementation shall choose one behavior, document it as implementation-defined, and preserve that selection consistently within a toolchain release line.
- A conforming implementation shall emit a diagnostic when the selected behavior can cause cross-module incompatibility under strict profiles.
- Informative text never overrides normative text.

### 0.2.3 Policy for de-facto behavior without a stable standard <a id="part-0-2-3"></a>

Some Objective-C behaviors are de-facto rather than formally standardized. For those behaviors:

- The implementation shall bind behavior to explicit snapshot profiles (`NR-OBJC-RUNTIME`, `NR-ABI-PLATFORM`) instead of an unversioned "current compiler/runtime" description.
- Each snapshot profile shall include: toolchain version(s), runtime family/version(s), target ABI set, and any deviations from this draft.
- If a behavior is neither specified by this draft nor pinned by an `NR-*` snapshot, that behavior is implementation-defined and shall be documented before claiming conformance.
- Upgrading snapshot profiles is a versioned compatibility event and shall follow [Part 1](#part-1) compatibility rules.

### 0.2.4 Cross-reference rule for Parts 3-12 <a id="part-0-2-4"></a>

Parts 3-12 shall cite baseline dependencies by stable identifiers from [Section 0.2.1](#part-0-2-1) (for example, `NR-LLVM-OBJC`) rather than by vendor/tool names alone.

### 0.2.5 Citation granularity for external rules <a id="part-0-2-5"></a>

When a requirement depends on external behavior, the containing section shall cite the `NR-*` identifier and, where available, the external clause/section name.

## 0.3 Normative language and conformance tags <a id="part-0-3"></a>

### 0.3.1 Requirement words <a id="part-0-3-1"></a>

The terms **shall**, **must**, **shall not**, **must not**, **should**, and **may** are interpreted with RFC-style normative force.

### 0.3.2 Section tags and conformance impact <a id="part-0-3-2"></a>

This draft uses the following labels consistently:

- **Normative**: required for conformance.
- **Minimum**: normative floor; stronger behavior is permitted if it does not violate this draft.
- **Normative intent**: normative summary of required externally observable behavior when low-level lowering details are left implementation-specific.
- **Recommended**: quality-of-implementation guidance, not required for conformance unless the section also says "shall/must".
- **Informative** / **informative summary**: explanatory only.
- If a heading omits a status label, the section shall be treated as normative only when it contains explicit requirement words (`shall`, `must`, `shall not`, `must not`); otherwise it is informative guidance.

When a heading label and sentence-level requirement words appear to conflict, sentence-level requirement words control.

## 0.4 Definitions and shared terminology <a id="part-0-4"></a>

### 0.4.1 Ill-formed program; diagnostic required <a id="part-0-4-1"></a>

A program is **ill-formed** if it violates a syntactic or static semantic rule in this draft.
A conforming implementation shall emit at least one diagnostic in the required phase from [Section 0.4.6](#part-0-4-6).

Preferred wording in this draft is: **"ill-formed; diagnostic required"**.
Parts 1-12 shall use this exact phrase when introducing static prohibitions, optionally followed by profile-specific severity requirements.

### 0.4.2 Conditionally-supported construct <a id="part-0-4-2"></a>

A construct is **conditionally-supported** if support is optional.
If an implementation does not support that construct and a program requires it, the program is ill-formed for that implementation; diagnostic required.
The implementation shall document support status and any profile gating.

### 0.4.3 Undefined behavior <a id="part-0-4-3"></a>

**Undefined behavior (UB)** means no semantic requirements are imposed on the implementation after the UB point is reached, except where a rule explicitly requires a diagnostic or runtime handling.

### 0.4.4 Unspecified behavior <a id="part-0-4-4"></a>

**Unspecified behavior** means two or more outcomes are permitted and the implementation is not required to document or consistently choose one outcome.

### 0.4.5 Implementation-defined behavior <a id="part-0-4-5"></a>

**Implementation-defined behavior** means one of multiple permitted outcomes is selected by the implementation and that selection shall be documented.

### 0.4.6 Phase mapping for core behavior terms <a id="part-0-4-6"></a>

Unless a specific part overrides this table, the following phase mapping applies:

| Term                                           | Compile/import phase                                                                                        | Link phase                                                                                                           | Runtime phase                                                                                                                     |
| ---------------------------------------------- | ----------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| **Ill-formed**                                 | At least one diagnostic shall be emitted when the violation is detectable before code generation completes. | If detection requires whole-program information, at least one link-time diagnostic shall be emitted where practical. | Runtime handling is required only when a part explicitly designates a runtime-detected violation (for example trap/throw/assert). |
| **Conditionally-supported** (unsupported case) | Diagnostic required once unsupported use is determinable during compile/import.                             | Diagnostic required if unsupported use is first determinable during linkage/composition.                             | If support checks are intentionally deferred, runtime shall report failure per the relevant part.                                 |
| **Undefined behavior**                         | Diagnostic not required (optional diagnostics are permitted).                                               | Diagnostic not required.                                                                                             | No requirements after UB is reached unless explicitly added by another rule.                                                      |
| **Unspecified behavior**                       | Diagnostic not required.                                                                                    | Diagnostic not required.                                                                                             | Any permitted outcome may occur at each instance; documentation is not required.                                                  |
| **Implementation-defined behavior**            | Diagnostic not required solely due to the existence of alternatives; selected behavior shall be documented. | Same as compile/import.                                                                                              | Executions shall follow the documented selection when the behavior is exercised.                                                  |

### 0.4.7 Translation unit <a id="part-0-4-7"></a>

A translation unit is a single source file after preprocessing and module import processing, compiled as one unit.

### 0.4.8 Objective-C 3.0 mode <a id="part-0-4-8"></a>

Objective-C 3.0 mode is the compilation mode in which ObjC 3.0 grammar, defaults, and diagnostics are enabled ([Part 1](#part-1)).

### 0.4.9 Conformance level <a id="part-0-4-9"></a>

A conformance level is a set of additional requirements and diagnostics applied on top of ObjC 3.0 mode ([Part 1](#part-1)).

### 0.4.10 Retainable pointer type <a id="part-0-4-10"></a>

A retainable pointer type is a type for which ARC may insert retain/release operations. This includes:

- Objective-C object pointers (`id`, `Class`, `NSObject *`, etc.),
- block pointers,
- additional retainable families declared by annotations ([Part 8](#part-8)).

### 0.4.11 Object pointer type <a id="part-0-4-11"></a>

An object pointer type is a pointer to an Objective-C object, including `id` and `Class`. In this draft, object pointer excludes CoreFoundation toll-free bridged pointers unless explicitly annotated as retainable families ([Part 8](#part-8)).

### 0.4.12 Block pointer type <a id="part-0-4-12"></a>

A block pointer type is a pointer to a block literal/closure as defined by NR-BLOCKS-ABI.

### 0.4.13 Nullability <a id="part-0-4-13"></a>

Nullability is a type property (for object and block pointer types) indicating whether `nil` is a valid value. Nullability kinds are defined in [Part 3](#part-3).

### 0.4.14 Optional type <a id="part-0-4-14"></a>

An optional type is written `T?` in ObjC 3.0 mode. Optional types are a type-system feature for object and block pointer types (v1), distinct from Objective-C's "message to nil returns zero" runtime behavior ([Part 3](#part-3)).

### 0.4.15 IUO type <a id="part-0-4-15"></a>

An implicitly unwrapped optional type is written `T!`. In v1, IUO primarily supports migration and is discouraged in strict modes ([Part 3](#part-3)).

### 0.4.16 Carrier type <a id="part-0-4-16"></a>

A carrier type represents either a success value or an alternative early-exit/absence value and participates in propagation (`?`). In v1, carriers include:

- optional types (`T?`), and
- `Result<T, E>` ([Part 6](#part-6)).

### 0.4.17 Effects <a id="part-0-4-17"></a>

Effects are part of a function's type and govern call-site obligations. In v1, effects include `throws` ([Part 6](#part-6)) and `async` ([Part 7](#part-7)).

### 0.4.18 Executor <a id="part-0-4-18"></a>

An executor is an abstract scheduler for task continuations ([Part 7](#part-7)). Executors may represent:

- the main/UI executor,
- a global background executor,
- or custom executors defined by the runtime/library.

### 0.4.19 Task <a id="part-0-4-19"></a>

A task is a unit of asynchronous work managed by the concurrency runtime/library. Tasks may be child tasks or detached tasks and may be cancellable ([Part 7](#part-7)).

### 0.4.20 Actor <a id="part-0-4-20"></a>

An actor is a concurrency-isolated reference type whose mutable state is protected by isolation rules ([Part 7](#part-7)). Cross-actor interactions are mediated by `await` and Sendable-like constraints.

## 0.5 Relationship to C and C++ <a id="part-0-5"></a>

Objective-C 3.0 remains a superset of C and compatible with Objective-C++ as implemented by the toolchain.

- Unless explicitly overridden by this draft, C/C++ undefined, unspecified, and implementation-defined behavior classes are inherited from `NR-C18` and `NR-CPP20`.
- ObjC 3.0 may add stricter static constraints; where it does, behavior that would otherwise be inherited is reclassified as ill-formed; diagnostic required.
- ObjC 3.0 does not weaken inherited C/C++ undefined behavior into defined behavior unless a rule explicitly defines replacement semantics.
- In mixed Objective-C++ translation units, C++ core-language UB remains UB unless a stricter ObjC 3.0 rule rejects the construct earlier.

## 0.6 Open issues <a id="part-0-6"></a>

No open issues are tracked in this part for v0.11.

Resolved references: runtime manifest publication
(`spec/conformance/objc3-runtime-2025Q4_manifest_validation.md`), ABI
manifest publication
(`spec/conformance/objc3_abi_manifest_validation_v0.11_A02.md`), and
abstract-machine synchronization protocol
(`spec/process/ABSTRACT_MACHINE_SYNC_PROTOCOL.md`).

<!-- END PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md -->

---

<!-- BEGIN PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md -->

# Part 1 — Versioning, Compatibility, and Conformance <a id="part-1"></a>

_Working draft v0.11 — last updated 2026-02-27_

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

### 1.2.2 Translation-unit granularity (v1 decision) <a id="part-1-2-2"></a>

For Objective‑C 3.0 v1, language-version selection shall be **translation-unit-only**.

Mixing ObjC 3.0 and non‑ObjC3 language versions within a single translation unit is not conforming for v1.

A conforming implementation:

- shall treat the effective language version as fixed for the entire translation unit,
- shall reject per-region language-version switching forms (for example `push`/`pop`-style pragmas),
- shall diagnose any attempt to change language version after parsing has started.

### 1.2.3 Source directive (optional) <a id="part-1-2-3"></a>

A conforming implementation may provide a source directive such as:

- `#pragma objc_language_version(3)`

If supported in v1, the directive:

- shall apply to the entire translation unit,
- shall appear only at file scope before the first non-preprocessor declaration/definition token,
- shall reject region-scoped variants (for example `#pragma objc_language_version(push, 3)` / `pop`),
- shall diagnose conflicting selections (for example command line selects `3` while source selects a different version value).

### 1.2.4 Required conformance tests for language-version selection (normative minimum) <a id="part-1-2-4"></a>

A conforming implementation shall include tests that cover both accepted and rejected forms, including at minimum:

- TUV-01: accepted command-line translation-unit selection (`-fobjc-version=3`).
- TUV-02: accepted file-scope pragma form `#pragma objc_language_version(3)` before declarations.
- TUV-03: rejected region switching forms (`push`/`pop`, begin/end, or equivalent).
- TUV-04: rejected in-function or post-declaration version-selection pragma.
- TUV-05: rejected conflicting command-line/source version selections.

Test expectations and diagnostic portability requirements are defined in [§12.5.8](#part-12-5-8).

## 1.3 Reserved keywords and conflict handling <a id="part-1-3"></a>

### 1.3.1 Reserved keywords <a id="part-1-3-1"></a>

In ObjC 3.0 mode, at minimum the following tokens are reserved as keywords:

- Control flow: `defer`, `guard`, `match`, `case`
- Effects/concurrency: `async`, `await`, `actor`
- Errors: `try`, `throw`, `do`, `catch`, `throws`
- Bindings: `let`, `var`

### 1.3.2 Contextual keywords (non-normative guidance) <a id="part-1-3-1-2"></a>

Some tokens are treated as **contextual keywords** only within specific grammar positions, to reduce breakage in existing codebases.

Examples include:

- `borrowed` (type qualifier; [Part 8](#part-8), also cataloged in [B.8.3](#b-8-3))
- `move`, `weak`, `unowned` (block capture lists; [Part 8](#part-8), also cataloged in [B.8.5](#b-8-5))

Conforming implementations should avoid reserving these tokens globally.

### 1.3.3 Raw identifiers <a id="part-1-3-2"></a>

A conforming implementation shall provide a compatibility escape hatch for identifiers that collide with reserved keywords.

Two acceptable designs:

- a raw identifier syntax (e.g., `@identifier(defer)`), or
- a backtick escape (e.g., `` `defer` ``) in ObjC 3.0 mode only.

The toolchain shall also provide fix-its to mechanically rewrite collisions.

### 1.3.4 Backward compatibility note <a id="part-1-3-3"></a>

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

### 1.4.3 Mode-selection macros (required) <a id="part-1-4-3"></a>

A conforming implementation shall provide predefined mode-selection macros in ObjC 3.0 mode:

- `__OBJC3_STRICTNESS_LEVEL__` with values:
  - `0` for `permissive`,
  - `1` for `strict`,
  - `2` for `strict-system`.
- `__OBJC3_CONCURRENCY_MODE__` with values:
  - `0` for `off`,
  - `1` for `strict`.
- `__OBJC3_CONCURRENCY_STRICT__` as a boolean alias (`1` when `__OBJC3_CONCURRENCY_MODE__ == 1`, else `0`).

### 1.4.4 `__has_feature` integration (optional) <a id="part-1-4-4"></a>

A conforming implementation may expose ObjC 3.0 features through a `__has_feature`-style mechanism. If present, it should align with the per-feature macros and mode-selection macros.

## 1.5 Conformance levels (strictness) <a id="part-1-5"></a>

### 1.5.1 Levels <a id="part-1-5-1"></a>

A translation unit in ObjC 3.0 mode may additionally declare a conformance level.

Objective‑C 3.0 v1 defines exactly three strictness levels:

- **Permissive**: language features enabled; safety checks default to warnings; legacy patterns allowed.
- **Strict**: key safety checks are errors; opt-outs require explicit unsafe spellings.
- **Strict-system**: strict + additional system-API safety checks (resource cleanup correctness, borrowed pointer escape analysis, etc.).

### 1.5.2 Selecting a level <a id="part-1-5-2"></a>

A conforming implementation shall provide an option equivalent to:

- `-fobjc3-strictness=permissive|strict|strict-system`

### 1.5.3 Diagnostic escalation rule <a id="part-1-5-3"></a>

If a construct is ill‑formed in strict mode, it may still be accepted in permissive mode with a warning _only_ if:

- the compiler can preserve baseline behavior, and
- the behavior is not undefined.

## 1.6 Orthogonal checking modes (submodes) <a id="part-1-6"></a>

### 1.6.1 Strict concurrency checking <a id="part-1-6-1"></a>

Concurrency checking is an orthogonal strictness sub-mode: a translation unit may enable additional checking beyond the selected strictness level.

For v1, strict concurrency checking is **not** a fourth strictness level.

A conforming implementation shall provide an option equivalent to:

- `-fobjc3-concurrency=strict|off`

Strict concurrency checking enables additional diagnostics defined in [Part 7](#part-7)/12 (Sendable, actor isolation misuse, executor affinity misuse, etc.).

### 1.6.2 Required strictness/sub-mode consistency rules (normative) <a id="part-1-6-2"></a>

A conforming implementation shall apply strictness and strict concurrency as two axes:

- `-fobjc3-strictness=*` selects baseline strictness semantics.
- `-fobjc3-concurrency=*` enables or disables additional concurrency diagnostics/constraints from [Part 7](#part-7) and [Part 12](#part-12).

Consistency requirements:

- Enabling `-fobjc3-concurrency=strict` shall not weaken any diagnostic required by the selected strictness level.
- Disabling `-fobjc3-concurrency` shall not disable non-concurrency diagnostics required by the selected strictness level.
- Profile claims shall map to strictness/sub-mode combinations defined in [E.2](#e-2).

Conformance tests for this matrix are defined in [§12.5.10](#part-12-5-10).

### 1.6.3 Strict performance checking (optional) <a id="part-1-6-3"></a>

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

This part defines the _hook point_ and selection mechanism; profile contents are specified in **[CONFORMANCE_PROFILE_CHECKLIST.md](#e)**.

When emitting a machine-readable conformance report, implementations shall report the selected profile set using the schema in [§12.4.5](#part-12-4-5).

<!-- END PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md -->

---

<!-- BEGIN PART_2_MODULES_NAMESPACING_API_SURFACES.md -->

# Part 2 — Modules, Namespacing, and API Surfaces <a id="part-2"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 2.1 Purpose <a id="part-2-1"></a>

This part defines:

- module import rules and their interaction with headers,
- a **module-qualified name** mechanism to reduce global-prefix collision pressure,
- API surface contracts: visibility, stability, and availability,
- requirements for interface extraction/emission so ObjC 3.0 semantics survive separate compilation.

This part is closely related to:

- **[B](#b)** (canonical spellings for emitted interfaces),
- **[C](#c)** (separate compilation and lowering contracts), and
- **[D](#d)** (the normative checklist, version compatibility matrix, and profile-based importer validation for module metadata).

## 2.2 Modules <a id="part-2-2"></a>

### 2.2.1 `@import` <a id="part-2-2-1"></a>

Objective‑C 3.0 standardizes `@import` as the preferred import mechanism.

A module import shall:

- make the imported module’s public declarations visible,
- apply the module’s exported nullability metadata ([Part 3](#part-3)),
- apply the module’s exported availability metadata (if any),
- make any exported “strictness recommendation” visible to tooling ([Part 1](#part-1)),
- validate metadata version/capability compatibility and required semantic fields per [D.2](#d-2), [D.3.4](#d-3-4), and [D.3.5](#d-3-5).

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

A _module-qualified name_ is written:

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
Version-compatibility and profile-specific validation behavior for that contract is defined by [D.3.4](#d-3-4) and [D.3.5](#d-3-5).

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
- cross-module concurrency checking ([Part 7](#part-7)),
- and metadata version/capability checks with ignorable-field forward compatibility ([D.2](#d-2), [D.3.4](#d-3-4)).

### 2.5.3 Metadata versioning and compatibility on import (normative) <a id="part-2-5-3"></a>

When importing module metadata or emitted interfaces that carry serialized metadata, an importer shall:

- read `schema_major`, `schema_minor`, and `required_capabilities` (or an equivalent representation),
- classify imported fields as required semantic vs ignorable extension fields per [D.2.1](#d-2-1),
- apply the compatibility outcomes in [D.3.4](#d-3-4),
- reject unknown required fields/capabilities as hard errors,
- diagnose missing/invalid known-required semantic metadata using profile rules in [D.3.5](#d-3-5),
- permit unknown ignorable extension fields without semantic changes to required behavior.

### 2.5.4 Round-trip preservation requirement (normative) <a id="part-2-5-4"></a>

Toolchains claiming conformance shall provide tests and/or verification mode for:

- `emit interface -> import emitted interface -> compare with direct module import`,
- declaration-level preservation of [D.3.1](#d-3-1) [Table A](#d-3-1) effect/attribute tuples (`throws`, `async`, error-bridging markers, isolation/executor metadata, and dispatch markers),
- profile-gated metadata preservation for Strict Concurrency and Strict System claims (Sendable/task-spawn and borrowed/lifetime metadata),
- behavioral equivalence checks that preserve `try`/`await` obligations and profile-gated diagnostics after round-trip import.

Minimum round-trip and compatibility test obligations are specified in [D.4.1](#d-4-1), [D.4.2](#d-4-2), and [D.4.3](#d-4-3).

### 2.5.5 Portable concurrency metadata interface format (normative) <a id="part-2-5-5"></a>

For cross-toolchain interchange, a conforming implementation shall support a portable interface representation for concurrency metadata named:

- **OCI-1** (Objective-C Interface Concurrency Metadata, schema v1)

OCI-1 may be embedded in a textual interface or emitted as a sidecar payload, but it shall include:

- declaration identity key (stable symbol/declaration identifier),
- effect flags (`async`, `throws`),
- executor metadata (`objc_executor(...)` equivalent),
- actor isolation metadata (actor-bound vs nonisolated),
- Sendable-related boundary metadata required for strict-concurrency checks.

OCI-1 schema versioning shall follow the compatibility model in [D.2](#d-2) and [D.3.4](#d-3-4).
If OCI-1 data is absent for declarations that require concurrency metadata under the claimed profile, import shall diagnose per [D.3.5](#d-3-5).
Missing OCI-1 fields shall be diagnosed by category against [D.3.5](#d-3-5): `effects.*` as effect metadata, `isolation.*` as isolation metadata, and `sendable.*` as sendability/task-boundary metadata.

Toolchains may provide additional proprietary metadata formats, but they do not replace OCI-1 for portability claims.

## 2.6 Required diagnostics (minimum) <a id="part-2-6"></a>

- Using a module-qualified name for an unloaded/unimported module: error with fix-it to add `@import`.
- Resolving a module-qualified name to multiple candidates (ambiguous exports): error; recommend qualification or import narrowing.
- Redeclaring an imported API with mismatched effects/ABI-significant attributes: error (see [C.2](#c-2)).
- Importing metadata with incompatible `schema_major`, or with unknown required fields/capabilities: hard error ([D.3.4](#d-3-4)).
- Importing metadata with missing/unknown/mismatched semantic fields: diagnostics shall follow profile rules in [D.3.5](#d-3-5); required semantic metadata shall not be silently dropped.
- Missing or mismatched error-effect metadata (`throws`, `objc_nserror`, `objc_status_code(...)`) on imported declarations: hard error.
- Mismatch between emitted interface metadata and compiled module metadata for [Table A](#d-3-1) items: diagnosed per [D.3.5](#d-3-5) (ABI-significant mismatches are always errors).
- Claiming portable concurrency metadata support while omitting required OCI-1 fields for imported declarations: hard error in strict-concurrency/strict-system profiles; recoverable diagnostic in Core/Strict when allowed by [D.3.5](#d-3-5).
<!-- END PART_2_MODULES_NAMESPACING_API_SURFACES.md -->

---

<!-- BEGIN PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md -->

# Part 3 — Types: Nullability, Optionals, Pragmatic Generics, and Typed Key Paths <a id="part-3"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 3.0 Overview <a id="part-3-0"></a>

### v0.8 resolved decisions <a id="part-3-v0-8-resolved-decisions"></a>

- Optional chaining and optional message sends are **reference-only** in v1 (object/block/void only). Scalar/struct optional chaining is ill-formed.
- Optional message sends are **conditional calls**: argument expressions are evaluated only if the receiver is non-`nil`.
- Generic methods/functions are **deferred** in v1; generic _types_ remain supported.
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
- `optional`, `some`, `none` (reserved for future value-optional syntax)

In ObjC 3.0 mode, the type-constructor spelling `Optional<...>` is reserved for a future value-optional ABI.
User declarations shall not occupy unescaped `Optional`/`optional`/`some`/`none` spellings; use the raw-identifier escape in [§1.3.3](#part-1-3-2) when source compatibility requires these names.

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
   - `T?` (nullable; includes `id?`, `id<...>?`, and `Class?`)
   - `T!` (implicitly unwrapped optional; includes `id!`, `id<...>!`, and `Class!`; see [§3.3.3](#part-3-3-3))

#### 3.2.3.1 Canonicalization rule <a id="part-3-2-3-1"></a>

For type-checking purposes, implementations shall canonicalize:

- `T?` → `T _Nullable`
- `T!` → `T _Nullable` **plus** “implicitly-unwrapped” marker
- `id?` → `id _Nullable`
- `id!` → `id _Nullable` **plus** “implicitly-unwrapped” marker
- `Class?` → `Class _Nullable`
- `Class!` → `Class _Nullable` **plus** “implicitly-unwrapped” marker

If a type already includes an explicit nullability qualifier, the sugar form shall be rejected as redundant.

#### 3.2.3.2 `id` / `Class` sugar scope <a id="part-3-2-3-2"></a>

`id` and `Class` are in-scope for optional sugar in v1.

- `id?`/`id!` and `Class?`/`Class!` are well-formed and follow [§3.2.3.1](#part-3-2-3-1) canonicalization.
- `id<Proto>?`/`id<Proto>!` are also well-formed.
- Header import/export and module metadata shall preserve the same effective nullability for these spellings as for equivalent qualifier forms.

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

#### 3.2.5.1 Completeness scope and effective nullability <a id="part-3-2-5-1"></a>

For completeness checking, implementations shall inspect each exported type position where nullability applies ([§3.2.2](#part-3-2-2)), including:

- function/method/block parameters and returns,
- exported property/ivar types,
- typedef/typealias expansions referenced by exported declarations,
- nested object/block pointer positions inside block signatures and generic arguments.

Each inspected position has an **effective nullability** determined in this order:

1. explicit qualifier spelling (`_Nonnull`, `_Nullable`, `_Null_unspecified`, equivalent contextual keywords, or `T?`/`T!`);
2. enclosing nonnull-by-default region ([§3.2.4](#part-3-2-4));
3. otherwise **unspecified**.

A position whose effective nullability is unspecified is **incomplete**.
An exported declaration with one or more incomplete positions is **nullability-incomplete**.
Declarations with no inspected positions are **not applicable** for completeness scoring.

#### 3.2.5.2 Public interface/module completeness <a id="part-3-2-5-2"></a>

A module interface is **nullability-complete** for an import view if all exported, applicable declarations in that view are complete.
A module interface is **nullability-incomplete** if any exported, applicable declaration in that view is nullability-incomplete.

Intentional use of `_Null_unspecified` for compatibility is permitted, but still counts as incompleteness for this classification.

#### 3.2.5.3 Mixed C/ObjC/ObjC++ import views <a id="part-3-2-5-3"></a>

In mixed-language modules, completeness shall be evaluated against the declarations visible in the importer's active language view:

- **C view**: this part's completeness rules are not applicable to declarations that do not use Objective-C object/block pointer types.
- **ObjC view**: includes Objective-C declarations and any C declarations visible to ObjC that use Objective-C object/block pointer types.
- **ObjC++ view**: includes the ObjC view plus ObjC++-visible declarations (including C++ entities) that use Objective-C object/block pointer types.

A mixed-language module may be nullability-complete in one view and nullability-incomplete in another; diagnostics shall be based on the active importer view.

#### 3.2.5.4 Profile-specific diagnostic severity <a id="part-3-2-5-4"></a>

For conformance-profile behavior ([Part 1](#part-1), [E](#e)):

- Exported declaration is nullability-incomplete in the active import view:
  - Core: recoverable diagnostic; build/import may continue with unspecified assumptions.
  - Strict: hard error.
  - Strict Concurrency: hard error.
  - Strict System: hard error.
- Imported module is nullability-incomplete in the active import view:
  - Core: recoverable diagnostic; import may continue with unspecified assumptions.
  - Strict: hard error.
  - Strict Concurrency: hard error.
  - Strict System: hard error.
- Missing/mismatched nullability/default metadata, or interface/metadata mismatch for nullability [D Table A](#d-3-1) items:
  - Core: follow [D.3.5](#d-3-5) (recoverable unless ABI-significant).
  - Strict: hard error.
  - Strict Concurrency: hard error.
  - Strict System: hard error.

#### 3.2.5.5 Local completeness <a id="part-3-2-5-5"></a>

Within a translation unit, incomplete nullability is permitted but should be diagnosed when it impacts type safety (e.g., assigning “unspecified” to “nonnull”).
When such incompleteness crosses a module/public boundary, diagnostics shall follow [§3.2.5.4](#part-3-2-5-4).

### 3.2.6 Completeness representation in metadata and emitted interfaces <a id="part-3-2-6"></a>

Nullability completeness across separate compilation shall be represented and reconstructed as follows:

- The semantic source of truth is the nullability-related [D Table A](#d-3-1) entries (qualifiers, nonnull-by-default regions, optional spellings).
- Module metadata and textual interfaces shall preserve those entries so importers can recompute effective nullability and completeness deterministically.
- Interface emission shall preserve canonical nonnull-region spellings ([B.2](#b-2), [B.7](#b-7)) and shall not rewrite nullability-incomplete declarations into complete ones.
- If source uses `id?`/`Class?` (or `!` variants), emitted interfaces may canonicalize to qualifier spellings, but importers shall observe equivalent effective nullability.
- Implementations may additionally encode derived completeness summaries (for example, per-declaration or per-view completeness flags) as ignorable extension metadata under [D.2](#d-2)/[D.3.4](#d-3-4); if such summaries are absent, importers shall recompute from preserved nullability metadata.

---

## 3.3 Optionals <a id="part-3-3"></a>

### 3.3.1 Optional type sugar (`T?`) <a id="part-3-3-1"></a>

A type written with the suffix `?` denotes a nullable object/block pointer type.

Optional sugar (`?` and `!`) is permitted only on types where nullability applies ([§3.2.2]), including `id`, `id<...>`, `Class`, Objective‑C object pointers, and block pointers.
Applying optional sugar to any other type is ill-formed.

Examples:

```objc
NSString*? maybeName;
id<NSCopying>? maybeKey;
id? anyObj;
Class? maybeMetaClass;
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

### 3.3.5 Future value-optional ABI reservation (v1 guardrails) <a id="part-3-3-5"></a>

Objective‑C 3.0 v1 does not define a first-class value optional ABI (`Optional<T>` layout/tagged payload semantics).

The following spellings are reserved for a future revision and are ill-formed in v1 user code unless escaped per [§1.3.3](#part-1-3-2):

- `optional<...>` in type positions.
- `Optional<...>` in type positions.
- `.some(...)` and `.none` in optional-constructor/pattern positions.

#### 3.3.5.1 Future-compat constraints (v1) <a id="part-3-3-5-1"></a>

To avoid blocking a future value optional design:

- `T?`/`T!` in v1 remain reference-nullability sugar only and shall not imply a value layout contract.
- v1 parser and interface emitters shall keep the reserved spellings above unavailable for unrelated language/library features.
- Module metadata and textual interfaces shall preserve optional/nullability semantics via extensible encoding so a future value-optional kind can be added without redefining existing v1 fields.
- Diagnostics for non-reference optional operations should be worded as “not supported in v1” rather than “never supported,” preserving forward migration paths.

#### 3.3.5.2 Canonical future spelling policy (v0.11 decision) <a id="part-3-3-5-2"></a>

Per [D-013](DECISIONS_LOG.md#decisions-d-013), any future value-optional feature shall use
`Optional<T>` as the canonical source spelling.

- `optional<T>` remains reserved in v1 and shall not be treated as canonical.
- If a future implementation mode offers `optional<T>` as a compatibility alias, conforming modes shall still diagnose it and provide a fix-it to `Optional<T>`.
- Canonical textual interface emission for future value-optionals shall use `Optional<T>`.

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

- ordinary sends require the receiver type be nonnull, _unless_ the send is written as an optional send (`[x? foo]`).

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

### 3.5.3 Generic method/function declarations (v2 design path; reserved in v1) <a id="part-3-5-3"></a>

#### 3.5.3.1 v1 status <a id="part-3-5-3-1"></a>

Objective‑C 3.0 v1 defers generic methods/functions.
Toolchains shall reserve the syntax in [§3.5.3.2](#part-3-5-3-2), but in v1 source that uses it is ill‑formed.

#### 3.5.3.2 Candidate syntax (future revision) <a id="part-3-5-3-2"></a>

A generic parameter clause appears before the return type for both Objective‑C methods and C/ObjC functions.

```text
generic-method-declaration:
    ('-' | '+') generic-parameter-clause method-type-and-selector

generic-function-declaration:
    generic-parameter-clause declaration-specifiers declarator
```

Examples:

```objc
- <T: id<NSCopying>> (T)coerce:(id)value;
+ <T> (NSArray<T>*)singleton:(T)value;

<T> T OCIdentity(T value);
<T: NSObject> NSArray<T>* OCCollect(T first, ...);
```

#### 3.5.3.3 Lowering and mangling path (future revision) <a id="part-3-5-3-3"></a>

The viable path is **erased execution with preserved generic signatures**:

- Type checking uses declared generic parameters/constraints.
- ABI lowering erases generic parameters to their bounds (or `id` when unconstrained), unless a future explicit reification mode is enabled.
- Objective‑C method dispatch remains selector-based with one runtime IMP per declaration; type arguments do not create selector variants.
- Generic free functions use one emitted body per declaration and shall satisfy stable mangling invariants that include:
  - base function name,
  - generic arity,
  - normalized constraint signature (or digest),
  - and deterministic reproduction for identical declarations under one toolchain/policy.

Per [D-014](DECISIONS_LOG.md#decisions-d-014), v0.11 standardizes semantic invariants and policy
stability, not one byte-for-byte mangling string across all toolchains:

- Conforming toolchains shall publish a stable mangling policy identifier for enabled generic free-function support.
- Module metadata and textual interfaces shall preserve the semantic generic signature required for cross-tool conformance assertions.
- Conformance assertions shall validate semantic equivalence and policy-id stability; literal symbol-byte equality is only required within a single declared policy.

#### 3.5.3.4 Selector and metadata interaction (future revision) <a id="part-3-5-3-4"></a>

- Selector identity excludes the generic parameter clause.
- Two methods in the same class/protocol hierarchy shall not differ only by generic parameter names or constraints if selector pieces are identical.
- Overrides/redeclarations shall match selector, generic arity, and normalized constraints.
- Module metadata and textual interfaces shall preserve the generic signature for each generic method/function (parameter list plus constraints) so importers can type-check and validate redeclarations consistently.
- Import/redeclaration mismatches in preserved generic signatures are diagnosed per [§3.7.3](#part-3-7-3).

### 3.5.4 Type argument application <a id="part-3-5-4"></a>

Type arguments may be applied to generic types using angle brackets:

```objc
Box<NSString*>* b;
```

### 3.5.5 Erasure and runtime behavior <a id="part-3-5-5"></a>

Per [D-015](DECISIONS_LOG.md#decisions-d-015), future explicit reification mode is declaration-scoped.
Unless a declaration is explicitly marked `@reify_generics` (future extension), generic arguments are erased at runtime:

- they do not affect object layout,
- they do not affect message dispatch,
- they exist for type checking and tooling.

Additional constraints for any future reification-capable mode:

- Reification controls shall apply per declaration; enabling a module/profile mode alone shall not implicitly reify unrelated declarations.
- Module/profile controls may gate whether declaration-scoped reification syntax is accepted, but shall not change meaning of declarations that omit reification markers.
- Mixed modules that contain both erased and reified declarations shall preserve this distinction in metadata and textual interfaces.

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

Required diagnostics:

- assigning nullable/unspecified to nonnull without check,
- returning nullable from a function declared nonnull,
- producing or importing a nullability-incomplete public interface in the active language view (severity per [§3.2.5.4](#part-3-2-5-4)),
- missing/mismatched nullability-related module/interface metadata as validated by [D.3.5](#d-3-5).

### 3.7.2 Optional misuse <a id="part-3-7-2"></a>

Required diagnostics:

- using a `T?` value where `T` is required without binding/unwrapping,
- applying optional type sugar (`?`/`!`) to a type outside [§3.2.2](#part-3-2-2),
- applying optional type sugar (`?`/`!`) when nullability is already explicitly spelled on the same type position,
- in v1 mode, parsing value-optional constructor spellings (`optional<...>`, `Optional<...>`) in type positions emits `OPT-SPELL-RESERVED-V1`,
- in future value-optional compatibility mode, parsing lowercase `optional<...>` emits `OPT-SPELL-NONCANON`,
- in future value-optional canonical-only mode, parsing lowercase `optional<...>` emits `OPT-SPELL-NONCANON-UNSUPPORTED`,
- when a required optional-spelling fix-it cannot be applied because the token originates in non-rewritable macro expansion text, emit companion note `OPT-SPELL-NOFIX-MACRO`,
- declaring unescaped identifiers that occupy reserved spellings `Optional`, `optional`, `some`, or `none` in ObjC 3.0 mode,
- optional member access used on scalar members,
- ordinary message sends on nullable receivers in strict mode (suggest optional send or binding).

Required optional-spelling fix-it behavior (`optional<T>` -> `Optional<T>`):

1. For `OPT-SPELL-NONCANON` and `OPT-SPELL-NONCANON-UNSUPPORTED`, rewrite only the identifier token `optional` to `Optional`.
2. Preserve generic argument text and punctuation exactly (`<...>` unchanged).
3. Preserve leading/trailing trivia around the rewritten identifier token.
4. Emit one fix-it per noncanonical occurrence, including nested occurrences.
5. If the source range is not rewritable (for example, macro expansion text), suppress the edit and emit `OPT-SPELL-NOFIX-MACRO`.

Profile severity behavior for optional-spelling diagnostics:

| Validation condition                                                      | Core                                                                                 | Strict                          | Strict Concurrency              | Strict System                   |
| ------------------------------------------------------------------------- | ------------------------------------------------------------------------------------ | ------------------------------- | ------------------------------- | ------------------------------- |
| `OPT-SPELL-NONCANON` (future mode, compatibility alias accepted)          | Warning with required fix-it in migration mode; error with required fix-it otherwise | Error with required fix-it      | Error with required fix-it      | Error with required fix-it      |
| `OPT-SPELL-NONCANON-UNSUPPORTED` (future mode, canonical-only acceptance) | Error with required fix-it                                                           | Error with required fix-it      | Error with required fix-it      | Error with required fix-it      |
| `OPT-SPELL-RESERVED-V1` (v1 reservation guardrail)                        | Error                                                                                | Error                           | Error                           | Error                           |
| `OPT-SPELL-NOFIX-MACRO` companion note when rewrite unavailable           | Note (paired with owning warning/error)                                              | Note (paired with owning error) | Note (paired with owning error) | Note (paired with owning error) |

Conforming interface emission for future value-optionals shall print canonical `Optional<...>` spellings only.

### 3.7.3 Generics <a id="part-3-7-3"></a>

Required diagnostics:

- constraint violations,
- unsafe generic downcasts requiring explicit spelling,
- use of reserved generic method/function declaration syntax in v1,
- selector collisions where declarations differ only by generic signature,
- redeclaration/import mismatch in generic arity or constraints.

### 3.7.4 Key paths <a id="part-3-7-4"></a>

Required diagnostics:

- invalid key path components,
- converting key paths to strings without explicit conversion.

### 3.7.5 Nullability completeness conformance matrix ideas <a id="part-3-7-5"></a>

Conforming suites should include profile- and language-view matrix tests such as:

- `NC-01`: Public ObjC API fully annotated via `assume_nonnull` plus explicit `nullable` overrides.
  - Expected completeness status: complete (ObjC and ObjC++).
  - Core expectation: no completeness diagnostic.
  - Strict / Strict Concurrency / Strict System expectation: no completeness diagnostic.
- `NC-02`: Intentionally incomplete public ObjC method with unannotated object pointer outside any nonnull-default region.
  - Expected completeness status: incomplete.
  - Core expectation: recoverable diagnostic; import may continue with unspecified assumptions.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-03`: Public API explicitly uses `_Null_unspecified` on an object/block pointer.
  - Expected completeness status: incomplete (intentional).
  - Core expectation: recoverable diagnostic.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-04`: Incomplete nested position (for example, object pointer inside exported block typedef signature).
  - Expected completeness status: incomplete.
  - Core expectation: recoverable diagnostic.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-05`: Mixed C+ObjC module where C partition has only non-ObjC pointers/scalars and ObjC partition is complete.
  - Expected completeness status: complete in ObjC/ObjC++; not applicable in C view.
  - Core expectation: no completeness diagnostic in ObjC/ObjC++; none for C-view non-applicable declarations.
  - Strict / Strict Concurrency / Strict System expectation: no completeness diagnostic in ObjC/ObjC++.
- `NC-06`: Mixed module with ObjC view complete, but an ObjC++-only declaration uses an unannotated ObjC object pointer.
  - Expected completeness status: ObjC view complete; ObjC++ view incomplete.
  - Core expectation: recoverable diagnostic only for ObjC++ imports.
  - Strict / Strict Concurrency / Strict System expectation: hard error for ObjC++ imports.
- `NC-07`: Metadata/interface mismatch for nullability items in [D Table A](#d-3-1) (for example, dropped nonnull-default region tag).
  - Expected completeness status: validation failure.
  - Core expectation: recoverable diagnostic unless classified ABI-significant by [D.3.5](#d-3-5).
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-08`: Metadata payload drops required nullability qualifiers/defaults so importer cannot reconstruct effective nullability.
  - Expected completeness status: treated as incomplete/invalid semantic metadata.
  - Core expectation: recoverable diagnostic with conservative unspecified assumptions.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-09`: Public header exports `id?`/`Class?` spellings and is imported via textual interface in a clean build.
  - Expected completeness status: complete when effective nullability matches qualifier form.
  - Core expectation: no completeness diagnostic; importer behavior matches equivalent `id _Nullable`/`Class _Nullable` declarations.
  - Strict / Strict Concurrency / Strict System expectation: no completeness diagnostic.
- `NC-10`: Module metadata/interface round-trip for declarations that used `id?`/`Class?` in source.
  - Expected completeness status: complete when metadata preserves equivalent effective nullability.
  - Core expectation: mismatch or dropped optional/nullability metadata is diagnosed per `NC-07`/`NC-08`.
  - Strict / Strict Concurrency / Strict System expectation: hard error on required-metadata mismatch.

These tests should be run at least across ObjC and ObjC++ import modes; C-mode imports should assert non-applicability for declarations outside [§3.2.2](#part-3-2-2).

### 3.7.6 Future value-optional spelling conformance ideas <a id="part-3-7-6"></a>

Conforming suites should include reserved-spelling occupancy and migration-path tests such as:

- `VO-01`: Declaring `typedef int Optional;` in ObjC 3.0 mode is rejected; an escaped raw identifier form is accepted.
- `VO-02`: Declaring unescaped `some`/`none` identifiers in ObjC 3.0 mode is rejected; escaped raw identifier forms are accepted.
- `VO-03`: Parsing `Optional<int>` or `optional<int>` in a type position produces a reserved-for-future-extension diagnostic in v1.
- `VO-04`: Module import of APIs that used escaped raw identifiers for reserved spellings preserves identity without enabling unescaped spellings.
- `VO-05`: Future value-optional-enabled mode parses canonical `Optional<int>` and accepts with no optional-spelling diagnostic.
- `VO-06`: Future compatibility mode accepts `optional<int>` and emits `OPT-SPELL-NONCANON` with severity per [§3.7.2](#part-3-7-2), plus one-step fix-it to `Optional<int>`.
- `VO-07`: Future canonical-only mode parsing `optional<int>` emits `OPT-SPELL-NONCANON-UNSUPPORTED` as a hard error with one-step fix-it to `Optional<int>`.
- `VO-08`: v1 mode parsing `Optional<int>` or `optional<int>` emits `OPT-SPELL-RESERVED-V1` with reserved-for-future wording (severity per [§3.7.2](#part-3-7-2)).
- `VO-09`: Interface/module emission after parsing noncanonical source in compatibility mode serializes canonical `Optional<...>` only.
- `VO-10`: Noncanonical spelling originating in non-rewritable macro expansion emits the owning spelling diagnostic plus `OPT-SPELL-NOFIX-MACRO`, and no invalid edit.
- `VO-11`: Batch migrator over source files containing `optional<...>` rewrites occurrences to `Optional<...>` with no semantic delta and no unrelated token changes.

### 3.7.7 Generic method/function conformance ideas (future-feature gate) <a id="part-3-7-7"></a>

Conforming suites should include generic-method path tests such as:

- `GM-01`: In v1 mode, parsing the [§3.5.3.2](#part-3-5-3-2) generic method/function syntax produces a reserved-for-future-extension diagnostic.
- `GM-02`: In an implementation mode that enables generic methods/functions, same-selector declarations that differ only by generic signature are rejected.
- `GM-03`: Module/interface round-trip preserves generic method/function signatures (arity and constraints), and mismatch is diagnosed on import.
- `GM-04`: Generic Objective‑C methods keep selector identity independent of type arguments (single selector string, no type-argument selector variants).
- `GM-05`: Generic free functions produce deterministic mangling for identical declarations under the same toolchain and mangling policy ID.
- `GM-06`: Different generic signatures for the same base name produce distinct mangling outcomes and distinct preserved semantic signature records.
- `GM-07`: Cross-tool conformance compares semantic signatures and declared mangling policy IDs; direct string equality of symbols is not required across different policy IDs.
- `GM-08`: Enabling a module/profile generic mode without declaration-scoped `@reify_generics` markers does not reify declarations by default.

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

No open issues are tracked in this part for v0.11.

Resolved by decisions [D-013](DECISIONS_LOG.md#decisions-d-013),
[D-014](DECISIONS_LOG.md#decisions-d-014), and
[D-015](DECISIONS_LOG.md#decisions-d-015).

<!-- END PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md -->

---

<!-- BEGIN PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md -->

# Part 4 — Memory Management and Ownership <a id="part-4"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

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

- locals annotated with `objc_precise_lifetime` shall not be released before the end of their lexical scope.

This is foundational for borrowed interior pointers ([Part 8](#part-8)).

### 4.4.1 Lifetime shortening by conformance profile (normative) <a id="part-4-4-1"></a>

For strong retainable locals that are _not_ annotated with `objc_precise_lifetime`:

- **Core**: lifetime shortening is permitted to any point after the last semantic use, including before lexical scope end, only when no scope-exit action (`defer`, `@cleanup`, `@resource`) and no post-`await` continuation can observe the value.
- **Strict**: same permitted shortening as Core.
- **Strict Concurrency**: same permitted shortening as Strict; values that are needed after a potential suspension shall be kept alive in async-frame storage across suspension/resume.
- **Strict System**: shortening is permitted only when the implementation can additionally prove that borrowed-pointer and resource-correctness invariants from [Part 8](#part-8) are preserved.

Forbidden shortening:

- **All profiles**: releasing before a potential semantic use, before required scope-exit actions that may access the value, or before lexical scope end for a local annotated with `objc_precise_lifetime`.
- **Strict / Strict Concurrency / Strict System**: when proof is insufficient, the implementation shall choose the conservative longer lifetime.
- **Strict System**: releasing an owner while any derived `borrowed` value may still be used is ill-formed.

### 4.4.2 `objc_precise_lifetime` interactions (normative) <a id="part-4-4-2"></a>

For a local annotated with `objc_precise_lifetime`:

- Scope-exit actions in the same lexical scope (`defer`, `@cleanup`, `@resource`) shall execute before the local's implicit ARC release, so the value remains alive while those actions run.
- In async functions, if the lexical scope extends across an `await`, the value shall remain retained across suspension/resume until scope exit.
- Draining implicit autorelease pools at suspension points ([§4.7.1](#part-4-7-2), [C.7](#c-7)) shall not be used to release a `objc_precise_lifetime` local before lexical scope exit.

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

### 4.7.1 Async suspension and autorelease pools <a id="part-4-7-2"></a>

#### 4.7.1.1 Normative rule (Objective‑C runtimes) <a id="part-4-7-1"></a>

On Objective‑C runtimes with autorelease semantics, a conforming implementation shall ensure:

- Each _task execution slice_ (resume → next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule is required to avoid unbounded autorelease growth across long async chains and to make memory behavior predictable for Foundation-heavy code.

See [Part 7](#part-7) [§7.9](#part-7-9) and [C.7](#c-7).

## 4.8 Interactions with retainable C families <a id="part-4-8"></a>

Retainable families (CoreFoundation-like, dispatch/xpc integration, custom refcounted C objects) are specified in [Part 8](#part-8). [Part 4](#part-4) defines their participation in the ownership model:

- retainable family types may be treated as retainable under ARC.
- transfer annotations may apply to them.

## 4.9 Explicit unmanaged references (`Unmanaged`) <a id="part-4-9"></a>

Objective‑C 3.0 v1 standardizes an Unmanaged-like wrapper for explicit retain/release control.

### 4.9.1 Module placement and availability <a id="part-4-9-1"></a>

- The standardized surface shall be provided by `objc3.system`.
- Strict System conformance claims shall provide this surface.
- Core/Strict/Strict Concurrency implementations may provide it as an extension; if provided, semantics shall match this section.

### 4.9.2 Required API surface (normative minimum) <a id="part-4-9-2"></a>

`objc3.system` shall expose an API equivalent to:

- `Unmanaged<T>.passRetained(T value) -> Unmanaged<T>`
- `Unmanaged<T>.passUnretained(T value) -> Unmanaged<T>`
- `Unmanaged<T>.takeRetainedValue() -> T`
- `Unmanaged<T>.takeUnretainedValue() -> T`
- `Unmanaged<T>.retain() -> Unmanaged<T>`
- `Unmanaged<T>.release()`

Concrete names/spellings may differ, but module metadata and documentation shall publish a stable mapping to this semantic surface.

### 4.9.3 Ownership rules (normative) <a id="part-4-9-3"></a>

- `passRetained` consumes one owned (+1) reference from the caller into the wrapper.
- `passUnretained` stores a non-owning reference and does not retain.
- `takeRetainedValue` consumes one owned (+1) reference from the wrapper and returns it to normal ARC-managed ownership in the caller.
- `takeUnretainedValue` returns a non-owning reference; callers that need ownership must retain explicitly.
- `retain` performs one explicit retain on the wrapped reference.
- `release` performs one explicit release on the wrapped reference.

### 4.9.4 ARC interaction (normative) <a id="part-4-9-4"></a>

- In ARC mode, this wrapper is the standardized escape hatch for explicit retain/release operations.
- Direct manual retain/release calls on ObjC-integrated retainable families remain governed by [Part 8](#part-8) [§8.4.2](#part-8-4-2).
- ARC and optimizer transformations shall preserve the explicit side effects of wrapper `retain`/`release` operations.
- Values extracted by `takeRetainedValue`/`takeUnretainedValue` re-enter normal ARC lifetime rules in this part.

## 4.10 Open issues <a id="part-4-10"></a>

No open issues are tracked in this part for v1.

<!-- END PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md -->

---

<!-- BEGIN PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md -->

# Part 5 — Control Flow and Safety Constructs <a id="part-5"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 5.1 Purpose <a id="part-5-1"></a>

This part defines:

- `defer` as a core control-flow primitive,
- `guard` as a scope-exit conditional with refinement,
- `match` and pattern matching,
- rules that enable compilers to generate reliable cleanup and diagnostics.

This part specifies syntax and high-level semantics. Cross-cutting cleanup-scope ordering and transfer rules are centralized in [Part 8](#part-8) [§8.1](#part-8-1) and [§8.2.3](#part-8-2-3).

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

Executing a `defer` statement registers its body as a _scope-exit action_ in the innermost enclosing lexical scope.

- Scope-exit actions execute in **LIFO** order at scope exit.
- Scope exit includes normal exit and stack unwinding (including ObjC/C++ exception unwinding where the ABI runs cleanups).
- Ordering relative to implicit ARC releases is defined in [Part 8](#part-8) [§8.2.3](#part-8-2-3).

### 5.2.3 Restrictions (normative) <a id="part-5-2-3"></a>

A deferred body shall not perform a non-local exit from the enclosing scope. The following are ill‑formed inside a `defer` body:

- `return`, `break`, `continue`, `goto` that exits the enclosing scope,
- `throw` that exits the enclosing scope,
- C++ `throw` that exits the scope.
- calls to `longjmp`/`siglongjmp` that exit the enclosing scope or bypass cleanup scopes.

Rationale: defers must be reliably executed as cleanups; permitting non-local exits leads to un-auditable control flow.

### 5.2.4 Interaction with `async` <a id="part-5-2-4"></a>

A `defer` body in an `async` function executes when the scope exits, even if the function suspends/resumes multiple times ([Part 7](#part-7) [§7.9.2](#part-7-9-2)).

### 5.2.5 Non-local transfer across cleanup scopes <a id="part-5-2-5"></a>

`cleanup scope` is defined in [Part 8](#part-8) [§8.1](#part-8-1).

Normative non-local transfer behavior across cleanup scopes is centralized in [Part 8](#part-8) [§8.1](#part-8-1). Conforming implementations shall apply those rules unchanged to `defer` and other control-flow exits in this part.

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

Names proven by guard conditions are _refined_ after the guard:

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
- `case pattern => expression` is reserved for a future expression form ([§5.4.7](#part-5-4-7)).
- Fallthrough is not permitted.

### 5.4.3 Evaluation order (normative) <a id="part-5-4-3"></a>

- The scrutinee expression in `match (expr)` shall be evaluated exactly once.
- Case patterns are tested top-to-bottom.
- The first matching case is selected and its body executes.

### 5.4.4 Scoping (normative) <a id="part-5-4-4"></a>

Bindings introduced by a case pattern are scoped to that case body only.

### 5.4.5 Exhaustiveness (normative) <a id="part-5-4-5"></a>

In strict mode, `match` must be exhaustive when the compiler can prove the matched type is a _closed set_.

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

### 5.4.7 Reserved expression form (v2+ hook; rejected in v1) <a id="part-5-4-7"></a>

ObjC 3.0 v1 keeps `match` statement-only. A value-producing form is reserved for a future revision:

```text
match-expression (reserved):
    'match' '(' expression ')' '{' match-expression-case+ match-expression-default? '}'

match-expression-case (reserved):
    'case' pattern '=>' expression ';'

match-expression-default (reserved):
    'default' '=>' expression ';'
```

Reserved typing behavior (for v2+ design stability):

- each selected arm yields one value (non-`void`),
- all arm values must converge to a common result type under the same conversion lattice used for `?:`,
- expression form must be exhaustive for closed sets (or include `default`).

Required v1 behavior:

- using `match` in expression position is ill-formed,
- using `case ... => ...` is ill-formed and shall diagnose that this spelling is reserved for future expression-form `match`,
- parser/diagnostic conformance shall include [Appendix F](#f) rows `P-31`, `P-32`, and `P-33` ([Part 12](#part-12) [§12.5.1](#part-12-5-1), [§12.5.7](#part-12-5-7)).

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

### 5.5.5 Type-test patterns (optional feature set in v1) <a id="part-5-5-5"></a>

Type-test patterns are **optional** in v1 (not core).

Feature-test contract:

- `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__` shall be predefined as `1` when supported and `0` when unsupported.
- If `__has_feature` integration exists ([Part 1](#part-1) [§1.4.3](#part-1-4-3)), `__has_feature(objc3_match_type_test_patterns)` shall report the same capability state.

Syntax (when enabled):

- `is Type`
- `is Type let name` (bind as refined type)
- `is Type var name` (mutable refined binding)

Semantics (when enabled):

- For class types, it matches when the dynamic type is `Type` or a subclass.
- For protocol types, it matches when the value conforms to the protocol.
- `let`/`var` bindings introduced by `is Type let/var name` are scoped to the selected case body.

Module metadata / interface requirements (when used in exported syntax payloads):

- A module payload that requires importer understanding of type-test pattern syntax shall include required capability `objc3.pattern.type_test.v1` in `required_capabilities` ([D.2.1](#d-2-1)).
- Importers that do not support this capability shall reject import as a hard error per [D.2.3](#d-2-3) and [Table E](#d-3-5).
- Textual interfaces exposing such syntax shall guard it with `#if __OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__` (or provide a portable fallback spelling).

Unsupported-mode requirements (`__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0`):

- `case is ...` in a pattern position is ill-formed.
- Diagnostics shall explicitly state that type-test patterns are optional and disabled in the current mode, and suggest a mechanical rewrite (`default` + explicit `if`/cast chain) when feasible.

### 5.5.6 Guarded patterns are deferred in v1 (reserved syntax) <a id="part-5-5-6"></a>

Guarded patterns are **deferred** from v1 core syntax. The following slot is reserved for a future revision:

```text
guarded-match-case (reserved):
    'case' pattern 'where' expression ':' compound-statement
```

Reservation and compatibility requirements:

- In v1, `case pattern where condition:` is ill-formed and shall be rejected with a targeted “guarded patterns are deferred” diagnostic.
- `where` is reserved as a contextual keyword only in the post-pattern guard slot.
- Outside that slot, `where` remains usable as an identifier for source compatibility.
- Parser conformance shall include [Appendix F](#f) rows `P-37`, `P-38`, and `P-39` ([Part 12](#part-12) [§12.5.1](#part-12-5-1), [§12.5.7](#part-12-5-7)).

## 5.6 Required diagnostics <a id="part-5-6"></a>

Minimum diagnostics include:

- non-exiting `guard else` blocks (error),
- `defer` bodies containing non-local exits (error),
- unreachable `match` cases (warning),
- non-exhaustive `match` over `Result` in strict mode (error; fix-it: add missing case or `default`),
- `match` used where an expression is required (error; explain v1 statement-only rule),
- `case ... => ...` spelling in v1 (error; explain reserved future expression-form syntax),
- `case is ...` when `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0` (error; explain optional feature gate and suggest rewrite),
- importing module metadata that requires `objc3.pattern.type_test.v1` when unsupported locally (hard error),
- `case pattern where condition:` in v1 (error; explain guarded-pattern deferral and reservation).

## 5.7 Open issues <a id="part-5-7"></a>

None currently tracked in this part.

<!-- END PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md -->

---

<!-- BEGIN PART_6_ERRORS_RESULTS_THROWS.md -->

# Part 6 — Errors: Result, throws, try, and Propagation <a id="part-6"></a>

_Working draft v0.11 — last updated 2026-02-23_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 6.0 Overview <a id="part-6-0"></a>

### v0.10 resolved decisions <a id="part-6-v0-10-resolved-decisions"></a>

- ObjC 3.0 v1 does not add a dedicated `never throws` marker; absence of `throws` is the canonical non-throwing form.
- Generic non-throwing requirements are expressed using non-throwing function/block types, not a new keyword or attribute.
- Nil-to-error mapping is explicit and library-defined via canonical `objc3.errors` helpers (`orThrow` and `okOr`), not language sugar.
- Typed throws remains a planned post-v1 feature; v1 reserves syntax/metadata slots and requires compatibility diagnostics for non-v1 typed-throws metadata.

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

An _error value_ is any Objective‑C object value that conforms to `Error`.

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
- reserve typed-throws metadata slots with v1-default values as specified in [§6.3.7](#part-6-3-7),
- diagnose effect mismatches on redeclaration/import ([C.2](#c-2)),
- and provide a stable calling convention for throwing functions/methods.

The recommended calling convention is the _trailing error-out parameter_ described in [C.4](#c-4).

> Note: This draft intentionally chooses an error-out convention

**Reflection note (non-normative):** v1 does not require encoding `throws` in Objective‑C runtime type encodings. Toolchains may provide extended metadata as an extension.
rather than stack unwinding, to keep interoperability with NSError/return-code APIs straightforward and to align with existing Objective‑C runtime practices.

Throwing is part of a function’s type.

- `R (^)(Args) throws` is a throwing block type.
- `R (^)(Args)` is non-throwing.

A throwing function value cannot be assigned to a non-throwing function type without an explicit adapter.

### 6.3.6 Non-throwing declarations (v1 decision) <a id="part-6-3-6"></a>

ObjC 3.0 v1 does not define a dedicated `nothrows`/`never throws` keyword or standard attribute.

- A function or method declaration without `throws` shall be non-throwing.
- Module/interface metadata shall preserve non-throwing status by encoding the absence of `throws` (or an equivalent canonical `throws=false` bit); no additional source marker is required for conformance.

Recommended patterns for generic and callable APIs:

- Require non-throwing callable types directly (for example, `R (^)(Args)` rather than `R (^)(Args) throws`).
- If an API should accept both throwing and non-throwing callables, declare the parameter as throwing and rely on the implicit non-throwing to throwing conversion in [§6.3.4](#part-6-3-4).
- When adapting a throwing callable to a non-throwing callable, use an explicit adapter that handles the error path.

### 6.3.7 Reserved typed-throws slots (planned, not in v1) <a id="part-6-3-7"></a>

Typed throws is planned for a future revision, but it is not available in ObjC 3.0 v1.

Reserved source syntax slots:

- `throws(type-name)`
- `throws(type-name, ...)`

v1 parser and diagnostics requirements:

- A declaration using `throws(` ... `)` shall be rejected in v1 mode with a diagnostic that typed throws is unsupported in v1.
- A compiler shall not reinterpret `throws(...)` as bare `throws`, and shall not silently erase the parenthesized payload.
- A fix-it may suggest replacing `throws(...)` with bare `throws` when preserving behavior.

Reserved metadata slots for module/interface exchange:

- `throws_kind`: enum slot. v1 requires `untyped`; `typed` is reserved.
- `throws_type_arity`: unsigned slot. v1 requires `0`.
- `throws_type_refs`: sequence slot of canonical type references. v1 requires empty.

Compatibility constraints:

- A v1 consumer that imports non-v1 typed-throws metadata values (for example, `throws_kind=typed`, non-zero arity, or non-empty type refs) shall emit an incompatibility diagnostic and reject that declaration for v1 conformance.
- A producer targeting v1 shall not emit non-v1 typed-throws metadata values.
- If a future revision introduces typed throws, untyped and typed declarations are effect-signature-distinct across module boundaries unless that future revision explicitly defines a conversion rule.

---

## 6.4 `throw` statement <a id="part-6-4"></a>

### 6.4.1 Grammar <a id="part-6-4-1"></a>

```text
throw-statement:
    'throw' expression ';'
```

### 6.4.2 Static semantics <a id="part-6-4-2"></a>

A `throw` statement is permitted only within a `throws` function or within a `catch` block.

In v1, `throws` is untyped. The thrown expression shall be convertible to `id<Error>`.

Typed throws forms (for example, `throws(E)`) are reserved for a future revision; their v1 reservation and compatibility rules are defined in [§6.3.7](#part-6-3-7).

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
Convert explicitly using `guard let` or the canonical `objc3.errors` helper APIs in [§6.6.6](#part-6-6-6).

### 6.6.5 Diagnostics <a id="part-6-6-5"></a>

- Using `?` outside the follow-token restriction is ill-formed (fix-it: parenthesize).
- Using `?` without compatible early-exit carrier is ill-formed.

### 6.6.6 Explicit nil→error helpers (`orThrow`, `okOr`) <a id="part-6-6-6"></a>

The standard library defines canonical explicit adapters for optional-to-error mapping in `objc3.errors` ([S.2.2](#s-2-2)):

- `orThrow<T>(value: T?, makeError: () -> id<Error>) throws -> T`
- `okOr<T, E: Error>(value: T?, makeError: () -> E) -> Result<T, E>`

Interaction with `try` and postfix propagation is normative:

- `orThrow(...)` is throwing. A call shall follow normal `throws` call-site rules in [§6.3.3](#part-6-3-3), including `try` where required.
- `okOr(...)` is non-throwing and yields `Result<T, E>`.
- In a `throws` function, optional nil-to-error conversion shall be explicit (for example, `try orThrow(opt, makeError)` or `guard let ... else { throw ... }`).
- In a `Result<..., E>`-returning function, `okOr(opt, makeError)?` is the canonical propagation form.
- `try` does not perform optional carrier conversion by itself; it only handles throwing evaluation.

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

- None in this part as of v0.10.

## 6.13 Future extensions (non-normative) <a id="part-6-13"></a>

### 6.13.1 Typed throws <a id="part-6-13-1"></a>

A future revision may introduce typed throws syntax to restrict throwable error sets.
ObjC 3.0 v1 intentionally ships only untyped `throws` and uses the reservation rules in [§6.3.7](#part-6-3-7) to preserve forward compatibility with that future work.

<!-- END PART_6_ERRORS_RESULTS_THROWS.md -->

---

<!-- BEGIN PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md -->

# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors <a id="part-7"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 7.0 Overview <a id="part-7-0"></a>

### v0.10 resolved decisions <a id="part-7-v0-10-resolved-decisions"></a>

- v1 includes standard macro sugar for task spawning as a source-level convenience over library-defined task APIs; sugar expansion is semantically fixed but runtime entrypoint naming remains unfrozen.
- Explicit actor reentrancy controls are deferred from v1; reservation diagnostics and compatibility rules are defined.

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

### 7.4.4 Current executor model (normative) <a id="part-7-4-4"></a>

Each executing async task shall have a logical `current executor` value in task context metadata.

Rules:

- Entering an executor-annotated declaration (`objc_executor(X)`) sets `current executor := X` for the dynamic extent of that entry.
- Entering actor-isolated code sets `current executor` to the actor instance's serial executor for that entry.
- Calling unannotated synchronous declarations does not change `current executor`.
- Across suspension/resumption (`await`), the task's logical executor context shall be preserved, then updated as required by the resumed continuation's entry isolation/annotation.
- Detached tasks begin on an implementation-selected executor unless the API contract specifies one; once running, executor transitions shall still follow the same hop rules.

### 7.4.5 Proof rules and hop elision (normative) <a id="part-7-4-5"></a>

The compiler may treat an executor hop as provably unnecessary only when it can establish executor equivalence from language-visible facts.

Accepted proof sources:

- lexical/type-visible annotation identity in the current module (`objc_executor(X)` to `objc_executor(X)`),
- actor-isolated same-instance execution where the current continuation is already bound to that actor executor,
- imported module/interface metadata that preserves executor/isolation semantics per [D Table A](#d-3-1) and compatibility rules in [D.3.4](#d-3-4).

Conservative requirements:

- Missing, incompatible, or unverifiable executor metadata shall be treated as "not proven"; `await` and hop behavior remain required.
- Strict concurrency profiles shall diagnose missing/incompatible required metadata as errors per [D.3.5](#d-3-5), rather than assuming no-hop.
- Implementations may perform additional optimizer-local hop elimination only if they preserve the same observable suspension and isolation correctness required by this section.

### 7.4.6 Fairness and ordering guarantees (normative minimum) <a id="part-7-4-6"></a>

Objective-C 3.0 does not require a global scheduler policy, but requires the following minimum portability guarantees:

- **Serial exclusion**: a serial executor (including actor executors) shall not run two isolated work items concurrently.
- **Forward progress**: runnable work items enqueued on a live executor shall eventually execute absent permanent external blocking or process termination.
- **No strict global FIFO guarantee**: relative ordering across different executors, priorities, or detached domains is implementation-defined unless explicitly documented by the runtime profile.
- **Intra-actor safety**: regardless of queue policy, actor-isolated mutable state shall observe single-executor access and the reentrancy rules in [§7.7.3](#part-7-7-3).

### 7.4.7 Executor conformance tests (normative minimum) <a id="part-7-4-7"></a>

Conforming suites shall include at least:

- `EXE-01` required hop: call `objc_executor(main)` from known non-main context; `await` required and execution occurs on main executor.
- `EXE-02` hop elision: call target with proven same executor annotation; no hop required and behavior matches same-executor semantics.
- `EXE-03` cross-module proof: import executor metadata from another module and prove/no-prove hops consistently with in-module behavior.
- `EXE-04` metadata loss guard: remove/mismatch required executor metadata and assert profile-appropriate diagnostics per [D.3.5](#d-3-5).
- `EXE-05` progress smoke test: finite runnable queue on a live executor drains without starvation under documented runtime constraints.

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

### 7.5.5 Standard macro sugar for task spawning (normative, v1) <a id="part-7-5-5"></a>

Objective‑C 3.0 v1 standardizes optional source sugar for task spawning while keeping task semantics library-defined.

Required public macro interface (standard concurrency headers):

- `OBJC_TASK_SPAWN(block_expr)` where `block_expr` is an async callable expression accepted by the child-spawn API contract.
- `OBJC_TASK_DETACHED(block_expr)` where `block_expr` is an async callable expression accepted by the detached-spawn API contract.

Expansion and semantic requirements:

- `OBJC_TASK_SPAWN(e)` shall be semantically equivalent to a direct call to an API recognized as `__attribute__((objc_task_spawn))` ([§7.5.2.1](#part-7-5-2-1)).
- `OBJC_TASK_DETACHED(e)` shall be semantically equivalent to a direct call to an API recognized as `__attribute__((objc_task_detached))` ([§7.5.2.2](#part-7-5-2-2)).
- The macro layer shall not alter cancellation inheritance, executor behavior, or Sendable-like enforcement relative to the equivalent direct call.
- Conformance does not require fixed runtime symbol names or a fixed token-for-token expansion body; only semantic equivalence to the recognized spawn class is required.

Diagnostics requirements:

- Type/capture diagnostics that would be produced for the equivalent direct spawn call shall also be produced through macro sugar.
- Implementations shall diagnose misuse at or pointing to the user macro invocation/argument site (with expansion notes permitted), not only in synthetic internals.
- The unused-handle warning from [§7.5.4](#part-7-5-4) applies equally to macro spellings.

Metadata/interface emission requirements:

- Use of macro sugar shall not hide spawn classification from semantic analysis or emitted module/interface metadata.
- If exported inline declarations use `OBJC_TASK_SPAWN`/`OBJC_TASK_DETACHED`, importers shall observe equivalent concurrency metadata and diagnostics behavior as for direct recognized API calls.
- No additional v1 required capability is introduced solely for this sugar; it is defined as a source-level front-end convenience over existing spawn metadata contracts.

### 7.5.6 Task macro sugar conformance matrix (normative minimum) <a id="part-7-5-6"></a>

Conforming suites shall include at least:

- `TSUG-01` child-spawn macro equivalence: `OBJC_TASK_SPAWN(...)` and direct `objc_task_spawn`-recognized API call produce equivalent child cancellation/executor inheritance behavior.
- `TSUG-02` detached-spawn macro equivalence: `OBJC_TASK_DETACHED(...)` and direct `objc_task_detached`-recognized API call produce equivalent detached cancellation behavior.
- `TSUG-03` diagnostics parity: strict-mode Sendable-like and unused-handle diagnostics fire through macro spellings with primary locations at the user invocation/capture sites.
- `TSUG-04` metadata parity: an exported inline declaration using task macros emits/imports concurrency metadata equivalently to a direct recognized call, preserving cross-module checking.

## 7.6 Cancellation <a id="part-7-6"></a>

### 7.6.1 Cancellation model (normative) <a id="part-7-6-1"></a>

Cancellation in Objective-C 3.0 concurrency is **cooperative**.

- Requesting cancellation shall not preemptively terminate execution at an arbitrary instruction boundary.
- Each task shall carry a cancellation state (bit/token equivalent) that is:
  - set by a cancellation request,
  - idempotent (multiple requests are equivalent to one),
  - and monotonic (once cancelled, it stays cancelled for the task lifetime).
- A cancellation request by itself does not throw; it changes task state that code may observe.

### 7.6.2 Propagation and inheritance (normative) <a id="part-7-6-2"></a>

Cancellation propagation shall follow these rules:

- **Parent -> child (`objc_task_spawn`)**
  - A child task shall inherit the parent's cancellation state at spawn time.
  - If the parent is already cancelled, the child starts in the cancelled state.
  - If the parent is cancelled later, cancellation shall propagate to all live structured descendants.
- **Child -> parent/siblings**
  - Cancelling a child task shall not implicitly cancel its parent or sibling tasks.
- **Task groups (`objc_task_group`)**
  - Tasks added to a group are structured children for cancellation purposes.
  - Cancelling the enclosing task or the group scope shall cancel all unfinished tasks in that group.
  - If a group API exits by throwing (including cancellation), remaining unfinished group tasks shall be cancelled before scope unwind completes ([§7.5.2.3](#part-7-5-2-3)).
  - Group operations that propagate one child failure shall cancel unfinished siblings before propagating that failure.
- **Detached tasks (`objc_task_detached`)**
  - Detached tasks do not inherit cancellation unless an API explicitly specifies inherited cancellation.
  - Cancelling a parent task or task group shall not cancel detached tasks by default.
  - Detached tasks may be cancelled only through their own handle/runtime cancellation facility.
- **Across suspension, executor hops, and actor hops**
  - Cancellation state is task-local metadata and shall be preserved across `await` suspension/resume boundaries, executor hops, and actor hops.

### 7.6.3 Observation points and required behavior (normative minimum) <a id="part-7-6-3"></a>

A conforming standard library/runtime shall expose:

- a non-throwing query equivalent to `TaskIsCancelled()` / `Task.isCancelled`,
- and a throwing checkpoint equivalent to `TaskCheckCancellation()` / `Task.checkCancellation()`.

The following are required cancellation observation points:

1. When entering any standard-library operation documented as a cancellation point.
2. Immediately before a task would suspend in a standard-library wait primitive (for example: join, group wait/next, or sleep).
3. Immediately after resumption from such a wait primitive and before user continuation proceeds.
4. During structured group-scope finalization before waiting for remaining children.

When cancellation is observed:

- A throwing cancellation point shall complete by throwing the cancellation representation defined in [§7.6.4](#part-7-6-4), without beginning additional blocking work.
- A non-throwing cancellation point shall complete promptly using its documented cancelled result/status and shall not block indefinitely.
- Implementations shall not inject asynchronous exceptions into non-observation instruction boundaries.

### 7.6.4 Representation and interaction with `throws` / propagation operators <a id="part-7-6-4"></a>

Cancellation completion shall be representable as a distinguished error value (for example, `CancellationError` or an implementation-defined equivalent that can be reliably recognized by library predicates/pattern matching).

Rules:

- `await` alone does not transform cancellation into a thrown error.
- `try await` propagates cancellation exactly as other thrown errors when a callee/cancellation point throws the cancellation representation.
- `try? await` (or equivalent optionalizing propagation operator) shall convert cancellation throws into the operator's non-throwing "empty" result, consistent with its behavior for other errors.
- Joining a throwing task that completed due to cancellation shall throw the same cancellation representation.
- Non-throwing join/group APIs shall represent cancellation in their result/status model and shall not silently report successful completion.

### 7.6.5 Cleanup and unwind guarantees under cancellation (normative) <a id="part-7-6-5"></a>

Cancellation shall not weaken scope cleanup guarantees:

- `defer` handlers shall execute exactly once, in LIFO order, when scope exit occurs due to cancellation-driven unwind.
- Language/runtime cleanup for local resources (including ARC releases and any applicable destructor/finalizer semantics) shall run as on ordinary throw/return paths.
- Async-frame captured objects retained across suspension shall be released when the frame is destroyed, including cancellation unwind ([§7.9.3](#part-7-9-3)).
- Autorelease pool draining requirements remain in force on cancellation unwind ([§7.9.4](#part-7-9-4)).
- Structured group scopes shall not finish returning/unwinding until required child cancellation+completion conditions are satisfied ([§7.5.2.3](#part-7-5-2-3)).

### 7.6.6 Cancellation interaction matrix (normative summary) <a id="part-7-6-6"></a>

The matrix below is normative and cross-references required behavior:

| Construct                  | If cancellation is observed                                      | Required behavior                                                                                                         | Reference                                        |
| -------------------------- | ---------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------ |
| `defer`                    | During unwind from cancellation                                  | Execute all registered defers exactly once (LIFO) before scope exit completes                                             | [§7.6.5](#part-7-6-5), [§7.9.2](#part-7-9-2)     |
| `try await f()`            | `f`/callee cancellation point throws cancellation representation | Propagate as a thrown error via normal `throws` rules                                                                     | [§7.6.4](#part-7-6-4), [§7.9.1](#part-7-9-1)     |
| `try? await f()`           | Same as above                                                    | Convert throw (including cancellation) into the operator's non-throwing empty result                                      | [§7.6.4](#part-7-6-4)                            |
| `await` actor/executor hop | Caller task already cancelled before hop                         | Hop/call remains legal; cancellation state is preserved; no implicit throw unless code hits a throwing cancellation point | [§7.6.2](#part-7-6-2), [§7.7.2.1](#part-7-7-2-1) |
| Task/group wait or join    | Wait primitive observes cancellation                             | Throw cancellation (throwing form) or return cancelled status (non-throwing form); do not block indefinitely              | [§7.6.3](#part-7-6-3), [§7.6.4](#part-7-6-4)     |

### 7.6.7 Cancellation conformance tests (normative minimum) <a id="part-7-6-7"></a>

Conforming suites shall include at least:

- `CAN-01` parent cancellation propagates to live child tasks spawned with `objc_task_spawn`.
- `CAN-02` detached tasks do not inherit parent/group cancellation unless API explicitly opts in.
- `CAN-03` group failure path cancels unfinished siblings before propagating failure/cancellation.
- `CAN-04` cancellation-driven unwind executes `defer` and required resource cleanups exactly once.
- `CAN-05` `try? await` converts cancellation throw into the operator's non-throwing empty result.
- `CAN-06` cancellation state is preserved across executor and actor hops.
- `CAN-07` non-throwing cancellation points return documented cancelled status and do not report success.

---

## 7.7 Actors <a id="part-7-7"></a>

### 7.7.1 Actor types <a id="part-7-7-1"></a>

```objc
actor class Name : NSObject
@end
```

### 7.7.2 Isolation <a id="part-7-7-2"></a>

Actors provide data-race safety by associating each actor instance with a **serial executor** and treating the actor’s isolated mutable state as accessible only while running on that executor.

Informally: _outside code must hop onto the actor to touch its state._

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

Actor execution is reentrant at suspension points.

Normative rules:

- While an actor-isolated task is running between suspension points, no other actor-isolated task for the same actor may run concurrently.
- If actor-isolated code executes `await` (or another potentially suspending operation), the current task may suspend and other queued actor work may run before the original task resumes.
- Reentrancy occurs only at suspension points; there is no arbitrary mid-expression interleaving inside a non-suspending segment.

#### 7.7.3.1 Reentrancy points and invariants (normative) <a id="part-7-7-3-1"></a>

- Every potentially suspending operation in actor-isolated code is a reentrancy point boundary.
- Invariants that must hold across actor turns shall be re-established before reaching a reentrancy point.
- Code that requires a non-reentrant critical region shall avoid suspension inside that region (for example, compute local snapshots first, then `await` after region completion).

#### 7.7.3.2 Isolation boundary and atomicity expectations (normative) <a id="part-7-7-3-2"></a>

- Actor isolation guarantees single-executor access to actor-isolated mutable state; it does not provide multi-operation transactions across separate awaits/calls.
- A single actor-isolated member invocation is atomic only with respect to other actor-isolated entries until it reaches a suspension point.
- Cross-actor property/method access from outside the actor domain is a potentially suspending operation and requires `await` per [§7.7.2.1](#part-7-7-2-1).
- `atomic` property attributes do not weaken actor-isolation requirements and do not imply lock-free progress across actor turns.

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
- Parameters and return types of `nonisolated` members that may cross concurrency domains shall satisfy Sendable-like checking in strict concurrency mode ([§7.8.3](#part-7-8-3)).

> Note: Toolchains may additionally accept a contextual keyword spelling (`nonisolated`) as sugar, but emitted interfaces should use the canonical attribute spelling ([B.3.4](#b-3-4)).

### 7.7.5 Actor isolation/reentrancy conformance matrix (normative minimum) <a id="part-7-7-5"></a>

Conforming suites shall include at least:

- `ACT-01` cross-actor property/method access without `await` is rejected in strict concurrency mode.
- `ACT-02` same-actor isolated access without `await` (no suspension boundary crossed) is accepted.
- `ACT-03` actor method with an internal `await` permits interleaving by another queued actor task between pre- and post-await observations.
- `ACT-04` `objc_nonisolated` member that touches isolated mutable state without explicit hop is rejected.
- `ACT-05` `objc_nonisolated` member signatures with non-Sendable boundary types are rejected in strict concurrency mode.
- `ACT-06` cross-module import preserves actor isolation metadata; missing/mismatched metadata triggers profile-appropriate diagnostics.
- `ACT-07` explicit `nonreentrant` actor-member marker use in v1 mode is rejected with a targeted deferred-feature diagnostic.
- `ACT-08` explicit `__attribute__((objc_actor_nonreentrant))` use in v1 mode is rejected with a targeted deferred-feature diagnostic.
- `ACT-09` `nonreentrant` remains usable as an identifier outside reserved actor-member modifier positions.

### 7.7.6 Explicit actor reentrancy controls are deferred in v1 (reserved syntax) <a id="part-7-7-6"></a>

Objective‑C 3.0 v1 does not include explicit source-level actor nonreentrancy controls. Reentrant behavior at suspension points remains as defined by [§7.7.3](#part-7-7-3).

Reserved future spellings (not enabled in v1):

```text
actor-member-reentrancy-modifier (reserved):
    'nonreentrant'
  | '__attribute__((objc_actor_nonreentrant))'
```

Reservation and compatibility requirements:

- In v1 mode, declarations using either reserved spelling above are ill-formed and shall produce a targeted "explicit actor nonreentrancy is deferred in v1" diagnostic.
- `nonreentrant` is reserved as a contextual keyword only in actor-member reentrancy-modifier positions.
- Outside that slot, `nonreentrant` remains a valid identifier for source compatibility.
- Canonical v1 module/interface emission shall not claim or require explicit actor-reentrancy-control metadata.

Interaction with `await` and cancellation while deferred:

- `await` remains a reentrancy boundary exactly as specified in [§7.7.3.1](#part-7-7-3-1); no extra nonreentrancy exception exists in v1.
- Cancellation semantics and observation points are unchanged by this deferral and remain governed by [§7.6](#part-7-6).

---

## 7.8 Sendable-like checking <a id="part-7-8"></a>

### 7.8.1 Marker protocol <a id="part-7-8-1"></a>

```objc
@protocol Sendable
@end
```

### 7.8.2 Default Sendable-like status by category (normative) <a id="part-7-8-2"></a>

Unless overridden by explicit conformance or attributes, implementations shall classify values as follows:

- **Immutable/value-like data**: Sendable-like by default.
  - This includes C scalar/enumeration values and aggregates whose stored fields are all Sendable-like.
  - Aggregates containing borrowed pointers ([Part 8](#part-8) [§8.7](#part-8-7)) or resource-handle fields requiring cleanup ([Part 8](#part-8) [§8.3](#part-8-3)) are not implicitly Sendable-like.
- **Actor references**: Sendable-like by default.
  - References to `actor class` instances may cross concurrency domains; isolation rules still apply at use sites (`await`/hop requirements are unchanged).
- **Ordinary class instances (non-actor)**: Not Sendable-like by default.
  - A class instance is Sendable-like only if the type conforms to `Sendable` (or is otherwise recognized by toolchain rules as safe), or the type uses the explicit unsafe escape hatch ([§7.8.5](#part-7-8-5)).
  - `const` qualification on an object pointer does not by itself imply Sendable-like status.
- **Blocks/closures**: Not Sendable-like by default.
  - A block value is accepted at a concurrent boundary only when its capture set satisfies [§7.8.4](#part-7-8-4).

### 7.8.3 Enforcement sites (normative) <a id="part-7-8-3"></a>

In strict concurrency mode, the compiler shall enforce Sendable-like constraints at all of the following boundaries:

1. **Task spawning APIs** recognized by `__attribute__((objc_task_spawn))`, `__attribute__((objc_task_detached))`, and task-group add operations:
   - values captured by the spawned async block,
   - and values explicitly passed into the spawned work item.
2. **Cross-actor entry**:
   - arguments passed to actor-isolated members when a hop may be required,
   - returned values (and thrown values, if any) crossing back to the caller.
3. **`nonisolated` actor members**:
   - parameters and returns of `objc_nonisolated` members that are callable from arbitrary executors.
4. **Concurrent closure/block captures**:
   - implicit and explicit captures (including `self`) in closures known to execute concurrently with the caller.

In permissive modes, implementations may downgrade selected violations to warnings, but strict concurrency mode shall diagnose them as errors unless [§7.8.5](#part-7-8-5) applies.

### 7.8.4 Capture, pointer, and handle interaction rules (normative) <a id="part-7-8-4"></a>

- **`weak` captures**:
  - `weak` capture does not automatically make a value Sendable-like.
  - The referenced type must still be Sendable-like for the capture to cross a checked boundary.
- **`unowned` / `__unsafe_unretained` captures**:
  - Treated as unsafe by default for concurrent transfer.
  - Strict concurrency mode shall reject these captures unless the captured type/wrapper is explicitly admitted via [§7.8.5](#part-7-8-5).
- **Borrowed pointers** (`borrowed T *`, borrowed returns):
  - Borrowed values are non-Sendable across task/actor/nonisolated boundaries by default.
  - `objc_unsafe_sendable` does not waive borrowed-lifetime non-escaping requirements from [Part 8](#part-8) [§8.7](#part-8-7); violations remain diagnostics.
- **Resource handles / cleanup-managed values**:
  - Values governed by cleanup/resource annotations ([Part 8](#part-8) [§8.3](#part-8-3)) are non-Sendable by default, even when represented as scalar handles.
  - Crossing a concurrency boundary requires an explicit safe transfer abstraction (for example, a wrapper with defined ownership transfer semantics) or explicit unsafe admission per [§7.8.5](#part-7-8-5).

### 7.8.5 Unsafe escape hatch semantics (`objc_unsafe_sendable`) (normative) <a id="part-7-8-5"></a>

`__attribute__((objc_unsafe_sendable))` (see [B.3.3](#b-3-3)) provides explicit programmer assertion that a type/wrapper may be treated as Sendable-like when proof is unavailable.

Rules:

- The attribute may satisfy Sendable-like checking at the boundaries listed in [§7.8.3](#part-7-8-3).
- It does not relax unrelated concurrency rules:
  - missing required `await`/actor hop remains an error,
  - borrowed-pointer escape/lifetime violations remain diagnostics per [Part 8](#part-8),
  - resource cleanup/move correctness rules remain diagnostics per [Part 8](#part-8).
- Any acceptance that depends on `objc_unsafe_sendable` shall produce an explicit diagnostic note or warning in strict concurrency mode identifying the unchecked transfer site.
- Applying `objc_unsafe_sendable` to APIs that expose borrowed pointers or cleanup-managed resource handles should produce an additional warning that safety is user-asserted and not compiler-proven.

### 7.8.6 Conformance test matrix (normative minimum) <a id="part-7-8-6"></a>

A conforming implementation's test suite shall include, at minimum, the matrix below.

| Case      | Scenario                                                                                                                   | Expected result in strict concurrency mode              | Notes                                                          |
| --------- | -------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------- | -------------------------------------------------------------- |
| SND-01    | Spawned task captures only scalar/enum/aggregate values that are transitively Sendable-like                                | Accept                                                  | Validates default value-like sendability                       |
| SND-02    | Spawned task captures actor reference and performs actor-isolated access with `await`                                      | Accept                                                  | Actor references are Sendable-like; isolation still enforced   |
| SND-03    | Spawned/detached task captures ordinary mutable class instance with no `Sendable` conformance                              | Error                                                   | Non-actor class instances are non-Sendable by default          |
| SND-04    | Cross-actor call passes non-Sendable class argument or returns non-Sendable class value                                    | Error                                                   | Boundary enforcement for arguments/returns                     |
| SND-05    | `objc_nonisolated` member signature includes non-Sendable parameter/result type                                            | Error                                                   | Enforced even if body is otherwise valid                       |
| SND-06    | Concurrent closure uses `weak`/`unowned` capture of non-Sendable reference type                                            | Error                                                   | `weak`/`unowned` does not bypass checking                      |
| SND-07    | Borrowed pointer or cleanup-managed resource handle crosses task/actor boundary without safe wrapper                       | Error                                                   | Part 8 interaction rules remain in force                       |
| SND-08    | Same as SND-03/SND-04 but type is marked `objc_unsafe_sendable`                                                            | Accept with required unchecked-transfer warning/note    | Escape hatch is explicit and auditable                         |
| SND-XM-01 | Module `A` exports `Sendable`/`objc_unsafe_sendable` metadata; module `B` imports and uses it at sendability boundaries    | Import and checking behavior matches in-module behavior | Cross-module semantic preservation required by [D.3.1](#d-3-1) |
| SND-XM-02 | Module metadata/interface omits or mismatches Sendable/task-spawn metadata, then imported under strict concurrency profile | Hard error on import or use-site validation             | Required by [D.3.5](#d-3-5)                                    |

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

## 7.10 Diagnostics <a id="part-7-10"></a>

Minimum diagnostics:

- `await` outside async (error)
- calling async without await (error)
- crossing an executor boundary into `objc_executor(X)` without `await` when a hop may be required (error in strict concurrency mode)
- assuming executor-equivalence/hop elision without required proof metadata in strict concurrency mode (error; reference [§7.4.5](#part-7-4-5), [D.3.5](#d-3-5))
- cross-actor isolated access without `await` when a hop may be required (error in strict)
- Sendable violations in strict concurrency mode (error)
- use of `objc_unsafe_sendable` at a transfer boundary (explicit warning/note in strict concurrency mode)
- attempted use of `objc_unsafe_sendable` to suppress borrowed-pointer/resource correctness violations (diagnose underlying rule violation; do not suppress)
- unused task handles returned from `__attribute__((objc_task_spawn))` APIs (warning in strict concurrency mode)
- invalid use of `OBJC_TASK_SPAWN` / `OBJC_TASK_DETACHED` macro sugar (error/warning parity with equivalent direct spawn call; primary diagnostic on invocation/argument site)
- reserved explicit actor reentrancy-control syntax (`nonreentrant` modifier or `objc_actor_nonreentrant` attribute) used in v1 mode (error; explain deferral)

## 7.11 Open issues <a id="part-7-11"></a>

None currently tracked in this part.

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

<!-- END PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md -->

---

<!-- BEGIN PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md -->

# Part 8 — System Programming Extensions <a id="part-8"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 8.0 Scope and goals <a id="part-8-0"></a>

This part defines language features and attributes designed to accommodate “library-defined subsets” and system APIs, with improved safety, ergonomics, and performance predictability:

- Handle-based resources with strict cleanup requirements (IOKit, Mach).
- C libraries whose objects become ARC-managed under Objective‑C compilation (dispatch, XPC).
- CoreFoundation-style retainable APIs and ownership transfer attributes.
- Borrowed interior-pointer APIs that require explicit lifetime extension.
- Block capture patterns that commonly cause retain cycles (event handlers, callbacks).

This part intentionally provides _detailed normative semantics_ because system-library correctness depends on precise ordering.

## 8.0.1 Canonical attribute spellings (header interoperability) <a id="part-8-0-1"></a>

**Normative note:** The canonical spellings for all attributes and pragmas used in this part are cataloged in **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)**. Inline spellings here are illustrative; emitted interfaces and module metadata shall use the catalog spellings.
Many system-facing features in this part are intended to be expressed in headers and preserved by module interfaces. Per [Decision D-007](#decisions-d-007), a conforming implementation shall support canonical `__attribute__((...))` spellings for these features, so that:

- APIs can be declared without macro folklore,
- analyzers can reason about semantics,
- and mixed ObjC/ObjC++ builds remain source-compatible.

This part uses ergonomic `@resource(...)`/`@cleanup(...)`/`borrowed` spellings in examples; implementations may accept these as ObjC 3.0 sugar. The canonical header spellings are described inline where relevant.

---

## 8.1 Unified scope-exit action model <a id="part-8-1"></a>

The core `defer` scope-exit semantics (registration point, exit kinds, and LIFO execution) are defined in [Part 5](#part-5) [§5.2.2](#part-5-2-2).

This part extends that model: after successful initialization of a variable annotated under [§8.3](#part-8-3), the implementation shall register its cleanup action on the same scope-exit stack.

A _cleanup scope_ is any lexical scope that currently has one or more registered scope-exit actions (from `defer`, `@cleanup`, or `@resource`).

Normative non-local control-flow interaction rules:

- ObjC/C++ exception unwinding that crosses cleanup scopes shall execute each exited scope's registered actions exactly once, in LIFO order, before unwinding continues.
- A `longjmp`/`siglongjmp` transfer that bypasses cleanup scopes does not execute skipped actions; behavior is undefined unless the platform ABI explicitly guarantees cleanup execution for that transfer.
- In `strict` and `strict-system` profiles, if the implementation can prove a `longjmp`/`siglongjmp` may bypass a cleanup scope with pending actions (and no active ABI guarantee applies), the program is ill-formed and a diagnostic is required.

---

## 8.2 `defer` (normative) <a id="part-8-2"></a>

### 8.2.1 Syntax <a id="part-8-2-1"></a>

Canonical `defer` syntax is defined in [Part 5](#part-5) [§5.2.1](#part-5-2-1).

### 8.2.2 Semantics <a id="part-8-2-2"></a>

Canonical `defer` registration semantics are defined in [Part 5](#part-5) [§5.2.2](#part-5-2-2). For system constructs in this part, `defer` participates in the unified scope-exit model in [§8.1](#part-8-1).

### 8.2.3 Ordering with ARC releases <a id="part-8-2-3"></a>

In a scope:

- `defer` actions shall execute **before** implicit ARC releases of strong locals in that scope.

Rationale: ensures cleanup code can safely reference locals without surprising early releases.

### 8.2.4 Restrictions <a id="part-8-2-4"></a>

Canonical `defer` body restrictions are defined in [Part 5](#part-5) [§5.2.3](#part-5-2-3). Non-local transfer behavior when cleanup scopes are crossed is defined in [§8.1](#part-8-1).

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

### 8.3.6 Struct-field resource annotations (v2 capability; rejected in v1) <a id="part-8-3-6"></a>

#### 8.3.6.1 v1 status (normative) <a id="part-8-3-6-1"></a>

Objective‑C 3.0 v1 does not include `objc_resource(...)` / `@resource(...)` on struct/union fields.
In conforming v1 mode, such field annotations are ill-formed and shall be diagnosed.

#### 8.3.6.2 v2 capability and lowering contract (normative for claimants) <a id="part-8-3-6-2"></a>

A future revision may enable struct-field resource annotations via capability `objc3.system.aggregate_resources`.
Implementations claiming that capability shall enforce:

- aggregate initialization proceeds in field declaration order,
- each successfully initialized annotated field is registered for cleanup,
- aggregate destruction (normal scope exit) performs cleanup in reverse declaration order for annotated fields that are initialized and valid,
- if initialization of field `k` fails, previously initialized annotated fields `[0..k-1]` are cleaned exactly once in reverse declaration order before propagating failure,
- lowering preserves the same LIFO/once semantics as the scope-exit model in [§8.1](#part-8-1).

For this capability, unions with annotated resource fields remain out of scope unless and until a future revision defines active-member cleanup semantics.

#### 8.3.6.3 Illustrative cleanup ordering example <a id="part-8-3-6-3"></a>

For:

```c
struct Pair {
  __attribute__((objc_resource(close=CloseA, invalid=0))) Handle a;
  __attribute__((objc_resource(close=CloseB, invalid=0))) Handle b;
};
```

- if `a` initializes and `b` initialization fails, cleanup runs `CloseA(a)` only;
- if both initialize and scope exits normally, cleanup runs `CloseB(b)` then `CloseA(a)`.

---

## 8.4 Retainable C families and ObjC-integrated objects <a id="part-8-4"></a>

### 8.4.1 Motivation <a id="part-8-4-1"></a>

Some system libraries declare their objects as Objective‑C object types when compiling as Objective‑C so they can participate in ARC, blocks, and analyzer tooling. This behavior exists today in dispatch and XPC ecosystems.

Objective‑C 3.0 defines a language-level model so this is not “macro folklore.”

### 8.4.2 Retainable family declaration and canonical header forms <a id="part-8-4-2"></a>

Objective‑C 3.0 finalizes the canonical retainable-family marker as:

- `__attribute__((objc_retainable_family(FamilyName)))` on the family typedef.

Canonical headers shall identify family operations with:

- `__attribute__((objc_family_retain(FamilyName)))` on the retain function declaration,
- `__attribute__((objc_family_release(FamilyName)))` on the release function declaration,
- `__attribute__((objc_family_autorelease(FamilyName)))` on the autorelease function declaration (optional).

For families that are ObjC-integrated under Objective‑C compilation, the typedef shall additionally use `__attribute__((NSObject))` (or a canonical alias with identical semantics).

Illustrative header form:

```c
typedef struct OpaqueFoo *FooRef
  __attribute__((objc_retainable_family(Foo)));

FooRef FooRetain(FooRef x)
  __attribute__((objc_family_retain(Foo)));

void FooRelease(FooRef x)
  __attribute__((objc_family_release(Foo)));
```

Optional sugar (non-normative):

- `@retainable_family(FamilyName, retain: RetainFn, release: ReleaseFn, autorelease: AutoFn?)`.

### 8.4.3 Mapping to existing Clang attributes (normative compatibility) <a id="part-8-4-3"></a>

A conforming implementation shall accept the following widely deployed Clang attributes as compatibility aliases when they represent equivalent semantics:

- `__attribute__((NSObject))` on typedefs for ObjC-integrated retainable family types,
- ownership-transfer attributes on APIs returning/consuming family values:
  - `os_returns_retained` / `os_returns_not_retained` / `os_consumed`,
  - `cf_returns_retained` / `cf_returns_not_retained` / `cf_consumed`,
  - `ns_returns_retained` / `ns_returns_not_retained` / `ns_consumed`.

No existing single Clang attribute fully encodes the family declaration itself; `objc_retainable_family(FamilyName)` is the canonical Objective‑C 3.0 header/interface spelling.

### 8.4.4 ARC and non-ARC behavior (normative) <a id="part-8-4-4"></a>

If a family is ObjC-integrated and ARC is enabled:

- values of that family shall participate in ARC ownership operations as retainable references,
- direct calls to functions annotated `objc_family_retain`, `objc_family_release`, or `objc_family_autorelease` are ill-formed and shall be diagnosed as errors.

If ARC is disabled:

- direct calls to family retain/release/autorelease functions are permitted,
- the implementation shall still preserve retainable-family and transfer annotations for static analysis and interface emission.

### 8.4.5 Analyzer and optimizer integration <a id="part-8-4-5"></a>

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

### 8.7.3 Enforcement model (normative in strict-system) <a id="part-8-7-3"></a>

Strict-system conformance requires a front-end borrowed-pointer escape checker that is:

- flow-sensitive and intra-procedural,
- executed during ordinary semantic analysis for each translation unit,
- capable of diagnosing all required escaping-use cases in [§8.7.4](#part-8-7-4) without requiring an opt-in whole-program analyzer run.

Additional interprocedural/static-analyzer checks are recommended, but they are additive and shall not replace required front-end enforcement.

### 8.7.4 Escaping-use trigger points and minimum diagnostics (normative) <a id="part-8-7-4"></a>

In strict-system mode, a borrowed pointer value is treated as **non-escaping** unless an explicit unsafe escape marker is used.

The implementation shall diagnose, at minimum, the following _escaping uses_ as ill-formed (error in strict-system):

- storing the borrowed pointer into global/static/thread-local storage,
- storing it into heap-resident storage, including Objective‑C ivars/properties or memory reachable from dynamic allocation,
- capturing it by an escaping block (including passing that block to an escaping parameter),
- passing it as an argument to a parameter not proven non-escaping and not declared to preserve borrowed lifetime,
- returning it from a function/method unless the declaration uses `borrowed` return type plus `objc_returns_borrowed(owner_index=N)` naming a valid owner parameter,
- converting it to an untracked representation (`void *`, integer) when the resulting value may outlive the owner.

Each required diagnostic shall identify the escaping operation and provide at least one actionable fix direction:

- extend owner lifetime (`withLifetime` / `keepAlive`),
- copy data to owned storage,
- or mark the operation explicitly unsafe.

### 8.7.5 False-positive avoidance requirements (normative) <a id="part-8-7-5"></a>

In strict-system mode, implementations shall accept the following without borrowed-escape diagnostics:

- uses fully contained in a lexical region dominated by the owner lifetime,
- assignment to automatic local storage proven not to outlive the owner,
- passing to parameters proven non-escaping (`noescape` or equivalent),
- capture by blocks proven non-escaping and invoked synchronously in the same full-expression,
- copying data out of the borrowed region into owned storage when the borrowed pointer itself does not escape.

---

## 8.8 Block capture lists <a id="part-8-8"></a>

### 8.8.1 Syntax <a id="part-8-8-1"></a>

Blocks may include an explicit capture list:

```objc
^[weak self, move handle] { ... }
```

A capture list is a comma-separated list of _capture items_:

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

No open issues are tracked in this part for v1.

<!-- END PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md -->

---

<!-- BEGIN PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md -->

# Part 9 — Performance and Dynamism Controls <a id="part-9"></a>

_Working draft v0.11 — last updated 2026-02-23_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 9.1 Purpose <a id="part-9-1"></a>

Objective‑C’s dynamic dispatch is a strength, but “open world” dynamism can inhibit optimization and complicate reasoning about safety.

Objective‑C 3.0 provides **opt-in** controls that:

- enable direct calls and devirtualization where safe,
- communicate intent to compilers and reviewers,
- preserve dynamic behavior by default,
- remain safe under separate compilation and across module boundaries ([C.2](#c-2)).

This part is designed to accommodate both Cocoa-style dynamic patterns _and_ system/library subsets where predictability and performance matter.

## 9.2 Direct methods <a id="part-9-2"></a>

### 9.2.1 Concept <a id="part-9-2-1"></a>

A _direct method_ is a method that:

- is not dynamically dispatched by selector lookup,
- cannot be overridden,
- and is intended to be invoked via a direct call when statically referenced.

### 9.2.2 Canonical spelling <a id="part-9-2-2"></a>

A direct method is declared with:

```c
__attribute__((objc_direct))
```

applied to an Objective‑C method declaration.

Objective‑C 3.0 v1 also standardizes a class-level default:

```c
__attribute__((objc_direct_members))
```

applied to an Objective‑C class `@interface` / `@implementation`.

### 9.2.3 Semantics (normative) <a id="part-9-2-3"></a>

1. A direct method shall not be overridden in any subclass.
2. A direct method shall not be introduced or replaced by a category with the same selector.
3. A direct method shall not participate in message forwarding semantics (`forwardInvocation:`) as part of selector lookup.

Calling model:

- A direct method call shall be formed only by a _statically-resolved_ method reference (i.e., the compiler must know it targets a direct method declaration).
- Dynamic message sends (e.g., `objc_msgSend`, `performSelector:`) shall not be relied upon to find a direct method.

**Programmer model:** “If you want dynamic, do not mark it direct.”

### 9.2.4 Separate compilation notes (normative) <a id="part-9-2-4"></a>

Because direct methods and class-level direct defaults change call legality and dispatch surfaces, the `objc_direct` and `objc_direct_members` attributes (plus per-member `objc_dynamic` opt-outs) shall be:

- recorded in module metadata (see [D.3.1](#d-3-1) [Table A](#d-3-1)), and
- preserved in emitted interfaces.

Importers shall reconstruct the same **effective directness** for each method declaration.
Effect/attribute mismatches across modules are ill-formed ([C.2](#c-2)).

### 9.2.5 Recommended lowering (informative) <a id="part-9-2-5"></a>

Recommended lowering is described in [C.8](#c-8).
At a high level:

- the compiler emits a callable symbol for each method implementation,
- calls to an effectively-direct method use a direct call rather than `objc_msgSend`, and
- methods explicitly marked `objc_dynamic` remain selector-dispatched.

### 9.2.6 Class-level direct-members semantics (normative) <a id="part-9-2-6"></a>

For a class annotated with `objc_direct_members`:

- each method declared in the class interface/implementation is treated as if annotated `objc_direct`, unless explicitly annotated `objc_dynamic`,
- synthesized property accessors for properties declared in that class are treated the same way,
- category declarations are not implicitly covered by the class-level default.

Legality is identical to explicit `objc_direct`:

- overriding an effectively-direct method is ill-formed,
- introducing/replacing an effectively-direct selector from a category is ill-formed.

## 9.3 Final methods and classes <a id="part-9-3"></a>

### 9.3.1 Concept <a id="part-9-3-1"></a>

A _final_ declaration prohibits overriding/subclassing but does not necessarily remove the declaration from dynamic dispatch.

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

A _sealed_ class prohibits subclassing **outside** the defining module, but may allow subclassing **inside** the module.

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

### 9.5.3 Force-dynamic dispatch attribute (normative) <a id="part-9-5-3"></a>

Objective‑C 3.0 v1 standardizes:

```c
__attribute__((objc_dynamic))
```

applied to Objective‑C method declarations.

Semantics:

- calls that statically reference an `objc_dynamic` method shall use selector-based dynamic dispatch (`objc_msgSend`-family),
- `objc_dynamic` methods participate in forwarding, swizzling, and selector-based reflection,
- `objc_dynamic` is an explicit opt-out from `objc_direct_members` for that method.

Interaction with `direct`/`final`/`sealed`:

- `objc_dynamic` and `objc_direct` on the same method are ill-formed,
- `objc_dynamic` does not relax `objc_final` method/class legality rules,
- `objc_dynamic` does not relax `objc_sealed` cross-module subclassing restrictions.

Example: an `objc_final objc_dynamic` method is dynamically dispatched but still cannot be overridden.

## 9.6 ABI and visibility rules (normative for implementations) <a id="part-9-6"></a>

Implementations shall ensure that direct/final/sealed/dynamic controls remain safe under:

- separate compilation,
- LTO and non-LTO builds,
- and module boundaries.

Minimum requirements:

1. Attributes that affect dispatch legality (`objc_direct`, `objc_direct_members`, `objc_dynamic`) must be recorded in module metadata.
2. Interface emission must preserve the attributes using canonical spellings ([B.7](#b-7)).
3. Calling or redeclaring a method with incompatible assumptions about directness/dynamicness/finality across module boundaries must be diagnosed when possible ([Part 12](#part-12)).

## 9.7 Required diagnostics (minimum) <a id="part-9-7"></a>

- Declaring `objc_direct` on a method that is declared in an Objective‑C protocol and implemented dynamically: error (direct methods are not dispatchable by selector).
- Overriding a `objc_direct` or `objc_final` method: error.
- Overriding a method that is effectively direct via `objc_direct_members`: error.
- Declaring a category method with the same selector as a `objc_direct` method: error.
- Declaring both `objc_direct` and `objc_dynamic` on the same method: error.
- Attempting to subclass an `objc_final` class: error.
- Attempting to subclass an `objc_sealed` class from another module: error.
- In strict performance mode: warn when a direct-call-capable reference is forced through dynamic dispatch (e.g., via `performSelector:`), with note explaining semantics.

## 9.8 Open issues <a id="part-9-8"></a>

- None currently for v1 dispatch controls in this part.

## 9.9 Conformance tests (minimum) <a id="part-9-9"></a>

A conforming implementation’s suite shall include at least:

### 9.9.1 Direct-members legality and cross-module preservation <a id="part-9-9-1"></a>

- `PERF-DIRMEM-01` (same-module override legality): subclass override of a method made direct by `objc_direct_members` is diagnosed as ill-formed.
- `PERF-DIRMEM-02` (same-module opt-out): a method explicitly marked `objc_dynamic` inside an `objc_direct_members` class is not treated as direct; override legality follows normal plus `objc_final` rules.
- `PERF-DIRMEM-03` (cross-module override legality): module A exports an `objc_direct_members` class; module B attempts to override an effectively-direct member and receives an error.
- `PERF-DIRMEM-04` (cross-module round-trip): emitted interface/import round-trip preserves effective directness for members declared under `objc_direct_members`, including `objc_dynamic` opt-outs.

### 9.9.2 Force-dynamic dispatch behavior <a id="part-9-9-2"></a>

- `PERF-DYN-01` (dispatch path): calls to `objc_dynamic` methods lower through dynamic selector dispatch rather than direct-call lowering.
- `PERF-DYN-02` (mixed class behavior): in an `objc_direct_members` class, unannotated members lower as direct while `objc_dynamic` members remain dynamic.
- `PERF-DYN-03` (attribute conflict): `objc_direct` + `objc_dynamic` on the same method is rejected.
- `PERF-DYN-04` (final/sealed interaction): `objc_final objc_dynamic` methods remain non-overridable but dynamically dispatched; `objc_sealed` still rejects out-of-module subclassing regardless of method dispatch mode.
<!-- END PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md -->

---

<!-- BEGIN PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md -->

# Part 10 — Metaprogramming, Derives, Macros, and Property Behaviors <a id="part-10"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

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

### 10.2.4 Derive tiers and conformance requirements (normative) <a id="part-10-2-4"></a>

Objective-C 3.0 v1 defines derive support as tiered:

- **Core profile**: no individual derive is required for Core conformance.
- **Optional metaprogramming profile (`OPT-META`)**: derive support is required and shall expose a minimum portable set.

The minimum portable derive set for `OPT-META` claims is:

- `Equality`
- `Hash`
- `DebugDescription`
- `Codable`

Implementations may provide additional derives, but they shall not redefine the semantics of the portable set above.

### 10.2.5 Optional derive feature macros and metadata (normative) <a id="part-10-2-5"></a>

When `OPT-META` is supported, implementations shall expose feature macros for portable derives:

- `__OBJC3_FEATURE_DERIVE_EQUALITY__`
- `__OBJC3_FEATURE_DERIVE_HASH__`
- `__OBJC3_FEATURE_DERIVE_DEBUGDESCRIPTION__`
- `__OBJC3_FEATURE_DERIVE_CODABLE__`

If a toolchain claims support for a derive in source mode, the same derive capability shall be representable in module metadata and textual interfaces so downstream builds can validate availability under separate compilation ([Part 2](#part-2), [D](#d)).

## 10.3 AST macros <a id="part-10-3"></a>

### 10.3.1 Concept <a id="part-10-3-1"></a>

AST macros are compile-time programs that transform syntax/AST into expanded declarations or expressions.

### 10.3.2 Canonical marker attribute <a id="part-10-3-2"></a>

Macro entry points may be annotated with:

```c
__attribute__((objc_macro))
```

Macro entry points and derive implementations shall be distributed in a portable package model defined by [§10.3.6](#part-10-3-6).

### 10.3.3 Safety constraints (normative) <a id="part-10-3-3"></a>

A conforming implementation shall provide enforcement such that macros used in trusted builds are:

- deterministic (no nondeterministic inputs),
- sandboxable (no arbitrary filesystem/network unless explicitly permitted by the build system),
- and bounded (resource limits to keep builds predictable).

### 10.3.4 Expansion phases (normative) <a id="part-10-3-4"></a>

Macro expansion (and derive expansion) that introduces declarations affecting layout/ABI shall occur before:

- type layout is finalized, and
- code generation.

This ensures ABI is computed from the _expanded_ program ([C.9](#c-9)).

### 10.3.5 Interface emission (normative) <a id="part-10-3-5"></a>

If macro expansion introduces exported declarations, the emitted interface shall include the expanded declarations (or an equivalent representation that reconstructs them), not merely the macro invocation.

### 10.3.6 Macro/derive packaging and discovery model (normative) <a id="part-10-3-6"></a>

Objective-C 3.0 v1 standardizes a build-system supplied packaging model for macros/derives:

- A macro/derive package shall include a machine-readable manifest file named `objc3-meta.json`.
- The manifest shall include:
  - package identifier,
  - package semantic version,
  - deterministic content fingerprint,
  - declared macro/derive entry points,
  - declared required capabilities/toolchain revision constraints.
- Toolchains shall accept explicit package registration inputs equivalent to:
  - `-fobjc3-meta-package=<path>` (repeatable), and
  - a lockfile input equivalent to `-fobjc3-meta-lock=<path>`.

Discovery/load requirements:

- Package resolution and load order shall be deterministic.
- Builds shall not rely on ambient global package search paths for conformance claims; the package set must be explicit in build inputs.
- If a requested macro/derive cannot be resolved from declared packages, the compile is ill-formed and a diagnostic is required.

Reproducibility requirements:

- With identical source, package manifests, and lockfile, expansion results shall be byte-for-byte reproducible in emitted interface/module metadata.
- Implementations may support additional discovery mechanisms, but conforming behavior shall remain representable by the standardized explicit-package model above.

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

### 10.4.5 Property-behavior surface syntax decision (normative) <a id="part-10-4-5"></a>

Objective-C 3.0 v1 standardizes **attribute-based behavior declaration/application only**.
No additional behavior-block surface syntax is part of v1.

Future-compat reservation:

- `behavior` is reserved as a contextual keyword in property-behavior positions for possible v2 syntax.
- Forms equivalent to `behavior { ... }` are reserved and shall be rejected in v1 mode with a diagnostic indicating the syntax is reserved for future revisions.
- Implementations may provide experimental syntax only behind non-conforming extension flags and shall not emit such syntax in canonical interfaces.

## 10.5 Required diagnostics (minimum) <a id="part-10-5"></a>

- Derive request cannot be satisfied: error with notes listing missing members/protocol requirements.
- Macro expansion introduces exported declarations but cannot be represented in emitted interface: error (or emit-only warning in permissive mode).
- Property behavior composition conflict: error with explanation of conflict.
- Requested derive capability is not available in the active package set/profile claim: error with note naming required derive macro/capability.
- Unresolvable macro/derive package manifest or lockfile mismatch under conformance mode: error.
- Reserved property-behavior surface syntax used in v1 mode: error with note indicating attribute-based form.

## 10.6 Conformance tests (minimum) <a id="part-10-6"></a>

Conforming suites shall include at least:

- `META-PKG-01`: package discovery via explicit `-fobjc3-meta-package` inputs is deterministic and independent of ambient search paths.
- `META-PKG-02`: lockfile mismatch or missing package produces required diagnostic and blocks expansion.
- `META-PKG-03`: identical source + manifests + lockfile yields reproducible expansion outputs in interfaces/metadata.
- `META-DRV-01`: Core claim passes without requiring individual derives.
- `META-DRV-02`: `OPT-META` claim requires and validates `Equality`, `Hash`, `DebugDescription`, and `Codable` derive availability.
- `META-DRV-03`: missing optional derive capability while source requests it yields required diagnostic with derive capability ID/macro guidance.
- `META-SYN-01`: attribute-based behavior form parses and lowers as specified.
- `META-SYN-02`: reserved behavior-block syntax in v1 mode is rejected with required reserved-syntax diagnostic.

## 10.7 Open issues <a id="part-10-7"></a>

No open issues are tracked in this part for v0.11.

Resolved reference: macro/derive extension governance process
(`spec/governance/MACRO_DERIVE_EXTENSION_GOVERNANCE.md`).

<!-- END PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md -->

---

<!-- BEGIN PART_11_INTEROPERABILITY_C_CPP_SWIFT.md -->

# Part 11 — Interoperability: C, C++, and Swift <a id="part-11"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

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

### 11.2.3 Exporting `async` to C (normative for implementations) <a id="part-11-2-3"></a>

When an `async` function is made visible to C, a conforming implementation shall expose a completion-handler thunk using the canonical ABI shape in this section.

For a source declaration:

```objc
R f(A1 a1, A2 a2) async throws;
```

the exported C-callable thunk shall be equivalent to:

```c
typedef enum objc_async_completion_status {
    OBJC_ASYNC_COMPLETION_SUCCESS = 0,
    OBJC_ASYNC_COMPLETION_ERROR = 1,
    OBJC_ASYNC_COMPLETION_CANCELLED = 2,
} objc_async_completion_status_t;

typedef void (*f_completion_t)(
    void * _Nullable context,
    objc_async_completion_status_t status,
    R result,
    id<Error> _Nullable error);

void f_async_c(
    A1 a1,
    A2 a2,
    void * _Nullable context,
    f_completion_t _Nonnull completion);
```

For `R == void`, the completion callback shall omit the `result` parameter.

The canonical parameter order is:

1. original source parameters (`A1`, `A2`, ...),
2. trailing `void * _Nullable context`,
3. trailing nonnull completion callback.

Completion delivery rules:

- The completion callback shall be invoked exactly once for each thunk invocation.
- `completion == NULL` is a contract violation; behavior is undefined unless a platform profile defines a checked trap.
- Completion may occur after `f_async_c` returns; callers shall not assume synchronous callback delivery.

Error and cancellation representation at the C boundary:

- On success: `status == OBJC_ASYNC_COMPLETION_SUCCESS`, `error == nil`, and `result` contains the function result.
- On thrown failure: `status == OBJC_ASYNC_COMPLETION_ERROR`, `error != nil`, and `result` is unspecified and shall be ignored by C callers.
- On cancellation: `status == OBJC_ASYNC_COMPLETION_CANCELLED`, `error != nil`, and `error` shall be the distinguished cancellation representation required by [Part 7](#part-7) [§7.6.4](#part-7-6-4). `result` is unspecified and shall be ignored by C callers.
- For declarations that are `async` but not `throws`, `OBJC_ASYNC_COMPLETION_ERROR` shall not be produced.

Ownership and lifetime rules:

- `context` is caller-owned opaque state. The implementation shall pass it through unchanged to `completion` and shall not free or retain it.
- The caller shall keep `context` valid until `completion` returns.
- After `completion` returns, the implementation shall not read from or write to `context`.
- Objective‑C object/block values delivered as `result` (when applicable) or `error` are passed at +0 ownership. If C/ObjC caller code needs them beyond callback return, it shall retain/copy them before returning from `completion`.
- The implementation shall keep delivered values alive for the dynamic extent of `completion`.

The thunk ABI shall be stable under separate compilation and recordable in module metadata ([C.2](#c-2), [C.5](#c-5)).

### 11.2.4 Required conformance tests for C callers of exported `async` APIs <a id="part-11-2-4"></a>

A conforming implementation shall ship tests where a C translation unit invokes exported async thunks and validates all of:

- Separate-compilation ABI stability: export the thunk in one module/translation unit, call it from a separate C translation unit, and link successfully.
- Canonical signature shape: generated declarations include the trailing `context` parameter and trailing completion callback in the required order.
- `void`-result shape: for exported `async` APIs with `R == void`, the callback signature omits `result` and remains callable from C.
- Exactly-once completion: each invocation yields one and only one callback on success, thrown error, and cancellation paths.
- Context round-trip identity: callback observes the same `context` pointer bit-pattern passed by the caller.
- Status/error invariants: success sets `error == nil`; failure and cancellation set `error != nil`; callers ignore `result` unless status is success.
- Cancellation mapping: cancellation is reported with `OBJC_ASYNC_COMPLETION_CANCELLED` and a distinguished cancellation error object as required by [Part 7](#part-7).
- Lifetime transfer contract: values retained/copied inside completion remain valid after completion returns, while unretained +0 values are not required to outlive callback scope.

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

None currently for the v1 portability baseline in this part.

<!-- END PART_11_INTEROPERABILITY_C_CPP_SWIFT.md -->

---

<!-- BEGIN PART_12_DIAGNOSTICS_TOOLING_TESTS.md -->

# Part 12 — Diagnostics, Tooling, and Test Suites <a id="part-12"></a>

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

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

For v1, strict concurrency checking is an orthogonal sub-mode selected independently from strictness level ([Part 1](#part-1) [§1.6](#part-1-6)).

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

A conforming implementation shall provide diagnostics (at least warnings by default; errors where required by selected strictness/profile) for:

- **Non-local control flow across cleanup scopes** ([Part 5](#part-5) [§5.2.3](#part-5-2-3), [§5.2.5](#part-5-2-5); [Part 8](#part-8) [§8.1](#part-8-1), [§8.2.4](#part-8-2-4)):
  - non-local exits inside `defer` bodies (`return`, `break`, `continue`, `goto`, ObjC/C++ `throw`, `longjmp`, `siglongjmp`) shall be diagnosed (these are ill-formed; in `strict` and `strict-system` profiles the diagnostic shall be an error),
  - a provable `longjmp`/`siglongjmp` that bypasses an active cleanup scope with pending actions shall be diagnosed as an error in `strict` and `strict-system` profiles.

- **Resource cleanup correctness** ([Part 8](#part-8) [§8.3](#part-8-3)):
  - missing required cleanup for `objc_resource(...)` or `@resource(...)` declarations (if required by the selected profile),
  - in v1 mode, use of `objc_resource(...)` / `@resource(...)` on struct/union fields is diagnosed as unsupported unless an aggregate-resource capability is explicitly enabled,
  - double-close patterns detectable intra-procedurally,
  - use-after-move / use-after-discard for moved-from resources (where move checking is enabled).

- **Borrowed pointer escaping** ([Part 8](#part-8) [§8.7](#part-8-7)):
  - returning a `borrowed` pointer without the required `objc_returns_borrowed(owner_index=...)` marker (or with an invalid owner index),
  - storing a borrowed pointer into longer-lived storage (global/static/thread-local, ivar/property, heap) in strict-system profiles,
  - passing a borrowed pointer to a parameter not proven non-escaping in strict-system profiles,
  - capturing a borrowed pointer in an escaping block in strict-system profiles,
  - converting a borrowed pointer to `void *`/integer when it can escape owner lifetime in strict-system profiles.

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

### 12.4.4 Conformance report emission and validation (normative) <a id="part-12-4-4"></a>

A conforming implementation shall provide a machine-readable conformance report emission surface equivalent to:

- `--emit-objc3-conformance=<path>`
- `--emit-objc3-conformance-format=json|yaml`
- `--validate-objc3-conformance=<path>`

Compatibility notes:

- JSON output is required.
- YAML output is optional; if provided, it shall be semantically equivalent to the JSON model in [§12.4.5](#part-12-4-5).
- Implementations may provide different option spellings, but shall document the mapping to this canonical surface.
- Validation failure shall return a non-zero exit status and a stable diagnostic code.

### 12.4.5 Conformance report schema v1 (normative) <a id="part-12-4-5"></a>

The canonical v1 report data model is:

```json
{
  "schema_id": "objc3-conformance-report/v1",
  "generated_at": "2026-01-31T15:04:05Z",
  "toolchain": {
    "name": "string",
    "vendor": "string",
    "version": "string",
    "target_triple": "string"
  },
  "language": {
    "language_family": "objective-c",
    "language_version": "3.0",
    "spec_revision": "v1"
  },
  "mode": {
    "strictness": "permissive|strict|strict-system",
    "concurrency": "off|strict"
  },
  "profiles": [
    {
      "id": "core|strict|strict-concurrency|strict-system",
      "status": "claimed|passed|failed|not-claimed",
      "evidence": ["string"]
    }
  ],
  "optional_features": [
    {
      "id": "string",
      "status": "claimed|not-claimed|experimental",
      "version": "string",
      "notes": "string"
    }
  ],
  "versions": {
    "frontend": "string",
    "runtime": "string",
    "stdlib": "string",
    "module_format": "string"
  },
  "known_deviations": [
    {
      "id": "string",
      "summary": "string",
      "status": "open|accepted|waived|fixed-pending-release",
      "affects": ["string"],
      "tracking": "string"
    }
  ]
}
```

Conformance requirements for this schema:

- All top-level keys shown above shall be present.
- `mode.strictness` and `mode.concurrency` shall match the effective command-line mode selection used for the conformance run.
- `profiles`, `optional_features`, and `known_deviations` shall be arrays (empty arrays are permitted).
- `versions` shall list concrete shipped component versions; `"unknown"` is not conforming for a released claim.
- Each known deviation shall have a stable `id` and a non-empty `summary`.

## 12.5 Conformance test suite (minimum expectations) <a id="part-12-5"></a>

A conforming implementation shall ship or publish a test suite that covers at least:

### 12.5.0 Required repository structure (normative) <a id="part-12-5-0"></a>

The conformance suite shall include at least these buckets (directory names may vary if mapping is documented):

- `tests/conformance/parser/`
- `tests/conformance/semantic/`
- `tests/conformance/lowering_abi/`
- `tests/conformance/module_roundtrip/`
- `tests/conformance/diagnostics/`

Bucket responsibilities:

- parser: grammar/disambiguation and syntax acceptance/rejection.
- semantic: typing/effects/isolation/nullability behavior.
- lowering_abi: ABI/lowering/runtime-contract behavior.
- module_roundtrip: emit/import/metadata preservation tests.
- diagnostics: required diagnostics and fix-it verification.

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
- Calling executor-annotated or actor-isolated _synchronous_ members from outside requires `await` and schedules onto the correct executor ([D-011](#decisions-d-011)).

### 12.5.5 Non-local exits and cleanup scopes <a id="part-12-5-5"></a>

- **Negative (strict/strict-system):** compile-fail tests where a `defer` body contains each forbidden non-local exit form (`return`, `break`, `continue`, `goto`, ObjC `@throw`, C++ `throw`, `longjmp`, `siglongjmp`).
- **Negative (strict/strict-system):** compile-fail tests with statically provable `longjmp`/`siglongjmp` transfers that bypass pending cleanup actions from `defer`, `@cleanup`, or `@resource`.
- **Conformance runtime (ObjC++):** C++ exception unwind across nested cleanup scopes executes scope-exit actions exactly once per registration, in LIFO order, before scope-final ARC releases.
- **Conformance runtime (ObjC EH, where supported):** Objective‑C exception unwind follows the same cleanup ordering/once semantics as C++ unwind.
- **Profile/ABI guard:** no test may require defined behavior for `longjmp`/`siglongjmp` bypass of cleanup scopes unless the implementation explicitly documents an ABI guarantee for cleanup execution on that transfer.

### 12.5.6 Profile coverage minima (normative minimum) <a id="part-12-5-6"></a>

At minimum, the published suite shall include:

- **Core**
  - parser: 15 tests,
  - semantic: 25 tests,
  - lowering/ABI: 10 tests,
  - module round-trip: 12 tests,
  - diagnostics: 20 tests.
- **Strict**
  - at least 10 additional strict-only diagnostics/fix-it tests beyond Core.
- **Strict Concurrency**
  - at least 12 additional isolation/Sendable/executor-boundary tests.
- **Strict System**
  - at least 12 additional borrowed/resource/capture-list tests.

Counts are minimums; implementations may exceed them.

### 12.5.7 Portable diagnostic assertion format (normative) <a id="part-12-5-7"></a>

Conformance diagnostics shall be assertable in a toolchain-portable structure with at least:

- stable diagnostic `code`,
- `severity` (`error`/`warning`/`note`),
- source `span` (line/column range),
- required `fixits` when the spec requires mechanical fix-it support.

Native harness syntax is permitted, but the implementation shall be able to emit this canonical structure for conformance reporting.

### 12.5.8 Language-version selection acceptance/rejection tests (normative minimum) <a id="part-12-5-8"></a>

The conformance suite shall include explicit tests for Objective‑C 3.0 v1 translation-unit-only language-version selection:

- TUV-01 (accept): command line `-fobjc-version=3` applies to the entire translation unit.
- TUV-02 (accept): `#pragma objc_language_version(3)` at file scope before declarations applies to the entire translation unit.
- TUV-03 (reject): per-region language-version switching forms (for example `push`/`pop`, begin/end, or equivalent) are rejected.
- TUV-04 (reject): a version-selection pragma inside a function/body scope is rejected.
- TUV-05 (reject): a version-selection pragma that conflicts with command-line mode selection is rejected.

For reject cases, the suite shall assert stable diagnostic code/severity/span metadata per [§12.5.7](#part-12-5-7).

### 12.5.9 Conformance-report schema/content tests (normative minimum) <a id="part-12-5-9"></a>

The conformance suite shall include machine-readable report tests:

- CRPT-01 (emit-json): `--emit-objc3-conformance=<path>` produces JSON that validates against [§12.4.5](#part-12-4-5).
- CRPT-02 (required-keys): emitted report includes `profiles`, `optional_features`, `versions`, and `known_deviations`.
- CRPT-03 (yaml-equivalence): when YAML emission is supported, JSON and YAML emissions for the same build parse to equivalent data.
- CRPT-04 (reject-missing-required): `--validate-objc3-conformance=<path>` rejects a report missing any required top-level key.
- CRPT-05 (reject-invalid-enum): validator rejects invalid `profiles[*].id` or `profiles[*].status` values.
- CRPT-06 (known-deviation-shape): each `known_deviations` entry is validated for required `id`, `summary`, and `status` fields.

### 12.5.10 Strictness/sub-mode consistency tests (normative minimum) <a id="part-12-5-10"></a>

The conformance suite shall include matrix tests proving consistent behavior between strictness and strict concurrency sub-mode:

- SCM-01: `-fobjc3-strictness=strict -fobjc3-concurrency=off` keeps strict non-concurrency diagnostics as errors and allows concurrency-only diagnostics to follow non-strict-concurrency policy.
- SCM-02: `-fobjc3-strictness=strict -fobjc3-concurrency=strict` upgrades required concurrency diagnostics to strict-concurrency behavior.
- SCM-03: `-fobjc3-strictness=strict-system -fobjc3-concurrency=off` keeps strict-system diagnostics active.
- SCM-04: `-fobjc3-strictness=strict-system -fobjc3-concurrency=strict` enforces both strict-system and strict-concurrency requirements.
- SCM-05: preprocessing tests verify `__OBJC3_STRICTNESS_LEVEL__`, `__OBJC3_CONCURRENCY_MODE__`, and `__OBJC3_CONCURRENCY_STRICT__` values for each matrix point.
- SCM-06: conformance report `mode` fields and claimed `profiles[*].id` values are consistent with the mode-selection matrix.

### 12.5.11 Retainable-family ARC/non-ARC tests (normative minimum) <a id="part-12-5-11"></a>

For [Part 8](#part-8) [§8.4](#part-8-4), the conformance suite shall include:

- RF-ARC-01 (negative): with ARC enabled, direct calls to functions annotated `objc_family_retain(...)`, `objc_family_release(...)`, or `objc_family_autorelease(...)` for an ObjC-integrated family are diagnosed as errors.
- RF-ARC-02 (positive): with ARC enabled, transfer-annotated family APIs type-check and lower without requiring explicit manual retain/release calls.
- RF-MRR-01 (positive): with ARC disabled, balanced explicit calls to family retain/release functions are accepted.
- RF-MRR-02 (negative/diagnostic): with ARC disabled and strict-system enabled, statically provable intra-procedural over-release or leak patterns on retainable-family values are diagnosed.
- RF-MOD-01 (round-trip): emitted/imported interfaces preserve `objc_retainable_family(...)`, `objc_family_*`, and ObjC-integration markers with semantics unchanged.

### 12.5.12 Borrowed-pointer strict-system tests (normative minimum) <a id="part-12-5-12"></a>

For [Part 8](#part-8) [§8.7](#part-8-7), the conformance suite shall include:

- BRW-NEG-01: storing a borrowed pointer into global/static/thread-local storage is a strict-system error.
- BRW-NEG-02: storing a borrowed pointer into heap/ivar/property storage is a strict-system error.
- BRW-NEG-03: capturing a borrowed pointer into an escaping block is a strict-system error.
- BRW-NEG-04: passing a borrowed pointer to a parameter not proven non-escaping is a strict-system error.
- BRW-NEG-05: returning borrowed storage without valid `objc_returns_borrowed(owner_index=...)` is a strict-system error.
- BRW-POS-01: passing borrowed pointers to parameters explicitly marked non-escaping is accepted without escape diagnostics.
- BRW-POS-02: capturing borrowed pointers in blocks proven non-escaping and synchronously invoked is accepted without escape diagnostics.
- BRW-POS-03: copying from a borrowed region into owned storage (without pointer escape) is accepted.
- BRW-POS-04: temporary local stack assignments that do not outlive owner lifetime are accepted.

### 12.5.13 Aggregate-resource initialization/cleanup tests (normative minimum for capability claimants) <a id="part-12-5-13"></a>

For implementations claiming [Part 8](#part-8) [§8.3.6](#part-8-3-6) capability `objc3.system.aggregate_resources`, the conformance suite shall include:

- AGR-NEG-01 (v1 gate): in Objective‑C 3.0 v1 mode without the capability claim, `objc_resource(...)` on struct/union fields is rejected with a stable diagnostic.
- AGR-RT-01 (partial init failure): when field `k` initialization fails, already-initialized annotated fields `[0..k-1]` are cleaned exactly once in reverse declaration order.
- AGR-RT-02 (multi-field failure order): for three or more annotated fields, injected failure at each position verifies reverse-order partial cleanup of the initialized prefix.
- AGR-RT-03 (normal destruction order): when all annotated fields initialize successfully, scope exit cleanup runs in reverse declaration order.

## 12.6 Debuggability requirements (minimum) <a id="part-12-6"></a>

For features like macros and async:

- stack traces must preserve source locations,
- debugging tools must be able to map generated code back to user code (macro expansion mapping),
- async tasks should be nameable/inspectable at least in debug builds.

## 12.7 Open issues <a id="part-12-7"></a>

No open issues are tracked in this part for v1.

<!-- END PART_12_DIAGNOSTICS_TOOLING_TESTS.md -->
