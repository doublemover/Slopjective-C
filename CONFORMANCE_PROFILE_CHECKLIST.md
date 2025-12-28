# Objective‑C 3.0 — Conformance Profile Checklist
_Working draft v0.10 — last updated 2025-12-28_

This document defines **conformance profiles** for Objective‑C 3.0 v1 and provides a **checklist** for compiler/toolchain implementers (and, where relevant, SDK authors) to make conformance claims concrete and testable.

It is intentionally redundant with the rest of the specification: the goal is to let an implementer answer “Do we actually implement ObjC 3.0?” without reading every part end-to-end.

## E.1 How to use this checklist

### E.1.1 Conformance is a claim about a toolchain, not just a compiler
A conformance claim applies to the **toolchain bundle**:

- frontend (parser + type checker),
- code generator and optimizer,
- runtime support (Objective‑C runtime + concurrency runtime hooks where applicable),
- standard libraries/modules required by the language features,
- interface emission + module metadata support,
- diagnostics and migration tooling requirements.

### E.1.2 Tags
Checklist items are annotated with tags indicating which conformance profile requires them.

- **[CORE]** required for **ObjC 3.0 v1 Core**
- **[STRICT]** required for **ObjC 3.0 v1 Strict** (in addition to CORE)
- **[CONC]** required for **ObjC 3.0 v1 Strict Concurrency** (in addition to STRICT)
- **[SYSTEM]** required for **ObjC 3.0 v1 Strict System** (in addition to STRICT, and typically CONC)

Optional feature sets:

- **[OPT-META]** metaprogramming feature set (Part 10)
- **[OPT-SWIFT]** Swift interop feature set (Part 11, Swift-facing portions)
- **[OPT-CXX]** C++-enhanced interop feature set (Part 11, C++-specific portions)

### E.1.3 What it means to “pass”
A toolchain **passes** a profile when it satisfies *all* checklist items tagged for that profile, and when it can demonstrate (via tests) that required module metadata and runtime contracts behave correctly under **separate compilation**.

## E.2 Profile definitions (normative)

### E.2.1 ObjC 3.0 v1 Core
A toolchain claiming **ObjC 3.0 v1 Core** shall:

- implement the language mode selection and feature-test mechanisms (Part 1),
- implement modules/interface emission and required metadata preservation (Part 2, B, D),
- implement the v1 type safety surface: nullability defaults, optionals, pragmatic generics, key paths as specified (Part 3),
- implement memory-management and ARC interactions required by the above (Part 4),
- implement the control-flow constructs that are part of the v1 surface (Part 5),
- implement the error model (`throws`, `try`, bridging) (Part 6, C),
- implement the concurrency model (`async/await`, executors, cancellation, actors) sufficiently to compile and run correct programs (Part 7, C).

“Core” does **not** require the optional feature sets (metaprogramming macros, Swift overlays) unless separately claimed.

### E.2.2 ObjC 3.0 v1 Strict
A toolchain claiming **ObjC 3.0 v1 Strict** shall:

- support strictness selection (Part 1) and treat “strict-ill‑formed” constructs as errors,
- provide the required diagnostics and fix-its for migration (Part 12),
- ensure canonical spellings are emitted in textual interfaces (B, Part 2, Part 12).

### E.2.3 ObjC 3.0 v1 Strict Concurrency
A toolchain claiming **ObjC 3.0 v1 Strict Concurrency** shall:

- support strict concurrency checking selection (Part 1),
- enforce actor isolation, executor hops, and Sendable-like rules as specified (Part 7),
- provide diagnostics for common data-race hazards and isolation violations (Part 12),
- preserve concurrency-relevant metadata across modules (D Table A).

### E.2.4 ObjC 3.0 v1 Strict System
A toolchain claiming **ObjC 3.0 v1 Strict System** shall:

- support strict-system strictness selection (Part 1),
- implement the system-programming extensions in Part 8 (resources, borrowed pointers, capture lists) and their associated diagnostics,
- preserve borrowed/lifetime annotations across modules (B.8, D Table A),
- provide at least intra-procedural enforcement for borrowed-pointer escape analysis and resource use-after-move patterns (Part 8, Part 12).

“Strict System” is intended for SDKs and codebases like IOKit, DriverKit, and other “handle heavy” or ownership-sensitive domains.

## E.3 Checklist

### E.3.1 Language mode selection, feature tests, and versioning
- [ ] **[CORE]** Provide a language mode selection mechanism equivalent to `-fobjc-version=3`. (Part 1 §1.2)
- [ ] **[CORE]** Provide a source-level mechanism (pragma or directive) to set ObjC 3.0 mode for a translation unit, or document that only the flag form is supported. (Part 1)
- [ ] **[CORE]** Provide feature test macros for major feature groups (optionals, throws, async/await, actors, direct/final/sealed, derives/macros if implemented). (Part 1 §1.4)
- [ ] **[STRICT]** Provide strictness selection equivalent to `-fobjc3-strictness=permissive|strict|strict-system`. (Part 1 §1.5)
- [ ] **[CONC]** Provide concurrency checking selection equivalent to `-fobjc3-concurrency=strict|off`. (Part 1 §1.6)

### E.3.2 Modules, namespacing, and interface emission
- [ ] **[CORE]** Implement module-aware compilation for Objective‑C declarations sufficient to support stable import, name lookup, and diagnostics. (Part 2)
- [ ] **[CORE]** Provide a textual interface emission mode (or equivalent) that can be used for verification in CI. (Part 2, Part 12)
- [ ] **[CORE]** Emit **canonical spellings** for ObjC 3.0 features in textual interfaces, per B. (B.1, B.7; Part 2)
- [ ] **[CORE]** Preserve and record all “must preserve” metadata in D Table A. (D.3.1)
- [ ] **[STRICT]** Provide an interface verification mode that detects mismatch between compiled module metadata and emitted textual interface. (Part 12)

### E.3.3 Type system: nullability defaults, optionals, generics, key paths
- [ ] **[CORE]** Support nullability qualifiers and treat them as part of the type system in ObjC 3.0 mode. (Part 3)
- [ ] **[CORE]** Support nonnull-by-default regions with canonical pragma spellings (B.2). (Part 3, B.2)
- [ ] **[STRICT]** Diagnose missing nullability where required by strictness level; provide fix-its. (Part 12)
- [ ] **[CORE]** Support optional types `T?` and IUO `T!` where specified, including:
  - optional binding (`if let`, `guard let`) (Part 3, Part 5),
  - optional chaining / optional message send `[receiver? sel]` (Part 3, C.3.1),
  - postfix propagation `expr?` per carrier rules (Part 3; Part 6; C.3.3).
- [ ] **[CORE]** Enforce v1 restriction that optional chaining is reference-only for member returns (D‑001; Part 3).
- [ ] **[CORE]** Preserve optional and nullability information in module metadata and textual interfaces (D Table A).
- [ ] **[CORE]** Support pragmatic generics on types (erased at runtime but checked by compiler) as specified. (Part 3)
- [ ] **[CORE]** Defer generic methods/functions (do not implement, or gate behind an extension flag) per D‑008. (Part 3; Decisions Log)
- [ ] **[CORE]** Support key path literal and typing rules to the extent specified by Part 3, or clearly mark as unsupported if still provisional and do not claim the feature macro. (Part 3)

### E.3.4 Memory management and lifetime
- [ ] **[CORE]** Preserve ObjC ARC semantics and ensure new language features lower without violating ARC rules. (Part 4)
- [ ] **[CORE]** Implement the suspension-point autorelease pool contract (D‑006; C.7; Part 7). This includes:
  - creating an implicit pool for each async “slice” as specified,
  - draining it at each suspension point.
- [ ] **[STRICT]** Provide diagnostics for suspicious lifetime extensions, escaping of stack-bound resources, and unsafe bridging where specified. (Part 4, Part 12)

### E.3.5 Control flow and safety constructs
- [ ] **[CORE]** Implement `defer` with LIFO scope-exit semantics (Part 5; Part 8 §8.2).
- [ ] **[CORE]** Ensure `defer` executes on normal exit and stack unwinding exits (where applicable) as specified. (Part 5/8)
- [ ] **[CORE]** Implement `guard` and refinement rules for optionals/patterns. (Part 5)
- [ ] **[CORE]** Implement `match` (pattern matching) to the extent specified, or clearly mark as provisional and do not claim a feature macro if not implemented. (Part 5)
- [ ] **[STRICT]** Diagnose illegal non-local exits from `defer` bodies (Part 5/8) and other ill-formed constructs.

### E.3.6 Errors and `throws`
- [ ] **[CORE]** Implement untyped `throws` as the v1 model (D‑002; Part 6).
- [ ] **[CORE]** Provide a stable ABI/lowering model for `throws` (D‑009; C.4; D Table B).
- [ ] **[CORE]** Support `try`, `do/catch`, and propagation rules that integrate with optionals as specified. (Part 6)
- [ ] **[CORE]** Support NSError/status-code bridging attributes where specified (B.4; Part 6), including module metadata preservation (D Table A).
- [ ] **[STRICT]** Provide diagnostics for ignored errors, missing `try`, and invalid bridging patterns. (Part 12)

### E.3.7 Concurrency: `async/await`, executors, cancellation, actors
- [ ] **[CORE]** Implement `async` and `await` grammar and typing rules as specified. (Part 7)
- [ ] **[CORE]** Implement a coroutine-based lowering model for `async` (D‑010; C.5) and preserve required ABI metadata (D Table B).
- [ ] **[CORE]** Implement cancellation propagation and task-context behavior as specified. (Part 7; C.5.2)
- [ ] **[CORE]** Implement executor affinity annotations and call-site behavior:
  - accept canonical `objc_executor(...)` spellings (B.3.1),
  - preserve in module metadata (D Table A),
  - perform required hops as specified. (Part 7; C.5.3)
- [ ] **[CORE]** Implement actor declarations and actor isolation rules (Part 7; C.6).
- [ ] **[STRICT]** Provide diagnostics for isolation violations, invalid executor annotations, and suspicious cross-actor calls. (Part 12)
- [ ] **[CONC]** Enforce Sendable-like constraints for:
  - captured values in `async` blocks/tasks,
  - parameters/returns across actor boundaries,
  - values crossing executor domains. (Part 7; D Table A)
- [ ] **[CONC]** Enforce D‑011: require `await` for any potentially-suspending operation, not only explicit `async` calls. (Decisions Log; Part 7)

### E.3.8 System programming extensions (Part 8)
- [ ] **[SYSTEM]** Support canonical attribute spellings for Part 8 features (B.8), including:
  - `objc_resource(close=..., invalid=...)`,
  - `objc_returns_borrowed(owner_index=...)`,
  - `borrowed T *` type qualifier,
  - capture list contextual keywords.
- [ ] **[SYSTEM]** Implement resource cleanup semantics and associated diagnostics (Part 8 §8.3; Part 12 §12.3.6).
- [ ] **[SYSTEM]** Implement `withLifetime` / `keepAlive` semantics and ensure they interact correctly with ARC. (Part 8 §8.6)
- [ ] **[SYSTEM]** Implement borrowed pointer rules and diagnostics (Part 8 §8.7) at least intra-procedurally.
- [ ] **[SYSTEM]** Preserve borrowed/lifetime annotations in module metadata (D Table A).
- [ ] **[SYSTEM]** Implement capture lists (Part 8 §8.8) with required evaluation order and move/weak/unowned semantics.
- [ ] **[SYSTEM]** Provide diagnostics for:
  - borrowed escape to heap/global/ivar,
  - borrowed escape into `@escaping` blocks,
  - use-after-move for resources,
  - dangerous `unowned` captures. (Part 12)

### E.3.9 Performance and dynamism controls
- [ ] **[CORE]** Accept and preserve canonical spellings for `objc_direct`, `objc_final`, `objc_sealed` (B.5; Part 9).
- [ ] **[CORE]** Enforce legality rules across categories/extensions and module boundaries as specified. (Part 9; C.8; D)
- [ ] **[STRICT]** Provide diagnostics for calling direct methods via dynamic dispatch, illegal overrides of final/sealed, and related misuse. (Part 12)

### E.3.10 Metaprogramming (optional feature set)
- [ ] **[OPT-META]** Implement derives (`objc_derive(...)`) with deterministic, tool-visible expansion. (Part 10; B.6.1)
- [ ] **[OPT-META]** Implement macros (`objc_macro(...)`) with sandboxing / safety constraints as specified. (Part 10; B.6.2)
- [ ] **[OPT-META]** Preserve macro/derive expansions in module metadata and textual interfaces as required by D. (D Table A)

### E.3.11 Interoperability (optional feature sets)
- [ ] **[CORE]** Maintain full interop with C and Objective‑C runtime behavior. (Baseline + Part 11)
- [ ] **[OPT-CXX]** Document and test ObjC++ interactions for ownership, `throws`, and `async` lowering. (Part 11; C)
- [ ] **[OPT-SWIFT]** Provide a Swift interop story (import/export) for:
  - optionals/nullability,
  - `throws` bridging,
  - `async/await` bridging,
  - actors and isolation metadata. (Part 11)

### E.3.12 Diagnostics, tooling, and tests
- [ ] **[CORE]** Implement the minimum diagnostic groups in Part 12:
  - nullability/optionals,
  - throws,
  - concurrency,
  - modules/interface emission,
  - performance controls. (Part 12 §12.3)
- [ ] **[STRICT]** Provide fix-its and migrator support to move legacy code toward canonical spellings and safer idioms. (Part 12 §12.4)
- [ ] **[CORE]** Provide or publish a conformance test suite covering:
  - parsing/grammar,
  - type system and diagnostics,
  - dynamic semantics,
  - runtime contracts (throws/async lowering),
  - metadata/interface preservation. (Part 12 §12.5)
- [ ] **[SYSTEM]** Include tests for borrowed-pointer escape diagnostics and resource cleanup semantics. (Part 8; Part 12)

## E.4 Recommended evidence for a conformance claim (non-normative)
A serious conformance claim should ship with:

- a public “conformance manifest” listing enabled profiles and feature-test macros,
- CI proofs that:
  - module interfaces round-trip (emit → import) without semantic loss,
  - D Table A metadata is preserved under separate compilation,
  - runtime contracts for `throws` and `async` behave correctly under optimization,
- migration tooling notes for large codebases (warning groups, fix-its, staged adoption).

