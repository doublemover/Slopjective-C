# ObjC 3.0 Spec Punch List (v0.10)
_Generated: 2025-12-28_
This is a prioritized, GitHub-issue–style punch list for the Objective‑C 3.0 draft spec bundle.
Each item includes **scope**, **why it matters**, and concrete **acceptance criteria** (what “done” looks like).
> Note: Line numbers refer to the v0.10 bundle as extracted from `spec.zip`.
## P0 — Blockers (required before calling this a real “spec”)
## P0-01: Publish a stable Normative References Index and baseline definition
**Labels:** priority:P0, type:spec-gap, area:baseline
**Touches:** 02_PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md §0.2/§0.6

**Problem**
Part 0 states that ObjC 3.0 builds on a de‑facto baseline (ARC, blocks, modules, etc.), but the draft does not yet pin that baseline with stable identifiers and versions. Without a stable normative reference index, two implementers can disagree on baseline corner cases while both claiming conformance.

**Acceptance criteria**
- [ ] Add a dedicated section enumerating every external normative reference (title, version/date/commit, and the exact scope it covers).
- [ ] Define what happens when a normative reference conflicts with an ObjC 3.0 rule (priority / override rules).
- [ ] Add a policy for referencing de‑facto behavior when no stable external spec exists (e.g., reference a specific compiler/runtime version set).
- [ ] Update cross-references throughout Parts 3–12 to use the stable identifiers.

**Notes / pointers**
This is already listed as an open issue in Part 0 (§0.6).

## P0-02: Add a unified Abstract Machine: evaluation order, lifetime, cleanup, and suspension semantics
**Labels:** priority:P0, type:spec-gap, area:semantics, area:baseline
**Touches:** 02_PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md §0.6; Parts 3/6/7/8 (cross-cutting)

**Problem**
Multiple features depend on a shared semantic core: optional chaining (`?.` and `[x? sel]`), postfix propagation (`?`), `throws` and `try`, `defer`/cleanup ordering, and `await` suspension points. Right now, ordering and cleanup rules are specified piecemeal, which risks contradictions and divergent implementations.

**Acceptance criteria**
- [ ] Create a new “Abstract Machine” section (Part 0 or a dedicated Part) defining: expression evaluation order guarantees; temporary lifetime; cleanup stack behavior; scope-exit action ordering; and how suspension points interact with all of the above.
- [ ] Explicitly specify whether ObjC 3.0 adds any evaluation-order guarantees beyond baseline C/ObjC, and where.
- [ ] Define how `await` interacts with scope-exit actions, ARC releases, autorelease pools, and propagation operators.
- [ ] Add a conformance test matrix covering edge cases that combine `defer`, `try`, postfix `?`, `?.`, and `await`.

**Notes / pointers**
Also listed as an open issue in Part 0 (§0.6).

## P0-03: Define core spec terms: ill‑formed, required diagnostic, undefined behavior, unspecified/implementation-defined
**Labels:** priority:P0, type:spec-gap, area:baseline, area:docs
**Touches:** 02_PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md §0.3/§0.4; Many parts use “ill-formed”

**Problem**
The draft frequently says “the program is ill‑formed” but does not define what that means in this spec (e.g., is a diagnostic required? at compile time? link time?). The draft also never explicitly states how much C/C++ undefined behavior is inherited, which is important for both safety claims and optimizer latitude.

**Acceptance criteria**
- [ ] Add definitions for: ill‑formed (diagnostic required), conditionally-supported, undefined behavior, unspecified behavior, and implementation-defined behavior, and how they map to compile/link/runtime phases.
- [ ] State explicitly whether ObjC 3.0 inherits C/C++ UB as-is (likely yes) and where ObjC 3.0 adds additional constraints.
- [ ] Standardize the wording style: prefer either “ill‑formed; diagnostic required” or “error” consistently.

## P0-04: Provide a complete formal grammar + operator precedence/associativity table for all new syntax
**Labels:** priority:P0, type:spec-gap, area:grammar
**Touches:** 03_PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md §1.3; 05_PART_3_TYPES_…; 07_PART_5_CONTROL_FLOW_…; 08_PART_6_ERRORS_…; 09_PART_7_CONCURRENCY_…; 10_PART_8_SYSTEM_…

**Problem**
The draft contains several “grammar summaries” and local disambiguation rules, but not a single authoritative grammar + precedence table. This is a blocker for independent implementations, and it’s especially risky given the new uses of `?`, `?.`, `??`, `try?`, and `await`.

**Acceptance criteria**
- [ ] Add a complete grammar appendix (EBNF or similar) that integrates all ObjC 3.0 additions into the baseline grammar.
- [ ] Add an operator precedence + associativity table that explicitly includes `?.`, postfix propagation `?`, `??`, `await`, and `try` forms, and shows interactions with C operators like `?:`, casts, and `->`/`.`.
- [ ] Document all disambiguation rules (e.g., postfix `?` follow-token restrictions) and required diagnostics/fix-its for ambiguous parses.
- [ ] Add parser-focused conformance tests for the ambiguous/edge grammar cases.

## P0-05: Specify cancellation semantics (propagation, observation points, and interaction with cleanup)
**Labels:** priority:P0, type:spec-gap, area:concurrency
**Touches:** 09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md (cancellation sections); 01C.5/01D tables; Stdlib contract

**Problem**
Cancellation is referenced, but the draft does not yet fully specify the semantics needed for portability: how cancellation propagates, when it is checked, how it interacts with `throws`, `defer`, resource cleanups, and what is guaranteed at suspension points.

**Acceptance criteria**
- [ ] Define cancellation propagation rules (parent→child tasks, task groups, detached tasks).
- [ ] Define the required observation points (which `await`/stdlib calls must check cancellation; what happens when cancellation is observed).
- [ ] Specify cancellation’s interaction with `throws` and propagation (`?`) — e.g., whether cancellation is an `Error`, how it is represented, and how it is caught/handled.
- [ ] Specify cleanup guarantees: `defer`/resource cleanups must run on cancellation-driven unwinds.
- [ ] Add conformance tests for cancellation in combination with `defer`, `try`, `?`, and actor hops.

## P0-06: Define Sendable-like checking precisely (what is Sendable and how checking works across modules)
**Labels:** priority:P0, type:spec-gap, area:concurrency, area:type-system
**Touches:** 09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md (Sendable sections); 01D.3.1 Table A metadata

**Problem**
The draft introduces a Sendable-like concept, but implementers need precise rules: what is Sendable by default, how generic constraints work, how to handle Objective‑C mutability patterns, and what must be preserved in module metadata for cross-module checking.

**Acceptance criteria**
- [ ] Define the default Sendable status for common categories (immutable values, actor references, class instances, blocks).
- [ ] Define checking rules at all enforcement sites (task spawning, cross-actor calls, nonisolated members, captures).
- [ ] Define how Sendable interacts with `weak`/`unowned` captures, borrowed pointers, and resource handles.
- [ ] Define `@unsafeSendable` (or equivalent) semantics and required diagnostics.
- [ ] Add conformance tests that cross module boundaries (imported types with/without Sendable annotations).

## P0-07: Specify the executor model (current executor, proving executor, hop elision, and fairness)
**Labels:** priority:P0, type:spec-gap, area:concurrency
**Touches:** 09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md; 01B.3/01C.5/01D.3 tables

**Problem**
Executors are central to the `await` requirement rule (“potentially suspending operations”), but the spec doesn’t yet nail down how the compiler/runtime determine the current executor, what is “provable”, and what guarantees exist around hops.

**Acceptance criteria**
- [ ] Define what constitutes the “current executor” and how it is tracked through calls and suspension.
- [ ] Define proof rules (when the compiler can omit an executor hop; what metadata enables proof across module boundaries).
- [ ] Define minimum fairness/ordering expectations for executor scheduling (or explicitly declare them unspecified).
- [ ] Add conformance tests covering hop elision and required hops in cross-module scenarios.

## P0-08: Specify actor isolation + reentrancy semantics at the level needed for portability
**Labels:** priority:P0, type:spec-gap, area:concurrency
**Touches:** 09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md; 01C.6

**Problem**
Actor isolation rules are described, but the draft needs a stronger portability contract: what reentrancy means, what state is protected, what invariants hold across `await`, and what is guaranteed for actor method entry.

**Acceptance criteria**
- [ ] Define reentrancy precisely (what can interleave with what, and at which points).
- [ ] Define the isolation boundary for properties vs methods, including atomicity expectations (if any).
- [ ] Specify rules for `nonisolated` members, including Sendable constraints for parameters/returns.
- [ ] Add test cases demonstrating allowed and disallowed patterns, including cross-actor property access and async calls.

## P0-09: Write a minimum Standard Library Contract: required modules, types, functions, and versioning
**Labels:** priority:P0, type:spec-gap, area:stdlib
**Touches:** Multiple parts: Result/Error/KeyPath/Concurrency/Macros references; 01E checklist references stdlib

**Problem**
The language spec relies on library-defined items (e.g., `Result<T,E>`, task operations, key paths), but the required surface is currently described in terms like “equivalent expressive power”. That is too vague to test and too risky for portability.

**Acceptance criteria**
- [ ] List the required modules (by name) that a conforming toolchain must provide, and the minimum exported API for each.
- [ ] Specify minimal semantics for required types (`Error`, `Result`, key paths) and concurrency primitives (spawning, joining, cancellation).
- [ ] Define versioning/ABI expectations for these modules (what’s stable, what’s profile-specific).
- [ ] Add conformance tests that validate the presence and behavior of required APIs.

## P0-10: Finalize module metadata encoding + versioning rules (and make them testable)
**Labels:** priority:P0, type:spec-gap, area:modules, area:abi
**Touches:** 01D_MODULE_METADATA_AND_ABI_TABLES.md; 04_PART_2_MODULES_…; 01C.2

**Problem**
The spec correctly emphasizes separate compilation, but it still needs explicit rules for metadata versioning, forward/backward compatibility, and how importers must validate metadata.

**Acceptance criteria**
- [ ] Define a metadata versioning scheme and compatibility rules (what breaks; what is ignored; what is an error).
- [ ] Specify how missing metadata is diagnosed in each conformance profile (Core/Strict/etc.).
- [ ] Add round-trip tests: emit module interface → import → verify effects/attributes preserved (including concurrency and error effects).

## P0-11: Define a portable interface format for concurrency metadata beyond module files (or explicitly defer)
**Labels:** priority:P0, type:spec-gap, area:concurrency, area:modules
**Touches:** 09_PART_7_CONCURRENCY_… §7.11 (open issue); 04_PART_2_MODULES_…

**Problem**
Part 7 explicitly raises portability of concurrency metadata beyond module files as an open issue. Without a portable format, cross-toolchain interoperability is hard (e.g., Swift interop, indexing, build systems).

**Acceptance criteria**
- [ ] Decide on a portable representation (textual interface, JSON, IR, etc.) OR explicitly state it is out of scope for v1 and define the consequences.
- [ ] If in-scope: define required fields and stability guarantees; update 01D tables accordingly.
- [ ] Add tooling tests that validate export/import of concurrency metadata across the chosen format.

## P0-12: Specify the async-to-C completion ABI signature (platform profile item if needed)
**Labels:** priority:P0, type:spec-gap, area:interop, area:abi
**Touches:** 13_PART_11_INTEROPERABILITY_C_CPP_SWIFT.md §11.6 (open issue); 01C.5 lowering

**Problem**
Interop section notes that a precise async-to-C completion thunk signature is still open. Without it, cross-language FFI for async APIs is underspecified and toolchains will diverge.

**Acceptance criteria**
- [ ] Define a canonical completion/thunk ABI for exporting `async` functions to C (including error/cancellation representation if applicable).
- [ ] Specify how lifetime/ownership of captured context is managed across the boundary.
- [ ] Add conformance tests for calling an exported async function from C and receiving the completion correctly.

## P0-13: Specify strict nullability completeness rules for mixed-language modules
**Labels:** priority:P0, type:spec-gap, area:interop, area:nullability
**Touches:** 13_PART_11_INTEROPERABILITY_C_CPP_SWIFT.md §11.6 (open issue); 05_PART_3_TYPES_…

**Problem**
Nullability-by-default and optionals rely on consistent completeness rules across headers/modules. The interop section lists this as open; until specified, strict modes are not reliably portable.

**Acceptance criteria**
- [ ] Define what “nullability complete” means for headers/modules and how completeness is checked (including imported C/ObjC++).
- [ ] Specify how incomplete nullability is diagnosed in each conformance profile (warning vs error).
- [ ] Define how completeness is represented in module metadata/interface emission.
- [ ] Add conformance tests with intentionally incomplete nullability annotations.

## P0-14: Define interaction with non-local control flow (longjmp, C++ exceptions) for cleanup constructs
**Labels:** priority:P0, type:spec-gap, area:system, area:safety
**Touches:** 07_PART_5_CONTROL_FLOW_…; 10_PART_8_SYSTEM_…; 14_PART_12_DIAGNOSTICS_…

**Problem**
The spec introduces `defer` and other cleanup actions, but portability requires explicit rules for non-local exits (e.g., `longjmp`, C++ exceptions) and what is guaranteed to run. This directly affects safety claims and correctness for mixed ObjC/ObjC++ code.

**Acceptance criteria**
- [ ] Specify what happens to scope-exit actions and resource cleanups under `longjmp` and C++ exceptions (per platform if needed).
- [ ] Define which constructs are diagnosed as ill-formed in strict profiles (e.g., `longjmp` across cleanup scopes).
- [ ] Add conformance tests (at least negative tests) for illegal non-local exits across cleanups.

## P0-15: Create a conformance test suite skeleton with required coverage buckets
**Labels:** priority:P0, type:tooling, area:conformance
**Touches:** 14_PART_12_DIAGNOSTICS_TOOLING_TESTS.md; 01E_CONFORMANCE_PROFILE_CHECKLIST.md

**Problem**
The spec frequently says behavior must be testable, but a v1 spec needs a concrete, public conformance suite structure (even if partial) to prevent divergence.

**Acceptance criteria**
- [ ] Define the repository structure for tests (parser, semantic, lowering/ABI, module metadata round-trip, diagnostics).
- [ ] For each conformance profile, list mandatory test groups and minimum counts/coverage.
- [ ] Include cross-module tests for effects/attributes preservation (throws/async/actor isolation/etc.).
- [ ] Define how tests assert diagnostics (messages, codes, fix-its) in a toolchain-portable way.

## P0-16: Fix duplicate section numbering in Part 1 (two “1.3.1” headings)
**Labels:** priority:P0, type:editorial-bug, area:docs
**Touches:** 03_PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md lines 40–41

**Problem**
Part 1 has two consecutive headings numbered **1.3.1** (“Reserved keywords” and “Contextual keywords”). This breaks unambiguous referencing.

**Acceptance criteria**
- [ ] Renumber one heading (e.g., make contextual keywords **1.3.2**) and update any cross references accordingly.
- [ ] Add a doc-lint check to prevent duplicate section IDs.

## P0-17: Fix duplicate section numbering in Part 4 (two “4.7” headings)
**Labels:** priority:P0, type:editorial-bug, area:docs
**Touches:** 06_PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md lines 68 and 77

**Problem**
Part 4 contains two top-level headings numbered **4.7**. This makes citations ambiguous and suggests a renumbering slip.

**Acceptance criteria**
- [ ] Renumber to a unique hierarchy (e.g., keep 4.7 as the umbrella section and make the second heading 4.7.1, renumbering its subsections accordingly; or renumber the second to 4.8 and shift later sections).
- [ ] Update any cross references (Part 7 and 01C references to autorelease pool rules).

## P0-18: Fix section order/numbering in Part 7 (7.12 appears before 7.10/7.11)
**Labels:** priority:P0, type:editorial-bug, area:docs, area:concurrency
**Touches:** 09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md lines 334, 368, 379

**Problem**
Part 7 places section **7.12** before **7.10** and **7.11**, making the numbering non-monotonic and confusing to reference.

**Acceptance criteria**
- [ ] Reorder the sections so numbering is monotonic (7.10 → 7.11 → 7.12), OR renumber the affected sections to match the current ordering.
- [ ] Ensure cross references (e.g., §7.12.5 references) still resolve correctly.

## P0-19: Fix incomplete rule text in Part 6: `throw` static semantics ends with an orphaned “or”
**Labels:** priority:P0, type:definition-bug, area:errors, area:docs
**Touches:** 08_PART_6_ERRORS_RESULTS_THROWS.md lines 159–165 (§6.4.2)

**Problem**
Section **6.4.2** says “The thrown expression must be convertible to:” then lists `id<Error>` and ends with “or” before immediately starting the next heading. This reads like a half-removed typed-throws rule.

**Acceptance criteria**
- [ ] Either remove the dangling “or” and make the rule complete for untyped throws, OR add the missing second bullet if another case is intended.
- [ ] Add a doc-lint check for bullet lists that end with conjunctions like “or/and”.

## P0-20: Define what “recommended” and “normative intent” mean, and standardize markings across documents
**Labels:** priority:P0, type:spec-clarity, area:docs
**Touches:** 01C_LOWERING_AND_RUNTIME_CONTRACTS.md (many sections); Various parts use “normative intent”

**Problem**
The draft uses labels like “recommended”, “normative intent”, “minimum”, and “informative summary”. Without a clear legend, readers can’t tell which parts are required for conformance vs guidance.

**Acceptance criteria**
- [ ] Add a terminology legend (Part 0 or front matter) defining each tag and its conformance impact.
- [ ] Audit the spec and ensure every section is clearly tagged as normative vs informative vs recommended, with consistent wording.
- [ ] Update 01E checklist so it references only normative requirements (and explicitly calls out recommended QoI items separately).

## P1 — Major (should be resolved for v1 to be robust)
## P1-01: Decide whether version switching is translation-unit-only or can be per-region
**Labels:** priority:P1, type:design-decision, area:versioning
**Touches:** 03_PART_1_VERSIONING_COMPATIBILITY_CONFORMANCE.md line 36 (block quote)

**Problem**
Part 1 notes an open issue about per-region language-version switching. This impacts migration tooling and macro/header interop.

**Acceptance criteria**
- [ ] Make an explicit v1 decision (TU-only vs allow per-region).
- [ ] If per-region is allowed: specify semantic/ABI constraints and required tooling support.
- [ ] Add tests demonstrating the allowed form (or tests that reject it if TU-only).

## P1-02: Standardize optional sugar for `id` and `Class` (or explicitly forbid)
**Labels:** priority:P1, type:design-decision, area:types
**Touches:** 05_PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md §3.9 (open issue)

**Problem**
Part 3 open issue: whether to standardize `id?` / `Class?` sugar and how it maps to nullability qualifiers.

**Acceptance criteria**
- [ ] Decide on a spelling and type mapping for `id?`/`Class?` (or declare out-of-scope).
- [ ] Update the optional type sugar rules and diagnostics accordingly.
- [ ] Add conformance tests covering header import/export and module metadata.

## P1-03: Decide whether to introduce a first-class value Optional ABI in a future revision and reserve syntax accordingly
**Labels:** priority:P1, type:roadmap, area:abi, area:types
**Touches:** 05_PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md §3.9 (open issue)

**Problem**
Part 3 open issue about introducing a value Optional ABI (`Optional<T>`). Even if out-of-scope for v1, the spec should reserve syntax and avoid corner decisions that make it impossible later.

**Acceptance criteria**
- [ ] Document the reserved syntax/keywords needed to add value optionals later (if desired).
- [ ] Add a “future compatibility” note listing current v1 constraints that must not block value optionals.
- [ ] Add tests (or at least rules) preventing user code from occupying reserved spellings in ObjC 3.0 mode.

## P1-04: Decide whether and how to support generic methods/functions without breaking selector grammar
**Labels:** priority:P1, type:design-decision, area:generics
**Touches:** 05_PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md §3.9 (open issue)

**Problem**
Part 3 open issue: generic methods/functions are a common request but collide with ObjC selector and runtime dispatch conventions.

**Acceptance criteria**
- [ ] Document at least one viable design path (or explicitly reject for v1+v2).
- [ ] If accepted: specify syntax, mangling/lowering, and how it interacts with selectors/metadata.
- [ ] If rejected: reserve relevant keywords/syntax to avoid future incompatibility.

## P1-05: Decide whether to add a first-class Unmanaged wrapper type (or keep as library-only)
**Labels:** priority:P1, type:design-decision, area:memory
**Touches:** 06_PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md §4.9 (open issue)

**Problem**
Part 4 open issue: a first-class unmanaged wrapper can improve explicit retain/release interop and non-ARC contexts.

**Acceptance criteria**
- [ ] Decide if Unmanaged-like wrappers are standardized in v1, and in which module.
- [ ] If standardized: specify API surface, ownership rules, and interaction with ARC.
- [ ] Add tests for explicit retain/release operations under different conformance profiles.

## P1-06: Specify and test lifetime shortening behavior precisely (optimizer-facing contract)
**Labels:** priority:P1, type:spec-gap, area:memory, area:optimizer
**Touches:** 06_PART_4_MEMORY_MANAGEMENT_OWNERSHIP.md §4.9 (open issue); 14_PART_12_DIAGNOSTICS_TOOLING_TESTS.md

**Problem**
The spec references precise lifetime and lifetime shortening but does not yet spell out a portable, testable contract. Without it, optimizers may break resource cleanup or borrowed-pointer assumptions.

**Acceptance criteria**
- [ ] Define what lifetime shortening is permitted vs forbidden under each profile.
- [ ] Specify how `objc_precise_lifetime` interacts with `defer`, resources, and `await` suspension.
- [ ] Add optimizer-sensitive tests (e.g., compile with optimization on) to validate required behavior.

## P1-07: Decide whether `match` becomes an expression form in a future revision (and reserve syntax)
**Labels:** priority:P1, type:design-decision, area:control-flow
**Touches:** 07_PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md §5.7 (open issue)

**Problem**
Part 5 open issue: `match` as an expression affects typing rules, exhaustiveness checking, and lowering.

**Acceptance criteria**
- [ ] Document whether `match` will remain statement-only or may become an expression in v2+.
- [ ] If expression-form is planned: reserve the syntax/typing behavior now to avoid conflicts.
- [ ] Add tests ensuring current v1 behavior is unambiguous.

## P1-08: Standardize type-test patterns lowering and semantics (or keep optional and define portability expectations)
**Labels:** priority:P1, type:design-decision, area:patterns
**Touches:** 07_PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md §5.7 (open issue); §5.5.5 mentions optional in v1

**Problem**
Type-test patterns are marked optional. Optional features are fine, but the spec should define either a portability contract (feature test macros, metadata) or a v1 decision.

**Acceptance criteria**
- [ ] Decide whether type-test patterns are core in v1 or remain optional.
- [ ] If optional: define feature test macro(s), module metadata requirements, and diagnostics when used without support.
- [ ] If core: define lowering (runtime checks vs static reasoning) and conformance tests.

## P1-09: Decide on guarded patterns (`case pat where cond`) and keyword reservation
**Labels:** priority:P1, type:design-decision, area:patterns
**Touches:** 07_PART_5_CONTROL_FLOW_SAFETY_CONSTRUCTS.md §5.7 (open issue)

**Problem**
Guarded patterns require keyword and grammar commitments. If not implemented now, reserve necessary keywords/structure.

**Acceptance criteria**
- [ ] Make a decision: include in v1/v2 or explicitly defer.
- [ ] If deferred: reserve the keyword(s) and grammar slots needed to avoid future conflicts.
- [ ] Add parser tests ensuring current grammar doesn’t accidentally accept ambiguous forms.

## P1-10: Decide whether to standardize a “never throws” marker (or rely solely on absence of `throws`)
**Labels:** priority:P1, type:design-decision, area:errors
**Touches:** 08_PART_6_ERRORS_RESULTS_THROWS.md §6.12 (open issue)

**Problem**
Open issue: a marker for “never throws” could be useful in generic contexts, but it may be redundant. The spec should resolve this to avoid ecosystem fragmentation.

**Acceptance criteria**
- [ ] Decide whether a marker exists (attribute or type syntax) or not.
- [ ] If yes: specify semantics, metadata, and diagnostics.
- [ ] If no: document recommended patterns for expressing the concept without a marker.

## P1-11: Provide a standard library helper for nil→error mapping (or explicitly avoid as language sugar)
**Labels:** priority:P1, type:design-decision, area:errors, area:stdlib
**Touches:** 08_PART_6_ERRORS_RESULTS_THROWS.md §6.12 (open issue)

**Problem**
Open issue: mapping `nil` to a thrown error is common in ObjC code bases. If the language avoids implicit mapping, providing a standard helper improves ergonomics without baking semantics into the core language.

**Acceptance criteria**
- [ ] Specify the canonical helper name/signature(s) in the stdlib contract (P0-09).
- [ ] Define how it interacts with postfix propagation `?` and `try`.
- [ ] Add conformance tests for the helper’s behavior.

## P1-12: Decide whether to add typed throws in a future revision and reserve syntax
**Labels:** priority:P1, type:roadmap, area:errors, area:generics
**Touches:** 08_PART_6_ERRORS_RESULTS_THROWS.md §6.13.1 and §6.12 (open issue)

**Problem**
Typed throws has significant ABI/generics/interop implications. Even if deferred, the spec should reserve syntax and avoid choices that block it.

**Acceptance criteria**
- [ ] Document whether typed throws is a planned future feature or explicitly rejected.
- [ ] If planned: reserve syntax/metadata slots and document compatibility constraints.
- [ ] If rejected: document rationale and ensure no misleading partial syntax appears.

## P1-13: Decide whether to provide macro-based sugar for task spawning
**Labels:** priority:P1, type:design-decision, area:concurrency, area:macros
**Touches:** 09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md §7.11 (open issue)

**Problem**
Open issue: macro sugar could provide ergonomics (`task {}`-like) without freezing semantics too early, but it requires macro packaging and phase rules.

**Acceptance criteria**
- [ ] Decide if v1 includes a standard macro sugar for spawning.
- [ ] If yes: define the macro interface, expansion, and diagnostics; ensure it doesn’t lock in runtime semantics prematurely.
- [ ] Add tests showing consistent expansion and metadata emission.

## P1-14: Decide whether to expose actor reentrancy controls in the type system
**Labels:** priority:P1, type:design-decision, area:concurrency
**Touches:** 09_PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md §7.11 (open issue)

**Problem**
Open issue: explicit reentrancy controls (e.g., `nonreentrant`) could make actor invariants easier to reason about but add complexity.

**Acceptance criteria**
- [ ] Make a v1/v2 decision and reserve syntax if deferred.
- [ ] If adopted: define semantics, metadata, and interaction with `await` and cancellation.
- [ ] Add tests for both allowed and rejected patterns.

## P1-15: Finalize spelling for `@retainable_family` and mapping to existing Clang attributes
**Labels:** priority:P1, type:design-decision, area:system, area:interop
**Touches:** 10_PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md §8.10 (open issue); 01B catalog

**Problem**
Open issue: retainable families should have a final canonical spelling to avoid ecosystem fragmentation and ensure header interop.

**Acceptance criteria**
- [ ] Choose the final spelling and document canonical header forms in 01B.
- [ ] Specify how it maps (if at all) to existing Clang attributes.
- [ ] Add tests validating behavior under ARC and non-ARC.

## P1-16: Decide enforcement strategy for borrowed pointers (front-end vs analyzer) and define required diagnostics
**Labels:** priority:P1, type:design-decision, area:system, area:safety
**Touches:** 10_PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md §8.10 (open issue); §8.7.3 strict-system escape analysis

**Problem**
Borrowed pointers are only useful if enforcement is reliable. The draft raises whether enforcement is front-end only or requires analyzer support; this affects conformance claims and tooling requirements.

**Acceptance criteria**
- [ ] Decide required enforcement mechanism(s) for strict-system conformance.
- [ ] Define the minimum diagnostics that must be emitted and when.
- [ ] Add tests that cover escaping uses and false-positive avoidance.

## P1-17: Decide whether resource annotations can be applied to struct fields (RAII-like aggregates)
**Labels:** priority:P1, type:design-decision, area:system
**Touches:** 10_PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md §8.10 (open issue)

**Problem**
Extending resource cleanup to aggregates would materially improve ergonomics and correctness for low-level code, but it has ABI and lowering implications.

**Acceptance criteria**
- [ ] Decide if struct-field resources are in-scope for v1/v2.
- [ ] If in-scope: specify lowering, initialization rules, and cleanup order for aggregates.
- [ ] Add tests demonstrating initialization failure + partial cleanup ordering.

## P1-18: Decide on class-level “all members direct” attribute
**Labels:** priority:P1, type:design-decision, area:performance
**Touches:** 11_PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md §9.8 (open issue); 01C.8 lowering

**Problem**
Open issue: a class-level attribute would reduce boilerplate for performance-oriented APIs but needs clear interaction with overrides and ABI.

**Acceptance criteria**
- [ ] Decide whether the attribute is standardized in v1.
- [ ] If standardized: specify semantics, metadata, and lowering.
- [ ] Add tests for override legality and cross-module behavior.

## P1-19: Decide on an attribute that forces dynamic dispatch (debugging / swizzling-friendly)
**Labels:** priority:P1, type:design-decision, area:performance
**Touches:** 11_PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md §9.8 (open issue)

**Problem**
Open issue: forcing dynamic dispatch is useful for debugging and compatibility with swizzling-heavy code bases; if not standardized, vendors will invent incompatible spellings.

**Acceptance criteria**
- [ ] Decide whether a force-dynamic attribute exists in v1.
- [ ] If yes: specify semantics and interaction with `direct`/`final`/`sealed`.
- [ ] Add tests demonstrating dispatch behavior.

## P1-20: Decide macro/derive packaging format
**Labels:** priority:P1, type:design-decision, area:macros, area:tooling
**Touches:** 12_PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md §10.6 (open issue)

**Problem**
Without a standardized packaging/distribution format, macros/derives won’t be portable across toolchains and build systems.

**Acceptance criteria**
- [ ] Choose a packaging model (SwiftPM-like, compiler-integrated, build-system supplied) and document it.
- [ ] Specify how the compiler discovers and loads macros/derives in a toolchain-portable way.
- [ ] Add tests for macro discovery and reproducible builds.

## P1-21: Decide which derives are standardized in core vs shipped in a standard library module
**Labels:** priority:P1, type:design-decision, area:macros, area:stdlib
**Touches:** 12_PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md §10.6 (open issue)

**Problem**
Derives affect portability: if core derives differ between toolchains, source-level conformance is undermined.

**Acceptance criteria**
- [ ] List the derives that are required for Core conformance vs optional stdlib modules.
- [ ] Define feature test macros and metadata for optional derives.
- [ ] Add conformance tests for required derives.

## P1-22: Decide whether to standardize a canonical surface syntax for property behaviors beyond attributes
**Labels:** priority:P1, type:design-decision, area:macros, area:syntax
**Touches:** 12_PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md §10.6 (open issue)

**Problem**
Property behaviors/wrappers currently rely on attribute-style spellings. A canonical surface syntax could improve readability but must be reserved early to avoid conflicts.

**Acceptance criteria**
- [ ] Make a decision (v1 attributes only vs v2 syntax).
- [ ] If deferring: reserve syntax/keywords needed for future behavior blocks.
- [ ] Add parser tests ensuring compatibility.

## P1-23: Standardize a format for reporting conformance results across toolchains
**Labels:** priority:P1, type:tooling, area:conformance
**Touches:** 14_PART_12_DIAGNOSTICS_TOOLING_TESTS.md §12.7 (open issue); 01E checklist

**Problem**
If multiple toolchains claim conformance, consumers need a portable way to see exactly what is implemented and which profile is met.

**Acceptance criteria**
- [ ] Define a machine-readable conformance report format (JSON/YAML) listing profiles, optional features, versions, and known deviations.
- [ ] Add a required `--emit-objc3-conformance` (or similar) tooling surface for conforming toolchains (if desired).
- [ ] Add tests that validate report schema and content.

## P1-24: Clarify whether “strict concurrency checking” is a separate conformance level or a strictness sub-mode
**Labels:** priority:P1, type:design-decision, area:concurrency, area:conformance
**Touches:** 14_PART_12_DIAGNOSTICS_TOOLING_TESTS.md §12.7 (open issue); 01E checklist tags; 03_PART_1 conformance model

**Problem**
The draft uses both conformance profiles and strictness sub-modes; concurrency checking is currently ambiguous. This affects flags, documentation, and conformance claims.

**Acceptance criteria**
- [ ] Make an explicit decision and update Part 1 + 01E tag model accordingly.
- [ ] Define the canonical compiler flags or feature-test macros that correspond to the decision.
- [ ] Add tests ensuring the chosen mode/level behaves consistently.

## P2 — Quality / Editorial / Process (high leverage, lower urgency)
## P2-01: Add automated spec linting (CI) for section numbering, ordering, and structural hygiene
**Labels:** priority:P2, type:process, area:docs
**Touches:** All markdown files

**Problem**
Several numbering issues slipped in (duplicate sections, non-monotonic ordering, incomplete bullets). A lightweight linter prevents these from regressing and makes the spec easier to cite.

**Acceptance criteria**
- [ ] Add a script/CI check that enforces: unique section IDs per document; monotonic ordering; balanced code fences; no orphan list conjunctions; and valid cross references.
- [ ] Run the linter in CI and fail on violations.
- [ ] Document the lint rules in CONTRIBUTING.md (or similar).

## P2-02: Consolidate duplication of core rules across parts (single source of truth + explicit cross references)
**Labels:** priority:P2, type:docs, area:structure
**Touches:** 07_PART_5_CONTROL_FLOW_… and 10_PART_8_SYSTEM_… (defer/cleanup overlap); 01C lowering vs parts

**Problem**
Some rules appear in multiple places (e.g., `defer` + cleanup model). That’s not inherently wrong, but it increases the risk of drift.

**Acceptance criteria**
- [ ] Identify duplicated normative rules and select a canonical home section for each.
- [ ] Replace duplicates with cross references where feasible.
- [ ] Add a “cross-cutting index” section pointing to the canonical rule locations.

## P2-03: Expand examples and edge-case guidance for new punctuation-heavy features
**Labels:** priority:P2, type:docs, area:ergonomics
**Touches:** 05_PART_3 (?. / ??); 08_PART_6 (postfix ? / try?); 09_PART_7 (await requirement); 10_PART_8 (capture lists, borrowed pointers)

**Problem**
Features that introduce new punctuation (`?.`, postfix `?`, `??`, capture lists) are especially prone to misparsing and user confusion. More examples + expected diagnostics reduce friction.

**Acceptance criteria**
- [ ] Add at least 5 edge-case examples per feature, including ambiguous parses and required fix-its.
- [ ] Include examples that cross feature boundaries (e.g., `try await` + postfix `?` + `?.`).
- [ ] Add corresponding conformance tests for each example.

