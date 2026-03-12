# M267-A002 Packet: Frontend NSError And Status Bridging Surface Completion

Issue: `#7270`

## Intent

Complete the truthful frontend/source-model surface for canonical Part 6 NSError and status bridging markers before later sema, lowering, and runtime issues widen runnable error behavior.

## Scope

- Admit `__attribute__((objc_nserror))` on functions and Objective-C methods.
- Admit `__attribute__((objc_status_code(success: ..., error_type: ..., mapping: ...)))` on functions and Objective-C methods.
- Normalize these markers into one deterministic Part 6 frontend summary.
- Fail closed on malformed `objc_status_code(...)` payloads.
- Keep runtime bridge execution deferred; this issue is frontend/source-closure only.

## Required code anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native/src/20-grammar.md`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- canonical bridge markers are parser/AST/frontend-owned source state
- the Part 6 manifest summary now counts marker sites and required clause sites
- malformed payloads fail closed in the parser
- runtime `try`, bridge temporaries, status-to-error conversion, and native thrown-error ABI remain deferred to later `M267` issues

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive source-only fixture writes a manifest carrying the canonical bridge-marker counters under `--no-emit-ir --no-emit-object`
- the checker accepts a trailing `runtime-aware import/module frontend closure not ready` runner status as orthogonal to the source-surface proof
- one malformed negative fixture proves deterministic `O3P280` diagnostics
