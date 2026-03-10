# M256 Class Realization Runtime Contract and Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-class-realization-freeze/m256-d001-v1`
Status: Accepted
Issue: `#7139`
Scope: M256 lane-D runtime freeze that captures the current live class realization boundary consuming emitted realization records, normalizing class receivers onto the metaclass chain, attaching preferred category implementation records, and using protocol records as declaration-aware negative lookup evidence only.

## Objective

Freeze the current runtime-owned class realization contract so the next implementation issue can extend one truthful live boundary instead of rediscovering class graphs, category attachment, or protocol-aware negative checks out-of-band.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-class-realization-freeze/m256-d001-v1`
   - class realization model `registered-class-bundles-realize-one-deterministic-class-metaclass-chain-per-class-name`
   - metaclass graph model `known-class-and-class-self-receivers-normalize-onto-the-metaclass-record-chain`
   - category attachment model `preferred-category-implementation-records-attach-after-class-bundle-resolution`
   - protocol check model `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-runtime-checks`
   - fail-closed model `invalid-bundle-graphs-category-conflicts-and-ambiguous-runtime-resolution-fail-closed`
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic `Objc3RuntimeClassRealizationSummary()` surface.
3. `parse/objc3_parser.cpp` remains explicit that parser does not decide runtime class realization, metaclass normalization, category attachment, or protocol-aware negative dispatch behavior.
4. `sema/objc3_semantic_passes.cpp` remains explicit that sema owns realized-class legality, category merge/conflict decisions, protocol conformance legality, and inheritance identities; runtime consumes that closure rather than reinterpreting source.
5. `ir/objc3_ir_emitter.cpp` publishes both:
   - `; executable_realization_records = ...`
   - `; runtime_class_realization = ...`
6. `runtime/objc3_runtime.h`, `runtime/objc3_runtime_bootstrap_internal.h`, and `runtime/objc3_runtime.cpp` keep the current public/private proof surface stable while documenting that live class realization sits behind the existing lookup/dispatch ABI.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m256_d001_class_realization_runtime_library.objc3` proves the emitted artifact set contains:
   - `module.manifest.json`
   - `module.runtime-registration-manifest.json`
   - `module.ll`
   - `module.obj`
   - `module.object-backend.txt`
2. The emitted IR must carry:
   - `; executable_realization_records = ...`
   - `; runtime_class_realization = contract=objc3c-runtime-class-realization-freeze/m256-d001-v1`
   - tokens for the class realization, metaclass graph, category attachment, protocol check, and fail-closed models
3. The runtime probe `tests/tooling/runtime/m256_d001_class_realization_runtime_probe.cpp` proves the live runtime behavior over the compiled object:
   - receiver `1042` resolves `inheritedValue` to `implementation:Base::instance_method:inheritedValue` on class `Base`
   - receiver `1042` resolves `tracedValue` to `implementation:Widget(Tracing)::instance_method:tracedValue` on class `Widget`
   - receiver `1043` resolves `classValue` to `implementation:Widget::class_method:classValue`
   - receiver `1041` normalizes onto `1043` and reuses the metaclass cache entry for `classValue`
   - receiver `1042` falls back on `ignoredValue` while preserving category/protocol probe counts and materializing an unresolved negative cache entry
4. Runtime registration snapshots must agree with the emitted registration manifest counts and translation-unit identity key.

## Non-Goals and Fail-Closed Rules

- `M256-D001` does not add property storage realization.
- `M256-D001` does not add ivar layout realization.
- `M256-D001` does not add synthesized accessor execution.
- `M256-D001` does not add executable protocol-body dispatch.
- `M256-D001` does not add cross-image class coalescing beyond the current ordered image walk.
- If emitted realization bundles are ambiguous or category attachment conflicts remain unresolved, runtime must fail closed instead of selecting an arbitrary implementation.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/runtime/README.md`
- `tests/tooling/runtime/README.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m256-d001-class-realization-runtime-contract`.
- `package.json` includes `test:tooling:m256-d001-class-realization-runtime-contract`.
- `package.json` includes `check:objc3c:m256-d001-lane-d-readiness`.

## Validation

- `python scripts/check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m256-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m256/M256-D001/class_realization_runtime_contract_summary.json`
