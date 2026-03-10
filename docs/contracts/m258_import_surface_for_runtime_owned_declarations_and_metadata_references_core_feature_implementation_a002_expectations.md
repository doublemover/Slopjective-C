# M258 Import Surface For Runtime-Owned Declarations And Metadata References Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1`
Status: Accepted
Issue: `#7159`
Scope: M258 lane-A core feature implementation for the emitted runtime-aware import/module frontend closure.

## Objective

Turn the frozen `M258-A001` runtime-aware import/module surface into one real emitted compiler artifact that preserves runtime-owned declaration inventories and metadata-reference inventories for later cross-translation-unit consumers.

Canonical surface path:
`frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_frontend_closure`

Canonical emitted artifact:
`module.runtime-import-surface.json`

## Required Invariants

1. `pipeline/objc3_frontend_artifacts.cpp` remains the canonical construction point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_frontend_closure`
   - `module.runtime-import-surface.json`
   - one deterministic replay key derived from the frozen `M258-A001` surface plus live runtime metadata source records
2. The emitted surface/artifact publishes, at minimum:
   - preserved declaration/import-graph counts from `M258-A001`
   - runtime-owned declaration inventories for classes, protocols, categories, properties, methods, and ivars
   - metadata-reference inventories for superclass edges, protocol edges, property accessor selectors, property ivar-binding symbols, and method selectors
3. The landed flags are explicitly `true` for:
   - runtime-aware import declarations
   - module metadata import surface
   - runtime-owned declaration import surface
   - runtime metadata reference import surface
   - public frontend API module surface
4. `driver/objc3_objc3_path.cpp` and `libobjc3c_frontend/frontend_anchor.cpp` both emit the same canonical artifact on successful frontend compilation.
5. `ir/objc3_ir_emitter.cpp` remains explicit that IR is still translation-unit local; A002 does not yet lower foreign metadata into IR.
6. `libobjc3c_frontend/api.h` is explicit that embedding consumes the emitted filesystem artifact and still does not expose an in-memory imported-module handle ABI.

## Happy-Path Coverage

The checker must prove two real compile paths:

1. Metadata-rich class/protocol/property/ivar fixture
   - emits the semantic surface
   - emits `module.runtime-import-surface.json`
   - carries nonzero runtime-owned declaration and metadata-reference inventories
2. Metadata-rich category/protocol/property fixture
   - emits the same artifact through both the native driver and the frontend C API runner
   - preserves deterministic category/protocol/property metadata-reference shape

## Non-Goals And Fail-Closed Rules

- `M258-A002` does not yet lower imported runtime-owned declarations into LLVM IR.
- `M258-A002` does not yet realize foreign metadata references in the live runtime.
- `M258-A002` does not yet expose in-memory imported-module handles or foreign payload injection through the public embedding ABI.
- Drift between the semantic surface and emitted artifact must fail closed.

## Architecture And Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build And Readiness Integration

- `package.json` includes `check:objc3c:m258-a002-import-surface-runtime-owned-declarations-and-metadata-references`.
- `package.json` includes `test:tooling:m258-a002-import-surface-runtime-owned-declarations-and-metadata-references`.
- `package.json` includes `check:objc3c:m258-a002-lane-a-readiness`.

## Validation

- `python scripts/check_m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation.py -q`
- `python scripts/run_m258_a002_lane_a_readiness.py`

## Evidence Path

- `tmp/reports/m258/M258-A002/runtime_aware_import_module_frontend_closure_summary.json`
