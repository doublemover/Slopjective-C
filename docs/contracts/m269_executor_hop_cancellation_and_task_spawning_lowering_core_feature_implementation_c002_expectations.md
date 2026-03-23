# M269 Executor Hop, Cancellation, And Task Spawning Lowering Expectations (C002)

Contract ID: `objc3c-part7-task-runtime-lowering-implementation/m269-c002-v1`
Issue: `#7300`

## Required outcomes

- the IR emitter must rewrite the supported task-runtime symbol family onto private runtime helpers instead of leaving those calls as ordinary extern placeholders
- awaited task-group wait sites must also emit `objc3_runtime_executor_hop_i32` before the existing continuation-helper resume path
- the helper ABI must remain private to `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- the runtime must expose `objc3_runtime_copy_task_runtime_state_for_testing` for issue-local proof
- the issue-local runtime probe must live at `tests/tooling/runtime/m269_c002_task_runtime_lowering_probe.cpp`

## Truth constraints

- current native front-door compilation for this Part 7 slice is still constrained by retained `O3S260` / `O3L300` gates outside the direct `M269-C002` implementation surface
- the issue therefore proves the runtime helper cluster dynamically and proves the lowering rewrite through deterministic source anchors in `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- no public runtime ABI widening is allowed here
