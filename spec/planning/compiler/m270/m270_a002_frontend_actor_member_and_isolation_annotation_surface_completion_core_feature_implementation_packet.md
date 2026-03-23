# M270-A002 Packet: Frontend Actor-Member And Isolation-Annotation Surface Completion - Core Feature Implementation

Issue: `#7308`

## Intent

Close the remaining lane-A frontend/source-model gap so `M270` has one deterministic actor-member input language contract before semantic legality and runtime work begin.

## Scope

- admit contextual `actor class` declarations through the parser/frontend path
- admit callable `__attribute__((objc_nonisolated))` on actor members
- preserve existing `async` and `objc_executor(...)` actor-member spellings on the same surface
- publish a dedicated frontend semantic packet for actor-member/isolation-annotation source closure
- keep actor-member legality, cross-actor diagnostics, sendability enforcement, and runnable actor runtime behavior explicitly deferred

## Required code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`

## Truth boundary

- `actor class` is admitted as a contextual parser surface rather than a reserved lexer keyword
- `objc_nonisolated` is admitted as a callable attribute on actor methods
- the emitted semantic packet is frontend-only and does not claim actor-member legality, cross-actor diagnostics, sendability enforcement, or runnable actor runtime behavior

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest publishes deterministic actor-member/isolation counts under `frontend.pipeline.semantic_surface.objc_part7_actor_member_and_isolation_source_closure`

## Next issue

- `M270-B001`
