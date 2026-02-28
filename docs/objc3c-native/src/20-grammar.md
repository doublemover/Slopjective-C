<!-- markdownlint-disable-file MD041 -->

## Supported `.objc3` grammar (implemented today)

```ebnf
program         = { module_decl | global_let | function_decl } EOF ;

module_decl     = "module" ident ";" ;
global_let      = "let" ident "=" expr ";" ;
function_decl   = "fn" ident "(" [ param { "," param } ] ")" [ "->" return_type ] ( block | ";" ) ;
param           = ident ":" param_type [ param_suffix ] ;
param_type      = "i32" | "bool" | "BOOL" | "NSInteger" | "NSUInteger" | "id" ;
param_suffix    = "<" ident { "<" | ">" | ident } ">" [ "?" | "!" ] | "?" | "!" ;
type            = "i32" | "bool" | "BOOL" | "NSInteger" | "NSUInteger" | "void" | "id" ;
return_type     = type [ return_suffix ] ;
return_suffix   = "<" ident { "<" | ">" | ident } ">" [ "?" | "!" ] | "?" | "!" ;

block           = "{" { stmt } "}" ;
stmt            = let_stmt | assign_stmt | return_stmt | if_stmt | do_while_stmt | for_stmt | switch_stmt | while_stmt | break_stmt | continue_stmt | empty_stmt | block_stmt | expr_stmt ;
let_stmt        = "let" ident "=" expr ";" ;
assign_stmt     = ident assign_op expr ";" | ident update_op ";" | update_op ident ";" ;
assign_op       = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>=" ;
update_op       = "++" | "--" ;
return_stmt     = "return" [ expr ] ";" ;
if_stmt         = "if" "(" expr ")" block [ "else" block ] ;
do_while_stmt   = "do" block "while" "(" expr ")" ";" ;
for_stmt        = "for" "(" for_clause ";" [ expr ] ";" for_clause ")" block ;
for_clause      = "" | "let" ident "=" expr | ident assign_op expr | ident update_op | update_op ident | expr ;
switch_stmt     = "switch" "(" expr ")" "{" { switch_case } "}" ;
switch_case     = "case" ( number | bool_lit ) ":" { stmt } | "default" ":" { stmt } ;
while_stmt      = "while" "(" expr ")" block ;
break_stmt      = "break" ";" ;
continue_stmt   = "continue" ";" ;
empty_stmt      = ";" ;
block_stmt      = "{" { stmt } "}" ;
expr_stmt       = expr ";" ;

expr            = conditional ;
conditional     = logical_or [ "?" expr ":" conditional ] ;
logical_or      = logical_and { "||" logical_and } ;
logical_and     = bitwise_or { "&&" bitwise_or } ;
bitwise_or      = bitwise_xor { "|" bitwise_xor } ;
bitwise_xor     = bitwise_and { "^" bitwise_and } ;
bitwise_and     = equality { "&" equality } ;
equality        = relational { ("==" | "!=") relational } ;
relational      = shift { ("<" | "<=" | ">" | ">=") shift } ;
shift           = additive { ("<<" | ">>") additive } ;
additive        = multiplicative { ("+" | "-") multiplicative } ;
multiplicative  = unary { ("*" | "/" | "%") unary } ;
unary           = "!" unary | "+" unary | "-" unary | "~" unary | postfix ;
postfix         = primary | call ;
call            = ident "(" [ expr { "," expr } ] ")" ;
message_send    = "[" postfix ident [ ":" expr { ident ":" expr } ] "]" ;
primary         = number | bool_lit | ident | "(" expr ")" | message_send ;
bool_lit        = "true" | "false" ;
```

Lexical support:

- Identifiers: `[A-Za-z_][A-Za-z0-9_]*`
- Integers: decimal, binary (`0b`/`0B`), octal (`0o`/`0O`), and hex (`0x`/`0X`) with optional `_` separators between digits
- Comments: line (`// ...`) and non-nested block (`/* ... */`)
- Tokens: `(` `)` `[` `]` `{` `}` `,` `:` `;` `=` `+=` `-=` `*=` `/=` `%=` `&=` `|=` `^=` `<<=` `>>=` `++` `--` `==` `!` `!=` `<` `<=` `>` `>=` `<<` `>>` `&` `|` `^` `&&` `||` `~` `+` `-` `*` `/` `%`
  - `?` is tokenized for parameter-suffix parsing (`id?`) and deterministic diagnostics for unsupported non-`id` suffix usage.
- Keywords include `module`, `let`, `fn`, `return`, `if`, `else`, `do`, `for`, `switch`, `case`, `default`, `while`, `break`, `continue`, `i32`, `bool`, `BOOL`, `NSInteger`, `NSUInteger`, `void`, `id`, `true`, `false`.

## Language-version pragma prelude contract

Implemented lexer contract for `#pragma objc_language_version(...)`:

- Accepted prelude form:
  - `#pragma objc_language_version(3)`
- Enforced placement:
  - pragma directives are consumed in the file-scope prelude before declarations/tokens.
  - non-leading directives emit deterministic `O3L008`.
- Enforced multiplicity:
  - at most one language-version pragma is accepted as canonical prelude contract.
  - duplicates emit deterministic `O3L007`.
- Malformed directives emit deterministic `O3L005`.
- Unsupported version payloads emit deterministic `O3L006`.

Pipeline/manifest replay contract includes a `language_version_pragma_contract` packet with:

- `directive_count`
- `duplicate`
- `non_leading`
- `first_line`/`first_column`
- `last_line`/`last_column`

## Lexer token metadata contract (M137-E001)

- Canonical metadata type lives in `native/objc3c/src/token/objc3_token_contract.h` and is consumed by parser/AST boundaries.
- Stable token-kind surface for semantic suffix tracking:
  - `Objc3SemaTokenKind::PointerDeclarator`
  - `Objc3SemaTokenKind::NullabilitySuffix`
- Stable metadata row fields:
  - `kind`
  - `text`
  - `line`
  - `column`
- Parser integration contract:
  - `native/objc3c/src/parse/objc3_parser.cpp` emits metadata with `MakeObjc3SemaTokenMetadata(...)`.
- AST integration contract:
  - `native/objc3c/src/ast/objc3_ast.h` stores suffix token evidence in `std::vector<Objc3SemaTokenMetadata>` fields.

## Parser subsystem + AST builder scaffolding contract (M138-E001)

- Parser implementation remains in:
  - `native/objc3c/src/parse/objc3_parser.h`
  - `native/objc3c/src/parse/objc3_parser.cpp`
- Parser-to-AST-builder contract surface remains in:
  - `native/objc3c/src/parse/objc3_ast_builder_contract.h`
  - `native/objc3c/src/parse/objc3_ast_builder_contract.cpp`
- Pipeline boundary contract:
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` consumes `BuildObjc3AstFromTokens(tokens)`.
  - Pipeline must not include parser implementation headers directly (`parse/objc3_parser.h`) or inline parser internals.
- AST scaffolding contract types are anchored in `native/objc3c/src/ast/objc3_ast.h`:
  - `Expr`
  - `Stmt`
  - `FunctionDecl`
  - `Objc3Program`

## M224 frontend release-readiness compliance profile

Frontend/parser GA-readiness expectations (current enforced behavior):

- parser/AST entry contract remains deterministic through `BuildObjc3AstFromTokens(...)` boundary wiring.
- pragma prelude enforcement remains fail-closed (`O3L005`, `O3L006`, `O3L007`, `O3L008`) with manifest-visible pragma contract metadata.
- token/AST bridge remains contract-driven (`Objc3SemaTokenMetadata` evidence captured for parser-to-sema handoff).
- no direct parser implementation header leakage across pipeline boundary (`parse/objc3_parser.h` remains out of pipeline public surface).

Operator evidence sequence for frontend readiness:

1. run parser/AST extraction checks (`npm run test:objc3c:parser-ast-extraction`).
2. run parser extraction contract gate (`npm run test:objc3c:parser-extraction-ast-builder-contract`).
3. run M224 frontend docs contract test (`python -m pytest tests/tooling/test_objc3c_m224_frontend_release_contract.py -q`).

## M225 frontend roadmap-seeding packet

Post-1.0 backlog seeding for frontend/parser work should capture these deterministic frontend signals:

- pragma-prelude diagnostics distribution (`O3L005`/`O3L006`/`O3L007`/`O3L008`) from parser-boundary test runs.
- manifest packet stability for `frontend.language_version_pragma_contract` fields.
- parser/AST boundary invariants proving `BuildObjc3AstFromTokens(...)` stays the single pipeline ingress.
- token-to-sema bridge continuity via `Objc3SemaTokenMetadata` evidence surfaces.

Recommended seeding commands (frontend lane):

1. `npm run test:objc3c:parser-ast-extraction`
2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
3. `python -m pytest tests/tooling/test_objc3c_m225_frontend_roadmap_seed_contract.py -q`

## M221 frontend GA blocker burn-down packet

GA blocker closure for frontend/parser requires deterministic parser/AST boundary evidence and fail-closed pragma diagnostics.

- Required blocker-closure signals:
  - prelude pragma diagnostics remain stable for `O3L005`/`O3L006`/`O3L007`/`O3L008`.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - pipeline manifest continues exporting `frontend.language_version_pragma_contract`.
  - token-to-sema bridge metadata remains present through `Objc3SemaTokenMetadata`.
- Required proof commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m225_frontend_roadmap_seed_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m221_frontend_ga_blocker_contract.py -q`

## M220 frontend public-beta triage packet

Public-beta triage for frontend/parser uses deterministic intake signals and patch-loop replay evidence.

- Required beta triage signals:
  - pragma-prelude diagnostics remain stable for `O3L005`/`O3L006`/`O3L007`/`O3L008`.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` stays present and deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required triage commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m221_frontend_ga_blocker_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m220_frontend_public_beta_contract.py -q`

## M219 frontend cross-platform parity packet

Cross-platform frontend parity (Windows/Linux/macOS) is tracked via deterministic parser/AST signals and replay-stable diagnostics.

- Required parity signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable across platforms.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains present and deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required parity commands (run in order per platform):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m220_frontend_public_beta_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m219_frontend_cross_platform_contract.py -q`

