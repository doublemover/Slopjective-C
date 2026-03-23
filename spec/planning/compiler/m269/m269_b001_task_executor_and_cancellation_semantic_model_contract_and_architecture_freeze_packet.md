# M269-B001 Packet: Task, Executor, And Cancellation Semantic Model - Contract And Architecture Freeze

Issue: `#7296`

## Intent

Freeze the truthful Part 7 semantic packet that carries task lifetime, executor-affinity, cancellation-observation, and structured-task legality before runnable lowering or scheduler-backed execution is introduced.

## Scope

- publish one dedicated semantic surface for task/executor/cancellation legality
- consume the existing `M269-A002` task-group/cancellation source packet
- carry the current live task-runtime/cancellation semantic counts into the manifest deterministically
- keep the lane-B boundary truthful: semantic legality is live, runnable lowering and runtime behavior remain deferred

## Required code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- the semantic packet consumes `M269-A002` source closure instead of widening the source surface again
- task lifetime legality, executor-affinity legality, cancellation observation legality, and structured task-group legality are now sema-owned
- runnable lowering, executor runtime behavior, and scheduler runtime behavior remain later `M269` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted semantic packet fields directly from the manifest
