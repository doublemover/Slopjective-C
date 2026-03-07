# M251 LLVM Global and Section Scaffold for Runtime Metadata Payloads Expectations (C002)

Contract ID: `objc3c-runtime-metadata-section-scaffold/m251-c002-v1`

Scope: Lane-C real LLVM global/section scaffold emission for runtime metadata payloads.

## Required outcomes

1. `Objc3RuntimeMetadataSectionScaffoldSummary` becomes the canonical lane-C scaffold packet for emitted runtime metadata globals.
2. The native IR path emits a retained image-info global named `__objc3_image_info` in `objc3.runtime.image_info`.
3. The native IR path emits retained aggregate globals named `__objc3_sec_class_descriptors`, `__objc3_sec_protocol_descriptors`, `__objc3_sec_category_descriptors`, `__objc3_sec_property_descriptors`, and `__objc3_sec_ivar_descriptors`.
4. Descriptor placeholders use the canonical descriptor prefix `__objc3_meta_` and land in the logical runtime metadata sections frozen by `M251-C001`.
5. The scaffold is retained through `@llvm.used` so later object emission and object inspection can observe the globals even before full payload layouts land.
6. Manifest JSON and emitted LLVM IR publish the same scaffold contract surface through `runtime_metadata_section_scaffold_contract_id` and `!objc3.objc_runtime_metadata_section_scaffold`.
7. Validation evidence is written to `tmp/reports/m251/M251-C002/runtime_metadata_section_scaffold_summary.json`.

## Dynamic proof points

- A manifest-only probe on `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3` proves the scaffold summary is emitted, fail-closed, retained with `llvm.used`, and reports non-zero descriptor counts for class, protocol, property, and ivar families.
- A native compile probe on `tests/tooling/fixtures/native/hello.objc3` proves LLVM IR/object emission carries the retained image-info and zero-count aggregate section globals for class, protocol, category, property, and ivar sections, `@llvm.used`, and the named metadata node `!objc3.objc_runtime_metadata_section_scaffold` through the real LLVM/object path.

## Validation commands

- `python scripts/check_m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads.py`
- `python -m pytest tests/tooling/test_check_m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads.py -q`
- `npm run check:objc3c:m251-c002-lane-c-readiness`
