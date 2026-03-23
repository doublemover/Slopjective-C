# M269-A002 Packet: Frontend Task-Group And Cancellation Surface Completion - Core Feature Implementation

Issue: `#7295`

## Intent

Complete the remaining frontend/source-model work for task creation, cancellation, and the supported task-group surface before the runnable `M269` lanes widen into execution behavior.

## Scope

- publish one dedicated frontend semantic packet for task creation/task-group/cancellation source sites
- widen the parser-owned task-runtime hook profile so task-creation and supported task-group identifiers are counted truthfully
- prove the happy path through the frontend runner only
- keep runnable task allocation, scheduler-backed execution, and live cancellation deferred

## Required code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- the current supported task-group surface remains callable-identifier based
- the emitted packet is source-only and remains a handoff surface for later `M269` runnable work
- no runnable task allocation, executor hop execution, or scheduler-backed cancellation behavior is claimed here

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with `--no-emit-ir --no-emit-object`
- dynamic checker verifies the new task-group/cancellation packet and the widened async-effect packet counts directly from the emitted manifest
