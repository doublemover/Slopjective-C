# M255 Runtime Call ABI Generation For Instance And Class Sends Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-runtime-call-abi-instance-class-dispatch/m255-c002-v1`

## Scope

`M255-C002` turns the frozen `M255-C001` dispatch ABI into live code generation for the non-deferred runtime path.

## Required outcomes

1. Normalized instance sends lower directly to `objc3_runtime_dispatch_i32`.
2. Normalized class sends lower directly to `objc3_runtime_dispatch_i32`.
3. Deferred super/dynamic/direct handling stays on `objc3_msgsend_i32` until `M255-C003`.
4. Selector operands remain lowered cstring pointers and the fixed `i32[4]` argument ABI stays unchanged.
5. One issue-local positive fixture must prove both emitted IR and executable happy-path behavior.
6. Evidence must land under `tmp/reports/m255/M255-C002/`.

## Code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
- `tests/tooling/fixtures/native/m255_instance_class_runtime_dispatch.objc3`

## Spec anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
