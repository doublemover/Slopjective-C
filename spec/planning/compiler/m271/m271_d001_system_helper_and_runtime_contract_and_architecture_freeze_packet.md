# M271-D001 Packet: System Helper And Runtime Contract - Contract And Architecture Freeze

## Summary

Freeze the current Part 8 runtime/helper boundary as a truthful reuse contract over the private ARC/autorelease helper cluster and the existing packaged runtime archive path.

## Scope

- emit one explicit lowering/runtime summary for the Part 8 lane-D helper boundary
- anchor the private helper surface in runtime and process code
- prove the happy path with one native fixture and one linked runtime probe

## Required boundary

- keep `M271-C001` as the only Part 8 lowering contract
- consume `M271-C003` as the borrowed/retainable ABI packet above that lowering contract
- freeze the helper/runtime slice with:
  - `objc3_runtime_retain_i32`
  - `objc3_runtime_release_i32`
  - `objc3_runtime_autorelease_i32`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
  - `objc3_runtime_copy_memory_management_state_for_testing`
  - `objc3_runtime_copy_arc_debug_state_for_testing`

## Truth constraints

- cleanup execution and resource invalidation proof still ride existing cleanup lowering plus autoreleasepool state
- retainable-family helper integration still rides the private ARC helper cluster
- the packaged runtime archive path remains the same
- no public runtime ABI widening
- no dedicated borrowed-pointer runtime helper or Part 8 runtime import surface
- escaping cleanup/resource ownership transfer remains deferred
