# M266-A002 Packet: Frontend Pattern Grammar And Guard Surface Completion Core Feature Implementation

## Goal

Widen the truthful Part 5 frontend-owned grammar surface beyond the `M266-A001` freeze without claiming runnable defer or full match semantics.

## Required implementation

- Keep the semantic-surface packet path fixed at `frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure`.
- Admit `guard` condition lists containing optional bindings followed by boolean clauses.
- Admit statement-form `match (...) { ... }`.
- Admit only the following pattern forms in this issue:
  - wildcard `_`
  - literal integer, `true`, `false`, and `nil`
  - binding patterns `let name` / `var name`
  - result-case patterns such as `.Ok(let value)` and `.Err(let error)`
- Keep the following fail closed with targeted parser diagnostics:
  - `defer`
  - expression-form `match` arms using `=>`
  - guarded patterns using `where`
  - type-test patterns using contextual `is`
- Preserve deterministic manifest replay through the Part 5 summary payload.
- Preserve minimal semantic validation so admitted frontend shapes do not immediately fail on ordinary bool guard expressions or case-body scope construction.

## Code anchors

- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CROSS_CUTTING_RULE_INDEX.md`

## Evidence

- write issue evidence under `tmp/reports/m266/M266-A002/`
- prove the positive frontend manifest with `m266_frontend_pattern_guard_surface_positive.objc3`
- prove the reserved/unsupported fail-closed diagnostics with:
  - `m266_match_expression_fail_closed_negative.objc3`
  - `m266_match_guarded_pattern_fail_closed_negative.objc3`
  - `m266_match_type_test_pattern_fail_closed_negative.objc3`
