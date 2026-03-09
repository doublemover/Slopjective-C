# M255 Super, Nil, And Direct Dispatch Lowering Through Runtime Entrypoints Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-runtime-call-abi-super-nil-direct-dispatch/m255-c003-v1`

## Scope

`M255-C003` extends the live lane-C dispatch cutover beyond the initial instance/class path.

## Required outcomes

1. Normalized super sends lower directly to `objc3_runtime_dispatch_i32`.
2. Canonical nil-receiver sends lower through `objc3_runtime_dispatch_i32` instead of lowering-side elision.
3. `objc3_runtime_dispatch_i32` returns `0` for nil receivers.
4. Normalized dynamic sends remain on `objc3_msgsend_i32` until `M255-C004`.
5. Reserved direct-dispatch surfaces fail closed if they reach IR lowering.
6. Evidence must land under `tmp/reports/m255/M255-C003/`.

## Lowering models

- Active lowering model: `instance-class-super-and-nil-sends-lower-directly-to-canonical-runtime-entrypoint`
- Deferred lowering model: `dynamic-sends-stay-on-compatibility-bridge-until-m255-c004`
- Unsupported fallback model: `direct-dispatch-fails-closed-until-supported-surface-materializes`

## Code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
- `tests/tooling/fixtures/native/m255_nil_runtime_dispatch.objc3`

## Spec anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
