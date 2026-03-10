# M256-D001 Class Realization Runtime Contract and Architecture Freeze Packet

Packet: `M256-D001`
Milestone: `M256`
Wave: `W48`
Lane: `D`
Issue: `#7139`
Contract ID: `objc3c-runtime-class-realization-freeze/m256-d001-v1`
Dependencies: `None`

## Objective

Freeze the runtime implementation contract for class realization, metaclass graph construction, category attachment, and protocol-aware negative checks over the current live executable object-model surface.

## Canonical Runtime Surface

- contract id `objc3c-runtime-class-realization-freeze/m256-d001-v1`
- class realization model `registered-class-bundles-realize-one-deterministic-class-metaclass-chain-per-class-name`
- metaclass graph model `known-class-and-class-self-receivers-normalize-onto-the-metaclass-record-chain`
- category attachment model `preferred-category-implementation-records-attach-after-class-bundle-resolution`
- protocol check model `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-runtime-checks`
- fail-closed model `invalid-bundle-graphs-category-conflicts-and-ambiguous-runtime-resolution-fail-closed`
- emitted IR comment `; runtime_class_realization = ...`
- proof source `tests/tooling/runtime/m256_d001_class_realization_runtime_probe.cpp`

## Acceptance Criteria

- Add explicit D001 constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add one deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/parse/objc3_parser.cpp` explicit that parser does not decide runtime class realization behavior.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema remains the owner of realization legality, category conflict legality, protocol conformance legality, and inheritance identities.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish `; runtime_class_realization = ...` adjacent to the existing realization-record summary.
- Keep `native/objc3c/src/runtime/objc3_runtime.h` and `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` as the stable public/private proof surface for the current class realization boundary.
- Update `native/objc3c/src/runtime/objc3_runtime.cpp` documentation anchors so the current live runtime behavior is explicit and deterministic.
- Add deterministic docs/spec/package/checker/test evidence under `tmp/reports/m256/M256-D001/`.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m256_d001_class_realization_runtime_library.objc3` proving the emitted artifact set contains:
   - `module.manifest.json`
   - `module.runtime-registration-manifest.json`
   - `module.ll`
   - `module.obj`
   - `module.object-backend.txt`
2. The emitted IR must publish:
   - `; executable_realization_records = ...`
   - `; runtime_class_realization = contract=objc3c-runtime-class-realization-freeze/m256-d001-v1`
   - the class realization, metaclass graph, category attachment, protocol check, and fail-closed tokens
3. Runtime probe execution proves:
   - instance dispatch to `inheritedValue` resolves `Base`
   - category-backed dispatch to `tracedValue` resolves `Widget(Tracing)` with one category probe and zero protocol probes
   - metaclass dispatch to `classValue` resolves `Widget::class_method:classValue`
   - known-class receiver normalization reuses the metaclass cache entry for `classValue`
   - `ignoredValue` remains unresolved, falls back compatibly, and leaves one unresolved negative cache entry with preserved category/protocol probe counts
4. Runtime registration snapshots must agree with the emitted registration manifest counts and translation-unit identity key.
5. Lane-D readiness replays `M256-C003` before the D001 checker so the frozen runtime boundary remains aligned with the live realization-record artifact surface.

## Non-Goals

- `M256-D001` does not add property storage realization.
- `M256-D001` does not add ivar layout realization.
- `M256-D001` does not add synthesized accessor execution.
- `M256-D001` does not add executable protocol-body dispatch.
- `M256-D001` does not add cross-image class coalescing beyond the current ordered image walk.

## Validation

- `python scripts/check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m256_d001_class_realization_runtime_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m256-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m256/M256-D001/class_realization_runtime_contract_summary.json`
