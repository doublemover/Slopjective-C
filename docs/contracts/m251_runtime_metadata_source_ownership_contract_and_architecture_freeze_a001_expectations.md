# M251 Runtime Metadata Source Ownership Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-runtime-metadata-source-ownership-freeze/m251-a001-v1`
Status: Accepted
Scope: M251 lane-A contract and architecture freeze for frontend-owned runtime metadata source records.

## Objective

Freeze the frontend/runtime ownership boundary for class, protocol, category,
property, ivar, and method metadata source records so later lowering/runtime
work preserves one canonical source schema and fails closed on drift.

## Required Invariants

1. `ast/objc3_ast.h` remains the canonical source-anchor declaration point for:
   - `Objc3InterfaceDecl` / `Objc3ImplementationDecl` class records
   - `Objc3ProtocolDecl` protocol records
   - category-bearing interface / implementation declarations
   - `Objc3PropertyDecl` property records
   - `Objc3MethodDecl` method records
   - `Objc3PropertyDecl.ivar_binding_symbol` ivar source packets
2. `pipeline/objc3_frontend_types.h` defines
   `Objc3RuntimeMetadataSourceOwnershipBoundary` as the canonical lane-A
   ownership packet.
3. `pipeline/objc3_frontend_pipeline.cpp` synthesizes the ownership packet in a
   deterministic, fail-closed way from frontend source records.
4. `pipeline/objc3_frontend_artifacts.cpp` publishes runtime metadata source
   ownership evidence into the manifest and forwards it into IR metadata.
5. `ir/objc3_ir_emitter.h` / `ir/objc3_ir_emitter.cpp` preserve the runtime
   metadata source ownership metadata node.
6. `driver/objc3_objc3_path.cpp` continues to emit manifest/IR ownership
   evidence before object emission.
7. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicitly marked as
   test-only evidence, not the native runtime library.

## Non-Goals and Fail-Closed Rules

- `M251-A001` does not implement native runtime metadata extraction.
- `M251-A001` does not implement metadata section/object-file emission.
- `M251-A001` does not implement runtime registration/startup wiring.
- `M251-A001` does not replace the test shim with an in-tree runtime library.
- The ownership packet must therefore remain:
  - frontend-owned,
  - not ready for lowering,
  - marked as native-runtime-library absent,
  - marked as test-shim-only topology.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m251-a001-runtime-metadata-source-ownership-contract`.
- `package.json` includes
  `test:tooling:m251-a001-runtime-metadata-source-ownership-contract`.
- `package.json` includes `check:objc3c:m251-a001-lane-a-readiness`.

## Validation

- `python scripts/check_m251_a001_runtime_metadata_source_ownership_contract.py`
- `python -m pytest tests/tooling/test_check_m251_a001_runtime_metadata_source_ownership_contract.py -q`
- `npm run check:objc3c:m251-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m251/M251-A001/runtime_metadata_source_ownership_contract_summary.json`
