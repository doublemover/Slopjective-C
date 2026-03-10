# M258-D002 Packaging, Link, And Runtime Registration Across Module Boundaries Core Feature Implementation Packet

Packet: `M258-D002`
Milestone: `M258`
Lane: `D`
Issue: `#7165`
Dependencies: `M258-D001`, `M258-C002`
Next issue: `M258-E001`

## Objective

Implement the real lane-D cross-module packaging path so imported runtime
metadata from other modules can participate in deterministic link planning and
multi-image runtime registration without claiming later method-binding work is
already complete.

## Required implementation

- Load imported runtime-import-surface peer artifacts from the downstream
  compile path:
  - imported registration manifest
  - imported discovery artifact
  - imported runtime-metadata linker response file
  - imported object artifact
- Validate those imported peer artifacts fail closed against the local compile:
  - runtime library archive path
  - object format
  - translation-unit identity model
  - translation-unit identity key uniqueness
  - registration ordinal uniqueness and ordering
- Emit the authoritative downstream D002 artifacts:
  - `module.cross-module-runtime-link-plan.json`
  - `module.cross-module-runtime-linker-options.rsp`
- Preserve deterministic ordering rules:
  - module names lexicographic for inventory
  - link objects ascending by registration ordinal then translation-unit
    identity key
  - merged linker flags in ordered imported/local registration order
- Prove the happy path by linking provider + consumer object artifacts with the
  merged response file into a runtime probe that validates startup
  registration, imported metadata realization, imported protocol conformance,
  replay, and replay-stable dispatch.
- Keep the canonical docs/spec/code anchors explicit about the truthful D002
  boundary: real packaging/runtime registration is landed, but source method
  body binding across module boundaries still belongs to later work.

## Canonical anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/libobjc3c_frontend/api.h`
- `package.json`

## Validation evidence

- `tmp/reports/m258/M258-D002/cross_module_runtime_packaging_summary.json`
- `tmp/artifacts/compilation/objc3c-native/m258/d002/provider/`
- `tmp/artifacts/compilation/objc3c-native/m258/d002/consumer/`
- `tmp/reports/m258/M258-D002/probe/`
