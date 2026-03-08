# M253 Deterministic Ordering, Visibility, and Relocation Semantics Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-runtime-metadata-layout-policy/m253-b002-v1`
Status: Accepted
Issue: `#7088`
Scope: M253 lane-B semantic finalization of the runtime metadata layout policy
so emitted IR/object paths consume one normalized ordering, visibility,
relocation, retention, and linkage decision surface.

## Objective

Turn the frozen `M253-B001` policy into a real lowering capability. The emitter
must stop relying on ad hoc hardcoded family emission order and instead consume
one normalized layout policy packet built from canonical ABI/scaffold inputs.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point
   for:
   - contract id `objc3c-runtime-metadata-layout-policy/m253-b002-v1`
   - normalized layout-policy family input/output packets
   - canonical family tokens `class`, `protocol`, `category`, `property`,
     `ivar`
   - the inherited B001 ordering/visibility/relocation policy tokens.
2. `lower/objc3_lowering_contract.cpp` implements fail-closed normalization
   that accepts only the canonical ABI/scaffold inputs:
   - descriptor linkage `private`
   - aggregate linkage `internal`
   - metadata visibility `hidden`
   - retention root `llvm.used`
   - family order `class`, `protocol`, `category`, `property`, `ivar`
   - retained-global count equal to descriptor inventory plus image-info and
     family aggregates.
3. `ir/objc3_ir_emitter.cpp` consumes the normalized policy as the real source
   of truth and publishes it through:
   - named metadata `!objc3.objc_runtime_metadata_layout_policy`
   - node `!55`
   - emitted comment line `; runtime_metadata_layout_policy = ...`
   - policy-driven family emission rather than hardcoded repeated calls.
4. `io/objc3_process.cpp` remains explicit that llvm-direct object emission
   preserves the normalized semantic-finalization boundary and may not reorder
   or reinterpret it.
5. Happy-path native emission over `hello.objc3` still reaches `module.obj`
   while preserving:
   - `@__objc3_image_info` first
   - family order `class`, `protocol`, `category`, `property`, `ivar`
   - zero-sentinel aggregate payloads for zero-count families
   - no COMDAT
   - no explicit `hidden` visibility spelling on local-linkage metadata
     globals
   - `@llvm.used` retention order.

## Dynamic Coverage

1. Runner manifest-only probe over
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   proves the canonical runtime-export/scaffold inputs remain ready and non-zero
   for class/protocol/property/ivar descriptor families.
2. Native compile probe over `tests/tooling/fixtures/native/hello.objc3`
   proves emitted IR/object output carries the normalized policy through
   `!objc3.objc_runtime_metadata_layout_policy`, `!55`, and the
   `; runtime_metadata_layout_policy =` replay line.

## Non-Goals and Fail-Closed Rules

- `M253-B002` does not yet add COFF/ELF/Mach-O-specific policy variants; that
  remains `M253-B003`.
- `M253-B002` does not add emitted method, selector, or string-pool section
  families.
- `M253-B002` does not introduce runtime registration or startup bootstrap.
- If canonical ABI/scaffold inputs drift, policy construction must fail closed
  instead of silently emitting a second ordering model.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m253-b002-deterministic-ordering-visibility-and-relocation-semantics-core-feature-implementation`.
- `package.json` includes
  `test:tooling:m253-b002-deterministic-ordering-visibility-and-relocation-semantics-core-feature-implementation`.
- `package.json` includes `check:objc3c:m253-b002-lane-b-readiness`.

## Validation

- `python scripts/check_m253_b002_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_b002_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m253/M253-B002/deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation_summary.json`
