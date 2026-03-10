# M257 Synthesized Accessor And Property Metadata Lowering Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1`

## Outcome

`M257-C003` turns sema-approved effective property accessors into real executable method bodies. Lowering must:

1. synthesize missing implementation-owned getter/setter method entries for effective instance accessors,
2. emit deterministic storage globals keyed by synthesized binding symbols,
3. widen property descriptor payloads with effective accessor, binding, layout, and accessor implementation attachments,
4. prove the happy path through a linked runtime probe over a native `.objc3` fixture.

## Required anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Proof artifacts

- Fixture: `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3`
- Runtime probe: `tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp`
- Summary: `tmp/reports/m257/M257-C003/synthesized_accessor_property_lowering_summary.json`

## Non-goals

- true runtime instance allocation
- reflective property registration helpers
- property metadata runtime query APIs
