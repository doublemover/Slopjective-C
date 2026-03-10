# M257-C002 Ivar Offset And Layout Emission Core Feature Implementation Packet

Packet: `M257-C002`
Milestone: `M257`
Wave: `W49`
Lane: `C`
Issue: `#7151`
Contract ID: `objc3c-executable-ivar-layout-emission/m257-c002-v1`
Dependencies:
- `M257-C001`

## Objective

Upgrade the frozen `M257-C001` property/ivar lowering bridge into real emitted
ivar offset globals, ivar layout records, and per-owner layout tables that are
retained inside the native object path.

## Canonical Ivar Layout Emission Boundary

- contract id `objc3c-executable-ivar-layout-emission/m257-c002-v1`
- descriptor model
  `ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment`
- offset-global model
  `one-retained-i64-offset-global-per-emitted-ivar-binding`
- layout-table model
  `declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size`
- scope model
  `sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation`
- fail-closed model
  `no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis`
- emitted IR comment `; executable_ivar_layout_emission = ...`
- emitted named metadata `!objc3.objc_executable_ivar_layout_emission`
- emitted payload anchors:
  - `@__objc3_meta_ivar_offset_####`
  - `@__objc3_meta_ivar_layout_record_####`
  - `@__objc3_meta_ivar_layout_table_####`

## Acceptance Criteria

- Add explicit ivar layout emission boundary constants in
  `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add a deterministic emission summary helper in
  `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/ast/objc3_ast.h` explicit that AST owns canonical
  layout symbol / slot / size / alignment identities.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema
  owns the approved layout shape and lowering only materializes offsets.
- Have `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` compute and
  publish the emitted layout replay key and readiness counters.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` emit retained ivar offset
  globals, retained layout records, expanded ivar descriptors, and per-owner
  layout tables in `objc3.runtime.ivar_descriptors`.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over the proof fixture must emit a non-empty
  `module.obj` with a non-empty `objc3.runtime.ivar_descriptors` section and
  relocations.

## Dynamic Probes

1. Native compile probe over
   `tests/tooling/fixtures/native/m257_ivar_layout_offset_emission_positive.objc3`
   proving emitted IR/object output carries:
   - `; executable_ivar_layout_emission = ...`
   - `!objc3.objc_executable_ivar_layout_emission`
   - three retained offset globals
   - three retained layout records
   - one retained layout table
   - exact layout proof points for `enabled`, `count`, and `token`
   - successful `module.obj` emission with a non-empty
     `objc3.runtime.ivar_descriptors` section.
2. Native compile probe over
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   (`m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`)
   proving the same emitted layout boundary for the mixed class/property/ivar
   metadata path.

## Non-Goals

- `M257-C002` does not add runtime instance allocation.
- `M257-C002` does not add accessor body synthesis.
- `M257-C002` does not add executable field access runtime semantics.
- `M257-C002` does not reinterpret AST/sema legality inside lowering.

## Validation Commands

- `python scripts/check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m257-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m257/M257-C002/ivar_layout_offset_emission_summary.json`
