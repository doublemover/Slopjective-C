# M253-C004 Method Property And Ivar List Emission Core Feature Implementation Packet

Packet: `M253-C004`
Milestone: `M253`
Wave: `W45`
Lane: `C`
Issue: `#7093`
Contract ID: `objc3c-runtime-member-table-emission/m253-c004-v1`
Dependencies:
- `M253-C003`
- `M252-C002`
- `M253-B003`

## Objective

Emit the next real executable metadata payload tranche by adding owner-scoped method tables plus real property and ivar descriptor payloads while keeping the already-closed class/protocol/category descriptor bundle shapes stable.

## Canonical Emission Boundary

- contract id `objc3c-runtime-member-table-emission/m253-c004-v1`
- method-list payload model `owner-scoped-method-table-globals-with-inline-entry-records`
- method-list grouping model `declaration-owner-plus-class-kind-lexicographic`
- property payload model `property-descriptor-records-with-accessor-and-binding-strings`
- ivar payload model `ivar-descriptor-records-with-property-binding-strings`
- emitted IR comment `; runtime_metadata_member_table_emission = ...`
- emitted LLVM metadata node `!objc3.objc_runtime_member_table_emission`

## Acceptance Criteria

- Add explicit C004 boundary constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Publish the deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` responsible for:
  - fail-closed declaration-owner/class-kind method-table bundle construction,
  - fail-closed property descriptor bundle construction,
  - fail-closed ivar descriptor bundle construction.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` emit:
  - real owner-scoped method-table globals into class/protocol/category descriptor sections,
  - real property descriptors into `objc3.runtime.property_descriptors`,
  - real ivar descriptors into `objc3.runtime.ivar_descriptors`,
  - stable family aggregates for property/ivar descriptor sections.
- Keep `native/objc3c/src/io/objc3_process.cpp` explicit that llvm-direct object emission preserves member-table payloads verbatim.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over the class/protocol/property/ivar and category/protocol/property fixtures must emit both `module.ll` and `module.obj` with the `llvm-direct` backend.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3` proving:
   - empty diagnostics,
   - `module.object-backend.txt == llvm-direct`,
   - `!objc3.objc_runtime_member_table_emission` is present,
   - method-list bundle count `5`,
   - method-entry count `5`,
   - property descriptor count `3`,
   - ivar descriptor count `1`,
   - real class/protocol method tables are present,
   - no property/ivar `[1 x i8] zeroinitializer` placeholder globals remain,
   - `llvm-readobj --sections module.obj` exposes nontrivial class/protocol/property/ivar bytes and relocations,
   - `llvm-objdump --syms module.obj` exposes `__objc3_meta_class_instance_methods_0001`, `__objc3_meta_property_0001`, and `__objc3_meta_ivar_0000`.
2. Native compile probe over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3` proving:
   - method-list bundle count `3`,
   - method-entry count `3`,
   - property descriptor count `3`,
   - ivar descriptor count `1`,
   - real category/protocol method tables are present,
   - no property/ivar `[1 x i8] zeroinitializer` placeholder globals remain,
   - `llvm-readobj --sections module.obj` exposes nontrivial protocol/category/property/ivar bytes and relocations,
   - `llvm-objdump --syms module.obj` exposes `__objc3_meta_category_instance_methods_0000`, `__objc3_meta_property_0001`, and `__objc3_meta_ivar_0000`.

## Non-Goals

- `M253-C004` does not add selector/string-pool families.
- `M253-C004` does not add runtime registration/bootstrap.
- `M253-C004` does not mutate the closed class/protocol/category descriptor bundle shapes from `M253-C002` / `M253-C003`.

## Validation Commands

- `python scripts/check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C004/method_property_and_ivar_list_emission_summary.json`
