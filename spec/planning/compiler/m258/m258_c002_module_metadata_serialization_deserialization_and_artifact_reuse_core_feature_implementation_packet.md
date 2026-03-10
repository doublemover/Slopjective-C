# M258-C002 Module Metadata Serialization, Deserialization, And Artifact Reuse Core Feature Implementation Packet

Packet: `M258-C002`
Milestone: `M258`
Lane: `C`
Issue: `#7163`
Dependencies: `M258-C001`, `M258-B002`
Next issue: `M258-D001`

## Objective

Implement real serialized runtime-metadata artifact reuse so downstream module imports can recover transitive object-model metadata from emitted artifacts rather than reparsing source.

## Required implementation

- Publish one semantic-surface contract at:
  - `frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_artifact_reuse`
- Emit a nested `serialized_runtime_metadata_reuse_payload` inside:
  - `module.runtime-import-surface.json`
- Populate that payload from:
  - transitive imported runtime metadata
  - current-module runtime metadata
- Teach the import-surface loader to prefer the nested payload when present.
- Preserve the current boundary:
  - runtime registration across module boundaries is still lane-D work
  - imported payloads are still not lowered directly into LLVM IR in this lane

## Canonical anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/libobjc3c_frontend/api.h`
- `package.json`

## Validation evidence

- `tmp/reports/m258/M258-C002/module_metadata_artifact_reuse_summary.json`
- `tmp/artifacts/compilation/objc3c-native/m258/c002-module-metadata-artifact-reuse/`
