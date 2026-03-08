# M253 Emitted Metadata Inventory Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-emitted-metadata-inventory-freeze/m253-a001-v1`
Status: Accepted
Issue: `#7085`
Scope: M253 lane-A contract and architecture freeze for the currently
supported emitted runtime metadata inventory.

## Objective

Freeze one deterministic emitted metadata inventory and section-ownership
boundary so later M253 section-emission issues extend a known object-file
surface instead of mutating it implicitly.

## Required Invariants

1. `ast/objc3_ast.h` remains the canonical declaration point for:
   - logical sections
     `objc3.runtime.image_info`,
     `objc3.runtime.class_descriptors`,
     `objc3.runtime.protocol_descriptors`,
     `objc3.runtime.category_descriptors`,
     `objc3.runtime.property_descriptors`,
     `objc3.runtime.ivar_descriptors`
   - descriptor symbol prefix `__objc3_meta_`
   - aggregate symbol prefix `__objc3_sec_`
   - image-info symbol `__objc3_image_info`
   - descriptor linkage `private`
   - aggregate linkage `internal`
   - metadata visibility `hidden`
   - retention root `llvm.used`
   - object-inspection commands
     `llvm-readobj --sections module.obj` and
     `llvm-objdump --syms module.obj`
2. `pipeline/objc3_frontend_types.h` remains the canonical summary declaration
   point for:
   - `Objc3RuntimeMetadataSectionAbiFreezeSummary`
   - `Objc3RuntimeMetadataSectionScaffoldSummary`
   - `Objc3RuntimeMetadataObjectInspectionHarnessSummary`
3. `ir/objc3_ir_emitter.cpp` continues to bind the emitted inventory to real IR
   through:
   - `!objc3.objc_runtime_metadata_section_abi`
   - `!objc3.objc_runtime_metadata_section_scaffold`
   - `!objc3.objc_runtime_metadata_object_inspection`
   - `@__objc3_image_info`
   - descriptor globals under `__objc3_meta_`
   - aggregate globals under `__objc3_sec_`
   - retention via `@llvm.used`
4. `lower/objc3_lowering_contract.h` and
   `lower/objc3_lowering_contract.cpp` remain explicit that lowering replay
   keys do not own or infer the object-file metadata inventory.
5. `io/objc3_process.cpp` remains explicit that llvm-direct object emission
   preserves the emitted inventory and does not rewrite or substitute another
   metadata layout.
6. The frozen emitted inventory is currently limited to:
   - image-info
   - class descriptor section
   - protocol descriptor section
   - category descriptor section
   - property descriptor section
   - ivar descriptor section

## Non-Goals and Fail-Closed Rules

- `M253-A001` does not publish the source-to-section completeness matrix.
- `M253-A001` does not introduce concrete descriptor payload layouts beyond the
  existing scaffold.
- `M253-A001` does not add startup registration or runtime bootstrap.
- `M253-A001` does not add standalone emitted method, selector, or string-pool
  sections.
- The next implementation issue `M253-A002` must therefore preserve this
  inventory while publishing the completeness matrix.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m253-a001-emitted-metadata-inventory-contract`.
- `package.json` includes
  `test:tooling:m253-a001-emitted-metadata-inventory-contract`.
- `package.json` includes `check:objc3c:m253-a001-lane-a-readiness`.

## Validation

- `python scripts/check_m253_a001_emitted_metadata_inventory_contract.py`
- `python -m pytest tests/tooling/test_check_m253_a001_emitted_metadata_inventory_contract.py -q`
- `npm run check:objc3c:m253-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m253/M253-A001/emitted_metadata_inventory_contract_summary.json`
