# M258-C001 Serialized Metadata Import And Lowering Contract And Architecture Freeze Packet

Packet: `M258-C001`
Milestone: `M258`
Lane: `C`
Issue: `#7162`
Dependencies: `M258-B002`
Next issue: `M258-C002`

## Objective

Freeze the boundary between imported runtime-surface semantic consumption and later serialized metadata rehydration / imported-payload IR lowering.

## Required implementation

- Publish one semantic-surface contract at:
  - `frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_import_lowering_contract`
- Source that contract from the live imported semantic-rule surface landed in `M258-B002`.
- Keep the current boundary explicit and fail closed:
  - imported-surface ingest is landed
  - serialized metadata rehydration is not landed
  - incremental reuse is not landed
  - imported metadata payload IR lowering is not landed
  - public imported-payload ABI handles are not landed
- Keep the IR emitter and public embedding ABI comments explicit about those non-goals.

## Canonical anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/libobjc3c_frontend/api.h`
- `package.json`

## Validation evidence

- `tmp/reports/m258/M258-C001/serialized_metadata_import_and_lowering_contract_summary.json`
- `tmp/artifacts/compilation/objc3c-native/m258/c001-serialized-metadata-import-lowering/`
