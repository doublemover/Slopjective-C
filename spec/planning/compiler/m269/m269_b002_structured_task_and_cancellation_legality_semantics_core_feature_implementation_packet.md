# M269-B002 Packet: Structured-Task And Cancellation Legality Semantics - Core Feature Implementation

Issue: `#7297`

## Intent

Implement the truthful Part 7 sema boundary that rejects illegal structured-task and cancellation usage and publishes a dedicated deterministic packet for those live legality checks.

## Scope

- reject task-runtime, task-group, and cancellation calls outside async functions and methods
- reject task-group add, wait-next, and cancel-all calls when no task-group scope exists in the same async callable
- reject detached task creation when mixed with a structured task-group callable
- reject cancellation handlers and cancel-all calls when no cancellation check exists in the same async callable
- publish a dedicated semantic surface for those live legality checks
- keep the lane-B boundary truthful: sema enforcement is live, runnable task lowering and runtime execution remain later work

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

- task/cancellation legality is now enforced live in sema
- `O3S227`, `O3S228`, `O3S229`, and `O3S230` are the fail-closed diagnostics for the currently supported structured-task boundary
- the semantic packet is deterministic and ready for later lowering/runtime expansion
- runnable task lowering, executor runtime behavior, and scheduler runtime behavior remain later `M269` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes `frontend.pipeline.semantic_surface.objc_part7_structured_task_and_cancellation_semantics` into the manifest with exact replay-stable counts
- dynamic checker proves focused negative fixtures fail closed with `O3S227`, `O3S228`, `O3S229`, and `O3S230`
