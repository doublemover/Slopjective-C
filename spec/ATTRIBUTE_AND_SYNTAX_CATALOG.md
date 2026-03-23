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
- `try`, `throw`, and `do/catch` are reserved frontend/source constructs in the
  current native implementation; A001 does not claim runnable semantics for them
- runnable propagation, catch handling, and native error ABI are still deferred
  to later `M267` issues

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
