# M274-C002 Packet: Foreign Call And Lifetime Lowering - Core Feature Implementation

- Issue: `M274-C002`
- Milestone: `M274`
- Lane: `C`
- Contract ID: `objc3c-part11-foreign-call-and-lifetime-lowering/m274-c002-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part11_foreign_call_and_lifetime_lowering`

## Summary

This packet captures the native lowering path for foreign calls, lifetime and ownership bridges, and metadata preservation. It is the implementation-side companion to the Part 11 lowering freeze from `M274-C001`.

## Lowering surface

- foreign-call lowering remains explicit and deterministic
- imported foreign call sites lower into deterministic IR and object-artifact facts
- lifetime and ownership bridge sites remain visible in the emitted manifest
- foreign-surface metadata preservation remains explicit and replay-stable
- the Part 11 interop lowering freeze remains an explicit dependency of this packet
- the C++ ownership/throws/async interaction packet remains an explicit dependency of this packet
- the IR proof includes a dedicated foreign-call and lifetime-lowering comment plus a dedicated metadata anchor

## Contract continuity

- the packet keeps the current manifest dependency contract IDs intact:
  - `objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1`
  - `objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1`

## Deferred boundary

- this issue does not claim full ABI runtime bridge execution for every interop surface
- unsupported or unmodeled interop combinations remain fail-closed rather than silently downgraded
- the lowering contract remains deterministic and evidence-backed
