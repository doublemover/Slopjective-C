# E.3 Concurrency/System/Optional Implementation Workpack (Issues #80-#107)

This workpack covers remaining Section-E implementation issues #80 through #107 for E.3.7-E.3.12.
Each checkbox is an executable issue task and is complete only when all done criteria are satisfied with linked evidence.

## Execution Rules

- Keep references pinned to existing spec anchors in `spec/*.md`.
- Place conformance tests in required buckets under `tests/conformance/`.
- Link commit SHA plus test evidence in each issue before closure.

## E.3.7 Concurrency: async/await, executors, cancellation, actors

- [x] **Issue #80 - Implement `async` and `await` grammar and typing rules**
  - Spec anchors: [E.3.7](../../../spec/CONFORMANCE_PROFILE_CHECKLIST.md#e-3-7), [Part 7](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7), [7.1](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-1), [7.2](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-2), [7.3](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-3).
  - Objective: Accept normative `async` declaration and `await` expression forms, and enforce effect typing rules during semantic analysis.
  - Required artifacts: parser grammar updates for `async`/`await`; semantic checks for async contexts and effect typing; stable diagnostics for invalid placements; tests in `tests/conformance/parser/`, `tests/conformance/semantic/`, and `tests/conformance/diagnostics/`.
  - Test IDs: `ASY-01`..`ASY-06`.
  - Dependencies: None.
  - Done criteria: Valid forms compile; invalid forms fail with stable diagnostic metadata (`code`, `severity`, `span`); all listed tests pass in CI.

- [x] **Issue #81 - Implement coroutine-based lowering for `async` and preserve ABI metadata**
  - Spec anchors: [D-010](../../../spec/DECISIONS_LOG.md#decisions-d-010), [C.5](../../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md#c-5), [Part 7 7.12.2](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-12-2), [D Table B](../../../spec/MODULE_METADATA_AND_ABI_TABLES.md#d-3-2).
  - Objective: Lower async functions to the v1 coroutine model and maintain ABI-stable call/lowering contracts across separate compilation.
  - Required artifacts: lowering pipeline implementation for async frames and suspension points; ABI metadata emission updates; module/interface preservation for async effects; tests in `tests/conformance/lowering_abi/` and `tests/conformance/module_roundtrip/`.
  - Test IDs: `CORO-01`..`CORO-05`.
  - Dependencies: #80.
  - Done criteria: Coroutine lowering is observable in emitted IR/ABI behavior; cross-module async calls remain ABI-compatible; metadata round-trip tests pass.

- [x] **Issue #82 - Implement cancellation propagation and task-context behavior**
  - Spec anchors: [Part 7 7.6](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-6), [7.6.2](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-6-2), [7.6.7](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-6-7), [C.5.2](../../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md#c-5-2).
  - Objective: Enforce cancellation inheritance/propagation rules and task-context semantics for child, detached, and group execution paths.
  - Required artifacts: runtime/task hook integration for cancellation state; semantic handling for cancellation-aware APIs; cancellation diagnostics and behavior tests in `tests/conformance/semantic/` and `tests/conformance/lowering_abi/`.
  - Test IDs: `CAN-01`..`CAN-07`.
  - Dependencies: #80, #81.
  - Done criteria: Cancellation behavior matches the normative matrix for child, detached, group, and hop scenarios; cancellation tests are deterministic and green.

- [x] **Issue #83 - Implement executor affinity annotations and call-site behavior**
  - Spec anchors: [B.3.1](../../../spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md#b-3-1), [Part 7 7.4.3](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-4-3), [7.4.5](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-4-5), [7.4.7](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-4-7), [D Table A](../../../spec/MODULE_METADATA_AND_ABI_TABLES.md#d-3-1).
  - Objective: Support canonical `objc_executor(...)` annotations, prove/elide hops where legal, and require call-site hops where required.
  - Required artifacts: parser support for canonical annotation spellings; executor-proof semantic engine; call-site hop insertion/checking; metadata/interface preservation updates; tests in `tests/conformance/semantic/`, `tests/conformance/lowering_abi/`, and `tests/conformance/module_roundtrip/`.
  - Test IDs: `EXE-01`..`EXE-05`, `EXEC-ATTR-01`..`EXEC-ATTR-03`.
  - Dependencies: #80, #81.
  - Done criteria: Executor annotations parse and emit canonically; hop requirements are enforced with correct elision behavior; cross-module executor proof tests pass.

- [x] **Issue #84 - Implement actor declarations and actor isolation rules**
  - Spec anchors: [Part 7 7.7](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-7), [7.7.2.1](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-7-2-1), [7.7.5](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-7-5), [C.6](../../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md#c-6).
  - Objective: Implement actor type declarations, isolation boundaries, and reentrancy model behavior required by v1.
  - Required artifacts: parser and semantic support for actor declarations and isolation modifiers; runtime/lowering wiring for actor dispatch boundaries; metadata preservation for isolation state; tests in `tests/conformance/semantic/`, `tests/conformance/lowering_abi/`, and `tests/conformance/module_roundtrip/`.
  - Test IDs: `ACT-01`..`ACT-09`.
  - Dependencies: #80, #83.
  - Done criteria: Cross-actor isolation is enforced; same-actor access behavior is correct; reentrancy and cross-module isolation preservation tests pass.

- [x] **Issue #85 - Provide strict diagnostics for isolation, executor annotation misuse, and suspicious cross-actor calls**
  - Spec anchors: [Part 12](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12), [12.3.3](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-3), [Part 7 7.10](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-10).
  - Objective: Add the required strict-concurrency diagnostic coverage for isolation/executor misuse and suspicious call patterns.
  - Required artifacts: diagnostic group and code definitions; strictness/sub-mode policy mapping; fix-it support where mechanical (`await` insertion and hop suggestions); tests in `tests/conformance/diagnostics/`.
  - Test IDs: `CONC-DIAG-01`..`CONC-DIAG-06`, `ACT-01`, `EXE-01`.
  - Dependencies: #83, #84, #87.
  - Done criteria: Required diagnostics fire in strict concurrency mode with stable metadata and expected severity; positive controls do not regress.

- [x] **Issue #86 - Enforce Sendable-like constraints across task/actor/executor boundaries**
  - Spec anchors: [Part 7 7.8](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-8), [7.8.3](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-8-3), [7.8.6](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-8-6), [D Table A](../../../spec/MODULE_METADATA_AND_ABI_TABLES.md#d-3-1).
  - Objective: Enforce Sendable-like rules for captured values, actor boundary arguments/results, and executor-domain crossings.
  - Required artifacts: type classification and boundary checking implementation; `objc_unsafe_sendable` escape-hatch handling with required warnings; cross-module metadata validation for sendability; tests in `tests/conformance/semantic/`, `tests/conformance/module_roundtrip/`, and `tests/conformance/diagnostics/`.
  - Test IDs: `SND-01`..`SND-08`, `SND-XM-01`, `SND-XM-02`.
  - Dependencies: #84.
  - Done criteria: Strict-concurrency mode rejects non-Sendable crossings at all required sites; metadata mismatch is diagnosed; sendability matrix tests pass.

- [x] **Issue #87 - Enforce D-011 await requirement for all potentially-suspending operations**
  - Spec anchors: [D-011](../../../spec/DECISIONS_LOG.md#decisions-d-011), [Part 7 7.3.2](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-3-2), [7.4.3.2](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-4-3-2), [7.7.2.1](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-7-2-1).
  - Objective: Require explicit `await` for every potentially-suspending operation, including async calls, executor hops, and actor-isolated cross-domain access.
  - Required artifacts: suspension-point effect analysis; diagnostics for missing and unnecessary `await`; tests spanning parser/semantic/diagnostic behavior.
  - Test IDs: `AWT-01`..`AWT-06`, `EXE-01`, `ACT-01`.
  - Dependencies: #80, #83, #84.
  - Done criteria: Missing `await` is rejected for all normative suspension sources; unnecessary `await` warning behavior is validated; no false-negative suspension cases remain in coverage.

## E.3.8 System programming extensions (Part 8)

- [x] **Issue #88 - Support canonical attribute spellings for Part 8 features**
  - Spec anchors: [E.3.8](../../../spec/CONFORMANCE_PROFILE_CHECKLIST.md#e-3-8), [Part 8 8.0.1](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-0-1), [B.8](../../../spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md#b-8).
  - Objective: Parse, type-check, and emit canonical spellings for system attributes and contextual keywords required in Part 8.
  - Required artifacts: attribute parser/binder support for `objc_resource(...)`, `objc_returns_borrowed(...)`, `borrowed`, and capture-list keywords; canonical interface emission updates; parser/module-roundtrip tests.
  - Test IDs: `SYS-ATTR-01`..`SYS-ATTR-08`.
  - Dependencies: None.
  - Done criteria: Canonical spellings are accepted and preserved in emitted interfaces; non-canonical sugar (if accepted) canonicalizes correctly; attribute parsing regressions are covered.

- [x] **Issue #89 - Implement resource cleanup semantics and required diagnostics**
  - Spec anchors: [Part 8 8.1](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-1), [8.3](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-3), [Part 12 12.3.6](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-6), [Part 12 12.5.13](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-13).
  - Objective: Implement cleanup scope registration, resource invalid-sentinel behavior, and strict-system diagnostics for resource misuse.
  - Required artifacts: cleanup registration and scope-exit ordering implementation; `take/reset/discard` semantics; diagnostics for double-cleanup/use-after-move/unsupported field annotations; tests in `tests/conformance/semantic/`, `tests/conformance/lowering_abi/`, and `tests/conformance/diagnostics/`.
  - Test IDs: `RES-01`..`RES-10`, `AGR-NEG-01`, `AGR-RT-01`..`AGR-RT-03`.
  - Dependencies: #88.
  - Done criteria: Cleanup runs exactly once in required order across normal and unwind exits; strict-system diagnostics for required failure modes are present; resource semantics tests are green.

- [x] **Issue #90 - Implement `withLifetime`/`keepAlive` semantics and ARC interaction**
  - Spec anchors: [Part 8 8.6](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-6), [8.6.1](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-6-1), [8.6.2](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-6-2), [8.6.3](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-6-3).
  - Objective: Implement explicit lifetime extension primitives and guarantee they compose correctly with ARC lifetime shortening and release points.
  - Required artifacts: semantic lowering for `withLifetime` and `keepAlive`; ARC interaction tests at suspension/scope boundaries; diagnostics for misuse; tests in `tests/conformance/semantic/` and `tests/conformance/lowering_abi/`.
  - Test IDs: `LIFE-01`..`LIFE-05`.
  - Dependencies: #88.
  - Done criteria: Lifetime extension behavior is observable and stable; ARC does not prematurely release protected values; misuse diagnostics and positive-path tests pass.

- [x] **Issue #91 - Implement borrowed pointer rules and diagnostics (intra-procedural minimum)**
  - Spec anchors: [Part 8 8.7](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-7), [8.7.3](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-7-3), [8.7.4](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-7-4), [Part 12 12.5.12](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-12).
  - Objective: Enforce strict-system borrowed-pointer escape rules with required intra-procedural, flow-sensitive checks.
  - Required artifacts: borrowed type and owner-index semantic model; escape checker integrated into normal semantic analysis; diagnostics with actionable fix directions; tests in `tests/conformance/semantic/` and `tests/conformance/diagnostics/`.
  - Test IDs: `BRW-NEG-01`..`BRW-NEG-05`, `BRW-POS-01`..`BRW-POS-04`.
  - Dependencies: #88, #90.
  - Done criteria: All required escape sites are diagnosed in strict-system mode; required non-escape scenarios are accepted; checker runs without requiring external whole-program analysis.

- [x] **Issue #92 - Preserve borrowed/lifetime annotations in module metadata**
  - Spec anchors: [D Table A](../../../spec/MODULE_METADATA_AND_ABI_TABLES.md#d-3-1), [Part 8 8.7.2](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-7-2), [Part 2](../../../spec/PART_2_MODULES_NAMESPACING_API_SURFACES.md#part-2).
  - Objective: Preserve borrowed and lifetime semantics across module boundaries and textual interfaces.
  - Required artifacts: metadata schema support for borrowed owner-index and lifetime annotations; textual interface emission/verification updates; cross-module roundtrip tests in `tests/conformance/module_roundtrip/`.
  - Test IDs: `BRW-META-01`..`BRW-META-03`.
  - Dependencies: #91.
  - Done criteria: Imported modules retain equivalent borrowed/lifetime semantics; metadata loss/mismatch is detected; module roundtrip tests pass.

- [x] **Issue #93 - Implement capture lists with required order and move/weak/unowned semantics**
  - Spec anchors: [Part 8 8.8](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-8), [8.8.2](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-8-2), [8.8.3](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-8-3), [8.8.4](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-8-4).
  - Objective: Implement capture-list parsing and semantics, including left-to-right evaluation and move-state behavior for resources.
  - Required artifacts: parser and semantic support for capture-list grammar; capture storage lowering for `strong`/`weak`/`unowned`/`move`; move-state diagnostics for resource handles; tests in `tests/conformance/parser/`, `tests/conformance/semantic/`, and `tests/conformance/diagnostics/`.
  - Test IDs: `CAP-01`..`CAP-08`.
  - Dependencies: #88, #89.
  - Done criteria: Capture ordering and modifier semantics match spec; resource move-state hazards are detected; capture-list tests pass.

- [x] **Issue #94 - Provide diagnostics for borrowed escapes, use-after-move, and dangerous `unowned` captures**
  - Spec anchors: [Part 12 12.3.6](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-6), [Part 8 8.7.4](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-7-4), [Part 8 8.8.6](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8-8-6).
  - Objective: Implement the strict-system diagnostics set for the required borrowed and capture-list safety violations.
  - Required artifacts: diagnostic codes and messages for each required violation family; policy wiring for strict-system severity; portable assertion fixtures in `tests/conformance/diagnostics/`.
  - Test IDs: `SYS-DIAG-01`..`SYS-DIAG-08`, `BRW-NEG-01`..`BRW-NEG-05`, `CAP-06`..`CAP-08`.
  - Dependencies: #89, #91, #93.
  - Done criteria: Each required violation emits a stable diagnostic with actionable guidance; strict-system severity is correct; no required case is untested.

## E.3.9 Performance and dynamism controls

- [x] **Issue #95 - Accept and preserve canonical `objc_direct`/`objc_final`/`objc_sealed` spellings**
  - Spec anchors: [E.3.9](../../../spec/CONFORMANCE_PROFILE_CHECKLIST.md#e-3-9), [B.5](../../../spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md#b-5), [Part 9](../../../spec/PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md#part-9), [D Table A](../../../spec/MODULE_METADATA_AND_ABI_TABLES.md#d-3-1).
  - Objective: Support canonical parsing and interface/module preservation for performance/dynamism control attributes.
  - Required artifacts: parser/semantic support for canonical spellings; canonical emission in textual interfaces; metadata preservation for dispatch legality attributes; tests in `tests/conformance/parser/` and `tests/conformance/module_roundtrip/`.
  - Test IDs: `PERF-ATTR-01`..`PERF-ATTR-04`.
  - Dependencies: None.
  - Done criteria: Canonical attributes parse and round-trip without semantic drift; cross-module imports preserve attribute intent.

- [x] **Issue #96 - Enforce legality rules across categories/extensions and module boundaries**
  - Spec anchors: [Part 9](../../../spec/PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md#part-9), [9.9.1](../../../spec/PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md#part-9-9-1), [C.8](../../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md#c-8), [D](../../../spec/MODULE_METADATA_AND_ABI_TABLES.md#d).
  - Objective: Enforce legality constraints for direct/final/sealed semantics in same-module and cross-module override/category scenarios.
  - Required artifacts: semantic legality checker across inheritance/category/extension edges; module-import validation for legality assumptions; conformance tests in `tests/conformance/semantic/` and `tests/conformance/module_roundtrip/`.
  - Test IDs: `PERF-DIRMEM-01`..`PERF-DIRMEM-04`, `PERF-LEG-01`..`PERF-LEG-03`.
  - Dependencies: #95.
  - Done criteria: Illegal override/category/subclass patterns are rejected with required diagnostics; legal opt-out and mixed-mode cases remain accepted.

- [x] **Issue #97 - Provide diagnostics for dynamic-dispatch misuse and final/sealed override violations**
  - Spec anchors: [Part 12 12.3.5](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-5), [Part 9 9.7](../../../spec/PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md#part-9-7), [Part 9 9.9.2](../../../spec/PART_9_PERFORMANCE_AND_DYNAMISM_CONTROLS.md#part-9-9-2).
  - Objective: Implement required diagnostic behavior for direct/final/sealed misuse, including strict-performance dynamic-dispatch warnings/errors.
  - Required artifacts: diagnostic codes/messages for misuse classes; strict-performance policy toggles; diagnostics tests with portable assertions.
  - Test IDs: `PERF-DYN-01`..`PERF-DYN-04`, `PERF-DIAG-01`..`PERF-DIAG-04`.
  - Dependencies: #95, #96.
  - Done criteria: All required misuse diagnostics are emitted with correct severity by profile; positive controls and dynamic opt-out behavior are preserved.

## E.3.10 Metaprogramming (optional feature set)

- [x] **Issue #98 - Implement derives (`objc_derive(...)`) with deterministic, tool-visible expansion**
  - Spec anchors: [E.3.10](../../../spec/CONFORMANCE_PROFILE_CHECKLIST.md#e-3-10), [Part 10](../../../spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md#part-10), [B.6.1](../../../spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md#b-6-1), [Part 10 10.6](../../../spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md#part-10-6).
  - Objective: Implement derive expansion with deterministic outputs and explicit capability gating for optional meta claims.
  - Required artifacts: derive expander implementation; deterministic expansion ordering rules; capability gate/macro exposure for optional features; tests in `tests/conformance/semantic/` and `tests/conformance/module_roundtrip/`.
  - Test IDs: `META-DRV-01`..`META-DRV-03`, `META-EXP-01`, `META-EXP-02`.
  - Dependencies: None.
  - Done criteria: Repeated builds produce byte-for-byte equivalent derive outputs; unsupported derive requests fail with required diagnostics; optional-claim gating behavior is test-covered.

- [x] **Issue #99 - Implement macros (`objc_macro(...)`) with sandboxing and safety constraints**
  - Spec anchors: [Part 10](../../../spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md#part-10), [B.6.2](../../../spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md#b-6-2), [Part 10 10.5](../../../spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md#part-10-5), [Part 10 10.6](../../../spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md#part-10-6).
  - Objective: Implement macro expansion under reproducible, sandboxed package resolution and safety diagnostics.
  - Required artifacts: macro expansion runtime with sandbox policy; package/lockfile resolution flow; safety diagnostics for unsupported/unsafe expansion cases; tests in `tests/conformance/semantic/` and `tests/conformance/diagnostics/`.
  - Test IDs: `META-PKG-01`..`META-PKG-03`, `META-MAC-01`..`META-MAC-04`.
  - Dependencies: #98.
  - Done criteria: Macro package resolution is deterministic; lockfile and sandbox violations are hard failures; expansion safety diagnostics match required behavior.

- [x] **Issue #100 - Preserve macro/derive expansions in module metadata and textual interfaces**
  - Spec anchors: [D Table A](../../../spec/MODULE_METADATA_AND_ABI_TABLES.md#d-3-1), [Part 10](../../../spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md#part-10), [Part 2](../../../spec/PART_2_MODULES_NAMESPACING_API_SURFACES.md#part-2).
  - Objective: Ensure macro and derive expansion artifacts are preserved and re-imported without semantic loss.
  - Required artifacts: metadata schema updates for expansion provenance and expanded declarations; textual interface emission updates; cross-module roundtrip tests in `tests/conformance/module_roundtrip/`.
  - Test IDs: `META-XM-01`..`META-XM-04`.
  - Dependencies: #98, #99.
  - Done criteria: Imported modules retain expansion semantics equivalent to in-module builds; metadata/interface mismatch is detected and reported.

## E.3.11 Interoperability (optional feature sets)

- [x] **Issue #101 - Maintain full interop with C and Objective-C runtime behavior**
  - Spec anchors: [E.3.11](../../../spec/CONFORMANCE_PROFILE_CHECKLIST.md#e-3-11), [Part 11](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11), [Part 11 11.2.4](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11-2-4), [C.2](../../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md#c-2), [C.5](../../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md#c-5).
  - Objective: Preserve C/ObjC runtime interop semantics for async/throws ABI surfaces and baseline runtime behaviors across language boundaries.
  - Required artifacts: exported C thunk ABI implementation for async APIs; runtime ownership/lifetime boundary handling; C caller conformance tests and Objective-C runtime compatibility tests in `tests/conformance/lowering_abi/` and `tests/conformance/semantic/`.
  - Test IDs: `INT-C-01`..`INT-C-12`.
  - Dependencies: #81, #82, #95.
  - Done criteria: C callers can invoke exported async thunks with required status/error invariants; separate-compilation ABI remains stable; runtime behavior matches baseline expectations.

- [x] **Issue #102 - Document and test ObjC++ interactions for ownership, `throws`, and `async` lowering**
  - Spec anchors: [Part 11 11.3](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11-3), [Part 6](../../../spec/PART_6_ERRORS_RESULTS_THROWS.md#part-6), [Part 7](../../../spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7), [C.5](../../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md#c-5).
  - Objective: Validate ObjC++ mode behavior for ownership, error propagation, and async lowering without semantic drift.
  - Required artifacts: ObjC++ interop guidance updates; ObjC++ parser/semantic/lowering compatibility fixes; dedicated ObjC++ conformance fixtures in `tests/conformance/semantic/` and `tests/conformance/lowering_abi/`.
  - Test IDs: `INT-CXX-01`..`INT-CXX-08`.
  - Dependencies: #101.
  - Done criteria: ObjC++ fixtures compile and run with behavior equivalent to ObjC mode for covered ownership/throws/async cases; interop docs match implemented behavior.

- [x] **Issue #103 - Provide Swift interop story for optionals, throws, async/await, and actor metadata**
  - Spec anchors: [Part 11 11.4](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11-4), [11.4.1](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11-4-1), [11.4.2](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11-4-2), [11.4.3](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11-4-3), [11.4.4](../../../spec/PART_11_INTEROPERABILITY_C_CPP_SWIFT.md#part-11-4-4).
  - Objective: Define and validate import/export mapping for Swift interop over optionals, error effects, async effects, and actor isolation metadata.
  - Required artifacts: Swift interop mapping documentation and implementation notes; import/export metadata preservation checks; cross-language fixtures in `tests/conformance/module_roundtrip/` and `tests/conformance/semantic/`.
  - Test IDs: `INT-SWIFT-01`..`INT-SWIFT-08`.
  - Dependencies: #101, #84, #100.
  - Done criteria: Interop fixtures preserve effect/isolation semantics across boundaries; required mapping behavior is documented and test-backed.

## E.3.12 Diagnostics, tooling, and tests

- [x] **Issue #104 - Implement minimum diagnostic groups from Part 12**
  - Spec anchors: [E.3.12](../../../spec/CONFORMANCE_PROFILE_CHECKLIST.md#e-3-12), [Part 12 12.3](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3), [12.3.1](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-1), [12.3.2](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-2), [12.3.3](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-3), [12.3.4](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-4), [12.3.5](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-3-5).
  - Objective: Implement baseline diagnostic groups and profile-aware severities for nullability/optionals, throws, concurrency, modules/interface emission, and performance controls.
  - Required artifacts: diagnostic group registry and IDs; strictness/profile severity policy mapping; conformance diagnostic fixtures in `tests/conformance/diagnostics/`.
  - Test IDs: `DIAG-GRP-01`..`DIAG-GRP-10`.
  - Dependencies: #85, #94, #97.
  - Done criteria: All required diagnostic groups exist and are profile-wired; each group has positive and negative test coverage; portable assertion shape is used.

- [x] **Issue #105 - Provide fix-its and migrator support for canonical spellings and safer idioms**
  - Spec anchors: [Part 12 12.4](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-4), [12.4.1](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-4-1), [Part 12 12.5.7](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-7).
  - Objective: Deliver migrator and fix-it capabilities for mechanical modernization and safety upgrades.
  - Required artifacts: migrator passes for nullability, optional-send rewrites, NSError-to-throws wrappers, and executor/actor hop suggestions; fix-it emission plumbing; tests in `tests/conformance/diagnostics/`.
  - Test IDs: `MIG-01`..`MIG-08`.
  - Dependencies: #104.
  - Done criteria: Required transformations are machine-applicable and deterministic; fix-its preserve semantics on test corpus; migrator fixtures pass.

- [x] **Issue #106 - Provide/publish conformance suite with required structure and coverage**
  - Spec anchors: [Part 12 12.5](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5), [12.5.0](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-0), [12.5.2](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-2), [12.5.6](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-6), [12.5.7](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-7), [12.5.9](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-9), [12.5.10](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-10).
  - Objective: Publish a conformance suite that meets required repository buckets, coverage breadth, profile minima, and portable diagnostic assertions.
  - Required artifacts: populated tests under `tests/conformance/parser/`, `tests/conformance/semantic/`, `tests/conformance/lowering_abi/`, `tests/conformance/module_roundtrip/`, and `tests/conformance/diagnostics/`; coverage mapping doc from issue to tests; CI workflow enforcing profile minima.
  - Test IDs: `TUV-01`..`TUV-05`, `CRPT-01`..`CRPT-06`, `SCM-01`..`SCM-06`, `EXE-01`..`EXE-05`, `CAN-01`..`CAN-07`, `ACT-01`..`ACT-09`, `SND-01`..`SND-XM-02`, `BRW-NEG-01`..`BRW-POS-04`, `PERF-DIRMEM-01`..`PERF-DYN-04`.
  - Dependencies: #80 through #105, plus external conformance-report dependencies #43 and #44.
  - Done criteria: Bucket structure and profile minimum counts are met; all required test families are wired into CI; published suite includes portable diagnostic assertions and traceable issue mapping.

- [x] **Issue #107 - Include tests for borrowed-pointer escape diagnostics and resource cleanup semantics**
  - Spec anchors: [Part 8](../../../spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md#part-8), [Part 12](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12), [12.5.12](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-12), [12.5.13](../../../spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md#part-12-5-13).
  - Objective: Deliver dedicated strict-system conformance coverage for borrowed-pointer escapes and resource cleanup ordering/misuse diagnostics.
  - Required artifacts: strict-system diagnostic fixtures for borrowed escapes and resource misuse; runtime/semantic cleanup-order fixtures; CI inclusion in strict-system profile runs.
  - Test IDs: `BRW-NEG-01`..`BRW-NEG-05`, `BRW-POS-01`..`BRW-POS-04`, `AGR-NEG-01`, `AGR-RT-01`..`AGR-RT-03`, `RES-01`..`RES-06`.
  - Dependencies: #89, #91, #94, #106.
  - Done criteria: Required borrowed and resource cleanup failure modes are covered and asserted with stable diagnostics; strict-system profile gates on these tests.
