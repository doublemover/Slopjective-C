# M253 Binary Inspection Harness For Emitted Metadata Conformance Corpus Expansion Expectations (C006)

Contract ID: `objc3c-runtime-binary-inspection-harness/m253-c006-v1`

## Required outcomes

1. `Objc3RuntimeMetadataBinaryInspectionHarnessSummary` becomes the canonical lane-C emitted-metadata binary inspection corpus packet.
2. `native/objc3c/src/lower/objc3_lowering_contract.h` and `native/objc3c/src/lower/objc3_lowering_contract.cpp` freeze:
   - positive corpus model `positive-structural-section-and-symbol-corpus-with-case-specific-absence-checks`,
   - negative corpus model `negative-compile-failure-gating-with-no-object-inspection`,
   - section inventory command `llvm-readobj --sections module.obj`,
   - symbol inventory command `llvm-objdump --syms module.obj`.
3. `native/objc3c/src/ir/objc3_ir_emitter.cpp` publishes the same contract in emitted LLVM IR through:
   - `; runtime_metadata_binary_inspection_harness = ...`
   - `!objc3.objc_runtime_binary_inspection_harness`
4. The positive corpus must cover all currently emitted metadata section families across:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
   - `tests/tooling/fixtures/native/execution/positive/message_send_runtime_shim.objc3`
5. The negative corpus must cover `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3` and fail closed with no emitted `module.obj` or `module.manifest.json`.
6. The harness must structurally assert, from produced object files, the presence and absence of:
   - `objc3.runtime.image_info`
   - `objc3.runtime.class_descriptors`
   - `objc3.runtime.protocol_descriptors`
   - `objc3.runtime.category_descriptors`
   - `objc3.runtime.property_descriptors`
   - `objc3.runtime.ivar_descriptors`
   - `objc3.runtime.selector_pool`
   - `objc3.runtime.string_pool`
7. The harness must inspect aggregate metadata symbols such as:
   - `__objc3_image_info`
   - `__objc3_sec_class_descriptors`
   - `__objc3_sec_protocol_descriptors`
   - `__objc3_sec_category_descriptors`
   - `__objc3_sec_property_descriptors`
   - `__objc3_sec_ivar_descriptors`
   - `__objc3_sec_selector_pool`
   - `__objc3_sec_string_pool`
8. Validation evidence is written to `tmp/reports/m253/M253-C006/binary_inspection_harness_summary.json`.
9. Non-goals remain explicit:
   - no new metadata families,
   - no runtime registration/bootstrap,
   - no descriptor-family rewiring beyond the already-closed `M253-C005` surface.
