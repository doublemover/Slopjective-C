# M253 Source-to-Section Mapping Completeness Matrix Conformance Matrix Implementation Expectations (A002)

- Contract ID: `objc3c-runtime-metadata-source-to-section-matrix/m253-a002-v1`
- Issue: `M253-A002`
- Dependencies: `M253-A001`
- Validation evidence path: `tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json`

## Required outcomes

1. `Objc3RuntimeMetadataSourceToSectionMatrixSummary` becomes the canonical lane-A packet for mapping executable metadata graph node kinds to the currently emitted runtime metadata inventory.
2. The canonical manifest surface is `frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_matrix`.
3. Row ordering is deterministic and frozen to source-graph node kind order:
   - `interface`
   - `implementation`
   - `class`
   - `metaclass`
   - `protocol`
   - `category`
   - `property`
   - `method`
   - `ivar`
4. Concrete emitted rows remain limited to the current A001 inventory:
   - `class` -> `objc3.runtime.class_descriptors` / `__objc3_meta_class_####` / `__objc3_sec_class_descriptors`
   - `protocol` -> `objc3.runtime.protocol_descriptors` / `__objc3_meta_protocol_####` / `__objc3_sec_protocol_descriptors`
   - `category` -> `objc3.runtime.category_descriptors` / `__objc3_meta_category_####` / `__objc3_sec_category_descriptors`
   - `property` -> `objc3.runtime.property_descriptors` / `__objc3_meta_property_####` / `__objc3_sec_property_descriptors`
   - `ivar` -> `objc3.runtime.ivar_descriptors` / `__objc3_meta_ivar_####` / `__objc3_sec_ivar_descriptors`
5. Explicit no-standalone-emission rows remain published for:
   - `interface`
   - `implementation`
   - `metaclass`
   - `method`
6. Emitted descriptor rows preserve the relocation model `zero-sentinel-or-count-plus-pointer-vector`.
7. Matrix rows bind to concrete proof inputs:
   - metadata graph fixtures `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   - metadata graph fixtures `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
   - object inspection fixture `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`
   - commands `llvm-readobj --sections module.obj` and `llvm-objdump --syms module.obj`
8. Code/spec anchors remain explicit in:
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/io/objc3_process.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_types.h`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
9. Validation evidence is written to `tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json`.

## Non-goals

- `M253-A002` does not introduce concrete descriptor payload layouts.
- `M253-A002` does not introduce startup registration or runtime loader bootstrap.
- `M253-A002` does not introduce standalone emitted method, selector, or string-pool sections.
- `M253-A002` does not alter the A001 emitted inventory families.
