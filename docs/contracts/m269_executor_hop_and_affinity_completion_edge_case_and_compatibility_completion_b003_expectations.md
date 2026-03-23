# M269 Executor-Hop And Affinity Completion Edge-Case And Compatibility Completion Expectations (B003)

Contract ID: `objc3c-part7-executor-hop-affinity-compatibility/m269-b003-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_executor_hop_and_affinity_compatibility_completion`

Required truths:

- async task callables now fail closed without `objc_executor(...)` affinity
- detached task creation now fails closed under `objc_executor(main)`
- the positive async fixture proves the dedicated Part 7 executor-affinity packet is emitted deterministically
- focused negative fixtures prove fail-closed diagnostics `O3S231` and `O3S232`
- this issue still does not claim runnable executor-hop lowering or scheduler runtime execution
