# M253-C006 Binary Inspection Harness For Emitted Metadata Conformance Corpus Expansion Packet

Packet: `M253-C006`
Milestone: `M253`
Lane: `C`
Depends on:
- `M253-C005`

## Goal

Expand lane-C binary inspection from the minimal `M251-C003` scaffold matrix into a deterministic emitted-metadata corpus that proves every currently emitted section family structurally from produced object files.

## Spec anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/io/objc3_process.cpp`

## Required implementation

1. Freeze contract id `objc3c-runtime-binary-inspection-harness/m253-c006-v1`.
2. Publish the lowering summary through `Objc3RuntimeMetadataBinaryInspectionHarnessSummary()`.
3. Publish the same contract in emitted LLVM IR through `; runtime_metadata_binary_inspection_harness = ...` and `!objc3.objc_runtime_binary_inspection_harness`.
4. Expand the positive corpus to cover:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
   - `tests/tooling/fixtures/native/execution/positive/message_send_runtime_shim.objc3`
5. Add a fail-closed negative corpus case on `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3` proving binary inspection does not run when semantic validation blocks object emission.
6. The harness must use:
   - `llvm-readobj --sections module.obj`
   - `llvm-objdump --syms module.obj`
7. Structural assertions must cover the currently emitted metadata section families:
   - `objc3.runtime.image_info`
   - `objc3.runtime.class_descriptors`
   - `objc3.runtime.protocol_descriptors`
   - `objc3.runtime.category_descriptors`
   - `objc3.runtime.property_descriptors`
   - `objc3.runtime.ivar_descriptors`
   - `objc3.runtime.selector_pool`
   - `objc3.runtime.string_pool`
8. Structural assertions must also cover aggregate metadata symbols and zero/nonzero offsets where applicable.
9. Validation evidence must land at `tmp/reports/m253/M253-C006/binary_inspection_harness_summary.json`.

## Non-goals

- No new emitted metadata families.
- No runtime registration/bootstrap.
- No reopening of `M253-C002` through `M253-C005` descriptor/pool non-goals.

## Validation

- `python scripts/check_m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion.py`
- `python -m pytest tests/tooling/test_check_m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion.py -q`
- `npm run check:objc3c:m253-c006-lane-c-readiness`
