# M268-B001 Packet: Async Effect And Suspension Semantic Model - Contract And Architecture Freeze

Issue: `#7284`

## Intent

Freeze the truthful Part 7 semantic packet that carries live async-effect, await-suspension, and concurrency-handoff legality before any runnable lowering or runtime execution claim is widened.

## Scope

- publish a dedicated semantic surface for async continuation legality and await suspension legality
- carry the current live actor-isolation, task-cancellation, and concurrency-replay guard summaries into the manifest deterministically
- keep the lane-B boundary truthful: semantic legality is live, runnable async execution is still deferred

## Required code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- the semantic packet is live and deterministic
- async/await legality and concurrency handoff are now sema-owned rather than parser-only
- runnable async frame lowering, suspension cleanup, and executor runtime execution remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes `frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model` into the manifest with deterministic counts under `--no-emit-ir --no-emit-object`
