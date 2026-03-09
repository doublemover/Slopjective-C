# M255 Lookup and Dispatch Runtime Contract and Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
Status: Accepted
Issue: `#7123`
Scope: M255 lane-D contract and architecture freeze for the runtime-owned
selector interning and dispatch boundary.

## Objective

Freeze the runtime-owned lookup/dispatch surface that supported live message
sends now target, so later selector-table, cache, and slow-path work extends one
preserved boundary instead of drifting across ad hoc runtime behavior.

## Required Outcomes

1. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical
   declaration point for:
   - `kObjc3RuntimeLookupDispatchContractId`
   - `kObjc3RuntimeLookupDispatchSelectorInterningModel`
   - `kObjc3RuntimeLookupDispatchLookupTableModel`
   - `kObjc3RuntimeLookupDispatchCacheModel`
   - `kObjc3RuntimeLookupDispatchProtocolCategoryModel`
   - `kObjc3RuntimeLookupDispatchCompatibilityModel`
   - `kObjc3RuntimeLookupDispatchFailureModel`
2. `native/objc3c/src/runtime/objc3_runtime.h` preserves the canonical runtime
   API surface:
   - `objc3_runtime_lookup_selector`
   - `objc3_runtime_dispatch_i32`
   - `objc3_runtime_selector_handle`
3. `native/objc3c/src/runtime/objc3_runtime.cpp` preserves the canonical
   runtime-owned storage/behavior anchors:
   - `selector_index_by_name`
   - `selector_slots`
   - `LookupSelectorUnlocked`
   - nil receiver returns `0` from `objc3_runtime_dispatch_i32`
4. `native/objc3c/src/ir/objc3_ir_emitter.cpp` explicitly documents that IR
   continues to target only the canonical lookup/dispatch boundary and does not
   materialize cache or slow-path helper symbols in `M255-D001`.
5. `native/objc3c/src/parse/objc3_parser.cpp` explicitly documents that parser
   normalization does not own selector interning, lookup tables, caches, or
   slow-path method resolution.
6. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` explicitly documents that
   the shim remains test-only compatibility evidence and is not the
   authoritative live runtime lookup/dispatch implementation.
7. The frozen semantic policy surface preserves:
   - selector interning model
     `process-global-selector-intern-table-stable-id-per-canonical-selector-spelling`
   - lookup-table model
     `metadata-backed-selector-lookup-tables-deferred-until-m255-d002`
   - cache model
     `method-cache-and-runtime-slow-path-deferred-until-m255-d003`
   - protocol/category model
     `protocol-and-category-aware-method-resolution-deferred-until-m255-d004`
   - compatibility model
     `compatibility-shim-remains-test-only-non-authoritative-runtime-surface`
   - failure model
     `runtime-lookup-and-dispatch-fail-closed-on-unmaterialized-resolution`
8. `tests/tooling/runtime/m255_d001_lookup_dispatch_runtime_probe.cpp` compiles
   against the frozen header/archive surface and proves:
   - null selector lookup fails closed
   - repeated selector lookup reuses one stable handle
   - different selector spellings get monotonic stable IDs
   - nil dispatch returns `0`
   - non-nil dispatch preserves deterministic formula parity
   - reset clears selector interning state back to stable ID `1`

## Non-Goals and Fail-Closed Rules

- `M255-D001` does not land metadata-backed selector lookup tables.
- `M255-D001` does not land method-cache materialization or runtime slow-path
  lookup.
- `M255-D001` does not land protocol/category-aware runtime method resolution.
- `M255-D002` must preserve the frozen public lookup/dispatch API while
  materializing selector interning and lookup tables.
- `M255-D003` must preserve the frozen public lookup/dispatch API while adding
  method-cache and slow-path behavior.
- `M255-D004` must preserve the frozen public lookup/dispatch API while adding
  protocol/category-aware resolution.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/runtime/README.md`
- `tests/tooling/runtime/README.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m255-d001-lookup-and-dispatch-runtime-contract-and-architecture-freeze`.
- `package.json` includes
  `test:tooling:m255-d001-lookup-and-dispatch-runtime-contract-and-architecture-freeze`.
- `package.json` includes `check:objc3c:m255-d001-lane-d-readiness`.

## Validation

- `python scripts/check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m255-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m255/M255-D001/lookup_dispatch_runtime_contract_summary.json`
