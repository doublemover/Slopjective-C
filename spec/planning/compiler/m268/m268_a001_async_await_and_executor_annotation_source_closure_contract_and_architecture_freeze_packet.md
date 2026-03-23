# M268-A001 Packet: Async, Await, And Executor-Annotation Source Closure Contract And Architecture Freeze

Issue: `#7282`

## Intent

Freeze one truthful Part 7 frontend/source-model boundary before later `M268` sema, lowering, continuation ABI, and runtime work widen the async surface.

## Scope

- admit `async fn` during frontend parsing
- admit source-owned `async` declaration modifiers on Objective-C methods
- admit `await` as a parser-owned expression marker
- admit canonical `objc_executor(...)` callable attributes on functions and Objective-C methods
- keep the claim strictly at source closure; no runnable continuation or executor runtime claim yet

## Required code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/ast/objc3_ast.h`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- `async`, `await`, and `objc_executor(...)` are now parser-owned source constructs
- this issue only freezes source ownership and deterministic parser metadata
- lowering, continuation ABI, suspension cleanup, and executor runtime behavior remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture compiles through the frontend-only runner and writes a manifest under `--no-emit-ir --no-emit-object`
- the checker verifies parser-owned async/await/executor snippets directly in source
