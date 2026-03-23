# M269-A001 Packet: Task, Executor, And Cancellation Source Closure Contract And Architecture Freeze

Issue: `#7294`

## Intent

Freeze one truthful source-owned Part 7 boundary for the task-runtime and cancellation vocabulary that later `M269` runnable work will consume.

## Scope

- preserve the current parser-owned task/executor/cancellation symbol-profile surface
- keep the admitted syntax limited to existing `async fn`, `await <expr>`, and canonical `objc_executor(...)` callable attributes
- prove the happy path through the frontend runner only
- keep runnable task allocation, scheduler hops, executor dispatch, and cancellation execution explicitly deferred

## Required code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- no dedicated `task` or `cancel` keyword is claimed here
- task-runtime hooks, cancellation checks, cancellation handlers, and suspension-point identifiers remain parser-owned identifier profiles
- the proof surface is frontend-only and does not claim runnable task scheduling

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest preserves deterministic task/executor/cancellation counts under `frontend.pipeline.semantic_surface`
