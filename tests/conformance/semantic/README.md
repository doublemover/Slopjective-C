# Semantic Bucket

Minimum scope:

- nullability completeness and optional restrictions,
- `throws`/`async` typing constraints,
- actor isolation and Sendable boundary checks,
- strict-profile-only ill-formed constructs.

## Type-system wave fixture set (E.3.3)

Current semantic fixtures for the E.3.3 type-system lane:

- `TYP-58-01.json`, `TYP-58-02.json` for nullability qualifier semantics.
- `NNB-59-01.json`, `NNB-59-02.json` for nonnull-by-default region behavior.
- `OPT-61-01.json`, `OPT-61-02.json`, `OPT-61-03.json` for optional and IUO
  behavior.
- `OPT-62-NEG-01.json`, `OPT-62-POS-01.json` for v1 reference-only optional
  chaining.
- `GEN-64-01.json`, `GEN-64-02.json` for pragmatic generic type behavior.
- `GEN-65-01.json`, `GEN-65-02.json` for D-008 generic method/function
  rejection, including removed extension-flag diagnostics.
- `KPATH-66-01.json`, `KPATH-66-02.json` for key path support and
  unsupported-mode diagnostics.
- `TYP-8013-01.json` through `TYP-8013-22.json` for issue #8013 typed object,
  nullability, canonical protocol conformance, protocol-composition,
  protocol-qualified message dispatch, typed object-pointer receiver dispatch,
  generic parameter binding/substitution/constraint diagnostics, nested generic
  specialization diagnostics and runtime-exported nested specialization
  metadata, declared generic variance metadata, covariant assignment admission,
  invariant-specialization assignment rejection, protocol-qualified generic
  arguments and unknown-protocol rejection, cross-module generic and protocol
  contract preservation through runtime import surfaces, inherited protocol
  requirement conflict rejection, category/protocol adoption preservation,
  fail-closed associated-type and dynamic protocol dispatch claims,
  generic-suffix, optional-flow, and key-path semantic model closure. These are
  backed by
  `tests/tooling/fixtures/native/type_semantic_model_closure_positive.objc3`,
  `tests/tooling/fixtures/native/type_semantic_nested_generic_positive.objc3`,
  `tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3`,
  `tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3`,
  `tests/tooling/fixtures/native/category_attachment_protocol_runtime_library.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_duplicate_protocol_composition.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_nullable_to_nonnull_flow.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_method_nullability_conflict.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_property_nullability_conflict.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_optional_required_conflict.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_unknown_protocol_composition.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_qualified_unknown_message.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_typed_object_receiver_unknown_message.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_substitution_unknown_message.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_nested_generic_constraint_violation.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_generic_unknown_protocol.objc3`,
  and `scripts/build_objc3c_type_semantic_model_closure.py`.
- `EFF-8014-01.json`, `EFF-8014-02.json`, `EFF-8014-03.json`, and
  `EFF-8014-04.json` for issue #8014
  effects and ownership semantic closure across ARC, escaping blocks/byref
  cells, copy/dispose helper symbolization, throws/unwind cleanup, async/task
  cancellation, actor metadata, Objective-C method-family retainable-result
  legality, and interop boundaries. These are backed by
  `tests/tooling/fixtures/native/effects_ownership_semantic_model_positive.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_effects_ownership_async_throws.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_effects_ownership_method_family_scalar_return.objc3`,
  and `scripts/build_objc3c_effects_ownership_semantic_model.py`.
- `XMOD-8015-01.json`, `XMOD-8015-02.json` for issue #8015 cross-module
  semantic contract and diagnostic closure across module import graph,
  namespace/API partitioning, incremental cache invalidation, conformance,
  diagnostics, and interop import metadata. These are backed by
  `tests/tooling/fixtures/native/cross_module_semantic_contracts_diagnostics_positive.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_cross_module_semantic_contracts_duplicate_module.objc3`,
  and `scripts/build_objc3c_cross_module_semantic_contracts_diagnostics.py`.
- `DEF-70-01.json`, `DEF-70-02.json` for `defer` LIFO scope-exit semantics.
- `GRD-72-01.json`, `GRD-72-02.json` for `guard` early-exit/refinement
  semantics.
- `MTC-73-01.json`, `MTC-73-GATE-01.json` for `match` behavior and
  hard-cutover disabled-feature rejection.
- `THR-75-01.json`, `THR-75-02.json` for untyped `throws` v1 semantics.
- `TRY-77-01.json`, `TRY-77-02.json` for `try`/`do-catch` and propagation
  semantics.
- `BRG-78-01.json`, `BRG-78-02.json` for bridging-attribute semantic checks.
- `ASY-03.json`, `ASY-04.json` for async/await typing and context constraints.
- `CAN-01.json`..`CAN-04.json` for cancellation propagation and task-context
  semantic behavior.
- `EXE-01.json`..`EXE-03.json`, `EXEC-ATTR-01.json`, `EXEC-ATTR-02.json` for
  executor affinity hop rules and annotation legality.
- `ACT-01.json`..`ACT-04.json` for actor declaration/isolation semantics,
  same-actor access behavior, and reentrancy boundary checks.
- `AWT-03.json`, `AWT-04.json` for D-011 await-requirement semantic checks and
  unnecessary-await warnings.
- `SND-01.json`..`SND-06.json` for Sendable-like constraints across task
  captures, actor boundaries, and nonisolated/executor crossings.
- `RES-01.json`..`RES-03.json` for resource cleanup semantic behavior and
  misuse checks.
- `LIFE-01.json`..`LIFE-03.json` for `withLifetime`/`keepAlive` semantic
  behavior and misuse diagnostics.
- `BRW-NEG-01.json`, `BRW-NEG-02.json`, `BRW-POS-01.json`..`BRW-POS-04.json`
  for borrowed-pointer escape rule coverage (intra-procedural minimum).
- `CAP-03.json`..`CAP-05.json` for capture-list evaluation order and
  move/weak/unowned semantic behavior.
- `PERF-DIRMEM-01.json`..`PERF-DIRMEM-03.json`, `PERF-LEG-01.json`,
  `PERF-LEG-02.json` for direct/final/sealed legality checks across inheritance
  and category edges.
- `META-DRV-01.json`..`META-DRV-03.json` for deterministic derive expansion and
  optional-meta capability gating.
- `META-PKG-01.json`..`META-PKG-03.json` for macro package determinism,
  lockfile policy, and sandbox source constraints.
- `INT-C-01.json`..`INT-C-06.json` for C/ObjC interop semantic checks over
  async/throws thunk signatures, parameter ordering, and ownership boundaries.
- `INT-CXX-01.json`..`INT-CXX-04.json` for ObjC++ interop semantic parity
  checks across ownership, throws, and async lowering expectations.
- `INT-SWIFT-01.json`..`INT-SWIFT-04.json` for Swift interop mapping semantics
  over optionals, throws, async/await, and actor-isolation metadata.

See `tests/conformance/semantic/manifest.json` for machine-readable indexing.
