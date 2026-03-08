# M253-A002 Source-to-Section Mapping Completeness Matrix Conformance Matrix Implementation Packet

- Packet: `M253-A002`
- Milestone: `M253`
- Lane: `A`
- Contract ID: `objc3c-runtime-metadata-source-to-section-matrix/m253-a002-v1`
- Dependencies: `M253-A001`
- Evidence path: `tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json`

## Objective

Publish the deterministic completeness matrix that maps every supported executable metadata graph node kind to the currently emitted runtime metadata section payload, symbol family, relocation behavior, and proof anchor.

## Required implementation

- Add a typed matrix row schema and canonical summary packet in `native/objc3c/src/pipeline/objc3_frontend_types.h`.
- Build the matrix in `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` from:
  - `Objc3ExecutableMetadataSourceGraph`
  - `Objc3RuntimeMetadataSectionAbiFreezeSummary`
  - `Objc3RuntimeMetadataSectionScaffoldSummary`
  - `Objc3RuntimeMetadataObjectInspectionHarnessSummary`
- Publish the canonical manifest surface at `frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_matrix`.
- Preserve the frozen row ordering model `source-graph-node-kind-order-v1`.
- Preserve explicit no-standalone-emission rows for `interface`, `implementation`, `metaclass`, and `method`.
- Preserve explicit emitted rows for `class`, `protocol`, `category`, `property`, and `ivar`.
- Preserve proof binding to:
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
  - `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`
  - `llvm-readobj --sections module.obj`
  - `llvm-objdump --syms module.obj`

## Acceptance

- Deterministic row set with 9 rows covering all currently supported executable metadata graph node kinds.
- Matrix rows map to concrete fixtures or inspection commands.
- Emitted rows preserve `zero-sentinel-or-count-plus-pointer-vector` relocation behavior.
- The checker and pytest fail closed on row drift, surface drift, or proof drift.
- `npm run check:objc3c:m253-a002-lane-a-readiness` passes.
