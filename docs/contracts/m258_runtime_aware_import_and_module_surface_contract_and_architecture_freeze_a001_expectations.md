# M258 Runtime-Aware Import and Module Surface Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-runtime-aware-import-module-surface/m258-a001-v1`
Status: Accepted
Issue: `#7158`
Scope: M258 lane-A contract and architecture freeze for the frontend-published runtime-aware import/module source surface.

## Objective

Freeze the frontend-owned import/module packet that later M258 work must preserve when it starts moving runtime-owned declarations and metadata references across translation units.

Canonical surface path:
`frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract`

## Required Invariants

1. `pipeline/objc3_frontend_artifacts.cpp` remains the canonical publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract`
   - one deterministic replay key derived from module identity, runtime-owned declaration counts, and current module-import-graph facts
2. The frozen semantic surface publishes, at minimum:
   - module identity
   - protocol/interface/implementation declaration counts
   - interface-category and implementation-category declaration counts
   - function declaration count
   - current module-import-graph counts
3. The frozen landed flags remain explicitly `false` for:
   - imported module artifact surface
   - imported runtime-owned declaration surface
   - imported runtime metadata reference surface
   - public frontend API module-import surface
4. `ir/objc3_ir_emitter.cpp` remains explicit that emitted IR preserves only the local translation-unit module-import graph profile for now.
5. `libobjc3c_frontend/api.h` remains explicit that embedding has no imported module handle or runtime metadata reference ABI yet and must therefore stay fail closed.

## Happy-Path Coverage

The checker must prove one real compile path using a metadata-rich fixture:

1. Metadata-rich local module path
   - the source declares protocols, interfaces, and implementations
   - the manifest publishes the `M258-A001` semantic surface packet
   - the packet records the current declaration counts and module-import-graph facts without claiming any imported-runtime capability exists

## Non-Goals and Fail-Closed Rules

- `M258-A001` does not add an imported-module artifact reader yet.
- `M258-A001` does not materialize imported runtime-owned declarations yet.
- `M258-A001` does not lower foreign runtime metadata references yet.
- `M258-A001` does not widen the public frontend API with module-import handles or foreign metadata payload inputs yet.
- `M258-A002` must preserve this frozen surface while turning it into a real compiler/runtime capability.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m258-a001-runtime-aware-import-and-module-surface-contract`.
- `package.json` includes `test:tooling:m258-a001-runtime-aware-import-and-module-surface-contract`.
- `package.json` includes `check:objc3c:m258-a001-lane-a-readiness`.

## Validation

- `python scripts/check_m258_a001_runtime_aware_import_module_surface_contract.py`
- `python -m pytest tests/tooling/test_check_m258_a001_runtime_aware_import_module_surface_contract.py -q`
- `python scripts/run_m258_a001_lane_a_readiness.py`

## Evidence Path

- `tmp/reports/m258/M258-A001/runtime_aware_import_module_surface_contract_summary.json`
