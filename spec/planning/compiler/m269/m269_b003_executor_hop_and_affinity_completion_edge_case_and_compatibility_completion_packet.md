# M269-B003 Packet: Executor-Hop And Affinity Completion - Edge-Case And Compatibility Completion

Issue: `#7298`

## Intent

Close the remaining executor-affinity gaps for the supported task surface before runnable lowering and scheduler-backed execution land.

## Scope

- reject async task callables that omit `objc_executor(...)` affinity
- reject detached task creation under `objc_executor(main)`
- publish a dedicated semantic surface for those live compatibility checks
- keep the lane-B boundary truthful: sema compatibility is live, runnable executor-hop lowering and runtime execution remain later work

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

- async task callables without affinity fail closed with `O3S231`
- detached task creation under main-executor affinity fails closed with `O3S232`
- the semantic packet is deterministic and ready for later lowering/runtime expansion
- runnable executor-hop lowering, task spawn runtime, and scheduler-visible execution remain later `M269` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes `frontend.pipeline.semantic_surface.objc_part7_executor_hop_and_affinity_compatibility_completion` into the manifest with exact replay-stable counts
- dynamic checker proves focused negative fixtures fail closed with `O3S231` and `O3S232`
