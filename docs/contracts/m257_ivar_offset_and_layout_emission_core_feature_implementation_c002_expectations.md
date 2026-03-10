# M257 Ivar Offset And Layout Emission Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-executable-ivar-layout-emission/m257-c002-v1`
Status: Accepted
Issue: `#7151`
Scope: M257 lane-C implementation that upgrades the frozen `M257-C001` property/ivar lowering bridge into real emitted ivar offset globals, ivar layout records, and per-owner layout tables inside the native IR/object path without yet introducing runtime instance allocation or accessor body synthesis.

## Objective

Implement real emitted ivar layout payloads for the executable property/ivar subset.
The compiler already owns canonical property/ivar layout identities in AST/sema.
This issue makes lane-C materialize those identities into retained object-section
artifacts so later runtime work can consume truthful offset/layout state instead
of re-deriving it.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point
   for:
   - `objc3c-executable-ivar-layout-emission/m257-c002-v1`
   - descriptor model
     `ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment`
   - offset-global model
     `one-retained-i64-offset-global-per-emitted-ivar-binding`
   - layout-table model
     `declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size`
   - scope model
     `sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation`
   - fail-closed model
     `no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis`.
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic summary for
   the ivar layout emission boundary.
3. `ast/objc3_ast.h` remains explicit that canonical layout symbol, slot,
   size, and alignment identities originate upstream of lowering.
4. `sema/objc3_semantic_passes.cpp` remains explicit that sema owns canonical
   slot/size/alignment identities and lowering may only materialize byte
   offsets from that approved shape.
5. `ir/objc3_ir_emitter.cpp` publishes the boundary directly into emitted IR
   through `; executable_ivar_layout_emission = ...`, emits
   `!objc3.objc_executable_ivar_layout_emission`, and retains concrete emitted
   ivar offset globals, layout records, and layout tables inside
   `objc3.runtime.ivar_descriptors`.
6. `pipeline/objc3_frontend_artifacts.cpp` remains the only frontend handoff
   path for contract ids, replay keys, and readiness accounting that gate this
   emitted layout surface.
7. Happy-path native emission must produce non-empty `module.obj` artifacts
   whose `objc3.runtime.ivar_descriptors` section contains real payload bytes
   and relocations instead of placeholder-only descriptor data.

## Dynamic Coverage

1. Native compile probe over
   `tests/tooling/fixtures/native/m257_ivar_layout_offset_emission_positive.objc3`
   proves the emitted IR carries the new layout-emission boundary line, emits
   three retained offset globals, three retained layout records, one per-owner
   layout table, and preserves exact slot/offset/size/alignment facts for:
   - `count` => slot `1`, offset `4`, size `4`, align `4`
   - `enabled` => slot `0`, offset `0`, size `1`, align `1`
   - `token` => slot `2`, offset `8`, size `8`, align `8`
   - owner table instance size `16`.
2. The same positive probe proves `@llvm.used` retains the emitted offset
   globals, layout records, and owner layout table.
3. The same positive probe proves `llvm-readobj --sections --relocations`
   reports a non-empty `objc3.runtime.ivar_descriptors` section with
   relocations.
4. Native compile probe over
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   proves the same boundary holds for the mixed metadata fixture and still emits
   at least one retained offset global, layout record, and layout table.

## Non-Goals and Fail-Closed Rules

- `M257-C002` does not add runtime instance allocation.
- `M257-C002` does not add accessor body synthesis.
- `M257-C002` does not re-derive layout legality inside lowering.
- `M257-C002` does not realize runtime field access semantics.
- If canonical layout identities are missing or inconsistent, lane-C must fail
  closed rather than silently invent offsets or tables.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m257-c002-ivar-offset-and-layout-emission`.
- `package.json` includes
  `test:tooling:m257-c002-ivar-offset-and-layout-emission`.
- `package.json` includes `check:objc3c:m257-c002-lane-c-readiness`.

## Validation

- `python scripts/check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m257_c002_ivar_offset_and_layout_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m257-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m257/M257-C002/ivar_layout_offset_emission_summary.json`
