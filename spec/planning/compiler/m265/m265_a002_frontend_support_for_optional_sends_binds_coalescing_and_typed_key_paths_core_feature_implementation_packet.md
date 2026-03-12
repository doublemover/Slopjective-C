# M265-A002 Packet

Issue: `M265-A002`

Objective:

- promote the Part 3 lane-A frontend boundary from pure fail-closed scaffolding into a truthful parser-owned source surface for optional bindings, optional sends, optional-member-access sugar, nil-coalescing, and typed key-path literals

Code anchors:

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/lex/objc3_lexer.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

Spec anchors:

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

Acceptance detail:

- manifest publishes `frontend.pipeline.semantic_surface.objc_part3_type_source_closure`
- the positive fixture proves parser-owned admission for `if let` / `guard let`, optional sends, `??`, and `@keypath(...)`
- the manifest truthfully publishes `optional_send_sites`, `nil_coalescing_sites`, and `typed_keypath_literal_sites`
- optional-member access no longer remains outside the admitted frontend surface
- deterministic evidence lands under `tmp/reports/m265/M265-A002/`

Non-goals:

- no runnable optional-flow lowering yet
- no truthful post-`guard let` scope realization yet
- no executable typed key-path runtime yet
- no executable typed key-path runtime yet
