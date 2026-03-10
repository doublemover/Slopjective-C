# M256-D003 Category Attachment and Protocol Conformance Runtime Checks Core Feature Implementation Packet

Packet: `M256-D003`
Issue: `#7141`
Milestone: `M256`
Lane: `D`
Wave: `W48`

## Summary
Implement runtime-owned category attachment and protocol conformance checks on top of the realized class graph so canonical object programs can prove live category-aware dispatch and protocol queries.

## Contract
- Contract ID: `objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1`
- Category attachment model: `realized-class-nodes-own-preferred-category-attachments-after-registration`
- Protocol conformance query model: `runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures`
- Fail-closed model: `invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries`

## Dependencies
- `M256-D002`
- `M256-B003`

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
- Fixture: `tests/tooling/fixtures/native/m256_d003_category_attachment_protocol_runtime_library.objc3`
- Probe: `tests/tooling/runtime/m256_d003_category_attachment_protocol_runtime_probe.cpp`
- Evidence: `tmp/reports/m256/M256-D003/category_attachment_protocol_conformance_runtime_checks_summary.json`

## Happy-path proof
1. Realized class graph publication exposes one attached category and two protocol-conformance edges.
2. Realized class `Widget` retains category owner `category:Widget(Tracing)` and attached protocol `Tracer`.
3. `objc3_runtime_dispatch_i32(1042, "tracedValue", ...)` resolves through the attached category and returns `13`.
4. `objc3_runtime_dispatch_i32(1043, "classValue", ...)` resolves the class method and returns `11`.
5. Runtime query `Widget -> Worker` conforms through the class protocol refs.
6. Runtime query `Widget -> Tracer` conforms through the attached category protocol refs.
7. Runtime query `Base -> Worker` remains non-conforming and fail-closed.
8. Dispatch of `ignoredValue` fails closed to compatibility fallback instead of fabricating a live method.

## Non-goals
- object allocation
- property / ivar storage realization
- executable protocol-body dispatch
- cross-image attachment coalescing beyond the current registration order

## Next issue
- `M256-D004`
