# E.3 Core/Strict Implementation Workpack (Issues #48-#79)

This workpack defines executable tasks for remaining Section-E Core/Strict
issues `#48` through `#79`, grouped by `E.3.1` through `E.3.6`.

## Anchor Stability Rules

- Keep all spec anchor tokens unchanged (for example `#e-3-1`, `#part-1-2`).
- Use only existing anchors from
  `spec/CONFORMANCE_PROFILE_CHECKLIST.md` and referenced Part documents.
- Do not rename test IDs once introduced in this workpack.

## Parallel Sequencing

- Wave 0 (foundation, mostly serial): `#48`, `#49`, `#51`, `#52`.
- Wave 1 (parallel after Wave 0):
  - Lane A modules/interfaces: `#53`, `#54`, `#55`, `#56`, `#57`.
  - Lane B type-system core: `#58`, `#59`, `#61`, `#64`, `#66`.
  - Lane C control-flow core: `#70`, `#72`, `#73`.
  - Lane D throws core: `#75`, `#76`, `#77`, `#78`.
- Wave 2 (strict overlays and cross-cutting contracts):
  - `#50`, `#60`, `#62`, `#63`, `#65`, `#67`, `#68`, `#69`, `#71`,
    `#74`, `#79`.
- Wave 3 (closure): add commit and test evidence to each issue before close.

## E.3.1 Language Mode Selection, Feature Tests, and Versioning

### Issue #48: CLI language-mode selection

- Spec anchors: `#e-3-1`, `#part-1-2`, `#part-1-2-4`, `#part-12-5-8`.
- Objective: implement TU-wide language-mode selection equivalent to
  `-fobjc-version=3`.
- Required artifacts: frontend option parser update, translation-unit mode
  plumbing, CLI/help text update, parser/diagnostics conformance tests.
- Test IDs: `TUV-01`, `LMV-48-NEG-01`.
- Dependencies: none.
- Done criteria:
  - `-fobjc-version=3` is accepted and applied to the full translation unit.
  - `TUV-01` and `LMV-48-NEG-01` pass with stable diagnostic metadata.

### Issue #49: Source-level TU mode selection

- Spec anchors: `#e-3-1`, `#part-1`, `#part-1-2-4`, `#part-12-5-8`.
- Objective: provide file-scope source selection for ObjC 3.0 mode, or
  explicitly document and enforce flag-only support.
- Required artifacts: pragma/directive parser support or flag-only policy doc,
  diagnostics for disallowed forms, parser/diagnostics tests.
- Test IDs: `TUV-02`, `TUV-04`, `TUV-05`, `LMV-49-DOC-01`.
- Dependencies: `#48`.
- Done criteria:
  - File-scope source mode form works as specified, or flag-only mode is
    documented and enforced with a stable diagnostic.
  - `TUV-02`, `TUV-04`, `TUV-05`, and `LMV-49-DOC-01` (flag-only path) pass.

### Issue #50: Feature-test macro surface

- Spec anchors: `#e-3-1`, `#part-1-4`.
- Objective: provide feature-test macros for major feature groups and keep
  values consistent with enabled capabilities.
- Required artifacts: predefined macro table, preprocessor emission hooks,
  macro reference docs, preprocessing tests.
- Test IDs: `FTM-50-01`, `FTM-50-02`, `FTM-50-03`.
- Dependencies: `#48`, `#51`.
- Done criteria:
  - Required feature macros are emitted with deterministic values by mode.
  - `FTM-50-*` tests pass for permissive, strict, and strict-system runs.

### Issue #51: Strictness selection controls

- Spec anchors: `#e-3-1`, `#part-1-5`, `#part-12-5-10`.
- Objective: implement strictness mode selection equivalent to
  `-fobjc3-strictness=permissive|strict|strict-system`.
- Required artifacts: driver/frontend flag wiring, mode propagation into
  semantic checks, diagnostics docs, matrix tests.
- Test IDs: `SCM-01`, `SCM-02`, `SCM-03`, `SCM-04`, `STR-51-CLI-01`.
- Dependencies: `#48`.
- Done criteria:
  - all three strictness values parse, propagate, and gate diagnostics
    according to profile rules.
  - `SCM-01`..`SCM-04` and `STR-51-CLI-01` pass in CI.

### Issue #52: Core/Strict backlog gap resolution

- Spec anchors: `#e-3-1`, `#part-1-6-1`, `#part-1-6-2`, `#part-12-5-10`.
- Objective: close issue `#52` by mapping concurrency sub-mode selection to
  the normative two-axis model (`-fobjc3-concurrency=strict|off` alongside
  strictness) and issue `#44` consistency rules.
- Required artifacts: diagnostics fixtures `SCM-05` and `SCM-06`, explicit
  cross-reference to Part 1 `§1.6.1`/`§1.6.2` and Part 12 `§12.5.10`, and
  closure notes confirming issue `#44` rule alignment.
- Test IDs: `SCM-05`, `SCM-06`.
- Dependencies: `#48`, `#49`, `#51`, `#44` (closed).
- Done criteria:
  - `-fobjc3-concurrency=strict|off` is implemented as an orthogonal sub-mode
    per Part 1 `§1.6.1`, not as a fourth strictness level.
  - Part 1 `§1.6.2`/issue `#44` consistency rules are enforced: enabling
    strict concurrency does not weaken required strictness diagnostics,
    disabling concurrency does not disable required non-concurrency diagnostics,
    and profile claims map to E.2 combinations.
  - `SCM-05` passes for all strictness/sub-mode matrix points and verifies
    `__OBJC3_STRICTNESS_LEVEL__`, `__OBJC3_CONCURRENCY_MODE__`, and
    `__OBJC3_CONCURRENCY_STRICT__` values.
  - `SCM-06` passes for all strictness/sub-mode matrix points and verifies
    conformance report `mode` fields and claimed `profiles[*].id` mappings.

## E.3.2 Modules, Namespacing, and Interface Emission

### Issue #53: Module-aware Objective-C compilation

- Spec anchors: `#e-3-2`, `#part-2`.
- Objective: implement module-aware compilation sufficient for stable imports,
  name lookup, and diagnostics.
- Required artifacts: module loader/index updates, import resolution rules,
  module-specific diagnostics, semantic/module_roundtrip tests.
- Test IDs: `MOD-53-01`, `MOD-53-02`.
- Dependencies: `#48`, `#49`.
- Done criteria:
  - module imports produce deterministic symbol visibility and lookup results.
  - `MOD-53-01` and `MOD-53-02` pass for single- and multi-module builds.

### Issue #54: Textual interface emission mode

- Spec anchors: `#e-3-2`, `#part-2`, `#part-12`.
- Objective: provide a textual interface emission mode usable in CI
  verification flows.
- Required artifacts: interface emitter command/mode, deterministic formatting
  rules, emit/import round-trip tests in module_roundtrip bucket.
- Test IDs: `IFC-54-01`, `IFC-54-02`.
- Dependencies: `#53`.
- Done criteria:
  - interface emission can be invoked non-interactively in CI builds.
  - `IFC-54-01` and `IFC-54-02` pass with deterministic output snapshots.

### Issue #55: Canonical spellings in interfaces

- Spec anchors: `#e-3-2`, `#b-1`, `#b-7`, `#part-2`.
- Objective: emit canonical ObjC 3.0 spellings in textual interfaces per
  Appendix B.
- Required artifacts: canonical spelling normalizer in interface emitter,
  canonicalization regression tests, docs for canonical output guarantees.
- Test IDs: `IFC-55-01`, `IFC-55-02`.
- Dependencies: `#54`.
- Done criteria:
  - non-canonical source spellings are normalized to canonical interface forms.
  - `IFC-55-01` and `IFC-55-02` pass across representative feature samples.

### Issue #56: D Table A metadata preservation

- Spec anchors: `#e-3-2`, `#d-3-1`.
- Objective: preserve and record all required "must preserve" metadata fields.
- Required artifacts: metadata writer/reader coverage for each required field,
  schema validation checks, module_roundtrip metadata tests.
- Test IDs: `META-56-01`, `META-56-02`.
- Dependencies: `#53`.
- Done criteria:
  - each required `D Table A` field is serialized, deserialized, and retained
    under separate compilation.
  - `META-56-01` and `META-56-02` pass with no metadata loss.

### Issue #57: Interface verification mode

- Spec anchors: `#e-3-2`, `#part-12`.
- Objective: provide verification mode that detects mismatch between compiled
  module metadata and emitted textual interface.
- Required artifacts: verifier command/mode, mismatch diff reporter, negative
  mismatch fixtures, diagnostics tests.
- Test IDs: `IFV-57-01`, `IFV-57-02`.
- Dependencies: `#54`, `#55`, `#56`, `#51`.
- Done criteria:
  - verifier fails on metadata/interface divergence and reports stable
    machine-assertable diagnostics.
  - `IFV-57-01` and `IFV-57-02` pass in strict profile runs.

## E.3.3 Type System: Nullability, Optionals, Generics, Key Paths

### Issue #58: Nullability qualifiers in type system

- Spec anchors: `#e-3-3`, `#part-3`.
- Objective: support nullability qualifiers as first-class type-system
  properties in ObjC 3.0 mode.
- Required artifacts: type-checker nullability model, qualifier propagation
  rules, semantic diagnostics, semantic tests.
- Test IDs: `TYP-58-01`, `TYP-58-02`.
- Dependencies: `#48`, `#49`.
- Done criteria:
  - nullability qualifiers participate in type equality/subtyping checks.
  - `TYP-58-01` and `TYP-58-02` pass for assignments, calls, and conversions.

### Issue #59: Nonnull-by-default regions

- Spec anchors: `#e-3-3`, `#part-3`, `#b-2`.
- Objective: implement nonnull-by-default regions with canonical pragma
  spellings.
- Required artifacts: pragma parser/AST support, scope tracking for defaults,
  canonical spelling handling, parser/semantic tests.
- Test IDs: `NNB-59-01`, `NNB-59-02`.
- Dependencies: `#58`.
- Done criteria:
  - default-nullability regions apply only to valid scopes and canonical forms.
  - `NNB-59-01` and `NNB-59-02` pass for nested and boundary scopes.

### Issue #60: Strict missing-nullability diagnostics and fix-its

- Spec anchors: `#e-3-3`, `#part-12`.
- Objective: diagnose required missing nullability in strict modes and provide
  mechanical fix-its.
- Required artifacts: strict-only diagnostic rules, fix-it generation logic,
  diagnostic code mapping, diagnostics tests.
- Test IDs: `NUL-60-NEG-01`, `NUL-60-FIX-01`.
- Dependencies: `#58`, `#59`, `#51`.
- Done criteria:
  - strict profiles emit required diagnostics with stable codes and spans.
  - fix-its in `NUL-60-FIX-01` are machine-applicable and correct.

### Issue #61: Optional and IUO type support

- Spec anchors: `#e-3-3`, `#part-3`, `#part-5`, `#c-3-1`, `#c-3-3`.
- Objective: implement `T?` and `T!` behavior, including binding, chaining, and
  postfix propagation rules.
- Required artifacts: parser/type-checker support for optional forms, flow
  typing for bindings, lowering behavior tests, semantic/lowering tests.
- Test IDs: `OPT-61-01`, `OPT-61-02`, `OPT-61-03`.
- Dependencies: `#58`.
- Done criteria:
  - optional binding/chaining/propagation behavior matches Part 3/5 semantics.
  - all `OPT-61-*` tests pass for both success and nil-carrier paths.

### Issue #62: Reference-only optional chaining restriction

- Spec anchors: `#e-3-3`, `#decisions-d-001`, `#part-3`.
- Objective: enforce v1 restriction that optional chaining on member returns is
  reference-only.
- Required artifacts: type-checker restriction checks, targeted diagnostics,
  semantic negative/positive tests.
- Test IDs: `OPT-62-NEG-01`, `OPT-62-POS-01`.
- Dependencies: `#61`.
- Done criteria:
  - value-return member chains that violate v1 restriction are rejected.
  - `OPT-62-NEG-01` and `OPT-62-POS-01` pass with stable diagnostics.

### Issue #63: Optional/nullability metadata preservation

- Spec anchors: `#e-3-3`, `#d-3-1`.
- Objective: preserve optional and nullability information in module metadata
  and textual interfaces.
- Required artifacts: metadata schema fields for optional/nullability, interface
  serializer updates, module_roundtrip tests.
- Test IDs: `META-63-01`, `IFC-63-01`.
- Dependencies: `#54`, `#56`, `#58`, `#59`, `#61`.
- Done criteria:
  - emitted interfaces and module metadata encode equivalent optional/null data.
  - `META-63-01` and `IFC-63-01` pass through emit/import cycles.

### Issue #64: Pragmatic generic types

- Spec anchors: `#e-3-3`, `#part-3`.
- Objective: support generic type parameters with compile-time checking and
  runtime erasure.
- Required artifacts: parser/type-checker generic type model, erasure lowering
  rules, semantic/lowering tests.
- Test IDs: `GEN-64-01`, `GEN-64-02`.
- Dependencies: `#58`.
- Done criteria:
  - generic type constraints are enforced at compile time.
  - generated runtime artifacts remain erased and `GEN-64-*` tests pass.

### Issue #65: Generic methods/functions deferral gate

- Spec anchors: `#e-3-3`, `#decisions-d-008`, `#part-3`.
- Objective: defer generic methods/functions in v1, or gate them behind an
  explicit extension flag.
- Required artifacts: parser/semantic gate checks, extension-flag plumbing,
  diagnostics docs, negative tests.
- Test IDs: `GEN-65-NEG-01`, `GEN-65-GATE-01`.
- Dependencies: `#64`.
- Done criteria:
  - generic methods/functions are rejected by default in v1 mode.
  - if extension mode exists, `GEN-65-GATE-01` proves opt-in behavior only.

### Issue #66: Key path literal and typing support

- Spec anchors: `#e-3-3`, `#part-3`.
- Objective: implement key-path literal typing rules, or mark unsupported and
  withhold feature-macro claim.
- Required artifacts: key-path parser/type checker, fallback unsupported marker,
  macro claim gating, semantic tests.
- Test IDs: `KP-66-01`, `KP-66-GATE-01`.
- Dependencies: `#50`, `#58`.
- Done criteria:
  - either full key-path behavior is implemented and tested, or unsupported
    state is explicit with correct macro gating.
  - `KP-66-01` and `KP-66-GATE-01` pass for the selected path.

## E.3.4 Memory Management and Lifetime

### Issue #67: ARC semantic preservation

- Spec anchors: `#e-3-4`, `#part-4`.
- Objective: preserve ARC semantics while integrating new language-feature
  lowering.
- Required artifacts: ARC correctness checks in lowering pipeline, regression
  fixtures for retains/releases, lowering_abi tests.
- Test IDs: `ARC-67-01`, `ARC-67-02`.
- Dependencies: `#58`, `#61`, `#64`.
- Done criteria:
  - lowered code for new features does not violate ARC ownership rules.
  - `ARC-67-01` and `ARC-67-02` pass under optimized and debug builds.

### Issue #68: Suspension-point autorelease pool contract

- Spec anchors: `#e-3-4`, `#decisions-d-006`, `#c-7`, `#part-7`,
  `#part-12-5-4`.
- Objective: implement implicit per-async-slice autorelease pools drained at
  suspension points.
- Required artifacts: runtime hook integration for slice pool creation/drain,
  async lowering hooks, runtime conformance tests.
- Test IDs: `ARP-68-RT-01`, `ARP-68-RT-02`.
- Dependencies: `#67`, `#80`, `#81`.
- Done criteria:
  - async slices create and drain autorelease pools per contract boundaries.
  - `ARP-68-RT-01` and `ARP-68-RT-02` pass with deterministic runtime traces.

### Issue #69: Strict lifetime and unsafe-escape diagnostics

- Spec anchors: `#e-3-4`, `#part-4`, `#part-12`.
- Objective: provide strict diagnostics for suspicious lifetime extension,
  stack-bound escape, and unsafe bridging.
- Required artifacts: strict diagnostic rules, fix-it or remediation hints,
  diagnostics fixtures for positive/negative cases.
- Test IDs: `LFT-69-NEG-01`, `LFT-69-FIX-01`.
- Dependencies: `#51`, `#67`, `#68`.
- Done criteria:
  - strict profiles diagnose required lifetime/escape hazards with stable codes.
  - `LFT-69-NEG-01` and `LFT-69-FIX-01` pass with expected spans/fix-its.

## E.3.5 Control Flow and Safety Constructs

### Issue #70: `defer` LIFO semantics

- Spec anchors: `#e-3-5`, `#part-5`, `#part-8-2`.
- Objective: implement `defer` with LIFO scope-exit semantics.
- Required artifacts: parser/AST for defer blocks, scope-stack lowering logic,
  semantic/lowering tests.
- Test IDs: `DEF-70-01`, `DEF-70-02`.
- Dependencies: `#48`.
- Done criteria:
  - defer blocks register in source order and execute in reverse order.
  - `DEF-70-01` and `DEF-70-02` pass for nested scopes.

### Issue #71: `defer` execution on normal and unwind exits

- Spec anchors: `#e-3-5`, `#part-5`, `#part-8-2`, `#part-12-5-5`.
- Objective: ensure `defer` executes for normal scope exit and stack unwinding
  exits where applicable.
- Required artifacts: unwind-path cleanup integration, exception-path runtime
  tests, non-local cleanup conformance fixtures.
- Test IDs: `DEF-71-RT-01`, `DEF-71-RT-02`.
- Dependencies: `#70`.
- Done criteria:
  - normal and unwind exits run each defer action exactly once in LIFO order.
  - `DEF-71-RT-01` and `DEF-71-RT-02` pass in supported EH configurations.

### Issue #72: `guard` and refinement rules

- Spec anchors: `#e-3-5`, `#part-5`.
- Objective: implement `guard` with mandatory early-exit semantics and optional
  or pattern refinement behavior.
- Required artifacts: parser/type-flow support for guard, refinement environment
  updates, semantic tests for control-flow guarantees.
- Test IDs: `GRD-72-01`, `GRD-72-02`.
- Dependencies: `#61`.
- Done criteria:
  - guard true-branch refinements are available after the guard statement.
  - `GRD-72-01` and `GRD-72-02` pass for optionals and pattern guards.

### Issue #73: `match` implementation or provisional gating

- Spec anchors: `#e-3-5`, `#part-5`.
- Objective: implement pattern-matching `match`, or explicitly gate as
  provisional and avoid feature-macro claims.
- Required artifacts: parser/semantic implementation or provisional gate path,
  macro gating behavior, semantic parser tests.
- Test IDs: `MTC-73-01`, `MTC-73-GATE-01`.
- Dependencies: `#50`, `#72`.
- Done criteria:
  - implemented path: `match` semantics follow Part 5 and tests pass.
  - provisional path: feature is blocked with clear diagnostics and macro off.

### Issue #74: Illegal non-local exits from `defer`

- Spec anchors: `#e-3-5`, `#part-5`, `#part-8`, `#part-12-5-5`.
- Objective: diagnose forbidden non-local exits from `defer` bodies and related
  ill-formed constructs in strict modes.
- Required artifacts: semantic validation for forbidden exits, diagnostic code
  map, strict diagnostics fixtures for each banned control transfer.
- Test IDs: `NLE-74-NEG-01`, `NLE-74-NEG-02`.
- Dependencies: `#51`, `#70`, `#71`.
- Done criteria:
  - forbidden `defer` exits are rejected with stable diagnostics in strict
    profiles.
  - `NLE-74-NEG-01` and `NLE-74-NEG-02` pass with asserted spans and severity.

## E.3.6 Errors and `throws`

### Issue #75: Untyped `throws` v1 model

- Spec anchors: `#e-3-6`, `#decisions-d-002`, `#part-6`.
- Objective: implement untyped `throws` as the v1 language model.
- Required artifacts: parser/effect typing support for untyped throws, function
  type representation updates, semantic tests.
- Test IDs: `THR-75-01`, `THR-75-02`.
- Dependencies: `#48`.
- Done criteria:
  - declarations and calls enforce untyped throw-effect rules from Part 6.
  - `THR-75-01` and `THR-75-02` pass for declaration, call, and override cases.

### Issue #76: Stable `throws` ABI and lowering

- Spec anchors: `#e-3-6`, `#decisions-d-009`, `#c-4`, `#d-3-2`.
- Objective: provide stable ABI and lowering contract for `throws`.
- Required artifacts: lowering ABI implementation, metadata emission for ABI
  fields, lowering_abi and module_roundtrip tests.
- Test IDs: `THR-76-ABI-01`, `THR-76-ABI-02`.
- Dependencies: `#75`.
- Done criteria:
  - generated call/return ABI for throwing functions is stable and documented.
  - `THR-76-ABI-01` and `THR-76-ABI-02` pass across optimization levels.

### Issue #77: `try`, `do/catch`, and propagation behavior

- Spec anchors: `#e-3-6`, `#part-6`, `#c-3-3`.
- Objective: support `try`, `do/catch`, and propagation semantics integrated
  with optional-carrier behavior.
- Required artifacts: parser/control-flow semantics for try/catch forms, effect
  propagation checks, semantic/runtime tests.
- Test IDs: `TRY-77-01`, `TRY-77-02`.
- Dependencies: `#61`, `#75`.
- Done criteria:
  - propagation and handling semantics match Part 6 in nested contexts.
  - `TRY-77-01` and `TRY-77-02` pass for success, throw, and rethrow paths.

### Issue #78: NSError/status-code bridging attributes

- Spec anchors: `#e-3-6`, `#b-4`, `#part-6`, `#d-3-1`.
- Objective: support bridging attributes and preserve them in module metadata.
- Required artifacts: attribute parser/type checker integration, lowering rules
  for bridge forms, metadata/interface preservation tests.
- Test IDs: `BRG-78-01`, `BRG-78-02`, `BRG-78-MOD-01`.
- Dependencies: `#53`, `#56`, `#75`.
- Done criteria:
  - supported bridging attributes type-check and lower as specified.
  - metadata/interface round-trip preserves bridge attributes; all `BRG-78-*`
    tests pass.

### Issue #79: Strict diagnostics for error misuse

- Spec anchors: `#e-3-6`, `#part-12`.
- Objective: provide strict diagnostics for ignored errors, missing `try`, and
  invalid bridging patterns.
- Required artifacts: strict diagnostics ruleset, fix-it guidance for common
  misuse, diagnostics tests with portable assertion metadata.
- Test IDs: `ERR-79-NEG-01`, `ERR-79-FIX-01`.
- Dependencies: `#51`, `#75`, `#77`, `#78`.
- Done criteria:
  - strict profiles emit required diagnostics with stable code/severity/span.
  - `ERR-79-NEG-01` and `ERR-79-FIX-01` pass with required fix-it assertions.
