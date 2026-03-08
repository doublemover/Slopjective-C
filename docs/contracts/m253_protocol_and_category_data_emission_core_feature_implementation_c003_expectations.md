# M253 Protocol And Category Data Emission Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-runtime-protocol-category-data-emission/m253-c003-v1`
Status: Accepted
Issue: `#7092`
Scope: M253 lane-C implementation of real protocol/category payload emission in native IR/object output.

## Objective

Replace the protocol/category placeholder payload model with real protocol descriptors, category records, inherited/adopted protocol-reference payloads, and category attachment lists in the emitted `objc3.runtime.protocol_descriptors` and `objc3.runtime.category_descriptors` sections while preserving the frozen inventory, ordering, visibility, object-format, and scaffold boundaries established by `M253-A001`, `M253-A002`, `M253-B001`, `M253-B002`, `M253-B003`, `M253-C001`, and `M253-C002`.

## Required Invariants

1. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-protocol-category-data-emission/m253-c003-v1`
   - protocol payload model `protocol-descriptor-bundles-with-inherited-protocol-ref-lists`
   - category payload model `category-descriptor-bundles-with-attachment-and-protocol-ref-lists`
   - protocol-reference model `count-plus-descriptor-pointer-protocol-ref-lists`
   - category-attachment model `count-plus-owner-identity-pointer-attachment-lists`.
2. `native/objc3c/src/lower/objc3_lowering_contract.cpp` publishes one deterministic C003 summary through `Objc3RuntimeMetadataProtocolCategoryEmissionSummary()` and keeps the non-goal boundary explicit.
3. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` must build fail-closed declaration-owner-ordered protocol bundles and source-record-ordered category bundles from the existing typed metadata handoff, including expansion of one combined category graph node into explicit interface/implementation descriptor records when both records are present.
4. `native/objc3c/src/ir/objc3_ir_emitter.h` and `native/objc3c/src/ir/objc3_ir_emitter.cpp` must carry the typed protocol/category bundle surface and emit:
   - `; runtime_metadata_protocol_category_emission = ...`
   - `!objc3.objc_runtime_protocol_category_emission`
   - real `@__objc3_meta_protocol_0000` / `@__objc3_meta_protocol_0001` descriptor globals
   - real `@__objc3_meta_category_0000` / `@__objc3_meta_category_0001` descriptor globals
   - `@__objc3_meta_protocol_inherited_protocol_refs_0001`
   - `@__objc3_meta_category_adopted_protocol_refs_0000`
   - `@__objc3_meta_category_attachments_0001`
   - one `{ i64, [2 x ptr] }` protocol aggregate and one `{ i64, [2 x ptr] }` category aggregate.
5. The emitted protocol/category families may no longer use `[1 x i8] zeroinitializer` placeholder descriptors on the happy path.
6. `native/objc3c/src/io/objc3_process.cpp` remains explicit that llvm-direct object emission preserves the emitted protocol/category descriptor bundles, protocol-ref lists, and owner-identity attachment lists verbatim.
7. Happy-path native emission over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3` must emit:
   - `module.ll`
   - `module.obj`
   - `module.manifest.json`
   - `module.object-backend.txt`
   - `module.runtime-metadata.bin`
   - an empty `module.diagnostics.txt`.
8. The emitted object must expose real `objc3.runtime.protocol_descriptors` and `objc3.runtime.category_descriptors` sections through `llvm-readobj --sections`, with nontrivial bytes and relocations rather than the frozen placeholder shape.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3` proves:
   - `module.object-backend.txt` is `llvm-direct`
   - the typed metadata handoff remains lowering-ready
   - `module.ll` carries `!objc3.objc_runtime_protocol_category_emission`
   - protocol bundle count `2`
   - category bundle count `2`
   - inherited protocol-ref list count `2`
   - adopted protocol-ref list count `2`
   - category attachment-list count `2`
   - no protocol/category family placeholder globals remain
   - `llvm-readobj --sections module.obj` exposes protocol/category sections with nontrivial bytes/relocations
   - `llvm-objdump --syms module.obj` exposes `__objc3_sec_protocol_descriptors` and `__objc3_sec_category_descriptors`.

## Non-Goals and Fail-Closed Rules

- `M253-C003` does not add selector/string-pool payloads.
- `M253-C003` does not add standalone property/ivar payload sections.
- `M253-C003` does not add runtime registration/bootstrap.
- Property and ivar families remain on their currently frozen payload models until later issues land.
- If protocol/category descriptor accounting drifts from the typed metadata/source-export/scaffold boundaries, emission must fail closed rather than silently shrinking or broadening the protocol/category families.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m253-c003-protocol-and-category-data-emission-core-feature-implementation`.
- `package.json` includes `test:tooling:m253-c003-protocol-and-category-data-emission-core-feature-implementation`.
- `package.json` includes `check:objc3c:m253-c003-lane-c-readiness`.

## Validation

- `python scripts/check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C003/protocol_and_category_data_emission_summary.json`
