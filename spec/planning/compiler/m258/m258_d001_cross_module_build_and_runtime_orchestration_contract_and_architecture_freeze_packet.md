# M258-D001 Cross-Module Build And Runtime Orchestration Contract And Architecture Freeze Packet

Packet: `M258-D001`
Milestone: `M258`
Lane: `D`
Issue: `#7164`
Dependencies: `M258-C002`, `M254-A002`
Next issue: `M258-D002`

## Objective

Freeze the truthful cross-module build/runtime orchestration boundary above the
transitive serialized import payload and the emitted local registration
manifest, without claiming that cross-module packaging or aggregated runtime
registration are already implemented.

## Required implementation

- Publish one semantic-surface contract at:
  - `frontend.pipeline.semantic_surface.objc_cross_module_build_runtime_orchestration_contract`
- Source that contract from:
  - `objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1`
  - `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- Keep the current boundary explicit and fail closed:
  - the transitive import payload determines the planned module-image set
  - the local registration manifest determines the local descriptor inventory
  - no cross-module link-plan artifact is emitted
  - imported registration manifests are not loaded
  - runtime-archive aggregation is not landed
  - aggregated runtime-registration launch orchestration is not landed
  - the public embedding ABI exposes no orchestration handles
- Keep the IR emitter and public embedding ABI comments explicit about those
  non-goals.

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

- `tmp/reports/m258/M258-D001/cross_module_build_runtime_orchestration_contract_summary.json`
- `tmp/artifacts/compilation/objc3c-native/m258/d001-cross-module-build-runtime-orchestration/`
