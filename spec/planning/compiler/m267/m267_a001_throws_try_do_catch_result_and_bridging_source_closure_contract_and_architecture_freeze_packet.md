# M267-A001 Packet: Throws, Try, Do/Catch, Result, And Bridging Source Closure Contract And Architecture Freeze

Issue: `#7269`

## Intent

Freeze one truthful Part 6 frontend/source-model boundary before later `M267` sema, lowering, and runtime work widens the runnable error surface.

## Scope

- Admit source-only `throws` declarations during frontend-only validation runs.
- Preserve deterministic result-like carrier profiles on functions and Objective-C methods.
- Preserve deterministic `NSError` bridging profiles on functions and Objective-C methods.
- Reserve `try`, `throw`, and `do/catch` as explicit fail-closed parser constructs.
- Publish one manifest summary instead of relying on older `M181` / `M182` / `M183` contract shards.

## Required code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- `throws` declarations are source-only in frontend-only runs.
- result-like profiling is parser/AST-owned metadata, not yet a runnable error ABI.
- `NSError` bridging profiling is parser/AST-owned metadata, not yet a runnable bridge path.
- `try`, `throw`, and `do/catch` remain fail-closed until later `M267` issues.

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive source-only fixture writes a manifest carrying `frontend.pipeline.semantic_surface.objc_part6_error_source_closure` under `--no-emit-ir --no-emit-object`
- the checker accepts a trailing `runtime-aware import/module frontend closure not ready` runner status as orthogonal to the Part 6 source-closure proof
- negative fixtures prove deterministic fail-closed diagnostics for `try`, `throw`, and `do/catch`
