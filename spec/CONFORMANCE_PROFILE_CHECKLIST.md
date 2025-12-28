# Objective‑C 3.0 — Conformance Profile Checklist {#e}

_Working draft v0.10 — last updated 2025-12-28_

This document defines **conformance profiles** for Objective‑C 3.0 v1 and provides a **checklist** for compiler/toolchain implementers (and, where relevant, SDK authors) to make conformance claims concrete and testable.

It is intentionally redundant with the rest of the specification: the goal is to let an implementer answer “Do we actually implement ObjC 3.0?” without reading every part end-to-end.

## E.1 How to use this checklist {#e-1}

### E.1.1 Conformance is a claim about a toolchain, not just a compiler {#e-1-1}

A conformance claim applies to the **toolchain bundle**:

- frontend (parser + type checker),
- code generator and optimizer,
- runtime support (Objective‑C runtime + concurrency runtime hooks where applicable),
- standard libraries/modules required by the language features,
- interface emission + module metadata support,
- diagnostics and migration tooling requirements.

### E.1.2 Tags {#e-1-2}

Checklist items are annotated with tags indicating which conformance profile requires them.

- **[CORE]** required for **ObjC 3.0 v1 Core**
- **[STRICT]** required for **ObjC 3.0 v1 Strict** (in addition to CORE)
- **[CONC]** required for **ObjC 3.0 v1 Strict Concurrency** (in addition to STRICT)
- **[SYSTEM]** required for **ObjC 3.0 v1 Strict System** (in addition to STRICT, and typically CONC)

Optional feature sets:

- **[OPT-META]** metaprogramming feature set ([Part 10](#part-10))
- **[OPT-SWIFT]** Swift interop feature set ([Part 11](#part-11), Swift-facing portions)
- **[OPT-CXX]** C++-enhanced interop feature set ([Part 11](#part-11), C++-specific portions)

### E.1.3 What it means to “pass” {#e-1-3}

A toolchain **passes** a profile when it satisfies _all_ checklist items tagged for that profile, and when it can demonstrate (via tests) that required module metadata and runtime contracts behave correctly under **separate compilation**.

## E.2 Profile definitions (normative) {#e-2}

### E.2.1 ObjC 3.0 v1 Core {#e-2-1}

A toolchain claiming **ObjC 3.0 v1 Core** shall:

- implement the language mode selection and feature-test mechanisms ([Part 1](#part-1)),
- implement modules/interface emission and required metadata preservation ([Part 2](#part-2), [B](#b), [D](#d)),
- implement the v1 type safety surface: nullability defaults, optionals, pragmatic generics, key paths as specified ([Part 3](#part-3)),
- implement memory-management and ARC interactions required by the above ([Part 4](#part-4)),
- implement the control-flow constructs that are part of the v1 surface ([Part 5](#part-5)),
- implement the error model (`throws`, `try`, bridging) ([Part 6](#part-6), [C](#c)),
- implement the concurrency model (`async/await`, executors, cancellation, actors) sufficiently to compile and run correct programs ([Part 7](#part-7), [C](#c)).

“Core” does **not** require the optional feature sets (metaprogramming macros, Swift overlays) unless separately claimed.

### E.2.2 ObjC 3.0 v1 Strict {#e-2-2}

A toolchain claiming **ObjC 3.0 v1 Strict** shall:

- support strictness selection ([Part 1](#part-1)) and treat “strict-ill‑formed” constructs as errors,
- provide the required diagnostics and fix-its for migration ([Part 12](#part-12)),
- ensure canonical spellings are emitted in textual interfaces ([B](#b), [Part 2](#part-2), [Part 12](#part-12)).

### E.2.3 ObjC 3.0 v1 Strict Concurrency {#e-2-3}

A toolchain claiming **ObjC 3.0 v1 Strict Concurrency** shall:

- support strict concurrency checking selection ([Part 1](#part-1)),
- enforce actor isolation, executor hops, and Sendable-like rules as specified ([Part 7](#part-7)),
- provide diagnostics for common data-race hazards and isolation violations ([Part 12](#part-12)),
- preserve concurrency-relevant metadata across modules ([D Table A](#d-3-1)).

### E.2.4 ObjC 3.0 v1 Strict System {#e-2-4}

A toolchain claiming **ObjC 3.0 v1 Strict System** shall:

- support strict-system strictness selection ([Part 1](#part-1)),
- implement the system-programming extensions in [Part 8](#part-8) (resources, borrowed pointers, capture lists) and their associated diagnostics,
- preserve borrowed/lifetime annotations across modules ([B.8](#b-8), [D Table A](#d-3-1)),
- provide at least intra-procedural enforcement for borrowed-pointer escape analysis and resource use-after-move patterns ([Part 8](#part-8), [Part 12](#part-12)).

“Strict System” is intended for SDKs and codebases like IOKit, DriverKit, and other “handle heavy” or ownership-sensitive domains.

## E.3 Checklist {#e-3}

### E.3.1 Language mode selection, feature tests, and versioning {#e-3-1}

- [ ] **[CORE]** Provide a language mode selection mechanism equivalent to `-fobjc-version=3`. ([Part 1](#part-1) [§1.2](#part-1-2)) ([Issue #48](https://github.com/doublemover/Slopjective-C/issues/48))
- [ ] **[CORE]** Provide a source-level mechanism (pragma or directive) to set ObjC 3.0 mode for a translation unit, or document that only the flag form is supported. ([Part 1](#part-1)) ([Issue #49](https://github.com/doublemover/Slopjective-C/issues/49))
- [ ] **[CORE]** Provide feature test macros for major feature groups (optionals, throws, async/await, actors, direct/final/sealed, derives/macros if implemented). ([Part 1](#part-1) [§1.4](#part-1-4)) ([Issue #50](https://github.com/doublemover/Slopjective-C/issues/50))
- [ ] **[STRICT]** Provide strictness selection equivalent to `-fobjc3-strictness=permissive|strict|strict-system`. ([Part 1](#part-1) [§1.5](#part-1-5)) ([Issue #51](https://github.com/doublemover/Slopjective-C/issues/51))
- [ ] **[CONC]** Provide concurrency checking selection equivalent to `-fobjc3-concurrency=strict|off`. ([Part 1](#part-1) [§1.6](#part-1-6)) ([Issue #52](https://github.com/doublemover/Slopjective-C/issues/52))

### E.3.2 Modules, namespacing, and interface emission {#e-3-2}

- [ ] **[CORE]** Implement module-aware compilation for Objective‑C declarations sufficient to support stable import, name lookup, and diagnostics. ([Part 2](#part-2)) ([Issue #53](https://github.com/doublemover/Slopjective-C/issues/53))
- [ ] **[CORE]** Provide a textual interface emission mode (or equivalent) that can be used for verification in CI. ([Part 2](#part-2), [Part 12](#part-12)) ([Issue #54](https://github.com/doublemover/Slopjective-C/issues/54))
- [ ] **[CORE]** Emit **canonical spellings** for ObjC 3.0 features in textual interfaces, per [B](#b). ([B.1](#b-1), [B.7](#b-7); [Part 2](#part-2)) ([Issue #55](https://github.com/doublemover/Slopjective-C/issues/55))
- [ ] **[CORE]** Preserve and record all “must preserve” metadata in [D Table A](#d-3-1). ([D.3.1](#d-3-1)) ([Issue #56](https://github.com/doublemover/Slopjective-C/issues/56))
- [ ] **[STRICT]** Provide an interface verification mode that detects mismatch between compiled module metadata and emitted textual interface. ([Part 12](#part-12)) ([Issue #57](https://github.com/doublemover/Slopjective-C/issues/57))

### E.3.3 Type system: nullability defaults, optionals, generics, key paths {#e-3-3}

- [ ] **[CORE]** Support nullability qualifiers and treat them as part of the type system in ObjC 3.0 mode. ([Part 3](#part-3)) ([Issue #58](https://github.com/doublemover/Slopjective-C/issues/58))
- [ ] **[CORE]** Support nonnull-by-default regions with canonical pragma spellings ([B.2](#b-2)). ([Part 3](#part-3), [B.2](#b-2)) ([Issue #59](https://github.com/doublemover/Slopjective-C/issues/59))
- [ ] **[STRICT]** Diagnose missing nullability where required by strictness level; provide fix-its. ([Part 12](#part-12)) ([Issue #60](https://github.com/doublemover/Slopjective-C/issues/60))
- [ ] **[CORE]** Support optional types `T?` and IUO `T!` where specified, including: ([Issue #61](https://github.com/doublemover/Slopjective-C/issues/61))
  - optional binding (`if let`, `guard let`) ([Part 3](#part-3), [Part 5](#part-5)),
  - optional chaining / optional message send `[receiver? sel]` ([Part 3](#part-3), [C.3.1](#c-3-1)),
  - postfix propagation `expr?` per carrier rules ([Part 3](#part-3); [Part 6](#part-6); [C.3.3](#c-3-3)).
- [ ] **[CORE]** Enforce v1 restriction that optional chaining is reference-only for member returns ([D-001](#decisions-d-001); [Part 3](#part-3)). ([Issue #62](https://github.com/doublemover/Slopjective-C/issues/62))
- [ ] **[CORE]** Preserve optional and nullability information in module metadata and textual interfaces ([D Table A](#d-3-1)). ([Issue #63](https://github.com/doublemover/Slopjective-C/issues/63))
- [ ] **[CORE]** Support pragmatic generics on types (erased at runtime but checked by compiler) as specified. ([Part 3](#part-3)) ([Issue #64](https://github.com/doublemover/Slopjective-C/issues/64))
- [ ] **[CORE]** Defer generic methods/functions (do not implement, or gate behind an extension flag) per [D-008](#decisions-d-008). ([Part 3](#part-3); Decisions Log) ([Issue #65](https://github.com/doublemover/Slopjective-C/issues/65))
- [ ] **[CORE]** Support key path literal and typing rules to the extent specified by [Part 3](#part-3), or clearly mark as unsupported if still provisional and do not claim the feature macro. ([Part 3](#part-3)) ([Issue #66](https://github.com/doublemover/Slopjective-C/issues/66))

### E.3.4 Memory management and lifetime {#e-3-4}

- [ ] **[CORE]** Preserve ObjC ARC semantics and ensure new language features lower without violating ARC rules. ([Part 4](#part-4)) ([Issue #67](https://github.com/doublemover/Slopjective-C/issues/67))
- [ ] **[CORE]** Implement the suspension-point autorelease pool contract ([D-006](#decisions-d-006); [C.7](#c-7); [Part 7](#part-7)). This includes: ([Issue #68](https://github.com/doublemover/Slopjective-C/issues/68))
  - creating an implicit pool for each async “slice” as specified,
  - draining it at each suspension point.
- [ ] **[STRICT]** Provide diagnostics for suspicious lifetime extensions, escaping of stack-bound resources, and unsafe bridging where specified. ([Part 4](#part-4), [Part 12](#part-12)) ([Issue #69](https://github.com/doublemover/Slopjective-C/issues/69))

### E.3.5 Control flow and safety constructs {#e-3-5}

- [ ] **[CORE]** Implement `defer` with LIFO scope-exit semantics ([Part 5](#part-5); [Part 8](#part-8) [§8.2](#part-8-2)). ([Issue #70](https://github.com/doublemover/Slopjective-C/issues/70))
- [ ] **[CORE]** Ensure `defer` executes on normal exit and stack unwinding exits (where applicable) as specified. ([Part 5](#part-5)/8) ([Issue #71](https://github.com/doublemover/Slopjective-C/issues/71))
- [ ] **[CORE]** Implement `guard` and refinement rules for optionals/patterns. ([Part 5](#part-5)) ([Issue #72](https://github.com/doublemover/Slopjective-C/issues/72))
- [ ] **[CORE]** Implement `match` (pattern matching) to the extent specified, or clearly mark as provisional and do not claim a feature macro if not implemented. ([Part 5](#part-5)) ([Issue #73](https://github.com/doublemover/Slopjective-C/issues/73))
- [ ] **[STRICT]** Diagnose illegal non-local exits from `defer` bodies ([Part 5](#part-5)/8) and other ill-formed constructs. ([Issue #74](https://github.com/doublemover/Slopjective-C/issues/74))

### E.3.6 Errors and `throws` {#e-3-6}

- [ ] **[CORE]** Implement untyped `throws` as the v1 model ([D-002](#decisions-d-002); [Part 6](#part-6)). ([Issue #75](https://github.com/doublemover/Slopjective-C/issues/75))
- [ ] **[CORE]** Provide a stable ABI/lowering model for `throws` ([D-009](#decisions-d-009); [C.4](#c-4); [D Table B](#d-3-2)). ([Issue #76](https://github.com/doublemover/Slopjective-C/issues/76))
- [ ] **[CORE]** Support `try`, `do/catch`, and propagation rules that integrate with optionals as specified. ([Part 6](#part-6)) ([Issue #77](https://github.com/doublemover/Slopjective-C/issues/77))
- [ ] **[CORE]** Support NSError/status-code bridging attributes where specified ([B.4](#b-4); [Part 6](#part-6)), including module metadata preservation ([D Table A](#d-3-1)). ([Issue #78](https://github.com/doublemover/Slopjective-C/issues/78))
- [ ] **[STRICT]** Provide diagnostics for ignored errors, missing `try`, and invalid bridging patterns. ([Part 12](#part-12)) ([Issue #79](https://github.com/doublemover/Slopjective-C/issues/79))

### E.3.7 Concurrency: `async/await`, executors, cancellation, actors {#e-3-7}

- [ ] **[CORE]** Implement `async` and `await` grammar and typing rules as specified. ([Part 7](#part-7)) ([Issue #80](https://github.com/doublemover/Slopjective-C/issues/80))
- [ ] **[CORE]** Implement a coroutine-based lowering model for `async` ([D-010](#decisions-d-010); [C.5](#c-5)) and preserve required ABI metadata ([D Table B](#d-3-2)). ([Issue #81](https://github.com/doublemover/Slopjective-C/issues/81))
- [ ] **[CORE]** Implement cancellation propagation and task-context behavior as specified. ([Part 7](#part-7); [C.5.2](#c-5-2)) ([Issue #82](https://github.com/doublemover/Slopjective-C/issues/82))
- [ ] **[CORE]** Implement executor affinity annotations and call-site behavior: ([Issue #83](https://github.com/doublemover/Slopjective-C/issues/83))
  - accept canonical `objc_executor(...)` spellings ([B.3.1](#b-3-1)),
  - preserve in module metadata ([D Table A](#d-3-1)),
  - perform required hops as specified. ([Part 7](#part-7); [C.5.3](#c-5-3))
- [ ] **[CORE]** Implement actor declarations and actor isolation rules ([Part 7](#part-7); [C.6](#c-6)). ([Issue #84](https://github.com/doublemover/Slopjective-C/issues/84))
- [ ] **[STRICT]** Provide diagnostics for isolation violations, invalid executor annotations, and suspicious cross-actor calls. ([Part 12](#part-12)) ([Issue #85](https://github.com/doublemover/Slopjective-C/issues/85))
- [ ] **[CONC]** Enforce Sendable-like constraints for: ([Issue #86](https://github.com/doublemover/Slopjective-C/issues/86))
  - captured values in `async` blocks/tasks,
  - parameters/returns across actor boundaries,
  - values crossing executor domains. ([Part 7](#part-7); [D Table A](#d-3-1))
- [ ] **[CONC]** Enforce [D-011](#decisions-d-011): require `await` for any potentially-suspending operation, not only explicit `async` calls. (Decisions Log; [Part 7](#part-7)) ([Issue #87](https://github.com/doublemover/Slopjective-C/issues/87))

### E.3.8 System programming extensions (Part 8) {#e-3-8}

- [ ] **[SYSTEM]** Support canonical attribute spellings for [Part 8](#part-8) features ([B.8](#b-8)), including: ([Issue #88](https://github.com/doublemover/Slopjective-C/issues/88))
  - `objc_resource(close=..., invalid=...)`,
  - `objc_returns_borrowed(owner_index=...)`,
  - `borrowed T *` type qualifier,
  - capture list contextual keywords.
- [ ] **[SYSTEM]** Implement resource cleanup semantics and associated diagnostics ([Part 8](#part-8) [§8.3](#part-8-3); [Part 12](#part-12) [§12.3.6](#part-12-3-6)). ([Issue #89](https://github.com/doublemover/Slopjective-C/issues/89))
- [ ] **[SYSTEM]** Implement `withLifetime` / `keepAlive` semantics and ensure they interact correctly with ARC. ([Part 8](#part-8) [§8.6](#part-8-6)) ([Issue #90](https://github.com/doublemover/Slopjective-C/issues/90))
- [ ] **[SYSTEM]** Implement borrowed pointer rules and diagnostics ([Part 8](#part-8) [§8.7](#part-8-7)) at least intra-procedurally. ([Issue #91](https://github.com/doublemover/Slopjective-C/issues/91))
- [ ] **[SYSTEM]** Preserve borrowed/lifetime annotations in module metadata ([D Table A](#d-3-1)). ([Issue #92](https://github.com/doublemover/Slopjective-C/issues/92))
- [ ] **[SYSTEM]** Implement capture lists ([Part 8](#part-8) [§8.8](#part-8-8)) with required evaluation order and move/weak/unowned semantics. ([Issue #93](https://github.com/doublemover/Slopjective-C/issues/93))
- [ ] **[SYSTEM]** Provide diagnostics for: ([Issue #94](https://github.com/doublemover/Slopjective-C/issues/94))
  - borrowed escape to heap/global/ivar,
  - borrowed escape into `@escaping` blocks,
  - use-after-move for resources,
  - dangerous `unowned` captures. ([Part 12](#part-12))

### E.3.9 Performance and dynamism controls {#e-3-9}

- [ ] **[CORE]** Accept and preserve canonical spellings for `objc_direct`, `objc_final`, `objc_sealed` ([B.5](#b-5); [Part 9](#part-9)). ([Issue #95](https://github.com/doublemover/Slopjective-C/issues/95))
- [ ] **[CORE]** Enforce legality rules across categories/extensions and module boundaries as specified. ([Part 9](#part-9); [C.8](#c-8); [D](#d)) ([Issue #96](https://github.com/doublemover/Slopjective-C/issues/96))
- [ ] **[STRICT]** Provide diagnostics for calling direct methods via dynamic dispatch, illegal overrides of final/sealed, and related misuse. ([Part 12](#part-12)) ([Issue #97](https://github.com/doublemover/Slopjective-C/issues/97))

### E.3.10 Metaprogramming (optional feature set) {#e-3-10}

- [ ] **[OPT-META]** Implement derives (`objc_derive(...)`) with deterministic, tool-visible expansion. ([Part 10](#part-10); [B.6.1](#b-6-1)) ([Issue #98](https://github.com/doublemover/Slopjective-C/issues/98))
- [ ] **[OPT-META]** Implement macros (`objc_macro(...)`) with sandboxing / safety constraints as specified. ([Part 10](#part-10); [B.6.2](#b-6-2)) ([Issue #99](https://github.com/doublemover/Slopjective-C/issues/99))
- [ ] **[OPT-META]** Preserve macro/derive expansions in module metadata and textual interfaces as required by D. ([D Table A](#d-3-1)) ([Issue #100](https://github.com/doublemover/Slopjective-C/issues/100))

### E.3.11 Interoperability (optional feature sets) {#e-3-11}

- [ ] **[CORE]** Maintain full interop with C and Objective‑C runtime behavior. (Baseline + [Part 11](#part-11)) ([Issue #101](https://github.com/doublemover/Slopjective-C/issues/101))
- [ ] **[OPT-CXX]** Document and test ObjC++ interactions for ownership, `throws`, and `async` lowering. ([Part 11](#part-11); C) ([Issue #102](https://github.com/doublemover/Slopjective-C/issues/102))
- [ ] **[OPT-SWIFT]** Provide a Swift interop story (import/export) for: ([Issue #103](https://github.com/doublemover/Slopjective-C/issues/103))
  - optionals/nullability,
  - `throws` bridging,
  - `async/await` bridging,
  - actors and isolation metadata. ([Part 11](#part-11))

### E.3.12 Diagnostics, tooling, and tests {#e-3-12}

- [ ] **[CORE]** Implement the minimum diagnostic groups in [Part 12](#part-12): ([Issue #104](https://github.com/doublemover/Slopjective-C/issues/104))
  - nullability/optionals,
  - throws,
  - concurrency,
  - modules/interface emission,
  - performance controls. ([Part 12](#part-12) [§12.3](#part-12-3))
- [ ] **[STRICT]** Provide fix-its and migrator support to move legacy code toward canonical spellings and safer idioms. ([Part 12](#part-12) [§12.4](#part-12-4)) ([Issue #105](https://github.com/doublemover/Slopjective-C/issues/105))
- [ ] **[CORE]** Provide or publish a conformance test suite covering: ([Issue #106](https://github.com/doublemover/Slopjective-C/issues/106))
  - parsing/grammar,
  - type system and diagnostics,
  - dynamic semantics,
  - runtime contracts (throws/async lowering),
  - metadata/interface preservation. ([Part 12](#part-12) [§12.5](#part-12-5))
- [ ] **[SYSTEM]** Include tests for borrowed-pointer escape diagnostics and resource cleanup semantics. ([Part 8](#part-8); [Part 12](#part-12)) ([Issue #107](https://github.com/doublemover/Slopjective-C/issues/107))

## E.4 Recommended evidence for a conformance claim (non-normative) {#e-4}

A serious conformance claim should ship with:

- a public “conformance manifest” listing enabled profiles and feature-test macros,
- CI proofs that:
  - module interfaces round-trip (emit → import) without semantic loss,
  - [D Table A](#d-3-1) metadata is preserved under separate compilation,
  - runtime contracts for `throws` and `async` behave correctly under optimization,
- migration tooling notes for large codebases (warning groups, fix-its, staged adoption).
