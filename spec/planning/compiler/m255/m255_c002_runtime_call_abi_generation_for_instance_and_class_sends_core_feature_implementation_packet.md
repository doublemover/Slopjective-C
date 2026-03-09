# M255-C002 Runtime Call ABI Generation For Instance And Class Sends Core Feature Implementation Packet

Packet: `M255-C002`
Milestone: `M255`
Lane: `C`
Issue: `#7120`
Contract ID: `objc3c-runtime-call-abi-instance-class-dispatch/m255-c002-v1`

## Objective

Generate live runtime ABI calls for normalized instance and class message sends instead of treating `objc3_msgsend_i32` as the only executable dispatch target.

## Dependencies

- `M255-C001`
- `M255-A002`
- `M255-B002`

## Required implementation boundary

- Instance sends lower directly to `objc3_runtime_dispatch_i32`.
- Class sends lower directly to `objc3_runtime_dispatch_i32`.
- Super/dynamic/deferred sends remain on `objc3_msgsend_i32` until `M255-C003`.
- Selector operands remain lowered cstring pointers.
- The fixed `i32[4]` argument ABI remains unchanged.

## Required proof

- An issue-local positive fixture proves two canonical runtime dispatch calls in emitted IR.
- The same fixture produces an executable that runs successfully and returns the deterministic expected value.
- A deferred-surface probe proves `objc3_msgsend_i32` remains the compatibility target for non-cutover super/dynamic sites.
- Evidence lands under `tmp/reports/m255/M255-C002/`.
