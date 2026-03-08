# M253 Metadata Section Emission Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1`
Status: Accepted
Issue: `#7090`
Scope: M253 lane-C freeze of the current real metadata section-emission boundary so later implementation issues can replace placeholder payload bytes without reopening the emitted-inventory contract.

## Objective

Freeze the boundary for native metadata section emission. The project now emits real metadata sections into native IR/object output, but the bytes are still scaffold placeholders. Lane-C work must start from that explicit boundary rather than from an implicit “no emission yet” assumption.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1`
   - payload model `scaffold-placeholder-payloads-until-m253-c002`
   - inventory model `image-info-plus-class-protocol-category-property-ivar-sections`
   - image-info payload model `internal-{i32,i32}-zeroinitializer-image-info`
   - descriptor payload model `private-[1xi8]-zeroinitializer-per-descriptor`
   - aggregate payload model `i64-count-plus-pointer-vector-aggregates`.
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic boundary summary for the current emission state; the boundary must be explicit that sections are real but payload bytes are still placeholders.
3. `ir/objc3_ir_emitter.cpp` publishes the boundary directly into emitted IR through `; runtime_metadata_section_emission_boundary = ...` before writing scaffold globals.
4. `io/objc3_process.cpp` remains explicit that llvm-direct object emission preserves the frozen placeholder-emission boundary and may not invent richer payloads or new metadata families during the freeze.
5. Happy-path native emission over `hello.objc3` must still emit:
   - `@__objc3_image_info = internal global { i32, i32 } zeroinitializer`
   - descriptor globals as `private global [1 x i8] zeroinitializer`
   - aggregates as either `{ i64 } { i64 0 }` or `{ i64, [N x ptr] }`
   - `@llvm.used` retention.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proves the emitted IR carries the new section-emission boundary comment and still emits the current scaffold placeholder payloads.
2. The same probe must still produce a non-empty `module.obj`.

## Non-Goals and Fail-Closed Rules

- `M253-C001` does not add encoded class/protocol/category/property/ivar record payloads yet.
- `M253-C001` does not add method, selector, or string-pool payload sections.
- `M253-C001` does not add runtime registration/bootstrap.
- If the current scaffold boundary drifts, later lane-C implementation must fail closed rather than silently broadening the emitted payload surface.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m253-c001-metadata-section-emission-contract`.
- `package.json` includes `test:tooling:m253-c001-metadata-section-emission-contract`.
- `package.json` includes `check:objc3c:m253-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m253_c001_metadata_section_emission_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m253_c001_metadata_section_emission_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m253-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C001/metadata_section_emission_contract_summary.json`
