# M271 System Helper And Runtime Contract Expectations (D001)

Contract ID: `objc3c-part8-system-helper-runtime-contract/m271-d001-v1`

## Required outcomes

- Freeze the current Part 8 runtime/helper boundary as a reuse contract over the existing private ARC/autorelease helper cluster.
- Keep the runtime surface private to `objc3_runtime_bootstrap_internal.h`.
- Keep the packaged runtime archive path unchanged.
- Prove the happy path with one positive native fixture and one linked runtime probe.

## Helper cluster covered by this freeze

- `objc3_runtime_retain_i32`
- `objc3_runtime_release_i32`
- `objc3_runtime_autorelease_i32`
- `objc3_runtime_push_autoreleasepool_scope`
- `objc3_runtime_pop_autoreleasepool_scope`
- `objc3_runtime_copy_memory_management_state_for_testing`
- `objc3_runtime_copy_arc_debug_state_for_testing`

## Non-goals

- Do not claim a dedicated borrowed-pointer runtime helper.
- Do not claim escaping cleanup/resource ownership-transfer support.
- Do not widen the public runtime header.
- Do not add a new Part 8 runtime import surface.
