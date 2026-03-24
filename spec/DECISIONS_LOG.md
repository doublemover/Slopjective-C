# Objective‑C 3.0 — Design Decisions Log (v0.11) {#decisions}

_Last updated: 2026-02-23_

This log captures explicit “ship/no‑ship” decisions made to keep Objective‑C 3.0 **ambitious but implementable** (especially under separate compilation).

---

## D-001: Optional chaining is reference-only in v1 {#decisions-d-001}

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

## D-002: `throws` is untyped in v1; typed throws deferred {#decisions-d-002}

**Decision:** The v1 `throws` effect is always **untyped**, with thrown values of type `id<Error>`.

Typed throws syntax (e.g., `throws(E)`) is reserved for future extension but is not part of v1 grammar/semantics.

**Rationale:**

- Objective‑C’s runtime dynamism and mixed-language interop (NSError, C return codes) favor a single error supertype.
- Typed throws adds significant complexity to generics, bridging, and ABI/lowering.

**Spec impact:** [Part 6](#part-6).

---

## D-003: Task spawning is library-defined in v1 (no `task {}` keyword) {#decisions-d-003}

**Decision:** Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided via the **standard library**.

The compiler recognizes task entry points via attributes (see [D-007](#decisions-d-007) and [Part 7](#part-7)).

**Rationale:** Keeps parsing surface small; avoids freezing spawn semantics before runtime patterns stabilize.

**Spec impact:** [Part 7](#part-7) [§7.5](#part-7-5).

---

## D-004: Executor annotations — canonical spelling and meaning (v1) {#decisions-d-004}

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

## D-005: Optional propagation (`T?` with postfix `?`) follows carrier rules (v1) {#decisions-d-005}

**Decision:** Postfix propagation `e?` is allowed on an optional `e : T?` **only** when the enclosing function returns an optional type.

- In an optional-returning function, `e?` yields `T` when non-`nil`, otherwise performs `return nil;`.
- Using `e?` in a function returning `Result<…>` or `throws` is **ill‑formed** in v1 (no implicit nil→error mapping).

**Rationale:** Prevents accidental “nil becomes an error” magic; keeps control-flow explicit.

**Spec impact:** [Part 3](#part-3) and [Part 6](#part-6).

---

## D-006: Autorelease pool boundaries at suspension points (v1) {#decisions-d-006}

**Decision:** On Objective‑C runtimes with autorelease semantics, each task _execution slice_ (resume → next suspension or completion) runs inside an implicit autorelease pool that is drained:

- before suspending at an `await`, and
- when the task completes.

**Rationale:** Predictable memory behavior for Foundation-heavy code; avoids autorelease buildup across long async chains.

**Spec impact:** [Part 7](#part-7), [Part 4](#part-4), and [C](#c).

---

## D-007: Canonical spellings and interface emission are attribute/pragma-first (v1) {#decisions-d-007}

**Decision:** Features that must survive **module interface emission** and **separate compilation** have canonical spellings defined in:

- **[ATTRIBUTE_AND_SYNTAX_CATALOG.md](#b)**

Sugar spellings (macros, `@`-directives, alternate attribute syntaxes) may exist, but the canonical emitted form shall use the catalog spellings.

**Rationale:** Keeps generated interfaces unambiguous and independent of user macro environments.

**Spec impact:** [B](#b), [Part 2](#part-2), [Part 12](#part-12).

---

## D-008: Generic methods are deferred in v1 {#decisions-d-008}

**Decision:** v1 includes **generic types** (pragmatic, erased generics) but defers **generic methods/functions**.

**Rationale:**

- Generic methods create difficult interactions with Objective‑C selector syntax, method redeclaration/overload rules, and module interface printing.
- The majority of practical value on Apple platforms comes from generic container types and constrained protocols.

**Spec impact:** [Part 3](#part-3) [§3.5](#part-3-5) (generic methods moved to future extensions).

---

## D-009: `throws` uses a stable “error-out” calling convention (v1) {#decisions-d-009}

**Decision:** v1 requires a stable ABI for `throws` that supports separate compilation. The recommended (and default) convention is a trailing error-out parameter (`outError`) of type `id<Error> _Nullable * _Nullable`.

**Rationale:** Matches long-standing Cocoa patterns (NSError-out) and is easy to lower in LLVM without stack unwinding.

**Spec impact:** C and [Part 6](#part-6).

---

## D-010: `async` lowers to coroutines scheduled by executors (v1) {#decisions-d-010}

**Decision:** v1 `async` semantics are implemented as coroutine state machines with suspension at `await`. Resumption is scheduled by the active executor, and the runtime/stdlib exposes enough primitives to:

- create tasks,
- hop executors,
- enqueue actor-isolated work, and
- propagate cancellation.

**Rationale:** This is the most direct path to implement structured `async/await` in LLVM while preserving ARC and Objective‑C runtime behavior.

**Spec impact:** C and [Part 7](#part-7).

---

## D-011: `await` is required for any potentially suspending operation (v1) {#decisions-d-011}

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

## D-012: Required module metadata set is normative (v1) {#decisions-d-012}

**Decision:** For ObjC 3.0 semantics to survive separate compilation, the required information enumerated in **[D](#d)** is normative: a conforming toolchain shall preserve that information in module metadata and in emitted textual interfaces.

**Rationale:** Without an explicit checklist, toolchains drift into “works in a single TU” but fails at module boundaries. [D](#d) makes the “separate compilation contract” testable.

**Spec impact:** [C](#c), [D](#d), [Part 2](#part-2), [Part 12](#part-12).

---

## D-013: Future value-optionals use canonical `Optional<T>` spelling {#decisions-d-013}

**Decision:** If a future value-optional feature is standardized, its canonical source spelling is `Optional<T>`.
`optional<T>` is not canonical and remains a reserved compatibility surface.

In conforming modes:

- parsers and interface emitters shall treat `Optional<T>` as the canonical spelling,
- textual interfaces shall emit `Optional<T>` when value-optionals are represented,
- any compatibility acceptance of `optional<T>` shall diagnose and offer a migration fix-it to `Optional<T>`.

**Rationale:** A single canonical spelling avoids dual-surface drift in tooling, formatting, metadata round-trips, and diagnostics while preserving a migration path for compatibility aliases.

**Spec impact:** [Part 3](#part-3) [§3.3.5](#part-3-3-5) and [§3.9](#part-3-9).

---

## D-014: Generic free-function mangling standardizes invariants, not one universal symbol string {#decisions-d-014}

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

## D-015: Future generic reification control is declaration-scoped {#decisions-d-015}

**Decision:** Any future explicit generic reification mode applies per declaration via `@reify_generics` (or equivalent canonical declaration-level form).
Module/profile switches may gate whether declaration-level syntax is allowed, but shall not implicitly reify declarations that omit explicit markers.

Conforming metadata/interface behavior shall preserve whether a declaration is erased or explicitly reified.

**Rationale:** Declaration-scoped control supports gradual adoption, avoids module-wide semantic surprises, and keeps mixed erased/reified codebases tractable.

**Spec impact:** [Part 3](#part-3) [§3.5.5](#part-3-5-5) and [§3.9](#part-3-9).

---

## D-016: Conformance and version claims are bounded by one live frontend inventory {#decisions-d-016}

**Decision:** The native `objc3c` toolchain shall publish one canonical frontend
feature-claim inventory that explicitly separates:

- runnable claims,
- source-only recognized claims,
- unsupported fail-closed claims.

Strictness, strict concurrency, effects, async/await, actors, blocks, and ARC
shall not be advertised as supported until they move into the runnable set.

**Rationale:** This prevents the project from over-claiming Objective-C 3
support based on parser/sema or contract-only progress.

**Spec impact:** [Part 1](#part-1), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-032: Part 9 freezes the existing runtime cache and fallback surface before widening live fast paths {#decisions-d-032}

**Decision:** `M272-D001` shall freeze the existing runtime dispatch boundary as
follows:

- direct `objc_direct` sends lowered by `M272-C002` remain exact LLVM calls and
  therefore do not enter the runtime dispatch/cache surface,
- `objc_dynamic` opt-out sends and unresolved selectors continue to execute
  through `objc3_runtime_dispatch_i32`,
- the canonical runtime proof surface remains
  `objc3_runtime_copy_method_cache_state_for_testing` plus
  `objc3_runtime_copy_method_cache_entry_for_testing`,
- cross-module link planning continues to retain imported direct-surface
  artifact paths so later runtime widening can stay provenance-aware.

**Rationale:** The current Part 9 runnable boundary is already truthful: exact
LLVM direct calls bypass runtime, while the existing method-cache / slow-path /
fallback runtime remains the only live dispatch engine. `M272-D001` should
freeze that boundary before `M272-D002` widens the live fast path.

**Spec impact:** [Part 9](#part-9) runtime behavior and [E](#e) conformance
proof policy.

---

## D-022: Dispatch-intent source admission stays parser-owned until legality and lowering land {#decisions-d-022}

**Decision:** The native `objc3c` frontend shall admit Part 9 dispatch-intent
and dynamism-control attributes as parser-owned `__attribute__((...))`
spellings on top of the existing token stream rather than widening the lexer
surface.

The truthful `M272-A001` source closure covers:

- callable `objc_direct`, `objc_final`, and `objc_dynamic`,
- container `objc_direct_members`, `objc_final`, and `objc_sealed`,
- deterministic frontend manifest publication for the Part 9 source packet.

That source closure shall not yet claim:

- override/subclass/category legality enforcement,
- direct-call lowering or selector-dispatch opt-out behavior,
- metadata or runtime dispatch-boundary realization.

**Rationale:** The source contract should become explicit before later `M272`
issues widen semantics, lowering, metadata, and runtime behavior. Keeping the
lane-A surface parser-owned also matches the existing truthful pattern for
later-stage Objective-C 3 feature families.

**Spec impact:** [Part 9](#part-9), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

## D-030: Part 9 direct/final/sealed lowering executes direct sites only on the concrete supported slice {#decisions-d-030}

**Decision:** `M272-C002` shall implement one narrow live lowering step:

- effective `objc_direct` sends on concrete `self` receivers, and
- effective `objc_direct` sends on known class receivers

shall lower as exact LLVM direct calls to the already-bound
`@objc3_method_*` implementation symbols.

This lane shall also preserve dispatch intent in emitted metadata by carrying:

- effective direct-dispatch bits on method-list entries,
- method-level `objc_final` bits on method-list entries, and
- container-level `objc_final` / `objc_sealed` bits on class/metaclass
  descriptor bundles.

This lane shall not claim:

- broad direct dispatch for unknown dynamic receivers,
- speculative optimizer-driven devirtualization, or
- any new public runtime ABI.

**Rationale:** the truthful capability here is a concrete lowering win on
already-known targets, not a generalized dispatch rewrite. The metadata must
carry the same intent bits so emitted artifacts do not drift from the lowered
call path.

**Spec impact:** [Part 9](#part-9), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

## D-031: Part 9 dispatch intent must survive runtime metadata and import-surface replay after live lowering lands {#decisions-d-031}

**Decision:** `M272-C003` shall preserve the `M272-C002` dispatch-intent facts
through the compiler-owned metadata/interface surfaces that remain relevant for
separate compilation:

- runtime metadata source records,
- emitted `module.runtime-import-surface.json` artifacts, and
- replay-stable frontend IR metadata.

The preserved Part 9 facts are:

- method-level effective direct-dispatch intent,
- method-level `objc_final` intent, and
- class-level `objc_final` / `objc_sealed` intent.

This lane shall fail closed on drifted or missing imported preservation packets
instead of silently claiming cross-module Part 9 preservation.

This lane shall not claim any new runtime dispatch boundary or optimizer
behavior beyond the `M272-C002` concrete direct-call slice.

**Rationale:** once direct-call lowering is live, the next truthful gap is
artifact/interface drift. Separate compilation cannot be considered stable if
the source-record and import-surface layers drop the same Part 9 intent bits
that local IR/object emission already preserves.

---

## D-023: Direct-members defaulting must publish one frontend-owned completion packet before legality widens {#decisions-d-023}

**Decision:** After the raw Part 9 dispatch-intent attribute spellings are
admitted, the frontend shall also publish one explicit source-completion packet
for the remaining parser-owned attribute/defaulting surface.

That completion packet shall preserve, at minimum:

- prefixed container attribute sites before `@interface` and `actor class`,
- effective direct member sites under `objc_direct_members`,
- defaulted direct-member sites that do not carry explicit method attributes,
- explicit `objc_dynamic` opt-out sites inside `objc_direct_members`
  containers.

This packet remains frontend-only. It shall not yet claim subclass/override
legality, direct-call lowering, metadata realization, or runtime dispatch
behavior.

**Rationale:** `objc_direct_members` changes the meaning of unannotated methods.
That defaulting surface must become explicit in emitted frontend evidence before
later `M272` lanes widen semantics and lowering.

**Spec impact:** [Part 9](#part-9), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-026: Part 9 legality freeze must reuse the existing override-accounting surface before lowering begins {#decisions-d-026}

**Decision:** `M272-B001` shall freeze one truthful Part 9 semantic-model
packet by combining:

- the existing dispatch-intent/defaulting source-completion packet from
  `M272-A002`, and
- the existing method-lookup/override conflict accounting surface already
  produced by semantic integration.

That lane-B packet shall preserve, at minimum:

- prefixed dispatch-intent container-attribute counts,
- direct-members / final / sealed container counts,
- effective direct-member/defaulted-direct/`objc_dynamic` opt-out counts,
- override lookup hits, misses, conflicts, and unresolved base-interface
  counts.

This packet remains sema/accounting only. It shall not yet claim:

- direct-call lowering,
- final/sealed legality enforcement,
- metadata realization, or
- runnable dispatch-boundary behavior.

**Rationale:** The compiler already has one deterministic override-accounting
surface. Reusing that surface keeps the Part 9 claim truthful and avoids
inventing a second override model before lowering/runtime work lands.

**Spec impact:** [Part 9](#part-9), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-027: Part 9 legality must fail closed on superclass finality/sealing and objc_direct override chains before lowering begins {#decisions-d-027}

**Decision:** `M272-B002` shall turn the Part 9 lane-B packet into live
semantic enforcement for:

- inheriting from an `objc_final` superclass,
- inheriting from an `objc_sealed` superclass,
- overriding an `objc_final` superclass method,
- participating in an override chain that uses `objc_direct` dispatch.

## D-028: Part 9 dispatch-intent compatibility must fail closed on unsupported topologies before lowering begins {#decisions-d-028}

**Decision:** `M272-B003` shall close the remaining compatibility slice for
Part 9 dispatch-intent markers by rejecting:

- `objc_direct` + `objc_dynamic` callable conflicts,
- `objc_final` + `objc_dynamic` callable conflicts,
- free functions carrying Part 9 dispatch-control callable attributes,
- protocol methods carrying Part 9 dispatch-control callable attributes,
- category methods carrying Part 9 dispatch-control callable attributes,
- categories carrying `objc_direct_members`, `objc_final`, or `objc_sealed`.

**Rationale:** `M272-B002` made inheritance and override legality truthful, but
the compiler still admitted dispatch-intent markers on topologies that Part 9
does not lower or realize yet. Those surfaces must fail closed in sema before
lane-C can claim a narrower, truthful lowering boundary.

**Diagnostic set:** `O3S311`, `O3S312`, `O3S313`, `O3S314`, `O3S315`,
and `O3S316`.

Those rules shall fail closed in sema with deterministic diagnostics before
direct-call lowering, metadata realization, or runnable dispatch-boundary work
is allowed to widen.

The active diagnostics are:

- `O3S307`
- `O3S308`
- `O3S309`
- `O3S310`

**Rationale:** finality, sealing, and direct-dispatch override restrictions are
language-level legality boundaries. They must be enforced before later `M272`
lanes start treating Part 9 intent as a lowering/runtime contract.

**Spec impact:** [Part 9](#part-9), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-029: Part 9 lowering freeze carries direct-call candidates and metadata-preserved dispatch intent before runtime realization {#decisions-d-029}

**Decision:** `M272-C001` shall freeze one truthful Part 9 lowering contract by
combining:

- the Part 9 semantic-model packet from `M272-B001`,
- the Part 9 legality packet from `M272-B002`, and
- the Part 9 compatibility packet from `M272-B003`.

That lane-C packet shall preserve, at minimum:

- direct-call candidate counts,
- direct-members defaulting and `objc_dynamic` opt-out counts,
- final/sealed container counts,
- metadata-preserved callable/container inventories, and
- guard-blocked plus contract-violation counts.

This packet remains lowering-only. It shall not yet claim:

- live direct-call selector bypass,
- runtime dispatch-boundary realization, or
- runnable metadata-driven dispatch behavior.

**Rationale:** the Part 9 lane-B packets already define the truthful legality
and compatibility boundary. Lane-C should carry those facts forward into one
deterministic emitted lowering contract instead of rediscovering them from raw
IR or overclaiming runtime behavior before the later M272 execution lanes land.

**Spec impact:** [Part 9](#part-9), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-021: Runtime/public capability reports must remain a truthful projection of the lowered conformance sidecar {#decisions-d-021}

**Decision:** The native `objc3c` pipeline shall publish machine-readable
`runtime_capability_report` and `public_conformance_report` payloads as a
direct projection of the lowered conformance-report truth surface rather than
introducing an independent release or CLI-owned claim model.

The current public claim set shall stay explicit:

- profile `core` is claimed,
- profiles `strict`, `strict-concurrency`, and `strict-system` are
  not-claimed,
- `strictness=permissive`,
- `concurrency=off`,
- optional features `throws`, `async-await`, `actors`, `blocks`, and `arc`
  remain not-claimed,
- replay-oriented publication remains deterministic until a later driver lane
  owns outward-facing timestamping and selection UX.

**Rationale:** The emitted capability report has value only if it is grounded in
the same truthful surface already enforced by the compiler. Splitting the claim
surface would recreate the over-claim problem `M264` is intended to remove.

**Spec impact:** [Part 12](#part-12) conformance publication semantics and
[D](#d) module/runtime metadata documentation.

---

## D-022: Driver publication and profile selection stay fail-closed until richer conformance operations land {#decisions-d-022}

**Decision:** The native driver shall expose a conformance profile selection
surface, but the currently runnable profile remains `core`. Known future
profiles (`strict`, `strict-concurrency`, `strict-system`) shall fail closed
before publication, and both active driver surfaces shall publish one
machine-readable `module.objc3-conformance-publication.json` artifact next to
the lowered conformance sidecar.

The publication artifact shall record:

- selected profile,
- supported vs rejected profile ids,
- publication surface kind,
- lowered/runtime/public contract ids,
- the canonical lowered sidecar path.

**Rationale:** Publication needs a truthful operator-facing boundary before the
later emit/validate/consume operations are added. That boundary must not imply
that unimplemented profiles are selectable today.

**Spec impact:** [Part 12](#part-12) conformance publication semantics and
[D](#d) emitted artifact inventory.

---

## D-023: Emit/validate conformance operations consume the shipped JSON sidecars only {#decisions-d-023}

**Decision:** The native toolchain shall expose explicit conformance operator
commands equivalent to `--emit-objc3-conformance`,
`--emit-objc3-conformance-format`, and `--validate-objc3-conformance`, but the
current runnable format remains JSON only.

The validation path shall consume the already-emitted
`module.objc3-conformance-report.json` and sibling
`module.objc3-conformance-publication.json`, then publish one deterministic
`module.objc3-conformance-validation.json` summary. YAML remains fail-closed
until a later lane implements it truthfully end to end.

**Rationale:** The spec already claims emit/validate operator equivalence. The
toolchain therefore needs a real operator surface, but that surface must remain
bounded to the shipped JSON sidecars instead of implying richer publication
formats or profile support that do not exist.

**Spec impact:** [Part 12](#part-12) conformance-report operations and emitted
artifact inventory.

---

## D-024: The M264 milestone gate freezes one core/json-only conformance boundary {#decisions-d-024}

**Decision:** The lane-E gate for `M264` shall freeze one integrated operator
boundary spanning the driver, frontend C API runner, lowered conformance
report, runtime/public capability payloads, D001 publication sidecar, and D002
validation sidecar.

That gate currently permits only:

- claimed profile `core`
- compatibility selection `canonical|legacy`
- migration-assist selection
- JSON conformance publication/validation

It explicitly does not permit:

- strict or strict-concurrency profile claims
- feature-macro publication for those unavailable modes
- YAML conformance publication or validation

**Rationale:** The milestone needs one explicit closeout boundary so future
milestones do not silently widen claims beyond the runnable native subset that
is actually shipped today.

**Spec impact:** [Part 12](#part-12) conformance publication/validation and
[D](#d) emitted artifact inventory.

---

## D-025: The M264 closeout publishes one release/runtime claim matrix {#decisions-d-025}

**Decision:** The `M264` closeout shall publish one compact machine-readable and
human-readable release/runtime claim matrix derived from the already-landed
`A002`, `B003`, `C002`, `D002`, and `E001` evidence plus live native/frontend
operator probes.

That matrix currently permits only:

- claimed profile `core`
- compatibility modes `canonical|legacy`
- migration assist
- JSON conformance emit/validate operations
- native CLI report/publication/validation evidence
- frontend C API report/publication evidence

It explicitly does not permit:

- profile claims for `strict`, `strict-concurrency`, or `strict-system`
- YAML conformance publication/validation
- feature-macro publication
- optional-feature claims for `throws`, `async-await`, `actors`, `blocks`, or
  `arc`

**Rationale:** The milestone needs one operator-facing release summary that is
truthful about the actually runnable native subset and does not rely on readers
to reconstruct the boundary from five prior issue summaries.

**Spec impact:** [Part 12](#part-12) conformance publication semantics and
[D](#d) module/runtime metadata documentation.

---

## D-020: Canonical interface and feature-macro claims stay bounded by shipped surfaces {#decisions-d-020}

**Decision:** Until the native toolchain ships a standalone textual interface
payload, separate-compilation truth must be expressed by the semantic claim
packet rather than by implied interface or macro surfaces.

The packet shall therefore publish that:

- canonical interface payload mode remains `no-standalone-interface-payload-yet`,
- future canonical interfaces and conformance reports must stay bounded to the
  live runnable subset plus the already-downgraded source-only claims,
- feature-macro publication remains suppressed across those surfaces until the
  corresponding executable/runtime-backed implementations exist.

**Rationale:** This prevents the current implementation from over-claiming
separate-compilation support merely because source-level planning or historical
conformance suites describe textual-interface and macro surfaces.

Current native implementation note (`M264-B003`): the semantic claim packet
publishes canonical interface truth as equivalent-only with payload mode
`no-standalone-interface-payload-yet`, and it emits the exact ordered suppressed
macro-claim set for strictness and strict-concurrency publication.

**Spec impact:** [B](#b), [D](#d), [Part 1](#part-1), and [Part 12](#part-12).

---

## D-017: Unsupported strictness and macro-claim surfaces stay unadvertised until executable {#decisions-d-017}

**Decision:** The native `objc3c` frontend shall explicitly advertise only the
currently live selection surfaces:

- language version,
- compatibility mode,
- migration assist.

Strictness selection, strict concurrency selection, and feature-macro claim
publication shall remain machine-readable but fail-closed as unsupported until
their corresponding executable/runtime-backed implementations exist.

**Rationale:** Truthful versioning/conformance reporting requires the frontend
and driver to expose what is actually selectable today, not what the long-range
spec eventually defines.

**Spec impact:** [Part 1](#part-1), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-018: Compatibility selections are live, source-only claims stay downgraded, and strictness stays rejected {#decisions-d-018}

**Decision:** The native `objc3c` sema layer shall publish one fail-closed
semantic legality packet that classifies the current frontend truth surface as:

- valid live selections: language version, compatibility mode, migration assist,
- downgraded recognized claims: source-only declaration/object-surface features,
- rejected claim surfaces: strictness, strict concurrency, and feature-macro publication.

Source-only recognized features shall not be promoted into runnable claims until
their lowering/runtime-backed implementations exist. Unsupported strictness and
feature-macro surfaces shall remain explicitly rejected until they become
executable.

**Rationale:** Truthful conformance requires a semantic boundary that explains
how live selections and non-runnable claims relate, rather than treating every
recognized surface as implementation-complete.

**Spec impact:** [Part 1](#part-1), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-020: Lowered versioned conformance reports must remain bounded to the truthful frontend claim surface {#decisions-d-020}

**Decision:** The native `objc3c` lowering path shall publish one machine-readable
versioned conformance-report sidecar derived from the already-truthful frontend
claim and semantic packets rather than introducing a second independent
authority.

That lowered report shall keep the current implementation boundary explicit:

- standalone textual interface payload mode remains
  `no-standalone-interface-payload-yet`,
- runnable claims remain bounded to the shipped native subset,
- recognized source-only claims remain downgraded,
- strictness, strict concurrency, and feature-macro publication remain
  unsupported until they become executable end to end.

**Rationale:** Truthful conformance reporting requires the emitted report to say
exactly what the frontend can currently prove, not what the long-range spec
eventually intends to support.

**Spec impact:** [Part 1](#part-1), [Part 12](#part-12), and
[E](#e) conformance evidence policy.

---

## D-019: Accepted unsupported source surfaces must fail before lowering/runtime handoff {#decisions-d-019}

**Decision:** When the native `objc3c` frontend accepts source syntax for a
feature family that is still not runnable end to end, semantic analysis shall
reject that source surface before lowering/runtime handoff rather than allowing
later lanes to imply runnable support.

For the current Objective-C 3 native subset, the live fail-closed source
rejection surface includes:

- `throws`,
- ARC ownership-qualified parameters on executable function/method signatures,
- ARC ownership-qualified returns on executable function/method signatures.

Block literals remain tracked in the unsupported feature inventory, but the
current parser path is still gated earlier than the B002 semantic rejection
surface and therefore shall not be overstated as part of the current live proof.

**Rationale:** Truthful conformance requires the compiler to reject accepted but
non-runnable source surfaces at the semantic boundary, not merely document them
as unsupported after the fact.

**Spec impact:** [Part 1](#part-1), [Part 12](#part-12), and
[E](#e) conformance evidence policy.
