# Lexer Token Contract Expectations (M137)

Contract ID: `objc3c-lexer-token-contract/m137-v1`

## Scope

This contract defines fail-closed integration requirements for M137 lexer subsystem extraction and token metadata stability.

## Deterministic requirements

| ID | Requirement |
| --- | --- |
| `M137-LEX-01` | `native/objc3c/src/token/objc3_token_contract.h` defines deterministic token metadata (`kind`, `text`, `line`, `column`) and preserves `Objc3SemaTokenKind::{PointerDeclarator,NullabilitySuffix}`. |
| `M137-LEX-02` | Parser and AST integration preserve token-contract boundary wiring (`parse/objc3_parser.cpp` consumes `MakeObjc3SemaTokenMetadata`, `ast/objc3_ast.h` imports the token contract header). |
| `M137-LEX-03` | Build surfaces keep lexer module registration deterministic in both CMake and direct native build script wiring (`src/lex/objc3_lexer.cpp`). |
| `M137-LEX-04` | CI/release gates fail closed on contract drift via `python scripts/check_m137_lexer_contract.py` and `npm run check:compiler-closeout:m137`. |

## Validation commands

- `python scripts/check_m137_lexer_contract.py`
- `npm run test:objc3c:lexer-extraction-token-contract`
- `npm run test:objc3c:lexer-parity`
- `npm run check:compiler-closeout:m137`

## Operator checklist

1. Run `python scripts/check_m137_lexer_contract.py`.
2. Run `npm run test:objc3c:lexer-extraction-token-contract`.
3. Run `npm run test:objc3c:lexer-parity`.
4. Run `npm run check:compiler-closeout:m137`.
5. If any check fails, treat it as release-blocking drift and restore contract wiring before promotion.
