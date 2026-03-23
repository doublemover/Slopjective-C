# M269 Task-Group And Runtime ABI Completion Expectations (C003)

Contract ID: `objc3c-part7-task-runtime-abi-completion/m269-c003-v1`
Issue: `#7301`

## Required outcomes

- the frontend must publish `frontend.pipeline.semantic_surface.objc_part7_task_group_and_runtime_abi_completion`
- emitted IR must publish:
  - `; part7_task_runtime_abi_completion = ...`
  - `!objc3.objc_part7_task_runtime_abi_completion = !{!93}`
- the ABI packet must preserve the helper list, helper counts, and the private runtime snapshot symbol
- the private runtime snapshot symbol remains `objc3_runtime_copy_task_runtime_state_for_testing`
- the issue-local runtime probe must live at `tests/tooling/runtime/m269_c003_task_runtime_abi_completion_probe.cpp`

## Truth constraints

- this issue completes the ABI/artifact-preservation layer on top of `M269-C002`; it does not widen the public runtime header
- current native front-door compilation for this Part 7 slice is still constrained by retained `O3S260` / `O3L300` gates outside the direct `M269-C003` implementation surface
- the issue therefore proves the private runtime ABI dynamically and proves packet/IR preservation through deterministic source anchors
