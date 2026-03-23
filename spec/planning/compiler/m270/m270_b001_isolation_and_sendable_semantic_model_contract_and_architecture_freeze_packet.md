# M270-B001 Packet: Isolation And Sendable Semantic Model - Contract And Architecture Freeze

Issue: `#7309`

## Intent

Freeze one truthful lane-B sema packet for actor isolation, sendability, and strict-concurrency handoff before later `M270` issues attempt broader actor legality or runtime behavior.

## Scope

- publish a dedicated actor/sendability sema packet
- consume the existing `M270-A002` actor-member source packet and the already-landed aggregated actor/sendability sema counters
- preserve deterministic sema/accounting carriage for actor isolation, actor hops, sendable markers, non-sendable crossings, and actor-member source metadata
- keep dedicated actor-isolation diagnostics, broad sendability enforcement, executor scheduling, and runnable actor runtime behavior explicitly deferred

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
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`

## Truth boundary

- the packet is sema/accounting only
- strict-concurrency selection/reporting remains fail-closed
- actor-member legality, cross-actor diagnostics, sendability enforcement, executor scheduling, and runnable actor runtime behavior remain later `M270` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest publishes deterministic actor/sendability sema counts under `frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendable_semantic_model`

## Next issue

- `M270-B002`
