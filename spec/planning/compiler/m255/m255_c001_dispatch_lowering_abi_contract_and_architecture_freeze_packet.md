# M255-C001 Dispatch Lowering ABI Contract and Architecture Freeze Packet

Packet: `M255-C001`
Milestone: `M255`
Lane: `C`
Issue: `#7119`
Contract ID: `objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1`

## Objective

Freeze the lowering ABI that bridges current message-send lowering to the future live runtime dispatch path.

## Frozen boundary

- Canonical runtime dispatch symbol: `objc3_runtime_dispatch_i32`
- Compatibility bridge symbol: `objc3_msgsend_i32`
- Selector lookup symbol: `objc3_runtime_lookup_selector`
- Selector handle type: `objc3_runtime_selector_handle`
- Receiver ABI type: `i32`
- Selector ABI type: `ptr`
- Fixed argument ABI type: `i32`
- Fixed argument slot count: `4`
- Result ABI type: `i32`
- Semantic surface path: `frontend.pipeline.semantic_surface.objc_runtime_dispatch_lowering_abi_contract`

## Explicit non-goals

- No runtime selector-handle lowering is emitted yet.
- No live call emission cutover to `objc3_runtime_dispatch_i32` lands yet.
- No super/nil/direct runtime-entrypoint cutover lands yet.

## Acceptance

- Code/spec/doc anchors are explicit and deterministic.
- Manifest semantic surface publishes the frozen ABI packet.
- IR publishes a replay-stable `runtime_dispatch_lowering_abi_boundary` comment.
- The default native path still lowers through `objc3_msgsend_i32` while freezing the canonical runtime ABI for `M255-C002`.
