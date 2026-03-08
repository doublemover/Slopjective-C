# M253-C001 Metadata Section Emission Contract and Architecture Freeze Packet

Packet: `M253-C001`
Milestone: `M253`
Wave: `W45`
Lane: `C`
Issue: `#7090`
Contract ID: `objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1`
Dependencies: None

## Objective

Freeze the current native metadata section-emission boundary so later lane-C implementation issues can move from scaffold placeholders to encoded payload records without drifting the known emitted inventory.

## Canonical Section-Emission Boundary

- contract id `objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1`
- payload model `scaffold-placeholder-payloads-until-m253-c002`
- inventory model `image-info-plus-class-protocol-category-property-ivar-sections`
- image-info payload model `internal-{i32,i32}-zeroinitializer-image-info`
- descriptor payload model `private-[1xi8]-zeroinitializer-per-descriptor`
- aggregate payload model `i64-count-plus-pointer-vector-aggregates`
- emitted IR comment `; runtime_metadata_section_emission_boundary = ...`

## Acceptance Criteria

- Add explicit section-emission boundary constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add a deterministic boundary summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish the boundary directly before scaffold-global emission.
- Keep `native/objc3c/src/io/objc3_process.cpp` explicit that llvm-direct emission preserves the placeholder boundary and does not invent richer payloads during the freeze.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over `hello.objc3` must still emit `module.obj`.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proving emitted IR/object output carries:
   - `; runtime_metadata_section_emission_boundary = ...`
   - `@__objc3_image_info` zeroinitializer payload
   - descriptor `private global [1 x i8] zeroinitializer`
   - aggregate count-plus-pointer-vector payload shapes
   - successful `module.obj` emission.

## Non-Goals

- `M253-C001` does not add encoded descriptor payload bytes.
- `M253-C001` does not add method, selector, or string-pool sections.
- `M253-C001` does not add runtime registration/bootstrap.

## Validation Commands

- `python scripts/check_m253_c001_metadata_section_emission_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m253_c001_metadata_section_emission_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m253-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C001/metadata_section_emission_contract_summary.json`
