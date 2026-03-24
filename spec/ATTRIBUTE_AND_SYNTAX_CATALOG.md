# Objective‑C 3.0 — Attribute and Syntax Catalog {#b}

_Working draft v0.11 — last updated 2026-02-23_

## B.0 Purpose {#b-0}

Objective‑C 3.0 introduces new language **effects**, **annotations**, and a small amount of **surface syntax**.
To keep separate compilation reliable and to keep module interfaces stable, this document defines the **canonical spellings** that:

- all conforming implementations shall accept; and
- any interface-generation tool shall emit.

Implementations may accept additional “sugar” spellings (keywords, pragmas, macros), but those spellings are **not** required for conformance unless explicitly stated elsewhere.

## B.1 Canonical vs. optional spellings {#b-1}

### B.1.1 Canonical spellings {#b-1-1}

The canonical spellings use forms already widely supported by LLVM/Clang-family toolchains:

- C/ObjC pragmas: `#pragma ...`
- GNU/Clang attributes: `__attribute__((...))`
- Existing Objective‑C keywords where the feature is inherently grammatical (`async`, `await`, `try`, `throw`, etc.)

### B.1.2 Optional sugar spellings {#b-1-2}

Implementations may additionally provide sugar spellings such as:

- `@`-directives (e.g., `@assume_nonnull_begin`) as aliases for pragmas.
- Framework macros (e.g., `NS_ASSUME_NONNULL_BEGIN`) as aliases for pragmas.
- Alternative attribute syntaxes (e.g., C++11 `[[...]]`) when compiling as ObjC++.

Such sugar spellings shall not change semantics.

## B.2 Nullability defaults {#b-2}

### B.2.1 Nonnull-by-default region pragmas (canonical) {#b-2-1}

Objective‑C 3.0 defines a nonnull-by-default region using:

```c
#pragma objc assume_nonnull begin
// declarations
#pragma objc assume_nonnull end
```

Semantics are defined in [Part 3](#part-3) ([§3.2.4](#part-3-2-4)).

### B.2.2 Optional aliases (non-normative) {#b-2-2}

Implementations may treat the following as aliases with identical semantics:

- `#pragma clang assume_nonnull begin/end`
- `NS_ASSUME_NONNULL_BEGIN/NS_ASSUME_NONNULL_END` (macro-based)

### B.2.2.1 Current Part 3 type-surface boundary (implementation note) {#b-2-2-1}

Current implementation status (`M265-C001`):

- protocol `@required` / `@optional` partitions are live in the frontend
- object-pointer nullability and pragmatic generic suffix carriers are live in
  parameter, return, and property type parsing
- optional binding forms `if let`, `if var`, `guard let`, and `guard var` are
  now sema-validated against admitted nullable ObjC-reference sources and
  refine those bindings to the nonnull path
- optional sends written as `[receiver? selector]` now lower natively with
  single-evaluation nil short-circuit behavior and still fail closed for
  non-ObjC-reference receivers
- ordinary sends now fail closed for nullable receivers unless the receiver has
  been proven nonnull or optional-send syntax is used
- nil-coalescing `??` is admitted as a parser-owned source form and now lowers
  as a real short-circuit path
- `guard let` / `guard var` `else` blocks now fail closed unless they exit the
  current scope
- typed key-path literals such as `@keypath(...)` are admitted as parser-owned
  source forms and the validated single-component subset now lowers on the
  native path into retained descriptor handles
- the current runtime/helper contract for that subset keeps optional sends on
  the public lookup/dispatch path, feeds validated single-component key-path
  handles into a private runtime registry/helper surface, and keeps unsupported
  key-path shapes compile-time fail-closed until later runtime work lands
- class-root key paths such as `@keypath(Person, name)` now fail closed unless
  the named component is a readable property on the root type
- generic Objective-C method declarations written as `- <T> ...` remain
  reserved for a future revision and now diagnose explicitly
- optional-member access `?.` now lowers natively through the optional-send
  nil-short-circuit path

### B.2.3 ARC source-surface and current mode boundary (implementation note) {#b-2-3}

Current implementation status (`M262-A001`):

- ownership qualifiers, weak/unowned property metadata, `@autoreleasepool`, and
  ARC fix-it/diagnostic surfaces are preserved by the frontend and semantic
  pipeline
- the native driver does not yet expose a runnable `-fobjc-arc` mode
- executable function/method ownership qualifiers are still rejected with the
  current unsupported-feature gate until later ARC automation issues land

### B.2.4 Explicit ARC mode handling for executable ownership surfaces (implementation note) {#b-2-4}

Current implementation status (`M262-A002`):

- the native driver now accepts `-fobjc-arc` and `-fno-objc-arc`
- ownership-qualified executable methods and functions are admitted under
  explicit ARC mode
- ownership-qualified property surfaces and block captures compile under
  explicit ARC mode
- non-ARC mode still rejects the same executable ownership-qualified
  method/function signatures with `O3S221`
- generalized ARC cleanup synthesis and full ARC lifetime automation are still
  deferred to later `M262` issues

### B.2.5 ARC semantic rules and forbidden forms (implementation note) {#b-2-5}

Current implementation status (`M262-B001`):

- explicit ARC mode does not yet imply generalized ARC inference
- conflicting property ownership forms still fail with deterministic diagnostics
- atomic ownership-aware properties still fail with deterministic diagnostics
- broader method-family ARC semantics remain deferred
- `M262-C003` now covers the supported cleanup-emission, weak current-property,
  and block-capture lifetime-extension lowering slice for explicit ARC mode
- `M262-C004` now covers the supported escaping-block plus autoreleasing-return
  lowering slice where branch-local cleanup and ARC return conventions must
  compose without manual ownership management

### B.2.6 ARC inference and lifetime-extension semantics (implementation note) {#b-2-6}

Current implementation status (`M262-B002`):

- under `-fobjc-arc`, unqualified object parameters and returns now infer a
  strong-owned retain/release profile for the supported runnable slice
- under `-fobjc-arc`, unqualified object property surfaces now infer a
  strong-owned lifetime profile in semantic integration
- the same source remains a zero-inference baseline without ARC mode
- full ARC cleanup synthesis and broader ARC interaction surfaces remain
  deferred to later `M262` issues

### B.2.7 ARC interaction semantics (implementation note) {#b-2-7}

Current implementation status (`M262-B003`):

- under `-fobjc-arc`, attribute-only strong properties now publish strong-owned
  lifetime and synthesized-accessor ownership packets
- under `-fobjc-arc`, attribute-only weak properties now publish weak lifetime
  and synthesized-accessor ownership packets
- explicit `__autoreleasing` returns remain profiled through autorelease
  insertion accounting
- owned vs non-owning block captures remain distinguishable in ARC retain/release
  accounting
- generalized ARC cleanup and broader ARC automation still remain deferred

### B.2.8 ARC lowering ABI and cleanup boundary (implementation note) {#b-2-8}

Current implementation status (`M262-C001`):

- lane C now freezes one truthful ARC lowering boundary instead of inferring it
  from older semantic packets
- the frozen boundary is the combination of:
  - ARC semantic packets from `M262-A002`, `M262-B001`, `M262-B002`, and
    `M262-B003`
  - unwind-cleanup accounting already carried by the lowering handoff
  - the current private helper entrypoints for retain/release, autorelease,
    weak property interaction, block helper ownership, and autoreleasepool
    scope
- this freeze does not yet claim general ARC cleanup insertion,
  autorelease-return rewrite automation, or public ARC runtime ABI exposure

### B.2.9 ARC automatic insertion boundary (implementation note) {#b-2-9}

Current implementation status (`M262-C002`):

- lane C now consumes the ARC semantic insertion packets and emits real helper calls for the supported runnable slice
- the supported live insertion surface currently covers:
  - retain-on-entry for ARC-owned runnable parameters
  - release-on-exit for tracked ARC-owned storage
  - autorelease-return lowering for supported autoreleasing returns
  - branch-local cleanup and ARC return conventions must compose without manual ownership management
- emitted IR now carries:
  - `; arc_automatic_insertions = ...`
  - `!objc3.objc_arc_automatic_insertions = !{...}`
- this implementation does not yet claim a generalized local cleanup stack,
  exception-cleanup widening, or cross-module ARC optimization

### B.2.10 Runtime ARC helper API surface (implementation note) {#b-2-10}

Current implementation status (`M262-D001`):

- lane D now freezes one truthful runtime/helper contract:
  - `objc3c-runtime-arc-helper-api-surface-freeze/m262-d001-v1`
- private ARC helper entrypoints and autoreleasepool hooks remain internal runtime ABI
- private ARC helper entrypoints and autoreleasepool hooks remain internal
  runtime ABI
- the public runtime header still remains registration, lookup, dispatch, and
  testing snapshots only
- the frozen private helper surface covers retain/release/autorelease,
  current-property access, weak-property access, and private autoreleasepool
  push/pop hooks
- emitted IR now carries:
  - `; runtime_arc_helper_api_surface = ...`
  - `!objc3.objc_runtime_arc_helper_api_surface = !{...}`
- this freeze does not yet claim a public ARC runtime helper ABI or any
  user-facing ARC runtime header widening

Current implementation status (`M262-D002`):

- lane D now proves one truthful runtime-support contract:
  - `objc3c-runtime-arc-helper-runtime-support/m262-d002-v1`
- private ARC helper entrypoints remain internal runtime ABI, but they are now
  validated as linked native ARC programs rather than freeze-only markers
- the supported runtime proof covers:
  - ARC-generated weak current-property helper lowering
  - ARC-generated autorelease-return execution through the runtime library
- emitted IR now carries:
  - `; runtime_arc_helper_runtime_support = ...`
  - `!objc3.objc_runtime_arc_helper_runtime_support = !{...}`
- `M262-D003` now layers private ARC debug counters and last-value/property
  context snapshots above that same supported runtime slice without changing
  the source syntax surface
- `M262-E001` freezes the runnable ARC gate above the current
  `A002/B003/C004/D003` proof chain and still does not change source syntax
- `M262-E002` closes only the currently supported ARC slice through integrated
  smoke rows, the private `M262-D003` property/runtime probe row, and the
  operator runbook; it does not widen ARC syntax or semantics

## B.3 Concurrency and executors {#b-3}

### B.3.1 Executor affinity annotation (canonical) {#b-3-1}

Executor affinity is expressed with:

```c
__attribute__((objc_executor(main)))
__attribute__((objc_executor(global)))
__attribute__((objc_executor(named("com.example.myexecutor"))))
```

Semantics are defined in [Part 7](#part-7) ([§7.4](#part-7-4)).

### B.3.2 Task-spawn recognition attributes (canonical) {#b-3-2}

The compiler recognizes standard-library task entry points via attributes:

```c
__attribute__((objc_task_spawn))
__attribute__((objc_task_detached))
__attribute__((objc_task_group))
```

Semantics are defined in [Part 7](#part-7) ([§7.5](#part-7-5)).

> Note: These attributes are intended to attach to **functions/methods** that take an `async` block/callback and create tasks/groups, not to arbitrary user functions.

### B.3.3 Unsafe Sendable escape hatch (canonical) {#b-3-3}

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

### B.3.4 Actor isolation modifier: nonisolated (canonical) {#b-3-4}

For actor types, implementations shall support a way to mark specific members as **nonisolated**.

Canonical attribute:

```c
__attribute__((objc_nonisolated))

M271-A001 source closure:
- `__attribute__((objc_resource(close=CloseFn, invalid=Expr))) let name = value;`
- `borrowed T *`
- `__attribute__((objc_returns_borrowed(owner_index=N)))`
- `^[weak x, unowned y, move z] { ... }`

M271-A002 frontend completion:
- `__attribute__((cleanup(CleanupFn))) let name = value;`
- `@cleanup(CleanupFn)` and `@resource(CloseFn, invalid: Expr)`
- explicit block capture lists now preserve plain, `weak`, `unowned`, and
  `move` item modes in the emitted frontend packet

M271-A003 retainable-family source completion:
- `__attribute__((objc_family_retain(FamilyName)))`
- `__attribute__((objc_family_release(FamilyName)))`
- `__attribute__((objc_family_autorelease(FamilyName)))`
- compatibility aliases on callables:
  `os_returns_retained`, `os_returns_not_retained`, `os_consumed`,
  `cf_returns_retained`, `cf_returns_not_retained`, `cf_consumed`,
  `ns_returns_retained`, `ns_returns_not_retained`, `ns_consumed`

M271-B001 semantic-model note:
- `frontend.pipeline.semantic_surface.objc_part8_system_extension_semantic_model`
- cleanup/resource locals, borrowed pointer spellings, explicit capture lists,
  and retainable-family declarations now share one truthful sema packet
- resource move legality, borrowed escape legality, retainable-family legality,
  lowering, and runtime behavior remain later `M271` work

M271-B002 semantic note:
- `frontend.pipeline.semantic_surface.objc_part8_resource_move_and_use_after_move_semantics`
- explicit `move` capture now transfers cleanup ownership only for
  cleanup/resource-backed locals
- live sema rejects non-resource `move` capture, later use after move, and
  duplicate cleanup transfer

M271-B003 semantic note:
- `frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_escape_analysis`
- borrowed pointers now cross call boundaries only when the callee parameter is
  explicitly marked `borrowed`
- live sema rejects unproven borrowed call escapes and invalid borrowed-return
  contracts

M271-B004 semantic note:
- `frontend.pipeline.semantic_surface.objc_part8_capture_list_and_retainable_family_legality_completion`
- duplicate explicit captures, weak/unowned explicit captures on non-object
  bindings, conflicting retainable-family annotations, and compatibility
  aliases without supporting object-return family surfaces now fail closed

Current implementation status (`M271-C001`):
- `frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract`
- emitted manifests and IR now carry one replay-stable lowering packet for
  cleanup/resource ownership counts, borrowed boundary counts, explicit capture
  inventories, and retainable-family callable inventories
- live cleanup/runtime carriers, borrowed lifetime runtime interop, and
  runnable retainable-family execution behavior remain later `M271` work

Current implementation status (`M271-C002`):
- stack/local cleanup and resource lowering is now live in emitted native IR
  and object artifacts
- explicit `move` captures from cleanup-backed locals now lower through emitted
  block dispose helpers on the supported non-promoting path
- actual escaping promotion of move-based cleanup/resource captures remains
  fail-closed until later runtime ownership-transfer work lands

Current implementation status (`M271-C003`):
- `frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_and_retainable_family_abi_completion`
- emitted manifests and IR now preserve one dedicated borrowed/retainable ABI
  packet above the frozen Part 8 lowering contract
- the packet carries borrowed-return attribute inventory together with
  retainable-family operation/compatibility-alias inventories on the supported
  native direct-call path
- borrowed lifetime runtime interop and runnable retainable-family behavior
  remain later `M271` lane-D work

Current implementation status (`M271-D001`):
- the current Part 8 runtime/helper freeze reuses the private ARC helper
  cluster:
  `objc3_runtime_retain_i32`,
  `objc3_runtime_release_i32`,
  `objc3_runtime_autorelease_i32`,
  `objc3_runtime_push_autoreleasepool_scope`,
  `objc3_runtime_pop_autoreleasepool_scope`
- runtime proof also reuses
  `objc3_runtime_copy_memory_management_state_for_testing` and
  `objc3_runtime_copy_arc_debug_state_for_testing`
- cleanup execution, resource invalidation proof, and retainable-family helper
  integration now freeze that existing helper/runtime slice without widening the
  public runtime header
- borrowed lifetime runtime enforcement and escaping cleanup/resource ownership
  transfer remain later `M271` lane-D work

Current implementation status (`M271-D002`):
- the supported Part 8 fixture path now links and executes through the emitted
  cleanup/resource body and the same private ARC/autorelease helper cluster
- linked runtime probes now call the emitted `helperSurface` function, route
  `CFRetain` / `CFRelease` / `CFAutorelease` through the private helper
  entrypoints, and observe `CloseFd` / `ReleaseTemp` cleanup execution directly
- the executable proof stays on the existing packaged runtime archive and
  emitted module object path
- borrowed lifetime runtime enforcement and escaping cleanup/resource ownership
  transfer remain later `M271` lane-D work

Current implementation status (`M271-E001`):
- lane-E now freezes the current runnable Part 8 slice on top of `M271-A003`,
  `M271-B004`, `M271-C003`, and `M271-D002`
- the truthful runnable proof remains the linked `M271-D002` `helperSurface`
  runtime probe rather than a widened front-door borrowed-pointer or
  resource-runtime claim
- the runnable matrix closeout remains `M271-E002`

Current implementation status (`M271-E002`):
- the milestone closeout now replays the published `M271-A003` through
  `M271-E001` proof chain and freezes one explicit runnable matrix for the
  already-landed Part 8 slice
- supported closeout rows cover retainable-family source completion,
  capture-list legality, borrowed/retainable ABI completion, live
  cleanup/runtime integration, and the lane-E conformance gate
- the next issue is `M272-A001`
```

This attribute may be applied to:

- Objective‑C methods and properties declared within an `actor class`.

Semantics are defined in [Part 7](#part-7) ([§7.7.4](#part-7-7-4)).

> Toolchains may additionally support a contextual keyword spelling (`nonisolated`) as sugar, but emitted interfaces should preserve semantics and may prefer the canonical attribute spelling.

## B.4 Errors {#b-4}

### B.4.1 NSError bridging attribute (canonical) {#b-4-1}

For interop with NSError-out-parameter conventions, implementations shall recognize:

```c
__attribute__((objc_nserror))
```

Semantics are defined in [Part 6](#part-6) ([§6.9](#part-6-9)).

### B.4.2 Status-code bridging attribute (canonical) {#b-4-2}

For return-code APIs, implementations shall recognize:

```c
__attribute__((objc_status_code(/* parameters */)))
```

Semantics are defined in [Part 6](#part-6) ([§6.10](#part-6-10)).

## B.5 Performance and dynamism controls {#b-5}

### B.5.1 Direct methods and class defaults (canonical) {#b-5-1}

Direct dispatch controls use:

```c
__attribute__((objc_direct))          // methods
__attribute__((objc_direct_members))  // classes
```

`objc_direct` applies to methods. `objc_direct_members` applies to classes and makes members direct by default as defined in [Part 9](#part-9).

### B.5.2 Final and sealed (canonical) {#b-5-2}

Objective‑C 3.0 uses the following canonical spellings:

```c
__attribute__((objc_final))                  // methods or classes
__attribute__((objc_sealed))                 // classes (module-sealed)
```

If a toolchain already provides an equivalent attribute (e.g., `objc_subclassing_restricted`), it may treat that attribute as an alias.

Semantics are defined in [Part 9](#part-9).

### B.5.3 Force-dynamic dispatch (canonical) {#b-5-3}

Objective‑C 3.0 v1 defines an explicit dynamic-dispatch marker:

```c
__attribute__((objc_dynamic))  // methods
```

Semantics are defined in [Part 9](#part-9), including interaction with `objc_direct`, `objc_final`, and `objc_sealed`.

## B.6 Metaprogramming {#b-6}

### B.6.1 Derive / synthesize (canonical) {#b-6-1}

Derivation requests use:

```c
__attribute__((objc_derive("TraitName")))
```

Semantics are defined in [Part 10](#part-10).

### B.6.2 Macro expansion (canonical) {#b-6-2}

If AST macros are supported, macro entry points may be annotated with:

```c
__attribute__((objc_macro))
```

The actual macro declaration syntax is implementation-defined ([Part 10](#part-10)), but interface emission shall preserve the canonical attributes and any synthesized declarations.

## B.7 Module interface emission requirements (normative) {#b-7}

If an implementation provides any facility that emits a textual interface for a module (e.g., generated headers, module interface stubs, API dumps), then:

1. The emitted interface shall be **semantics-preserving**: importing it must reconstruct the same declarations, effects, and attributes.
2. The emitter shall use the **canonical spellings** from this catalog (or semantically equivalent spellings defined as canonical elsewhere in the spec).
3. The emitted interface shall not depend on user macros for semantics (macros may remain for documentation convenience, but the semantic attributes/pragmas must be explicit).

## B.8 System programming extensions {#b-8}

This section catalogs canonical spellings for “system programming extensions” described in **[Part 8](#part-8)**.
These spellings are intended to survive header/module interface emission and mixed ObjC/ObjC++ builds.

### B.8.1 Resource cleanup on scope exit (canonical) {#b-8-1}

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

### B.8.2 Plain cleanup hook (canonical / existing) {#b-8-2}

**Variable attribute form (Clang existing):**

```c
__attribute__((cleanup(CleanupFn)))
```

**Optional sugar (non-normative):**

- `@cleanup(CleanupFn)` as described in [Part 8](#part-8).

### B.8.3 Borrowed interior pointers (canonical) {#b-8-3}

**Type qualifier form (canonical):**

```c
borrowed T *
```

The `borrowed` qualifier is a contextual keyword in type grammar ([Part 8](#part-8) [§8.7](#part-8-7)). It is not reserved as a general identifier.

### B.8.4 Function return is borrowed from an owner (canonical) {#b-8-4}

**Function/return attribute form:**

```c
__attribute__((objc_returns_borrowed(owner_index=N)))
```

- `N` is a 0-based index into the formal parameter list that identifies the “owner” parameter.

**Semantics:** See [Part 8](#part-8) [§8.7.2](#part-8-7-2). Toolchains shall preserve this attribute in module metadata ([D Table A](#d-3-1)).

### B.8.5 Block capture list contextual keywords (canonical) {#b-8-5}

The capture list feature in [Part 8](#part-8) uses contextual keywords:

- `weak`
- `unowned`
- `move`

These tokens are contextual within capture list grammar only and shall not be reserved globally.

### B.8.6 Retainable C family declarations (canonical) {#b-8-6}

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

## B.9 Reserved tokens {#b-9}

Objective‑C 3.0 reserves (at minimum) the following tokens as keywords:

- `async`, `await`, `actor`
- `throws`, `try`, `throw`, `do`, `catch`
- `defer`, `guard`, `match`, `case`
- `let`, `var`

Additional reserved keywords may be added by other parts.

## M267 current Part 6 error source boundary (implementation note)

Current implementation status (`M267-A001`):

- `throws` declaration modifiers on functions and Objective-C methods are now
  admitted as source-only frontend surfaces
- deterministic result-like carrier profiling remains attached to function and
  method declarations
- deterministic `NSError` bridging profiling remains attached to function and
  method declarations

## M268 current async source boundary (implementation note)

Current implementation status (`M268-A001`):

- `async fn` is now admitted as a parser-owned declaration form
- Objective-C methods now admit parser-owned `async` declaration modifiers
- `await` is now admitted as a parser-owned expression marker
- callable declarations now admit canonical `objc_executor(main)`,
  `objc_executor(global)`, and `objc_executor(named("..."))` attributes
- continuation lowering, executor binding, and runnable suspension semantics
  remain later `M268` work

Current implementation status (`M268-A002`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_async_source_closure`
- that packet records parser-owned `async`, `await`, and executor-affinity
  attribute usage deterministically
- it remains the parser-owned dependency for the live semantic packet below
- runnable continuation lowering and executor runtime behavior are still later
  `M268` work

Current implementation status (`M268-B001`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model`
- that packet records live async-continuation legality, await-suspension
  legality, actor-isolation handoff, task-cancellation handoff, and
  concurrency-replay guard handoff deterministically
- the current packet is truthful sema state only; runnable async frame
  lowering, suspension cleanup, and executor runtime execution remain later
  `M268` work

Current implementation status (`M268-B002`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics`
- `await` is now semantically restricted to async functions and Objective-C
  methods
- non-async `await` sites fail closed with diagnostic `O3S223`
- runnable async frame layout, resume lowering, suspension cleanup, and
  executor runtime behavior remain deferred to later `M268` issues

Current implementation status (`M268-B003`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion`
- non-async `objc_executor(...)` sites fail closed with diagnostic `O3S224`
- async function prototypes fail closed with diagnostic `O3S225`
- async throws functions fail closed with diagnostic `O3S226`
- runnable async frame layout, async error propagation, and executor runtime
  behavior remain deferred to later `M268` issues

## M269 task/runtime cancellation source boundary (implementation note)

Current implementation status (`M269-A001`):

- no dedicated `task` or `cancel` keyword is claimed by the current frontend
  surface
- task-runtime hooks, cancellation checks, cancellation handlers, and
  suspension-point identifiers are currently admitted through parser-owned
  callable symbol profiling on top of the existing async source forms
- canonical executor spellings remain `objc_executor(global)` and
  `objc_executor(named("..."))` for the proven happy path here
- runnable task creation, executor hops, cancellation execution, and scheduler
  ownership remain later `M269` work

Current implementation status (`M269-A002`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_task_group_and_cancellation_source_closure`
- task creation call sites are currently admitted through callable identifiers
  such as `task_spawn...` and `...detached_task...`
- the supported task-group surface is currently the callable-source subset that
  maps to scope, add-task, wait-next, and cancel-all identifiers
- the packet remains source-only and still defers runnable task/runtime behavior
  to later `M269` issues

Current implementation status (`M269-B001`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_task_executor_and_cancellation_semantic_model`
- the semantic packet consumes the `M269-A002` source packet and the existing
  task-runtime/cancellation sema summaries
- the live semantic packet now truthfully claims task lifetime legality,
  executor-affinity legality, cancellation observation legality, and
  structured-task legality
- runnable lowering, executor runtime behavior, and scheduler runtime behavior
  remain deferred to later `M269` issues

Current implementation status (`M269-B002`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_structured_task_and_cancellation_semantics`
- non-async task-runtime/task-group/cancellation calls fail closed with
  diagnostic `O3S227`
- task-group add, wait-next, and cancel-all calls without scope fail closed
  with diagnostic `O3S228`
- detached task creation mixed into a structured task-group callable fails
  closed with diagnostic `O3S229`
- cancellation handlers and cancel-all calls without a cancellation check fail
  closed with diagnostic `O3S230`

Current implementation status (`M269-B003`):

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_executor_hop_and_affinity_compatibility_completion`
- async task callables without `objc_executor(...)` affinity fail closed with
  diagnostic `O3S231`
- detached task creation under `objc_executor(main)` fails closed with
  diagnostic `O3S232`

Current implementation status (`M269-C001`):

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract`
- this lowering packet consumes the existing `M269-B001`, `M269-B002`, and
  `M269-B003` semantic packets and lowers them into explicit replay-stable lane
  contracts for actor isolation, task runtime interop, and concurrency replay
- emitted IR now carries replay keys and frontend lowering profiles for task
  creation, task-group artifacts, executor-hop boundaries, cancellation polls,
  and scheduler-visible replay handoff points
- native task spawn, executor hop, cancellation runtime entrypoints, and
  task-group ABI completion remain later `M269` issues

Current implementation status (`M269-C002`):

- the retained `M269-C001` lowering packet now drives live helper-backed IR
  rewrites for the recognized task-runtime symbol family
- supported task creation, task-group, cancellation-poll, and cancel-handler
  calls lower through private runtime helpers carrying stable executor tags
- awaited `task_group_wait_next` sites also emit
  `objc3_runtime_executor_hop_i32` before the continuation helper resume path
- current issue-local executable proof is runtime-helper focused because older
  `O3S260` / `O3L300` front-door gates still block some native end-to-end
  fixture shapes before IR/object emission

Current implementation status (`M269-C003`):

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_task_group_and_runtime_abi_completion`
- the ABI packet preserves the helper list, task-group helper count, and the
  private runtime snapshot symbol introduced by `M269-C002`
- emitted IR now carries `; part7_task_runtime_abi_completion = ...` plus
  `!objc3.objc_part7_task_runtime_abi_completion = !{!93}`

Current implementation status (`M269-D001`):

- the private runtime helper boundary for task creation, task-group control,
  cancellation observation, executor hops, and task-state publication is now
  frozen above the earlier `M269-C002`/`M269-C003` helper-backed slice
- the canonical private helper cluster is:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`
- the canonical private testing snapshot is
  `objc3_runtime_copy_task_runtime_state_for_testing`
- emitted IR now carries
  `!objc3.objc_part7_scheduler_executor_runtime_contract = !{!94}`
- this issue still does not claim a general scheduler implementation or
  cross-module task-runtime behavior

Current implementation status (`M269-D002`):

- the same private helper cluster is now published as a live runtime execution
  boundary for the supported Part 7 task slice
- emitted IR now carries:
  - `; part7_live_task_runtime_integration = ...`
  - `!objc3.objc_part7_live_task_runtime_integration = !{!95}`
- issue-local runtime proof validates task spawn, task-group scope/add/wait,
  cancel-all, cancellation polling, executor hops, and
  `objc3_runtime_copy_task_runtime_state_for_testing`
- the public runtime header still does not widen
- retained `O3S260` / `O3L300` metadata-export gates still block some broader
  front-door fixture shapes, so this issue keeps its claim scoped to the live
  helper-backed execution path

Current implementation status (`M269-D003`):

- the live task-runtime slice now publishes one hardening boundary for
  cancellation cleanup, autoreleasepool scopes, and reset-stable replay
- emitted IR now carries:
  - `; part7_task_runtime_hardening = ...`
  - `!objc3.objc_part7_task_runtime_hardening = !{!96}`
- issue-local runtime proof validates deterministic two-pass replay across:
  - `objc3_runtime_reset_for_testing`
  - `objc3_runtime_copy_task_runtime_state_for_testing`
  - `objc3_runtime_copy_memory_management_state_for_testing`
  - `objc3_runtime_copy_arc_debug_state_for_testing`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`

Current implementation status (`M269-E001`):

- lane-E now freezes the current runnable task/executor gate above
  `M269-A002`, `M269-B003`, `M269-C003`, and `M269-D003`
- the gate keeps the driver/manifest/frontend publication anchors explicit
  while relying on the hardened `M269-D003` runtime probe as the truthful live
  runnable proof
- broader front-door metadata export remains truthfully fail-closed here and is
  not widened by this gate
- the next issue is `M269-E002`

Current implementation status (`M269-E002`):

- the runnable task/executor closeout now replays `M269-A002` through
  `M269-E001` as one explicit matrix for the current Part 7 task/runtime slice
- supported rows cover task creation source closure, executor-affinity
  legality, task runtime ABI/helper publication, hardened cancellation/
  autorelease replay, and the lane-E conformance gate
- this closeout does not widen the surface beyond the already-landed
  helper-backed task/runtime slice
- the next issue is `M270-A001`

Current implementation status (`M268-C001`):

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_continuation_abi_and_async_lowering_contract`
- emitted manifests and IR carry the async continuation lowering replay key
  alongside the await suspension lowering replay key
- the current positive fixture freezes deterministic lowering counts for async
  entry points, await suspension points, and continuation handoff readiness
- runnable async frame layout, suspension cleanup, and executor runtime
  behavior remain deferred to later `M268` issues

Current implementation status (`M268-C002`):

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_async_function_await_and_continuation_lowering`
- the current runnable lowering slice supports:
  - async functions
  - async Objective-C methods
  - `await` on the supported non-suspending happy path
- emitted LLVM IR and object files currently realize that slice through direct
  calls rather than continuation allocation or state-machine emission
- continuation allocation, suspend/resume lowering, suspension cleanup, and
  executor runtime behavior remain deferred to later `M268` issues

Current implementation status (`M268-C003`):

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration`
- the current supported async slice now has explicit proof that it composes
  with:
  - autoreleasepool scope push/pop lowering
  - defer cleanup emission on scope exit
- the emitted IR proof remains a non-suspending direct-call path; it does not
  claim continuation-frame cleanup or executor resume scheduling
- the first private Part 7 continuation helper ABI is frozen separately in
  `M268-D001`; this issue does not yet claim compiled async code uses it
- `try`, `throw`, and `do/catch` are reserved frontend/source constructs in the
  current native implementation; A001 does not claim runnable semantics for them
- runnable propagation, catch handling, and native error ABI are still deferred
  to later `M267` issues

Current implementation status (`M268-D001`):

- emitted IR now publishes
  `; part7_continuation_runtime_helper = ...`
  and `!objc3.objc_part7_continuation_runtime_helper = !{!91}`
- the runtime now exposes a private helper cluster for:
  - logical continuation allocation
  - executor handoff
  - continuation resume
- the runtime also exposes
  `objc3_runtime_copy_async_continuation_state_for_testing`
  for deterministic probe coverage
- the current runnable async lowering slice still does not consume this helper
  cluster directly in compiled code; live executable integration is deferred to
  `M268-D002`

Current implementation status (`M268-D002`):

- emitted IR now publishes
  `; part7_live_continuation_runtime_integration = ...`
  and `!objc3.objc_part7_live_continuation_runtime_integration = !{!92}`
- the supported non-suspending `await` path now executes through the private
  continuation helper cluster in native object code
- runtime probes can observe deterministic allocation, handoff, resume, and
  handle-retirement traffic through
  `objc3_runtime_copy_async_continuation_state_for_testing`
- live suspension frames, async state machines, general executor scheduling,
  and cross-module runnable async claims remain deferred to later `M268`

Current implementation status (`M268-E001`):

- lane-E now freezes the runnable Part 7 gate over:
  - `M268-A002`
  - `M268-B003`
  - `M268-C003`
  - `M268-D002`
- the gate consumes the same emitted manifest/IR/object artifact triplet from
  the native CLI/frontend publication path
- the canonical gate fixture remains
  `tests/tooling/fixtures/native/m268_d002_live_continuation_runtime_integration_positive.objc3`
- matrix expansion and broader runnable async claims remain deferred to
  `M268-E002`

Current implementation status (`M268-E002`):

- the runnable Part 7 closeout matrix now consumes:
  - `M268-A002`
  - `M268-B003`
  - `M268-C003`
  - `M268-D002`
  - `M268-E001`
- closeout rows remain tied to the current direct-call, non-suspending async
  slice and its existing emitted manifest/IR/object evidence
- this closeout does not claim suspension frames, async state machines, general
  executor scheduling, or cross-module runnable async execution
  issues

## M267 current canonical error bridge-marker frontend boundary

Current implementation status (`M267-A002`):

- canonical declaration markers are now admitted on function and Objective-C
  method declarations:
  - `__attribute__((objc_nserror))`
  - `__attribute__((objc_status_code(success: ..., error_type: ..., mapping: ...)))`
- the frontend Part 6 semantic surface now counts marker sites and required
  status-code clause sites deterministically
- malformed `objc_status_code(...)` payloads fail closed in the parser
- runnable bridge execution, `try` lowering, and native thrown-error ABI remain
  deferred to later `M267` issues

## M267 current Part 6 semantic boundary

Current implementation status (`M267-B001`):

- sema now publishes one truthful Part 6 packet at
  `frontend.pipeline.semantic_surface.objc_part6_error_semantic_model`
- live today:
  - throws declaration carriage
  - deterministic result-like profile carriage
  - deterministic `NSError` bridging profile carriage
  - deterministic canonical bridge-marker carriage
- still deferred:
  - postfix propagation `?`
  - status-to-error runtime execution
  - native thrown-error ABI
- legacy throws/unwind shard summaries remain carried only as placeholder packets;
  they are not yet the runnable Part 6 propagation model

## M267 current Part 6 try/do/catch semantic boundary

Current implementation status (`M267-B002`):

- sema now publishes one truthful Part 6 packet at
  `frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics`
- live today:
  - `try`, `try?`, and `try!` semantic normalization
  - `throw` statement legality checking
  - `do/catch` catch-order and catch-binding legality checking
  - propagating-try context enforcement
  - throwing/bridged operand enforcement
- still deferred:
  - native IR/object/executable behavior for `try`, `throw`, and `do/catch`
  - runnable catch transfer
  - postfix propagation `?`
  - status-to-error runtime execution
  - native thrown-error ABI
- the older throws/unwind summaries still do not claim runnable propagation semantics

## M267 current Part 6 bridge legality boundary

Current implementation status (`M267-B003`):

- sema now publishes one truthful Part 6 packet at
  `frontend.pipeline.semantic_surface.objc_part6_error_bridge_legality`
- live today:
  - legality filtering for `__attribute__((objc_nserror))`
  - legality filtering for `__attribute__((objc_status_code(...)))`
  - deterministic out-parameter, return-shape, `error_type`, and mapping checks
  - only semantically valid bridge call surfaces qualify for `try`
- still deferred:
  - native status-to-error execution
  - runnable bridge lowering/runtime support
  - native thrown-error ABI

## M267 current Part 6 lowering boundary

`try`, `throw`, and `do/catch` are reserved parser-owned fail-closed constructs until the later runnable lowering tranche lands.

Current implementation status (`M267-C001`):

- lane C first froze one truthful lowering-boundary packet at
  `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`
- emitted IR now carries:
  - `; part6_throws_abi_propagation_lowering = ...`
  - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- `M267-C002` is the next issue.

Current implementation status (`M267-C002`):

- lowering now publishes one truthful Part 6 packet at
  `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`
- live today:
  - hidden error-out ABI lowering for throwing call targets
  - native `throw` propagation
  - `try`, `try?`, and `try!` lowering
  - `do/catch` dispatch
  - status-code / `NSError` bridge propagation
  - `; part6_throws_abi_propagation_lowering = ...`
  - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
  - `ready_for_runtime_execution=true`
- still deferred:
  - replay/inspection completion for separate compilation

Current implementation status (`M267-C003`):

- the compiler now also publishes the replay-completion packet at
  `frontend.pipeline.semantic_surface.objc_part6_result_and_bridging_artifact_replay`
- the same packet is emitted into:
  - `module.runtime-import-surface.json`
  - `module.part6-error-replay.json`
- emitted IR now carries:
  - `; part6_result_and_bridging_artifact_replay = ...`
  - `!objc3.objc_part6_result_and_bridging_artifact_replay = !{!88}`
- separate provider/consumer compilation now preserves imported provider replay
  keys for:
  - the Part 6 throws packet
  - the result-like lowering packet
  - the `NSError` bridging packet
- still deferred:
  - generalized foreign error-object ABI
  - runtime helper ABI freeze beyond the current emitted replay surfaces
  - broader cross-module preservation claims
  - generalized native thrown-error object ABI

Current implementation status (`M267-D001`):

- lane D now freezes one truthful private runtime helper contract:
  `objc3c-part6-error-runtime-and-bridge-helper-api/m267-d001-v1`
- runnable Part 6 lowering now routes:
  - thrown-error slot store/load
  - status-code bridge normalization
  - `NSError` bridge normalization
  - catch-kind matching
  through private runtime helpers instead of raw local-slot traffic
- emitted IR now carries:
  - `; part6_error_runtime_bridge_helper = ...`
  - `!objc3.objc_part6_error_runtime_bridge_helper = !{!89}`
- the helper ABI remains private to `objc3_runtime_bootstrap_internal.h`
- still deferred:

Current implementation status (`M267-D002`):

- lane D now proves one truthful live runtime integration contract:
  `objc3c-part6-live-error-runtime-integration/m267-d002-v1`
- linked runnable Part 6 programs now execute:
  - thrown-error store/load
  - status-code bridge normalization
  - `catch (NSError* error)` dispatch
  through the private runtime helper cluster
- emitted IR now carries:
  - `; part6_live_error_runtime_integration = ...`
  - `!objc3.objc_part6_live_error_runtime_integration = !{!90}`
- the live proof still remains limited to the supported `NSError` bridging
  slice and does not claim generalized foreign exceptions
  - public error-runtime headers
  - generalized foreign exception ABI
  - broader cross-module executable Part 6 claims

Current implementation status (`M267-D003`):

- imported runtime surfaces now preserve the current runnable Part 6 replay
  contract/source pair and the associated readiness + replay-key inventory
- cross-module link-plan emission now publishes:
  - `expected_part6_contract_id`
  - `expected_part6_source_contract_id`
  - imported Part 6 module names and readiness state
- mixed-module native builds now fail closed if an imported Part 6 replay
  surface is incomplete or drifted from the canonical runnable replay contract
- the runtime helper ABI remains unchanged in this tranche


## M265 imported Part 3 packets

Cross-module imports preserve optional/key-path runtime packets alongside the
runtime-owned declaration inventory. This keeps optional sends, optional-member
access, and validated typed key-path literals attached to explicit imported
semantic packets instead of relying on implicit recovery from unrelated
metadata-section counts.

## M265 executable type-surface gate (E001)

`objc3c-type-surface-executable-conformance-gate/m265-e001-v1`

The lane-E freeze for Part 3 accepts only the currently implemented runnable
slice. It binds the supported optional-binding, optional-send, nil-coalescing,
generic-preservation, and typed key-path facts into one executable gate.
`M265-E002` is the first issue allowed to broaden this gate into a runnable matrix.

## M265 runnable type-surface closeout matrix (E002)

`objc3c-runnable-type-surface-closeout/m265-e002-v1`

`M265-E002` closes only the currently supported Part 3 slice. Optionals and
validated typed key paths now have real runtime rows. Pragmatic generics stay
truthful through preserved metadata/replay evidence because they do not claim a
separate runtime behavior. `M266-A001` is the next issue after `M265` closeout.

## M267 runnable throws, Result, and bridge matrix closeout (E002)

`objc3c-part6-runnable-throws-result-and-bridge-matrix/m267-e002-v1`

Current implementation status (`M267-E002`):

- closes the current runnable Part 6 slice over the already published
  source/sema/lowering/runtime/cross-module evidence chain
- does not widen the accepted syntax beyond the existing `try`, `throw`, and
  `do/catch` surface already proven in `M267`
- does not add a new bridge helper ABI or metadata family
- preserves the truthful handoff from `M267-E001` to `M268-A001`

Current implementation status (`M267-E001`):

- freezes one executable gate over the current `M267-A002` through `M267-D003`
  Part 6 slice
- preserves the same published manifest/replay/link-plan evidence before the
  `M267-E002` closeout matrix

## M272 dispatch-intent source closure (A001)

Current implementation status (`M272-A001`):

- callable `__attribute__((objc_direct))`, `__attribute__((objc_final))`, and
  `__attribute__((objc_dynamic))` are now admitted on function and Objective-C
  method declarations
- container `__attribute__((objc_direct_members))`,
  `__attribute__((objc_final))`, and `__attribute__((objc_sealed))` are now
  admitted on Objective-C interface and `actor class` declarations
- the emitted frontend packet now publishes
  `frontend.pipeline.semantic_surface.objc_part9_dispatch_intent_and_dynamism_source_closure`
- legality enforcement, direct-call lowering, metadata realization, and runtime
  dispatch-boundary behavior remain later `M272` work

## M272 dispatch-intent attribute/defaulting completion (A002)

Current implementation status (`M272-A002`):

- prefixed container `__attribute__((...))` lists are now admitted before
  `@interface` and `actor class` declarations for the supported Part 9
  dispatch-intent attributes
- `objc_direct_members` containers now publish
  `frontend.pipeline.semantic_surface.objc_part9_dispatch_intent_attribute_and_defaulting_source_completion`
- the frontend completion packet preserves prefixed container-attribute sites,
  effective direct member sites, direct-members defaulted method sites, and
  `objc_dynamic` opt-out sites inside direct-members containers
- legality enforcement, direct-call lowering, metadata realization, and runtime
  dispatch-boundary behavior remain later `M272` work

## M272 dynamism/dispatch-control semantic model (B001)

Current implementation status (`M272-B001`):

- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part9_dynamism_and_dispatch_control_semantic_model`
- that packet reuses the existing Part 9 source-completion packet together
  with the established override lookup/conflict accounting surface
- the packet preserves prefixed/container direct-members counts, final/sealed
  container counts, effective direct-member sites, direct-members defaulted
  method sites, `objc_dynamic` opt-out sites, and override lookup
  hits/misses/conflicts plus unresolved base-interface counts
- direct-call lowering, final/sealed enforcement, metadata realization, and
  runtime dispatch-boundary behavior remain later `M272` work

## M272 override/finality/sealing legality enforcement (B002)

Current implementation status (`M272-B002`):

- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part9_override_finality_and_sealing_legality`
- Part 9 legality now fails closed on:
  - inheriting from an `objc_final` superclass,
  - inheriting from an `objc_sealed` superclass,
  - overriding an `objc_final` superclass method,
  - participating in an override chain that uses `objc_direct` dispatch
- the active diagnostics are:
  - `O3S307`
  - `O3S308`
  - `O3S309`
  - `O3S310`
- direct-call lowering, metadata realization, and runtime dispatch-boundary
  behavior remain later `M272` work

## M272 dispatch-control compatibility diagnostics (B003)

Current implementation status (`M272-B003`):

- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part9_dynamism_control_compatibility_diagnostics`
- Part 9 compatibility now fails closed on:
  - combining `objc_direct` with `objc_dynamic`,
  - combining `objc_final` with `objc_dynamic`,
  - using Part 9 dispatch-control callable attributes on free functions,
  - using Part 9 dispatch-control callable attributes on protocol methods,
  - using Part 9 dispatch-control callable attributes on category methods,
  - using `objc_direct_members`, `objc_final`, or `objc_sealed` on categories
- the active diagnostics are:
  - `O3S311`
  - `O3S312`
  - `O3S313`
  - `O3S314`
  - `O3S315`
  - `O3S316`
- direct-call lowering, metadata realization, and runtime dispatch-boundary
  behavior remain later `M272` work

## M272 dispatch-control lowering contract (C001)

Current implementation status (`M272-C001`):

- the frontend pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part9_dispatch_control_lowering_contract`
- that packet derives from the already-landed Part 9 lane-B semantic packets:
  - `objc_part9_dynamism_and_dispatch_control_semantic_model`,
  - `objc_part9_override_finality_and_sealing_legality`,
  - `objc_part9_dynamism_control_compatibility_diagnostics`
- the lowering packet preserves direct-call candidate counts, direct-members
  defaulting and `objc_dynamic` opt-out counts, final/sealed container counts,
  metadata-preserved callable/container counts, and zero guard-blocked plus
  zero contract-violation counts on the happy path
- emitted LLVM IR now carries the same replay-stable Part 9 lowering metadata
- live direct-call selector bypass, runtime dispatch-boundary realization, and
  runnable metadata-driven dispatch behavior remain later `M272` work

## M272 direct, final, and sealed dispatch lowering (C002)

Current implementation status (`M272-C002`):

- effective `objc_direct` sends on concrete `self` and known class receivers
  now lower as exact LLVM direct calls to bound `@objc3_method_*`
  implementation symbols
- `objc_dynamic` opt-out sites remain on `@objc3_runtime_dispatch_i32`
- emitted method-list payloads now preserve:
  - effective direct-dispatch intent bits
  - method-level `objc_final` bits
- emitted class/metaclass descriptor bundles now preserve:
  - container-level `objc_final` bits
  - container-level `objc_sealed` bits
- this does not claim broad dynamic-receiver direct dispatch or optimizer-led
  devirtualization beyond the supported concrete receiver slice

## M272 dispatch metadata and interface preservation (C003)

Current implementation status (`M272-C003`):

- runtime metadata source records now preserve:
  - effective direct-dispatch intent on methods
  - method-level `objc_final` intent
  - class-level `objc_final` / `objc_sealed` intent
- emitted `module.runtime-import-surface.json` artifacts now carry a dedicated
  Part 9 preservation packet at
  `frontend.pipeline.semantic_surface.objc_part9_dispatch_metadata_and_interface_preservation`
- imported runtime surfaces now reload the same Part 9 preservation packet and
  the widened class/method source-record fields for separate-compilation replay
- emitted LLVM IR now publishes one replay-stable Part 9 preservation summary
  above the `M272-C002` direct-call lowering slice

## M272 runtime fast-path integration contract (D001)

Current implementation status (`M272-D001`):

- Part 9 now freezes the existing runtime proof surface above direct-call
  lowering rather than introducing a second dispatch runtime
- direct `objc_direct` sends remain exact LLVM calls and therefore do not touch
  the runtime cache surface
- `objc_dynamic` opt-out sends and unresolved selectors continue to use:
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
- the cross-module link plan continues to publish imported direct-surface
  artifact paths so later runtime work can widen the live fast path without
  losing imported-module provenance
- this issue freezes the current contract only; runnable runtime fast-path
  widening remains `M272-D002`
