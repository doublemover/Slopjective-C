# Parser Subsystem and AST Scaffolding Contract Expectations (M138)

Contract ID: `objc3c-parser-ast-contract/m138-v1`

## Scope

This contract defines fail-closed integration requirements for parser subsystem extraction and AST builder scaffolding stability.

## Deterministic requirements

| ID | Requirement |
| --- | --- |
| `M138-PAR-01` | `parse/objc3_parser.h` + `parse/objc3_parser.cpp` remain the canonical parser implementation surface with deterministic `Objc3ParseResult` output structure (`Objc3ParsedProgram` wrapper + diagnostics). |
| `M138-PAR-02` | `parse/objc3_ast_builder_contract.h` + `parse/objc3_ast_builder_contract.cpp` remain the canonical parser-to-AST builder contract boundary, with pipeline consuming `BuildObjc3AstFromTokens(...)` instead of parser internals. |
| `M138-PAR-03` | `ast/objc3_ast.h` maintains AST scaffolding surface (`Expr`, `Stmt`, `FunctionDecl`, `Objc3Program`) as parser/lowering integration contract types. |
| `M138-PAR-04` | Build wiring remains deterministic in both `native/objc3c/CMakeLists.txt` and `scripts/build_objc3c_native.ps1` for `parse/objc3_parser.cpp` and `parse/objc3_ast_builder_contract.cpp`. |
| `M138-PAR-05` | CI/release gates fail closed on parser/AST contract drift via `python scripts/check_m138_parser_ast_contract.py` and `npm run check:compiler-closeout:m138`. |

## Validation commands

- `python scripts/check_m138_parser_ast_contract.py`
- `npm run test:objc3c:parser-ast-extraction`
- `npm run check:compiler-closeout:m138`

## Operator checklist

1. Run `python scripts/check_m138_parser_ast_contract.py`.
2. Run `npm run test:objc3c:parser-ast-extraction`.
3. Run `npm run check:compiler-closeout:m138`.
4. If any check fails, treat it as release-blocking drift and restore parser/AST contract wiring before promotion.
