# M258-B002 Imported Metadata Conformance, Effect, And Dispatch Preservation Rules Core Feature Implementation Packet

Packet: `M258-B002`
Milestone: `M258`
Lane: `B`
Issue: `#7161`
Dependencies: `M258-A002`, `M258-B001`
Next issue: `M258-C001`

## Objective

Land imported runtime metadata conformance, effect, and dispatch preservation as a real frontend capability for the supported executable core.

## Required implementation

- Accept repeated `--objc3-import-runtime-surface <path>` CLI inputs.
- Load emitted `module.runtime-import-surface.json` artifacts inside the frontend.
- Validate consumed artifacts fail closed:
  - duplicate input paths
  - duplicate imported module names
  - malformed JSON
  - contract-id / readiness mismatches
  - inventory-count mismatches
- Publish imported semantic preservation at:
  - `frontend.pipeline.semantic_surface.objc_imported_runtime_metadata_semantic_rules`
- Extend the emitted import artifact so imported consumers can recover:
  - property-attribute profiles
  - ownership-effect profiles
  - accessor ownership profiles
  - ivar layout slot/size/alignment fields
  - ivar source model
- Keep the current boundary explicit:
  - imported runtime metadata payloads are not lowered into IR yet
  - the public embedding ABI remains filesystem-artifact only

## Canonical anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/libobjc3c_frontend/api.h`
- `scripts/build_objc3c_native.ps1`

## Validation evidence

- `tmp/reports/m258/M258-B002/imported_runtime_metadata_semantic_rules_summary.json`
- `tmp/artifacts/compilation/objc3c-native/m258/b002-imported-runtime-metadata-semantic-rules/`
