# M271 Live Cleanup Helpers And Retainable-Family Integration Expectations (D002)

Contract ID: `objc3c-part8-live-cleanup-retainable-runtime-integration/m271-d002-v1`

## Required outcomes

- Prove the supported Part 8 cleanup/resource and retainable-family slice executes through the emitted native function body.
- Keep the runtime/helper boundary on the same private ARC/autorelease helper cluster frozen in `M271-D001`.
- Keep the packaged runtime archive path unchanged.
- Prove direct cleanup execution for `CloseFd` and `ReleaseTemp`.
- Prove retainable-family helper traffic through `objc3_runtime_retain_i32`, `objc3_runtime_release_i32`, and `objc3_runtime_autorelease_i32`.

## Non-goals

- Do not claim a dedicated borrowed-pointer runtime helper.
- Do not claim escaping cleanup/resource ownership transfer.
- Do not widen the public runtime header.
- Do not add a new Part 8 runtime package or import surface.
