# M269-C003 Packet: Task-Group And Runtime ABI Completion - Core Feature Expansion

Packet: `M269-C003`
Issue: `#7301`
Milestone: `M269`
Lane: `C`

## Objective

Implement the remaining ABI/artifact-preservation layer for the helper-backed Part 7 task-runtime slice.

## Scope

- publish a dedicated semantic-surface packet for the helper-backed task-runtime ABI
- publish a matching IR boundary and named metadata marker
- preserve helper symbol inventory, task-group helper counts, and the private runtime snapshot symbol in the emitted packet
- keep the runtime proof surface private and probeable

## Non-goals

- no public runtime helper ABI
- no claim that the retained `O3S260` / `O3L300` front-door gates are fixed here
- no scheduler implementation beyond preserving the helper/runtime proof surface already landed in `M269-C002`

## Required evidence

- deterministic source anchors in `native/objc3c/src/lower/objc3_lowering_contract.h`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, and `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- runtime probe at `tests/tooling/runtime/m269_c003_task_runtime_abi_completion_probe.cpp`
- summary written to `tmp/reports/m269/M269-C003/task_runtime_abi_completion_summary.json`
- next issue remains `M269-D001`
