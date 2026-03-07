# M251 Metadata Object Inspection Harness Conformance Matrix Implementation Expectations (C003)

Contract ID: `objc3c-runtime-metadata-object-inspection-harness/m251-c003-v1`

Scope: Lane-C deterministic object inspection matrix for emitted runtime metadata scaffold objects.

## Required outcomes

1. `Objc3RuntimeMetadataObjectInspectionHarnessSummary` becomes the canonical lane-C inspection matrix packet.
2. Manifest JSON and emitted LLVM IR publish the same inspection contract through `runtime_metadata_object_inspection_contract_id` and `!objc3.objc_runtime_metadata_object_inspection`.
3. The canonical zero-descriptor inspection fixture remains `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`.
4. Matrix row `zero-descriptor-section-inventory` runs `llvm-readobj --sections module.obj` against the emitted `module.obj`.
5. Matrix row `zero-descriptor-symbol-inventory` runs `llvm-objdump --syms module.obj` against the same emitted `module.obj`.
6. The section-inventory row proves the object carries `objc3.runtime.image_info`, `objc3.runtime.class_descriptors`, `objc3.runtime.protocol_descriptors`, `objc3.runtime.category_descriptors`, `objc3.runtime.property_descriptors`, and `objc3.runtime.ivar_descriptors`.
7. The symbol-inventory row proves the object retains `__objc3_image_info`, `__objc3_sec_class_descriptors`, `__objc3_sec_protocol_descriptors`, `__objc3_sec_category_descriptors`, `__objc3_sec_property_descriptors`, and `__objc3_sec_ivar_descriptors`.
8. Validation evidence is written to `tmp/reports/m251/M251-C003/runtime_metadata_object_inspection_harness_summary.json`.

## Dynamic proof points

- A native compile probe on `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3` proves manifest JSON publishes the inspection matrix contract and that emitted `module.obj` can be inspected with `llvm-readobj --sections` and `llvm-objdump --syms`.
- The object inspection probe proves the direct `llc` backend emitted the object and that the metadata section and retained-symbol inventory matches the published matrix.

## Validation commands

- `python scripts/check_m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m251-c003-lane-c-readiness`
