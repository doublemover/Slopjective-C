# M253 Method Property And Ivar List Emission Core Feature Implementation Expectations (C004)

Contract ID: `objc3c-runtime-member-table-emission/m253-c004-v1`
Status: Accepted
Issue: `#7093`
Scope: M253 lane-C implementation of real method/property/ivar payload emission in native IR/object output.

## Objective

Emit real owner-scoped method tables plus real property and ivar descriptor payloads in native LLVM IR/object output while preserving the already-closed class/protocol/category descriptor bundle shapes from `M253-C002` and `M253-C003`.

## Required Invariants

1. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-member-table-emission/m253-c004-v1`
   - method-list payload model `owner-scoped-method-table-globals-with-inline-entry-records`
   - method-list grouping model `declaration-owner-plus-class-kind-lexicographic`
   - property payload model `property-descriptor-records-with-accessor-and-binding-strings`
   - ivar payload model `ivar-descriptor-records-with-property-binding-strings`.
2. `native/objc3c/src/lower/objc3_lowering_contract.cpp` publishes one deterministic C004 summary through `Objc3RuntimeMetadataMemberTableEmissionSummary()` and keeps the non-goal boundary explicit.
3. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` must build fail-closed:
   - declaration-owner/class-kind ordered method-table bundles,
   - real property descriptor bundles,
   - real ivar descriptor bundles,
   from the existing typed metadata handoff without reopening the C002/C003 descriptor-family shapes.
4. `native/objc3c/src/ir/objc3_ir_emitter.h` and `native/objc3c/src/ir/objc3_ir_emitter.cpp` must carry the typed member-table surface and emit:
   - `; runtime_metadata_member_table_emission = ...`
   - `!objc3.objc_runtime_member_table_emission`
   - real owner-scoped method-table globals such as:
     - `@__objc3_meta_class_instance_methods_0001`
     - `@__objc3_meta_protocol_instance_methods_0004`
     - `@__objc3_meta_category_instance_methods_0000`
   - real property descriptor globals such as:
     - `@__objc3_meta_property_0000`
     - `@__objc3_meta_property_0001`
   - real ivar descriptor globals such as:
     - `@__objc3_meta_ivar_0000`
     - `@__objc3_sec_property_descriptors`
     - `@__objc3_sec_ivar_descriptors`.
5. Property and ivar families may no longer use `[1 x i8] zeroinitializer` placeholder descriptors on the happy path.
6. Class/protocol/category descriptor bundle shapes remain additive-only: `M253-C004` may emit adjacent owner-scoped member tables but must not regress the closed `M253-C002` / `M253-C003` descriptor families.
7. `native/objc3c/src/io/objc3_process.cpp` remains explicit that llvm-direct object emission preserves emitted method/property/ivar payloads verbatim.
8. Happy-path native emission over the two metadata fixtures must emit:
   - `module.ll`
   - `module.obj`
   - `module.manifest.json`
   - `module.object-backend.txt`
   - `module.runtime-metadata.bin`
   - an empty `module.diagnostics.txt`.
9. The emitted object must expose real method/property/ivar payload bytes through `llvm-readobj --sections`, with nontrivial bytes and relocations rather than the older placeholder shape.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3` proves:
   - `module.object-backend.txt` is `llvm-direct`
   - `module.ll` carries `!objc3.objc_runtime_member_table_emission`
   - method-list bundle count `5`
   - method-entry count `7`
   - property descriptor count `5`
   - ivar descriptor count `2`
   - counts include the declaration-owner split that is now carried forward into the executable object-model handoff
   - class and protocol sections contain adjacent method-table globals
   - property and ivar sections contain real descriptor payloads and aggregates
   - no property/ivar family placeholder globals remain
   - `llvm-readobj --sections module.obj` exposes nontrivial class/protocol/property/ivar sections.
2. Native compile probe over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3` proves:
   - method-list bundle count `3`
   - method-entry count `5`
   - property descriptor count `5`
   - ivar descriptor count `2`
   - counts include the category declaration-owner split that is now carried forward into the executable object-model handoff
   - category and protocol sections contain adjacent method-table globals
   - property and ivar sections contain real descriptor payloads and aggregates
   - no property/ivar family placeholder globals remain
   - `llvm-readobj --sections module.obj` exposes nontrivial protocol/category/property/ivar sections.

## Non-Goals and Fail-Closed Rules

- `M253-C004` does not add selector/string-pool families.
- `M253-C004` does not add runtime registration/bootstrap.
- `M253-C004` does not reopen the closed class/protocol/category descriptor bundle shapes from `M253-C002` / `M253-C003`.
- If method/property/ivar payload accounting drifts from the typed metadata/source-export/scaffold boundaries, emission must fail closed rather than silently shrinking or broadening the member-table payloads.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m253-c004-method-property-and-ivar-list-emission-core-feature-implementation`.
- `package.json` includes `test:tooling:m253-c004-method-property-and-ivar-list-emission-core-feature-implementation`.
- `package.json` includes `check:objc3c:m253-c004-lane-c-readiness`.

## Validation

- `python scripts/check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C004/method_property_and_ivar_list_emission_summary.json`
