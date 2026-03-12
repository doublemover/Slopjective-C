# M266 Frontend Pattern Grammar And Guard Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part5-control-flow-source-closure/m266-a002-v1`

## Required truths

- The frontend semantic-surface packet remains `frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure`.
- The admitted Part 5 frontend slice now includes:
  - `guard` condition lists with optional bindings plus boolean clauses
  - statement-form `match (...) { ... }`
  - wildcard, literal, binding, and result-case match patterns
- The frontend still fails closed for:
  - `defer`
  - expression-form `match` arms using `=>`
  - guarded patterns using `where`
  - type-test patterns using contextual `is`

## Dynamic proof

- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m266_frontend_pattern_guard_surface_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m266/a002/positive --emit-prefix module --no-emit-ir --no-emit-object`
- The emitted manifest summary must publish:
  - `guard_binding_sites = 1`
  - `guard_binding_clause_sites = 1`
  - `guard_boolean_condition_sites = 1`
  - `match_statement_sites = 2`
  - `match_case_pattern_sites = 6`
  - `match_default_sites = 2`
  - `match_wildcard_pattern_sites = 1`
  - `match_literal_pattern_sites = 2`
  - `match_binding_pattern_sites = 1`
  - `match_result_case_pattern_sites = 2`
- The negative fixtures must fail closed with:
  - `reserved expression-form 'match' arm [O3P156]`
  - `unsupported guarded match pattern [O3P157]`
  - `unsupported type-test match pattern [O3P158]`

## Anchor files

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CROSS_CUTTING_RULE_INDEX.md`
- `native/objc3c/src/token/objc3_token_contract.h`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `package.json`
