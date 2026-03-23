# M269-C002 Packet: Executor Hop, Cancellation, And Task Spawning Lowering - Core Feature Implementation

Packet: `M269-C002`
Issue: `#7300`
Milestone: `M269`
Lane: `C`

## Objective

Implement the retained Part 7 task-runtime lowering contract as a live helper-backed compiler/runtime capability for the currently recognized task symbol family.

## Scope

- lower `task_spawn_child` / `spawn_task` and `detached_task_create` through a private spawn helper
- lower `with_task_group_scope`, `task_group_add_task`, `task_group_wait_next`, and `task_group_cancel_all` through private task-group helpers
- lower `task_runtime_cancelled_value` and `task_runtime_on_cancel` through private cancellation helpers
- lower awaited task-group wait sites through `objc3_runtime_executor_hop_i32` before the existing continuation helper path resumes execution
- expose a private runtime testing snapshot so the issue-local probe can prove helper traffic deterministically

## Non-goals

- no public runtime helper ABI
- no claim that the older `O3S260` / `O3L300` front-door gates are fixed here
- no generalized scheduler implementation or task-group ABI beyond the helper-backed slice above

## Required evidence

- deterministic source anchors in `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, and `native/objc3c/src/runtime/objc3_runtime.cpp`
- runtime probe at `tests/tooling/runtime/m269_c002_task_runtime_lowering_probe.cpp`
- summary written to `tmp/reports/m269/M269-C002/task_runtime_lowering_implementation_summary.json`
- next issue remains `M269-C003`
