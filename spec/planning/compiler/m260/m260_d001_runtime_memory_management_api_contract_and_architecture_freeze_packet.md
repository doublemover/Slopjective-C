# M260-D001 Runtime Memory Management API Contract And Architecture Freeze Packet

Packet: `M260-D001`
Issue: `#7175`
Milestone: `M260`
Lane: `D`
Wave: `W52`

## Summary

Freeze the truthful runtime memory-management API boundary for retain/release,
autorelease, current-property helpers, and weak helper entrypoints after
`M260-C002`.

## Dependencies

- `M260-C002`

## Acceptance

- the stable public runtime header stays register/lookup/dispatch plus testing
  snapshots only
- lowered retain/release/autorelease/current-property/weak helper entrypoints
  remain private to `objc3_runtime_bootstrap_internal.h`
- emitted IR publishes the frozen runtime memory-management API contract and
  named metadata
- the happy path is proven by
  `tests/tooling/fixtures/native/m260_ownership_runtime_hook_emission_positive.objc3`
- the runtime behavior is proven by
  `tests/tooling/runtime/m260_d001_runtime_memory_management_api_probe.cpp`
- validation evidence lands at
  `tmp/reports/m260/M260-D001/runtime_memory_management_api_contract_summary.json`

## Code Anchors

- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/runtime/objc3_runtime.h`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`

## Spec Anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Next Issue

- `M260-D002`
