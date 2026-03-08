# M253-C003 Protocol And Category Data Emission Core Feature Implementation Packet

Packet: `M253-C003`
Milestone: `M253`
Wave: `W45`
Lane: `C`
Issue: `#7092`
Contract ID: `objc3c-runtime-protocol-category-data-emission/m253-c003-v1`
Dependencies:
- `M253-C002`
- `M252-C002`
- `M253-B002`

## Objective

Emit the next real executable metadata payload families into object files by replacing protocol/category placeholders with real protocol descriptors, category record descriptors, protocol-reference payloads, and category attachment lists.

## Canonical Emission Boundary

- contract id `objc3c-runtime-protocol-category-data-emission/m253-c003-v1`
- protocol payload model `protocol-descriptor-bundles-with-inherited-protocol-ref-lists`
- category payload model `category-descriptor-bundles-with-attachment-and-protocol-ref-lists`
- protocol-reference model `count-plus-descriptor-pointer-protocol-ref-lists`
- category-attachment model `count-plus-owner-identity-pointer-attachment-lists`
- emitted IR comment `; runtime_metadata_protocol_category_emission = ...`
- emitted LLVM metadata node `!objc3.objc_runtime_protocol_category_emission`

## Acceptance Criteria

- Add explicit C003 boundary constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Publish the deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` responsible for:
  - fail-closed protocol/category bundle construction from the typed metadata handoff,
  - expansion of one combined category graph node into explicit interface/implementation descriptor bundles,
  - fail-closed inherited/adopted protocol resolution against the emitted protocol descriptor inventory.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` emit real protocol/category descriptor bundles in `objc3.runtime.protocol_descriptors` and `objc3.runtime.category_descriptors` rather than placeholder bytes.
- Keep `native/objc3c/src/io/objc3_process.cpp` explicit that llvm-direct object emission preserves protocol/category payloads verbatim.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3` must emit both `module.ll` and `module.obj` with the `llvm-direct` backend.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3` proving:
   - empty diagnostics,
   - `module.object-backend.txt == llvm-direct`,
   - `!objc3.objc_runtime_protocol_category_emission` is present,
   - protocol bundle count `2`,
   - category bundle count `2`,
   - inherited protocol-ref list count `2`,
   - adopted protocol-ref list count `2`,
   - category attachment-list count `2`,
   - no protocol/category family `[1 x i8] zeroinitializer` placeholder globals remain,
   - `llvm-readobj --sections module.obj` exposes `objc3.runtime.protocol_descriptors` and `objc3.runtime.category_descriptors` with nontrivial bytes/relocations,
   - `llvm-objdump --syms module.obj` exposes `__objc3_sec_protocol_descriptors` and `__objc3_sec_category_descriptors`.

## Non-Goals

- `M253-C003` does not add selector/string-pool payloads.
- `M253-C003` does not add standalone property/ivar payload sections.
- `M253-C003` does not add runtime registration/bootstrap.

## Validation Commands

- `python scripts/check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C003/protocol_and_category_data_emission_summary.json`
