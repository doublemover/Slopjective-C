# M257-D003 Property Metadata Registration And Reflective Access Helpers Core Feature Expansion Packet

Packet: `M257-D003`
Issue: `#7155`
Contract ID: `objc3c-runtime-property-metadata-reflection/m257-d003-v1`

Summary:
Implement private runtime reflection helpers over realized property/accessor/layout metadata so testing and diagnostics can query the live runtime-owned property graph directly.

Inputs:

- `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
- `tests/tooling/runtime/m257_d003_property_metadata_reflection_probe.cpp`
- realized property/layout runtime from `M257-D002`
- synthesized accessor/property metadata from `M257-C003`

Required implementation points:

1. Runtime publishes aggregate property reflection state snapshots.
2. Runtime publishes per-property reflective entry snapshots by class/property name.
3. Reflective entries expose effective selectors, owner identities, synthesized binding symbols, and layout facts.
4. Misses and unknown classes stay deterministic and fail closed.
5. IR publishes `; runtime_property_metadata_reflection = ...` with deterministic inventory fields.
6. Historical `M257-C003` / `M257-D002` validation remains forward-compatible after D003 lands.

Evidence:

- `tmp/reports/m257/M257-D003/property_metadata_reflection_summary.json`

Touched surfaces:

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/runtime/README.md`
- `tests/tooling/runtime/README.md`
- `package.json`
