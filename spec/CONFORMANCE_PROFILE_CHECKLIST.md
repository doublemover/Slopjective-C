# Objective‑C 3.0 — Conformance Profile Checklist {#e}

_Working draft v0.11 — last updated 2026-02-23_

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
- **[CONC]** required for **ObjC 3.0 v1 Strict Concurrency** (strictness `strict` + concurrency sub-mode `strict`)
- **[SYSTEM]** required for **ObjC 3.0 v1 Strict System** (strictness `strict-system` + concurrency sub-mode `strict`)

Optional feature sets:

- **[OPT-META]** metaprogramming feature set ([Part 10](#part-10))
- **[OPT-SWIFT]** Swift interop feature set ([Part 11](#part-11), Swift-facing portions)
- **[OPT-CXX]** C++-enhanced interop feature set ([Part 11](#part-11), C++-specific portions)

### E.1.3 What it means to “pass” {#e-1-3}

A toolchain **passes** a profile when it satisfies _all_ checklist items tagged for that profile, and when it can demonstrate (via tests) that required module metadata and runtime contracts behave correctly under **separate compilation**.

### E.1.4 Normative vs QoI separation {#e-1-4}

- Sections **E.2** and **E.3** are normative and define conformance requirements.
- Section **E.4** is non-normative quality-of-implementation (QoI) guidance and is not required to claim conformance.
- Checklist items in **E.3** shall reference normative requirements only; recommendations belong in **E.4**.

### E.1.5 v0.13 Profile-Gate Delta Binding {#e-1-5}

For v0.13 planning-cycle closeout for issue `#792` in tranche
`BATCH-20260223-11S`, profile-gate delta evidence is anchored in:

- `spec/planning/v013_profile_gate_delta.md` (seed `V013-SPEC-04`, acceptance gate `AC-V013-SPEC-04`).
- `spec/planning/evidence/lane_d/v013_seed_spec04_validation_20260223.md` (lane D validation transcript and acceptance mapping continuity).

This anchor records post-v0.12 gate deltas for `core`, `strict`,
`strict-concurrency`, and `strict-system`, and binds the dependency chain
`EDGE-V013-002`, `EDGE-V013-003`, and transitive `EDGE-V013-018` for release
handoff consumers while preserving `AC-V013-SPEC-04-01` through
`AC-V013-SPEC-04-07` continuity.

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

- select strictness `strict` and strict concurrency sub-mode `strict`,
- treat strict concurrency as a strictness sub-mode, not as a fourth strictness level ([Part 1](#part-1)),
- enforce actor isolation, executor hops, and Sendable-like rules as specified ([Part 7](#part-7)),
- provide diagnostics for common data-race hazards and isolation violations ([Part 12](#part-12)),
- preserve concurrency-relevant metadata across modules ([D Table A](#d-3-1)).

### E.2.4 ObjC 3.0 v1 Strict System {#e-2-4}

A toolchain claiming **ObjC 3.0 v1 Strict System** shall:

- support strict-system strictness selection ([Part 1](#part-1)),
- enable strict concurrency sub-mode (`-fobjc3-concurrency=strict`) for profile-conforming runs,
- implement the system-programming extensions in [Part 8](#part-8) (resources, borrowed pointers, capture lists) and their associated diagnostics,
- preserve borrowed/lifetime annotations across modules ([B.8](#b-8), [D Table A](#d-3-1)),
- provide at least intra-procedural enforcement for borrowed-pointer escape analysis and resource use-after-move patterns ([Part 8](#part-8), [Part 12](#part-12)).

“Strict System” is intended for SDKs and codebases like IOKit, DriverKit, and other “handle heavy” or ownership-sensitive domains.

## E.3 Checklist {#e-3}

### E.3.1 Language mode selection, feature tests, and versioning {#e-3-1}

- [x] SPT-0024 **[CORE]** Provide a language mode selection mechanism equivalent to `-fobjc-version=3`. ([Part 1](#part-1) [§1.2](#part-1-2)) ([Issue #48](https://github.com/doublemover/Slopjective-C/issues/48)) Evidence: `tests/conformance/parser/TUV-01.json`, `tests/conformance/parser/LMV-48-NEG-01.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_48_cli_mode_selection`.
- [x] SPT-0025 **[CORE]** Provide a source-level mechanism (pragma or directive) to set ObjC 3.0 mode for a translation unit, or document that only the flag form is supported. ([Part 1](#part-1)) ([Issue #49](https://github.com/doublemover/Slopjective-C/issues/49)) Evidence: `tests/conformance/parser/TUV-02.json`, `tests/conformance/parser/TUV-05.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_49_source_mode_selection`.
- [x] SPT-0026 **[CORE]** Enforce the v1 translation-unit-only language-version model and reject per-region language-version switching forms. ([Part 1](#part-1) [§1.2.2](#part-1-2-2), [§1.2.3](#part-1-2-3)) ([Issue #21](https://github.com/doublemover/Slopjective-C/issues/21)) Evidence: `tests/conformance/parser/TUV-03.json`, `tests/conformance/parser/TUV-04.json`, `tests/conformance/parser/TUV-05.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_49_source_mode_selection`.
- [x] SPT-0027 **[CORE]** Include conformance tests for accepted translation-unit selection forms and rejected region/late/conflicting forms (`TUV-01`..`TUV-05`). ([Part 1](#part-1) [§1.2.4](#part-1-2-4), [Part 12](#part-12) [§12.5.8](#part-12-5-8)) ([Issue #21](https://github.com/doublemover/Slopjective-C/issues/21)) Evidence: `tests/conformance/parser/TUV-03.json`, `tests/conformance/parser/TUV-04.json`, `tests/conformance/parser/TUV-05.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_49_source_mode_selection`.
- [x] SPT-0028 **[CORE]** Provide feature test macros for major feature groups (optionals, throws, async/await, actors, direct/final/sealed, derives/macros if implemented). ([Part 1](#part-1) [§1.4](#part-1-4)) ([Issue #50](https://github.com/doublemover/Slopjective-C/issues/50)) Evidence: `tests/conformance/parser/FTM-50-01.json`, `tests/conformance/parser/FTM-50-03.json`. Validation: `tests/conformance/parser/manifest.json` group `issue_50_feature_test_macros`.
- [x] SPT-0029 **[STRICT]** Provide strictness selection equivalent to `-fobjc3-strictness=permissive|strict|strict-system`. ([Part 1](#part-1) [§1.5](#part-1-5)) ([Issue #51](https://github.com/doublemover/Slopjective-C/issues/51)) Evidence: `tests/conformance/diagnostics/SCM-01.json`, `tests/conformance/diagnostics/SCM-04.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_51_strictness_matrix`.
- [x] SPT-0030 **[STRICT]** Provide mode-selection macros `__OBJC3_STRICTNESS_LEVEL__`, `__OBJC3_CONCURRENCY_MODE__`, and `__OBJC3_CONCURRENCY_STRICT__`. ([Part 1](#part-1) [§1.4.3](#part-1-4-3)) ([Issue #44](https://github.com/doublemover/Slopjective-C/issues/44)) Evidence: `tests/conformance/diagnostics/SCM-01.json`, `tests/conformance/diagnostics/SCM-06.json`, `tests/conformance/parser/FTM-50-03.json`. Validation: `tests/conformance/diagnostics/manifest.json` groups `issue_51_strictness_matrix` and `issue_52_concurrency_submode_matrix`.
- [x] SPT-0031 **[CONC]** Provide concurrency sub-mode selection equivalent to `-fobjc3-concurrency=strict|off`; do not model strict concurrency as a fourth strictness level. ([Part 1](#part-1) [§1.6](#part-1-6), [§1.6.2](#part-1-6-2)) ([Issue #44](https://github.com/doublemover/Slopjective-C/issues/44)) Evidence: `tests/conformance/diagnostics/SCM-01.json`, `tests/conformance/diagnostics/SCM-06.json`, `tests/conformance/parser/FTM-50-03.json`. Validation: `tests/conformance/diagnostics/manifest.json` groups `issue_51_strictness_matrix` and `issue_52_concurrency_submode_matrix`.

### E.3.2 Modules, namespacing, and interface emission {#e-3-2}

- [x] SPT-0032 **[CORE]** Implement module-aware compilation for Objective‑C declarations sufficient to support stable import, name lookup, and diagnostics. ([Part 2](#part-2)) ([Issue #53](https://github.com/doublemover/Slopjective-C/issues/53)) Evidence: `tests/conformance/module_roundtrip/MOD-53-01.json`, `tests/conformance/module_roundtrip/MOD-53-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_53_module_aware_compilation`.
- [x] SPT-0033 **[CORE]** Provide a textual interface emission mode (or equivalent) that can be used for verification in CI. ([Part 2](#part-2), [Part 12](#part-12)) ([Issue #54](https://github.com/doublemover/Slopjective-C/issues/54)) Evidence: `tests/conformance/module_roundtrip/IFC-54-01.json`, `tests/conformance/module_roundtrip/IFC-54-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_54_textual_interface_emission`.
- [x] SPT-0034 **[CORE]** Emit **canonical spellings** for ObjC 3.0 features in textual interfaces, per [B](#b). ([B.1](#b-1), [B.7](#b-7); [Part 2](#part-2)) ([Issue #55](https://github.com/doublemover/Slopjective-C/issues/55)) Evidence: `tests/conformance/module_roundtrip/IFC-55-01.json`, `tests/conformance/module_roundtrip/IFC-55-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_55_canonical_interface_spellings`.
- [x] SPT-0035 **[CORE]** Preserve and record all “must preserve” metadata in [D Table A](#d-3-1). ([D.3.1](#d-3-1)) ([Issue #56](https://github.com/doublemover/Slopjective-C/issues/56)) Evidence: `tests/conformance/module_roundtrip/META-56-01.json`, `tests/conformance/module_roundtrip/META-56-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_56_metadata_preservation`.
- [x] SPT-0036 **[STRICT]** Provide an interface verification mode that detects mismatch between compiled module metadata and emitted textual interface. ([Part 12](#part-12)) ([Issue #57](https://github.com/doublemover/Slopjective-C/issues/57)) Evidence: `tests/conformance/module_roundtrip/IFV-57-01.json`, `tests/conformance/module_roundtrip/IFV-57-02.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_57_interface_verification_mode`.

### E.3.3 Type system: nullability defaults, optionals, generics, key paths {#e-3-3}

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

### E.3.4 Memory management and lifetime {#e-3-4}

- [x] SPT-0046 **[CORE]** Preserve ObjC ARC semantics and ensure new language features lower without violating ARC rules. ([Part 4](#part-4)) ([Issue #67](https://github.com/doublemover/Slopjective-C/issues/67)) Evidence: `tests/conformance/lowering_abi/ARC-67-01.json`, `tests/conformance/lowering_abi/ARC-67-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_67_arc_semantic_preservation`.
- [x] SPT-0047 **[CORE]** Implement the suspension-point autorelease pool contract ([D-006](#decisions-d-006); [C.7](#c-7); [Part 7](#part-7)). This includes: ([Issue #68](https://github.com/doublemover/Slopjective-C/issues/68)) Evidence: `tests/conformance/lowering_abi/ARP-68-RT-01.json`, `tests/conformance/lowering_abi/ARP-68-RT-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_68_suspension_autorelease_pool_contract`.
  - creating an implicit pool for each async “slice” as specified,
  - draining it at each suspension point.
- [x] SPT-0048 **[STRICT]** Provide diagnostics for suspicious lifetime extensions, escaping of stack-bound resources, and unsafe bridging where specified. ([Part 4](#part-4), [Part 12](#part-12)) ([Issue #69](https://github.com/doublemover/Slopjective-C/issues/69)) Evidence: `tests/conformance/diagnostics/LFT-69-NEG-01.json`, `tests/conformance/diagnostics/LFT-69-FIX-01.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_69_lifetime_escape_diagnostics`.

### E.3.5 Control flow and safety constructs {#e-3-5}

- [x] SPT-0049 **[CORE]** Implement `defer` with LIFO scope-exit semantics ([Part 5](#part-5); [Part 8](#part-8) [§8.2](#part-8-2)). ([Issue #70](https://github.com/doublemover/Slopjective-C/issues/70)) Evidence: `tests/conformance/semantic/DEF-70-01.json`, `tests/conformance/semantic/DEF-70-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_70_defer_lifo_semantics`.
- [x] SPT-0050 **[CORE]** Ensure `defer` executes on normal exit and stack unwinding exits (where applicable) as specified. ([Part 5](#part-5)/8) ([Issue #71](https://github.com/doublemover/Slopjective-C/issues/71)) Evidence: `tests/conformance/lowering_abi/DEF-71-RT-01.json`, `tests/conformance/lowering_abi/DEF-71-RT-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_71_defer_normal_and_unwind`.
- [x] SPT-0051 **[CORE]** Implement `guard` and refinement rules for optionals/patterns. ([Part 5](#part-5)) ([Issue #72](https://github.com/doublemover/Slopjective-C/issues/72)) Evidence: `tests/conformance/semantic/GRD-72-01.json`, `tests/conformance/semantic/GRD-72-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_72_guard_refinement_rules`.
- [x] SPT-0052 **[CORE]** Implement `match` (pattern matching) to the extent specified, or clearly mark as provisional and do not claim a feature macro if not implemented. ([Part 5](#part-5)) ([Issue #73](https://github.com/doublemover/Slopjective-C/issues/73)) Evidence: `tests/conformance/semantic/MTC-73-01.json`, `tests/conformance/semantic/MTC-73-GATE-01.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_73_match_or_gating`.
- [x] SPT-0053 **[STRICT]** Diagnose illegal non-local exits from `defer` bodies ([Part 5](#part-5)/8) and other ill-formed constructs. ([Issue #74](https://github.com/doublemover/Slopjective-C/issues/74)) Evidence: `tests/conformance/diagnostics/NLE-74-NEG-01.json`, `tests/conformance/diagnostics/NLE-74-NEG-02.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_74_defer_non_local_exit_diagnostics`.

### E.3.6 Errors and `throws` {#e-3-6}

- [x] SPT-0054 **[CORE]** Implement untyped `throws` as the v1 model ([D-002](#decisions-d-002); [Part 6](#part-6)). ([Issue #75](https://github.com/doublemover/Slopjective-C/issues/75)) Evidence: `tests/conformance/semantic/THR-75-01.json`, `tests/conformance/semantic/THR-75-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_75_untyped_throws_v1`.
- [x] SPT-0055 **[CORE]** Provide a stable ABI/lowering model for `throws` ([D-009](#decisions-d-009); [C.4](#c-4); [D Table B](#d-3-2)). ([Issue #76](https://github.com/doublemover/Slopjective-C/issues/76)) Evidence: `tests/conformance/lowering_abi/THR-76-ABI-01.json`, `tests/conformance/lowering_abi/THR-76-ABI-02.json`. Validation: `tests/conformance/lowering_abi/manifest.json` group `issue_76_throws_abi_stability`.
- [x] SPT-0056 **[CORE]** Support `try`, `do/catch`, and propagation rules that integrate with optionals as specified. ([Part 6](#part-6)) ([Issue #77](https://github.com/doublemover/Slopjective-C/issues/77)) Evidence: `tests/conformance/semantic/TRY-77-01.json`, `tests/conformance/semantic/TRY-77-02.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_77_try_do_catch_propagation`.
- [x] SPT-0057 **[CORE]** Support NSError/status-code bridging attributes where specified ([B.4](#b-4); [Part 6](#part-6)), including module metadata preservation ([D Table A](#d-3-1)). ([Issue #78](https://github.com/doublemover/Slopjective-C/issues/78)) Evidence: `tests/conformance/semantic/BRG-78-01.json`, `tests/conformance/module_roundtrip/BRG-78-MOD-01.json`. Validation: `tests/conformance/semantic/manifest.json` group `issue_78_bridging_attributes_semantic`.
- [x] SPT-0058 **[STRICT]** Provide diagnostics for ignored errors, missing `try`, and invalid bridging patterns. ([Part 12](#part-12)) ([Issue #79](https://github.com/doublemover/Slopjective-C/issues/79)) Evidence: `tests/conformance/diagnostics/ERR-79-NEG-01.json`, `tests/conformance/diagnostics/ERR-79-FIX-01.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_79_throws_misuse_diagnostics`.

### E.3.7 Concurrency: `async/await`, executors, cancellation, actors {#e-3-7}

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

### E.3.8 System programming extensions (Part 8) {#e-3-8}

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

### E.3.9 Performance and dynamism controls {#e-3-9}

- [x] SPT-0074 **[CORE]** Accept and preserve canonical spellings for `objc_direct`, `objc_final`, `objc_sealed` ([B.5](#b-5); [Part 9](#part-9)). ([Issue #95](https://github.com/doublemover/Slopjective-C/issues/95)) Evidence: `tests/conformance/parser/PERF-ATTR-01.json`, `tests/conformance/module_roundtrip/PERF-ATTR-04.json`. Validation: parser/module manifests for `issue_95_performance_attribute_*` groups.
- [x] SPT-0075 **[CORE]** Enforce legality rules across categories/extensions and module boundaries as specified. ([Part 9](#part-9); [C.8](#c-8); [D](#d)) ([Issue #96](https://github.com/doublemover/Slopjective-C/issues/96)) Evidence: `tests/conformance/semantic/PERF-DIRMEM-01.json`, `tests/conformance/semantic/PERF-LEG-02.json`, `tests/conformance/module_roundtrip/PERF-LEG-03.json`. Validation: semantic/module manifests for `issue_96_performance_legality_*` groups.
- [x] SPT-0076 **[STRICT]** Provide diagnostics for calling direct methods via dynamic dispatch, illegal overrides of final/sealed, and related misuse. ([Part 12](#part-12)) ([Issue #97](https://github.com/doublemover/Slopjective-C/issues/97)) Evidence: `tests/conformance/diagnostics/PERF-DYN-01.json`, `tests/conformance/diagnostics/PERF-DIAG-04.json`. Validation: `tests/conformance/diagnostics/manifest.json` group `issue_97_performance_misuse_diagnostics`.

### E.3.10 Metaprogramming (optional feature set) {#e-3-10}

- [x] SPT-0077 **[OPT-META]** Implement derives (`objc_derive(...)`) with deterministic, tool-visible expansion. ([Part 10](#part-10); [B.6.1](#b-6-1)) ([Issue #98](https://github.com/doublemover/Slopjective-C/issues/98)) Evidence: `tests/conformance/semantic/META-DRV-01.json`, `tests/conformance/module_roundtrip/META-EXP-02.json`. Validation: semantic/module manifests for `issue_98_*` groups.
- [x] SPT-0078 **[OPT-META]** Implement macros (`objc_macro(...)`) with sandboxing / safety constraints as specified. ([Part 10](#part-10); [B.6.2](#b-6-2)) ([Issue #99](https://github.com/doublemover/Slopjective-C/issues/99)) Evidence: `tests/conformance/semantic/META-PKG-01.json`, `tests/conformance/diagnostics/META-MAC-04.json`. Validation: semantic/diagnostics manifests for `issue_99_*` groups.
- [x] SPT-0079 **[OPT-META]** Preserve macro/derive expansions in module metadata and textual interfaces as required by D. ([D Table A](#d-3-1)) ([Issue #100](https://github.com/doublemover/Slopjective-C/issues/100)) Evidence: `tests/conformance/module_roundtrip/META-XM-01.json`, `tests/conformance/module_roundtrip/META-XM-04.json`. Validation: `tests/conformance/module_roundtrip/manifest.json` group `issue_100_macro_derive_roundtrip_preservation`.

### E.3.11 Interoperability (optional feature sets) {#e-3-11}

- [x] SPT-0080 **[CORE]** Maintain full interop with C and Objective‑C runtime behavior. (Baseline + [Part 11](#part-11)) ([Issue #101](https://github.com/doublemover/Slopjective-C/issues/101)) Evidence: `tests/conformance/semantic/INT-C-01.json`, `tests/conformance/lowering_abi/INT-C-12.json`. Validation: semantic/lowering manifests for `issue_101_c_interop_*` groups.
- [x] SPT-0081 **[OPT-CXX]** Document and test ObjC++ interactions for ownership, `throws`, and `async` lowering. ([Part 11](#part-11); C) ([Issue #102](https://github.com/doublemover/Slopjective-C/issues/102)) Evidence: `tests/conformance/semantic/INT-CXX-01.json`, `tests/conformance/lowering_abi/INT-CXX-08.json`. Validation: semantic/lowering manifests for `issue_102_objcxx_interop_*` groups.
- [x] SPT-0082 **[OPT-SWIFT]** Provide a Swift interop story (import/export) for: ([Issue #103](https://github.com/doublemover/Slopjective-C/issues/103)) Evidence: `tests/conformance/semantic/INT-SWIFT-01.json`, `tests/conformance/module_roundtrip/INT-SWIFT-08.json`. Validation: semantic/module manifests for `issue_103_swift_interop_*` groups.
  - optionals/nullability,
  - `throws` bridging,
  - `async/await` bridging,
  - actors and isolation metadata. ([Part 11](#part-11))

### E.3.12 Diagnostics, tooling, and tests {#e-3-12}

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

## E.4 Recommended evidence for a conformance claim (non-normative) {#e-4}

A serious conformance claim should ship with:

- a public machine-readable conformance report (JSON required; YAML optional) listing claimed profiles, optional feature sets, versions, and known deviations,
- CI proofs that:
  - module interfaces round-trip (emit → import) without semantic loss,
  - [D Table A](#d-3-1) metadata is preserved under separate compilation,
  - runtime contracts for `throws` and `async` behave correctly under optimization,
- migration tooling notes for large codebases (warning groups, fix-its, staged adoption).

