# M260-C002 Runtime Hook Emission For Retain, Release, Autorelease, And Weak Paths Core Feature Implementation Packet

Packet: `M260-C002`
Issue: `#7174`
Milestone: `M260`
Lane: `C`
Wave: `W52`

## Summary

Emit real runtime helper calls for synthesized retain, release, autorelease, and weak property paths so runtime-backed object ownership stops being a manifest-only lowering summary.

## Dependencies

- `M260-C001`
- `M260-B003`

## Acceptance

- synthesized strong-object property accessors emit runtime helper calls for read/retain/autorelease and retain/exchange/release
- synthesized weak-object property accessors emit runtime helper calls for weak load/store
- runtime dispatch carries the current receiver/property accessor context so helper calls operate on realized instance storage
- the happy path is proven by `tests/tooling/fixtures/native/m260_ownership_runtime_hook_emission_positive.objc3`
- the runtime behavior is proven by `tests/tooling/runtime/m260_c002_ownership_runtime_hook_probe.cpp`
- validation evidence lands at `tmp/reports/m260/M260-C002/ownership_runtime_hook_emission_summary.json`

## Code Anchors

- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`

## Spec Anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Next Issue

- `M260-D001`
