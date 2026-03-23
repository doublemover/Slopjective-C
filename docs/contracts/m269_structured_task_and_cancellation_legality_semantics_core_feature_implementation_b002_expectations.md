# M269 Structured-Task And Cancellation Legality Semantics Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-part7-structured-task-cancellation-semantics/m269-b002-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_structured_task_and_cancellation_semantics`

Required truths:

- sema now enforces that task-runtime, task-group, and cancellation calls require an async function or method
- sema now enforces that task-group add, wait-next, and cancel-all calls require a task-group scope in the same async callable
- sema now enforces that detached task creation cannot share a structured task-group callable
- sema now enforces that cancellation handlers and cancel-all calls require a cancellation check in the same async callable
- the positive async fixture proves the dedicated Part 7 structured-task packet is emitted deterministically
- focused negative fixtures prove fail-closed diagnostics `O3S227`, `O3S228`, `O3S229`, and `O3S230`
- this issue still does not claim runnable task lowering, executor runtime behavior, or scheduler runtime behavior
