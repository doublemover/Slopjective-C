# M269 Frontend Task-Group And Cancellation Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part7-task-group-cancellation-source-closure/m269-a002-v1`

## Required outcomes

1. The frontend must publish one deterministic source packet for task creation, supported task-group call sites, and cancellation-oriented call sites.
2. The packet must be emitted at `frontend.pipeline.semantic_surface.objc_part7_task_group_and_cancellation_source_closure`.
3. The current supported task-group source surface must remain callable-identifier based; this issue does not introduce dedicated task-group syntax.
4. The happy-path proof must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
5. The emitted manifest must preserve deterministic counts for:
   - task creation sites
   - task-group scope sites
   - task-group add-task sites
   - task-group wait-next sites
   - task-group cancel-all sites
   - cancellation-check sites
   - cancellation-handler sites
6. The older async-effect packet must still reflect the widened parser-side task-runtime hook surface truthfully.
7. This issue must not claim runnable task allocation, scheduler hops, or live cancellation runtime execution.
8. Validation evidence must land under `tmp/reports/m269/M269-A002/`.
