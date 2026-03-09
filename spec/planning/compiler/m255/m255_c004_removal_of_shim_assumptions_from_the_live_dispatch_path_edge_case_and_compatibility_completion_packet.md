# M255-C004 Removal Of Shim Assumptions From The Live Dispatch Path Edge-Case And Compatibility Completion Packet

Packet: `M255-C004`

## Goal

Remove the final live-path dependency on the compatibility dispatch symbol.

## Contract

- Contract ID: `objc3c-runtime-call-abi-live-dispatch-cutover/m255-c004-v1`
- Active lowering model: `all-supported-sends-lower-directly-to-canonical-runtime-entrypoint`
- Compatibility model: `compatibility-dispatch-symbol-remains-exported-but-not-emitted-on-live-path`
- Unsupported fallback model: `direct-dispatch-fails-closed-until-supported-surface-materializes`

## Required outcomes

1. Normalized dynamic sends lower directly to `objc3_runtime_dispatch_i32`.
2. Supported live dispatch surfaces emit only canonical runtime-dispatch calls.
3. `objc3_msgsend_i32` remains exported but is not emitted by the live path.
4. Direct dispatch remains unsupported and fail closed.
5. Evidence lands under `tmp/reports/m255/M255-C004/`.
