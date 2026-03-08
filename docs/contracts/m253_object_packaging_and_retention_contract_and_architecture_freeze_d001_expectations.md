# M253 Object Packaging And Retention Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-object-packaging-retention-boundary/m253-d001-v1`

## Required outcomes

1. `Objc3RuntimeMetadataObjectPackagingRetentionSummary` becomes the canonical lane-D object-packaging and retention freeze packet.
2. `native/objc3c/src/lower/objc3_lowering_contract.h` and `native/objc3c/src/lower/objc3_lowering_contract.cpp` freeze:
   - boundary model `current-object-file-boundary-with-retained-metadata-section-aggregates`,
   - retention-anchor model `llvm.used-plus-aggregate-section-symbols`,
   - object artifact `module.obj`,
   - aggregate symbol prefix `__objc3_sec_`.
3. `native/objc3c/src/ir/objc3_ir_emitter.cpp` publishes the same contract in emitted LLVM IR through:
   - `; runtime_metadata_object_packaging_retention = ...`
   - `!objc3.objc_runtime_object_packaging_retention`
4. The positive probe fixture is `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3` and must prove:
   - `module.manifest.json`, `module.ll`, and `module.obj` are emitted,
   - `module.object-backend.txt` is `llvm-direct`,
   - `llvm-readobj --sections module.obj` exposes the current runtime metadata section families,
   - `llvm-objdump --syms module.obj` exposes retained aggregate symbols rooted at `__objc3_sec_` plus `__objc3_image_info`.
5. The negative probe fixture is `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3` and must fail closed with no emitted `module.manifest.json`, no emitted `module.obj`, and no backend marker.
6. Validation evidence is written to `tmp/reports/m253/M253-D001/object_packaging_and_retention_contract_summary.json`.
7. Non-goals remain explicit:
   - no archive packaging contract yet,
   - no link-registration contract yet,
   - no startup registration/bootstrap yet.
