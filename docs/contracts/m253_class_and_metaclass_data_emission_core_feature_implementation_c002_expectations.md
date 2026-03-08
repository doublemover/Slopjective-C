# M253 Class And Metaclass Data Emission Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-runtime-class-metaclass-data-emission/m253-c002-v1`
Status: Accepted
Issue: `#7091`
Scope: M253 lane-C implementation of the first real executable metadata payload family in native IR/object output.

## Objective

Replace the class-family placeholder payload model with real class/metaclass descriptor bundles in the emitted `objc3.runtime.class_descriptors` section while preserving the frozen inventory, ordering, visibility, and object-format boundaries established by `M253-A001`, `M253-A002`, `M253-B001`, `M253-B002`, `M253-B003`, and `M253-C001`.

## Required Invariants

1. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-class-metaclass-data-emission/m253-c002-v1`
   - payload model `class-source-record-descriptor-bundles-with-inline-metaclass-records`
   - name model `shared-class-name-cstring-per-bundle`
   - super-link model `nullable-super-source-record-bundle-pointer`
   - method-list reference model `count-plus-owner-identity-pointer-method-list-ref`.
2. `native/objc3c/src/lower/objc3_lowering_contract.cpp` publishes one deterministic C002 summary through `Objc3RuntimeMetadataClassMetaclassEmissionSummary()` and keeps the non-goal boundary explicit.
3. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` admits metadata-rich native IR emission without the older global parse/lowering gate when the typed metadata handoff, source ownership boundary, export legality/enforcement, scaffold summary, source-to-section matrix, debug projection, and runtime-ingest packaging/binary boundaries are ready, and must build one declaration-owner-ordered `runtime_metadata_class_metaclass_bundles_lexicographic` vector.
4. `native/objc3c/src/ir/objc3_ir_emitter.h` and `native/objc3c/src/ir/objc3_ir_emitter.cpp` must carry the typed class/metaclass bundle surface and emit:
   - `; runtime_metadata_class_metaclass_emission = ...`
   - `!objc3.objc_runtime_class_metaclass_emission`
   - four real bundle globals in `objc3.runtime.class_descriptors`
   - four owner-identity cstrings
   - four instance-method-list reference globals
   - four metaclass-method-list reference globals
   - one `{ i64, [4 x ptr] }` class aggregate.
5. `native/objc3c/src/io/objc3_process.cpp` remains explicit that llvm-direct object emission preserves the inline class/metaclass/name/method-ref payloads verbatim.
6. Happy-path native emission over `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3` must emit:
   - `module.ll`
   - `module.obj`
   - `module.manifest.json`
   - `module.object-backend.txt`
   - `module.runtime-metadata.bin`
   - an empty `module.diagnostics.txt`.
7. The emitted IR must carry the C002 summary and real payload globals including:
   - `@__objc3_meta_class_0000`
   - `@__objc3_meta_class_0003`
   - `@__objc3_meta_class_owner_identity_0000`
   - `@__objc3_meta_class_owner_identity_0003`
   - `@__objc3_meta_class_instance_method_list_ref_0000`
   - `@__objc3_meta_metaclass_method_list_ref_0003`
   - `@__objc3_sec_class_descriptors = internal global { i64, [4 x ptr] } ...`
   and the class family may no longer use `[1 x i8] zeroinitializer` placeholders.
8. The emitted object must expose a real `objc3.runtime.class_descriptors` section through `llvm-readobj --sections`, with nontrivial bytes and relocations rather than the frozen zero/placeholder shape.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3` proves:
   - `module.object-backend.txt` is `llvm-direct`
   - the typed metadata handoff remains lowering-ready
   - the class/source graph counts remain synchronized with the emitted bundle count
   - `module.ll` carries `!objc3.objc_runtime_class_metaclass_emission`
   - `module.obj` exposes a real `objc3.runtime.class_descriptors` section through `llvm-readobj --sections`
   - `llvm-objdump --syms module.obj` still exposes the class aggregate symbol `__objc3_sec_class_descriptors`.

## Non-Goals and Fail-Closed Rules

- `M253-C002` does not introduce standalone metaclass sections.
- `M253-C002` does not introduce selector pools or string-pool sections.
- `M253-C002` does not introduce standalone method/property/ivar list payload sections.
- `M253-C002` does not add runtime registration/bootstrap.
- Protocol/category/property/ivar families remain on their currently frozen payload models until later issues land.
- If class-descriptor bundle accounting drifts from the typed metadata/source-export/scaffold boundaries, emission must fail closed rather than silently shrinking or broadening the class family.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m253-c002-class-and-metaclass-data-emission-core-feature-implementation`.
- `package.json` includes `test:tooling:m253-c002-class-and-metaclass-data-emission-core-feature-implementation`.
- `package.json` includes `check:objc3c:m253-c002-lane-c-readiness`.

## Validation

- `python scripts/check_m253_c002_class_and_metaclass_data_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_c002_class_and_metaclass_data_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C002/class_and_metaclass_data_emission_summary.json`
