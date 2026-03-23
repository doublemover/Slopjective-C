# M270-A001 Packet: Actor, Isolation, And Sendable Source Closure Contract And Architecture Freeze

Issue: `#7307`

## Intent

Freeze one truthful source-owned Part 7 boundary for actor, isolation, and sendability markers that later `M270` semantic and lowering work will consume.

## Scope

- preserve the current parser-owned actor/isolation/sendability symbol-profile surface
- keep the admitted source surface limited to the existing identifier and attribute markers already recognized by the frontend
- prove the happy path through the frontend runner only
- keep dedicated actor grammar, actor-member semantics, isolation diagnostics, and runnable actor runtime behavior explicitly deferred

## Required code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`

## Truth boundary

- no dedicated `actor`, `sendable`, or `nonisolated` keyword is claimed here
- actor-isolation declarations, actor hops, sendable markers, and non-sendable crossings remain parser-owned symbol profiles
- the proof surface is frontend-only and does not claim actor-member legality, cross-actor diagnostics, or runnable actor scheduling

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest preserves deterministic actor/isolation/sendability counts under `frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model`
