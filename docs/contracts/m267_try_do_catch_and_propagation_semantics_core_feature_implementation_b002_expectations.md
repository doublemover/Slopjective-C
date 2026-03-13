# M267 Try, Do/Catch, And Propagation Semantics Core Feature Implementation Expectations (B002)

Issue: `M267-B002`
Contract ID: `objc3c-part6-try-throw-do-catch-semantics/m267-b002-v1`
Surface path: `frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics`

## Goal

Make `try`, `throw`, and `do/catch` truthful live semantic surfaces in source-only native validation. Lowering, native thrown-error ABI, and runnable catch transfer remain later lane work.

## Required truths

- `try`, `try?`, and `try!` parse and normalize into deterministic semantic inventory.
- `throw` statements parse and undergo semantic legality checking.
- `do { ... } catch ...` parses and undergoes deterministic catch-shape legality checking.
- propagating `try` requires either a `throws` function or an enclosing `do/catch` handler surface.
- `try` operands must resolve to throwing or NSError-bridged call surfaces.
- `throw` is only legal inside a `throws` callable or inside a catch body.
- catch clauses after a catch-all fail closed.
- native IR/object/executable emission remains fail-closed for these constructs in this lane.
- the semantic packet must remain deterministic and replayable.

## Code anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Spec/doc anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/30-semantics.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_6_ERRORS_RESULTS_THROWS.md`
- `package.json`

## Required validation

- issue-local checker proves code/doc/spec/package anchors exist
- positive source-only semantic probe over `tests/tooling/fixtures/native/m267_try_do_catch_semantics_positive.objc3`
- negative source-only probe over `tests/tooling/fixtures/native/m267_try_requires_throwing_context_negative.objc3`
- negative source-only probe over `tests/tooling/fixtures/native/m267_try_requires_throwing_or_bridged_operand_negative.objc3`
- negative source-only probe over `tests/tooling/fixtures/native/m267_throw_requires_throws_or_catch_negative.objc3`
- negative source-only probe over `tests/tooling/fixtures/native/m267_catch_after_catch_all_negative.objc3`
- fail-closed native-emission probe over `tests/tooling/fixtures/native/m267_try_do_catch_native_fail_closed.objc3`
- issue-local pytest passes
- lane-B readiness runner passes
