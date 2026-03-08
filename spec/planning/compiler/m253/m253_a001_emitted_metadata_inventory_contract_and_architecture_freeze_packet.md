# M253-A001 Emitted Metadata Inventory Contract and Architecture Freeze Packet

Packet: `M253-A001`
Milestone: `M253`
Lane: `A`
Freeze date: `2026-03-08`
Dependencies: none

## Purpose

Freeze the currently supported emitted runtime metadata inventory so later
section-emission issues extend one deterministic object-file surface rather than
drifting section names, symbol families, or retention semantics.

## Scope Anchors

- Contract:
  `docs/contracts/m253_emitted_metadata_inventory_contract_and_architecture_freeze_a001_expectations.md`
- Checker:
  `scripts/check_m253_a001_emitted_metadata_inventory_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m253_a001_emitted_metadata_inventory_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m253-a001-emitted-metadata-inventory-contract`
  - `test:tooling:m253-a001-emitted-metadata-inventory-contract`
  - `check:objc3c:m253-a001-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `docs/objc3c-native.md`

## Fail-Closed Boundary

- Contract ID: `objc3c-emitted-metadata-inventory-freeze/m253-a001-v1`
- Emitted inventory kinds:
  - `objc3.runtime.image_info`
  - `objc3.runtime.class_descriptors`
  - `objc3.runtime.protocol_descriptors`
  - `objc3.runtime.category_descriptors`
  - `objc3.runtime.property_descriptors`
  - `objc3.runtime.ivar_descriptors`
- Symbol policy:
  - descriptor symbol prefix `__objc3_meta_`
  - aggregate symbol prefix `__objc3_sec_`
  - image-info symbol `__objc3_image_info`
  - descriptor linkage `private`
  - aggregate linkage `internal`
  - metadata visibility `hidden`
  - retention root `llvm.used`
- Inspection command anchors:
  - `llvm-readobj --sections module.obj`
  - `llvm-objdump --syms module.obj`
- Lowering replay keys and backend object emission must preserve this inventory
  rather than infer another layout.
- The next implementation issue `M253-A002` must preserve this freeze while
  publishing the source-to-section completeness matrix.

## Gate Commands

- `python scripts/check_m253_a001_emitted_metadata_inventory_contract.py`
- `python -m pytest tests/tooling/test_check_m253_a001_emitted_metadata_inventory_contract.py -q`
- `npm run check:objc3c:m253-a001-lane-a-readiness`

## Evidence Output

- `tmp/reports/m253/M253-A001/emitted_metadata_inventory_contract_summary.json`
