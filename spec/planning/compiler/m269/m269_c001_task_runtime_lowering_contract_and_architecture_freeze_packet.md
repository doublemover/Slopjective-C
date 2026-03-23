# M269-C001 Packet: Task Runtime Lowering Contract - Contract And Architecture Freeze

Issue: `#7299`

## Intent

Freeze the Part 7 task runtime lowering boundary as a real emitted compiler contract.

## Scope

- derive explicit Part 7 lowering contracts from the live `M269-B001`, `M269-B002`, and `M269-B003` semantic summaries
- publish a dedicated manifest packet for task creation, executor-hop affinity, cancellation polling, and task-group lowering state
- thread replay-stable actor-isolation, task-runtime-interop, and concurrency-replay lowering metadata into emitted LLVM IR frontend metadata

## Required code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- this issue does not invent runnable task spawning, executor scheduling, or task-group runtime execution
- the existing Part 7 task/concurrency IR metadata channels now become truthful emitted artifacts rather than remaining unset
- native task spawn/hop/cancel entrypoints and task-group ABI/runtime completion remain later `M269` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive task fixture writes `frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract` into the manifest with exact deterministic counts under `--no-emit-ir --no-emit-object`
- static checker proves the IR emitter carries the replay-key comments, frontend lowering profile comments, and named metadata anchors for actor isolation, task runtime interop, and concurrency replay
