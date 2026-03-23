# M269 Task, Executor, And Cancellation Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part7-task-executor-cancellation-source-closure/m269-a001-v1`

## Required outcomes

1. The compiler must admit one truthful frontend source boundary for task-runtime and cancellation-oriented callable code without inventing new reserved `task` or `cancel` keywords.
2. The admitted source surface must remain the existing async callable forms plus canonical `objc_executor(...)` callable attributes.
3. Parser-owned identifier profiling must remain the deterministic source contract for:
   - task-runtime hooks
   - cancellation checks
   - cancellation handlers
   - suspension-point identifiers
4. The happy-path proof for this issue must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
5. The emitted manifest must preserve the current task/executor/cancellation counts under `frontend.pipeline.semantic_surface`.
6. This issue must not widen the implementation claim into runnable task allocation, scheduler hops, executor dispatch, or live cancellation execution.
7. Validation evidence must land under `tmp/reports/m269/M269-A001/`.
