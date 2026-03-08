# M253-D001 Object Packaging And Retention Contract And Architecture Freeze Packet

Packet: `M253-D001`
Milestone: `M253`
Lane: `D`
Depends on:
- None

## Goal

Freeze the current produced-object packaging and retention boundary so later archive, link, and startup-registration work extends one explicit handoff instead of redefining the runtime metadata discovery roots.

## Spec anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/io/objc3_process.cpp`

## Required implementation

1. Freeze contract id `objc3c-runtime-object-packaging-retention-boundary/m253-d001-v1`.
2. Publish the lowering summary through `Objc3RuntimeMetadataObjectPackagingRetentionSummary()`.
3. Publish the same contract in emitted LLVM IR through `; runtime_metadata_object_packaging_retention = ...` and `!objc3.objc_runtime_object_packaging_retention`.
4. Freeze boundary model `current-object-file-boundary-with-retained-metadata-section-aggregates`.
5. Freeze retention-anchor model `llvm.used-plus-aggregate-section-symbols`.
6. Freeze current object artifact `module.obj` and aggregate symbol prefix `__objc3_sec_` as the discoverability roots later work must preserve.
7. Positive validation must prove the current object path emits:
   - `module.manifest.json`
   - `module.ll`
   - `module.obj`
   - `module.object-backend.txt`
   - runtime metadata sections discoverable through `llvm-readobj --sections module.obj`
   - retained aggregate metadata symbols discoverable through `llvm-objdump --syms module.obj`
8. Negative validation must fail closed on `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3` with no manifest, no object, and no backend marker.
9. Validation evidence must land at `tmp/reports/m253/M253-D001/object_packaging_and_retention_contract_summary.json`.

## Non-goals

- No archive packaging contract yet.
- No link-registration contract yet.
- No startup registration/bootstrap yet.

## Validation

- `python scripts/check_m253_d001_object_packaging_and_retention_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m253_d001_object_packaging_and_retention_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m253-d001-lane-d-readiness`
