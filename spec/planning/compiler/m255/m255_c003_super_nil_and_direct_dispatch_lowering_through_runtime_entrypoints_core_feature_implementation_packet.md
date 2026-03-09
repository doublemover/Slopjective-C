# M255-C003 Super, Nil, And Direct Dispatch Lowering Through Runtime Entrypoints Core Feature Implementation Packet

Packet: `M255-C003`
Milestone: `M255`
Lane: `C`
Issue: `#7121`
Contract ID: `objc3c-runtime-call-abi-super-nil-direct-dispatch/m255-c003-v1`

## Objective

Extend the live runtime dispatch cutover so canonical super and nil-receiver paths execute through `objc3_runtime_dispatch_i32`, while dynamic compatibility remains isolated and unsupported direct surfaces fail closed.

## Dependencies

- `M255-C002`
- `M255-B003`

## Required implementation boundary

- Normalized super sends lower directly to `objc3_runtime_dispatch_i32`.
- Canonical nil-receiver sends lower through `objc3_runtime_dispatch_i32` instead of lowering-side elision.
- `objc3_runtime_dispatch_i32` returns `0` for nil receivers.
- Dynamic sends remain on `objc3_msgsend_i32` until `M255-C004`.
- Reserved direct-dispatch surfaces fail closed if they survive into IR lowering.

## Required proof

- An issue-local nil fixture proves one canonical runtime dispatch call in emitted IR and executable happy-path behavior with a nil receiver.
- The existing super/dynamic method-family corpus proves super sends moved to the canonical runtime entrypoint while dynamic sends stayed on the compatibility bridge.
- Evidence lands under `tmp/reports/m255/M255-C003/`.
