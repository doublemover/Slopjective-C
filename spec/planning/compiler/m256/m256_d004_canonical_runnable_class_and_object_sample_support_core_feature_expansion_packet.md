# M256-D004 Canonical Runnable Class and Object Sample Support Core Feature Expansion Packet

Packet: `M256-D004`
Issue: `#7142`
Milestone: `M256`
Lane: `D`
Wave: `W48`

## Summary
Implement the truthful runnable object-sample boundary above the realized graph so native `.objc3` programs can allocate, initialize, and dispatch on realized classes while richer category/protocol cases remain proven through a dedicated runtime probe.

## Contract
- Contract ID: `objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1`
- Execution model: `canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch`
- Probe split model: `metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits`
- Fail-closed model: `metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open`

## Dependencies
- `M256-D003`

## Anchors
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`

## Required proof assets
- Executable sample: `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_sample.objc3`
- Metadata-rich runtime library: `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_runtime_library.objc3`
- Probe: `tests/tooling/runtime/m256_d004_canonical_runnable_object_probe.cpp`
- Evidence: `tmp/reports/m256/M256-D004/canonical_runnable_object_sample_support_summary.json`

## Happy-path proof
1. A canonical `.objc3` object sample compiles, links, and runs with `[[Widget alloc] init]`.
2. The sample returns `37` by combining instance dispatch, inherited class dispatch, and concrete class dispatch.
3. Runtime dispatch of `alloc`, `init`, and `new` resolves through runtime-owned builtin owners instead of fabricated source methods.
4. The realized graph still publishes one attached category and two protocol-conformance edges for the metadata-rich library fixture.
5. Attached-category dispatch still resolves `Widget(Tracing)::tracedValue`.
6. Runtime query `Widget -> Worker` still conforms directly, and `Widget -> Tracer` still conforms through the attached category.

## Non-goals
- property / ivar storage realization
- metadata-heavy all-in-one executable samples that still trip the runtime export gate
- cross-image allocation or coalescing semantics beyond the current single-image runtime path

## Next issue
- `M256-E001`
