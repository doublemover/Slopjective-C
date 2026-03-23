# M268-A002 Packet: Frontend Async Entry And Await Surface Completion - Core Feature Implementation

Issue: `#7283`

## Intent

Publish the remaining truthful frontend/source-model packet for Part 7 so async syntax is not merely parser-accepted but also emitted as deterministic manifest state.

## Scope

- publish a dedicated frontend semantic surface for async entry points, await expressions, and executor-affinity attributes
- count parser-owned async/await/executor usage deterministically across functions and Objective-C methods
- keep the lane-A claim source-only; no continuation/runtime execution claim yet

## Required code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- the frontend semantic packet is live and deterministic
- async syntax is source-owned, but runnable continuations, suspension cleanup, and executor runtime behavior remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes `frontend.pipeline.semantic_surface.objc_part7_async_source_closure` into the manifest with deterministic counts under `--no-emit-ir --no-emit-object`
