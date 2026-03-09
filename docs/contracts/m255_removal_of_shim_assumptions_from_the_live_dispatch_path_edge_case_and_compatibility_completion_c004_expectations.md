# M255 Removal Of Shim Assumptions From The Live Dispatch Path Edge-Case And Compatibility Completion Expectations (C004)

Contract ID: `objc3c-runtime-call-abi-live-dispatch-cutover/m255-c004-v1`

## Scope

`M255-C004` closes the lane-C live dispatch cutover by removing the last
compatibility-bridge assumption from emitted IR.

## Required outcomes

1. Normalized dynamic sends lower directly to `objc3_runtime_dispatch_i32`.
2. All supported live sends lower directly to `objc3_runtime_dispatch_i32`.
3. `objc3_msgsend_i32` remains exported only as compatibility/test evidence and is not emitted on the live path.
4. Reserved direct-dispatch surfaces remain fail closed if they reach IR lowering.
5. Validation evidence lands under `tmp/reports/m255/M255-C004/`.

## Lowering models

- Active lowering model: `all-supported-sends-lower-directly-to-canonical-runtime-entrypoint`
- Compatibility model: `compatibility-dispatch-symbol-remains-exported-but-not-emitted-on-live-path`
- Unsupported fallback model: `direct-dispatch-fails-closed-until-supported-surface-materializes`

## Code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c`

## Spec anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
