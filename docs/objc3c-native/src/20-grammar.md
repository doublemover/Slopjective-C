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

## M218 frontend RC provenance packet

Release-candidate automation for frontend/parser requires deterministic boundary evidence and provenance-ready replay markers.

- Required RC provenance signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required RC commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m219_frontend_cross_platform_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m218_frontend_rc_provenance_contract.py -q`

## M217 frontend differential parity packet

Frontend differential testing against baseline toolchains requires deterministic parser/AST evidence and replay-stable diagnostics.

- Required differential signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required differential commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m218_frontend_rc_provenance_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m217_frontend_differential_contract.py -q`

## M216 frontend conformance suite v1 packet

Frontend conformance suite v1 maps parser/AST behavior to deterministic spec-section evidence.

- Required conformance signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required conformance commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m217_frontend_differential_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m216_frontend_conformance_contract.py -q`

## M215 frontend SDK packaging packet

Frontend SDK/toolchain packaging for IDE workflows depends on deterministic parser/AST boundary evidence.

- Required SDK packaging signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required SDK packaging commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m216_frontend_conformance_contract.py -q`
4. `python -m pytest tests/tooling/test_objc3c_m215_frontend_sdk_packaging_contract.py -q`

## M142 frontend CLI and C API parity harness

For deterministic frontend parity between CLI and the embeddable C API surface, the frontend compile anchor must validate language/compatibility and normalize lowering contract inputs before pipeline execution.

Frontend packet map:

- `frontend parity packet 1.1 deterministic compile option prevalidation` -> `m142_frontend_cli_c_api_prevalidation_packet`

### 1.1 Deterministic compile option prevalidation packet

- Source frontend anchor markers: `ValidateSupportedLanguageVersion(options->language_version, language_version_error)`, `ValidateSupportedCompatibilityMode(options->compatibility_mode, compatibility_mode_error)`, and `SetUsageError(context, result, ...)`.
- Source lowering normalization markers: `Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);`, `TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)`, `frontend_options.lowering = normalized_lowering;`, and `CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options)`.
- Usage-error/fail-closed markers: `result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;`, `result->process_exit_code = 2;`, and `objc3c_frontend_set_error(context, lowering_error.c_str());`.
- Deterministic packet key: `m142_frontend_cli_c_api_prevalidation_packet`.

Recommended M142 frontend parity validation command:

- `python -m pytest tests/tooling/test_objc3c_m142_frontend_cli_c_api_parity_contract.py -q`

## M214 frontend daemonized compiler packet

Frontend daemon/watch mode requires deterministic parser/AST boundary evidence across incremental runs.

- Required daemonized signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required daemonized commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m215_frontend_sdk_packaging_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m214_frontend_daemonized_contract.py -q`

## M213 frontend debug-info fidelity packet

Frontend debug-info fidelity requires deterministic parser/AST boundary evidence tied to source-level stepping surfaces.

- Required debug-fidelity signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required debug-fidelity commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m214_frontend_daemonized_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m213_frontend_debug_fidelity_contract.py -q`

## M212 frontend code-action packet

Frontend code-action/refactor support requires deterministic parser/AST boundary evidence for safe rewrites.

- Required code-action signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required code-action commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m213_frontend_debug_fidelity_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m212_frontend_code_action_contract.py -q`

## M211 frontend LSP semantic packet

Frontend LSP semantic-token/navigation support requires deterministic parser/AST boundary evidence.

- Required LSP signals:
  - pragma-prelude diagnostics `O3L005`/`O3L006`/`O3L007`/`O3L008` remain stable.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(...)`.
  - manifest packet `frontend.language_version_pragma_contract` remains deterministic.
  - token bridge continuity remains visible via `Objc3SemaTokenMetadata`.
- Required LSP commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m212_frontend_code_action_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m211_frontend_lsp_contract.py -q`

## M210 frontend performance budgets and regression gates

Frontend performance-budget and regression gating requires deterministic lexer/parser throughput anchors, parser boundary stability, and manifest stage-count visibility.

- Required frontend regression-budget signals:
  - lexer ingress remains `std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);`.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(tokens)`.
  - parser diagnostics handoff remains `result.stage_diagnostics.parser = std::move(parse_result.diagnostics);`.
  - manifest pipeline stage counters remain emitted for `"lexer": {"diagnostics":...}` and `"parser": {"diagnostics":...}`.
  - pragma prelude contract remains exported via `frontend.language_version_pragma_contract`.
- Required frontend regression-gate commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m211_frontend_lsp_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m210_frontend_perf_regression_contract.py -q`

## M209 frontend profile-guided optimization hooks

Frontend profile-guided optimization (PGO) hook readiness uses deterministic lexer/parser profile surfaces that can seed cross-run optimization decisions.

- Required frontend PGO hook signals:
  - lexer hint counters remain deterministic: `result.migration_hints.legacy_yes_count`, `legacy_no_count`, `legacy_null_count`.
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(tokens)`.
  - parser diagnostics handoff remains `result.stage_diagnostics.parser = std::move(parse_result.diagnostics);`.
  - pragma contract counters remain exported: `directive_count`, `duplicate`, `non_leading`.
  - manifest frontend profile surface remains emitted with `"migration_hints":{"legacy_yes":...,"legacy_no":...,"legacy_null":...,"legacy_total":...}`.
- Required frontend PGO hook commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m210_frontend_perf_regression_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m209_frontend_pgo_contract.py -q`

## M208 frontend whole-module optimization controls

Frontend whole-module optimization controls require deterministic module-shape packet surfaces from parser ingress to manifest staging.

- Required frontend whole-module control signals:
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(tokens)`.
  - module AST extraction remains `const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);`.
  - module function set shaping remains deterministic via `manifest_functions.reserve(program.functions.size())`.
  - unique function identity set remains `std::unordered_set<std::string> manifest_function_names`.
  - manifest semantic surface remains emitted with `"declared_globals"`, `"declared_functions"`, `"resolved_global_symbols"`, and `"resolved_function_symbols"`.
- Required frontend whole-module commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m209_frontend_pgo_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m208_frontend_wmo_contract.py -q`

## M207 frontend dispatch-specific optimization passes

Frontend dispatch-specific optimization pass readiness requires deterministic message-send parse boundaries and stable dispatch-control surface export.

- Required frontend dispatch-optimization signals:
  - message-send parse entry remains `ParseMessageSendExpression()`.
  - message-send AST tagging remains `message->kind = Expr::Kind::MessageSend`.
  - postfix parser dispatch gate remains `if (Match(TokenKind::LBracket))`.
  - manifest lowering control export remains `"runtime_dispatch_symbol"`, `"runtime_dispatch_arg_slots"`, and `"selector_global_ordering"`.
  - frontend lowering knob export remains `"max_message_send_args"`.
- Required frontend dispatch-optimization commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m208_frontend_wmo_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m207_frontend_dispatch_optimizations_contract.py -q`

## M206 frontend canonical optimization pipeline stage-1

Frontend canonical optimization stage-1 relies on deterministic parser/module surfaces that feed stable optimization-control packets.

- Required frontend canonical optimization signals:
  - parser ingress remains exclusively `BuildObjc3AstFromTokens(tokens)`.
  - module set construction remains deterministic via `manifest_function_names.insert(fn.name).second`.
  - function signature surface remains emitted with `"function_signature_surface"`.
  - scalar signature counters remain surfaced as `"scalar_return_i32"`, `"scalar_return_bool"`, `"scalar_return_void"`, `"scalar_param_i32"`, and `"scalar_param_bool"`.
  - semantic surface counts remain emitted for `"declared_globals"`, `"declared_functions"`, `"resolved_global_symbols"`, and `"resolved_function_symbols"`.
- Required frontend canonical optimization commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m207_frontend_dispatch_optimizations_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m206_frontend_canonical_optimization_contract.py -q`

## M205 frontend macro security policy enforcement

Frontend macro-security policy enforcement relies on deterministic prelude-directive parsing and fail-closed diagnostics for malformed, unsupported, duplicate, or non-leading directives.

- Required frontend macro-security signals:
  - prelude directive ingest remains `ConsumeLanguageVersionPragmas(diagnostics)`.
  - directive parser entry remains `ConsumeLanguageVersionPragmaDirective(...)`.
  - directive placement enforcement remains `LanguageVersionPragmaPlacement::kNonLeading`.
  - fail-closed diagnostics remain `O3L005`, `O3L006`, `O3L007`, and `O3L008`.
  - manifest policy packet remains `frontend.language_version_pragma_contract` with `directive_count`, `duplicate`, and `non_leading`.
- Required frontend macro-security commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m206_frontend_canonical_optimization_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m205_frontend_macro_security_contract.py -q`

## M204 frontend macro diagnostics and provenance

Frontend macro diagnostics/provenance requires deterministic directive source-location capture and stable diagnostic formatting for replay.

- Required frontend macro-diagnostics signals:
  - deterministic diagnostic formatter remains `MakeDiag(...)` with `error:<line>:<column>: <message> [<code>]`.
  - directive provenance capture remains `first_line`, `first_column`, `last_line`, and `last_column`.
  - pipeline transport preserves directive provenance through `result.language_version_pragma_contract.*`.
  - manifest provenance packet remains `frontend.language_version_pragma_contract`.
  - fail-closed directive diagnostics remain `O3L005`, `O3L006`, `O3L007`, and `O3L008`.
- Required frontend macro-diagnostics commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m205_frontend_macro_security_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m204_frontend_macro_diagnostics_contract.py -q`


## M202 frontend derive/synthesis pipeline

Frontend derive/synthesis pipeline contract relies on deterministic semantic-surface synthesis from parser AST into sorted metadata handoff packets.

- Required frontend derive/synthesis signals:
  - synthesis ingress remains `BuildSemanticIntegrationSurface(...)`.
  - metadata handoff synthesis remains `BuildSemanticTypeMetadataHandoff(...)`.
  - deterministic ordering guard remains `IsDeterministicSemanticTypeMetadataHandoff(...)`.
  - synthesized global ordering packet remains `global_names_lexicographic`.
  - synthesized function packet remains `functions_lexicographic` with arity/param consistency checks.
- Required frontend derive/synthesis commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m203_frontend_compile_time_eval_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m202_frontend_derive_synthesis_contract.py -q`

## M201 frontend macro expansion architecture and isolation

Frontend macro-expansion architecture/isolation contract relies on deterministic literal-hint capture, strict prelude pragma boundaries, and isolated transport into sema passes.

- Required frontend macro-expansion/isolation signals:
  - lexer literal-hint capture remains `migration_hints_.legacy_yes_count`, `legacy_no_count`, and `legacy_null_count`.
  - prelude isolation contract remains `language_version_pragma_contract_` with prelude-only enforcement.
  - pipeline transport isolation remains `result.migration_hints.*` and `result.language_version_pragma_contract.*`.
  - sema ingestion remains isolated through `sema_input.migration_hints.*`.
  - literal-classification bridge remains `append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true")` and peers.
- Required frontend macro-expansion/isolation commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m202_frontend_derive_synthesis_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m201_frontend_macro_expansion_contract.py -q`

## M200 frontend interop integration suite and packaging

Frontend interop integration suite/packaging contract relies on deterministic message-send parse boundaries, stable selector/argument AST surfaces, and parser-to-pipeline handoff continuity.

- Required frontend interop integration/packaging signals:
  - message-send parse ingress remains `ParseMessageSendExpression()`.
  - interop call-form AST surface remains `Expr::Kind::MessageSend` with `selector` and `args`.
  - parser-to-frontend handoff remains `BuildObjc3AstFromTokens(tokens)` and `result.program = std::move(parse_result.program);`.
  - lowering-facing boundary guard remains `max_message_send_args = options.lowering.max_message_send_args`.
- Required frontend interop integration/packaging commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m201_frontend_macro_expansion_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m200_frontend_interop_packaging_contract.py -q`

## M199 frontend foreign type import diagnostics

Frontend foreign-type import diagnostics contract relies on deterministic parser-level unsupported-type boundaries, explicit imported-type allowlists, and stable type-spelling carrier fields into AST/sema.

- Required frontend foreign-type diagnostics signals:
  - unsupported parameter-type import diagnostics remain parser fail-closed `O3P108`.
  - unsupported return-type import diagnostics remain parser fail-closed `O3P114`.
  - imported-type allowlists remain explicit: `'id', 'Class', 'SEL', 'Protocol', or 'instancetype'`.
  - parser spelling carriers remain `param.id_spelling`, `param.class_spelling`, `param.instancetype_spelling`, `fn.return_id_spelling`, `fn.return_class_spelling`, and `fn.return_instancetype_spelling`.
  - AST carrier continuity remains `bool id_spelling = false;`, `bool class_spelling = false;`, `bool instancetype_spelling = false;`, and return equivalents.
- Required frontend foreign-type diagnostics commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m200_frontend_interop_packaging_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m199_frontend_foreign_type_diagnostics_contract.py -q`

## M198 frontend swift metadata bridge

Frontend Swift metadata-bridge contract relies on deterministic parser token-metadata capture, explicit AST carrier fields, and stable frontend-to-sema transport boundaries.

- Required frontend Swift metadata-bridge signals:
  - parser token metadata remains `MakeSemaTokenMetadata(...)`.
  - pointer/nullability metadata kinds remain `Objc3SemaTokenKind::PointerDeclarator` and `Objc3SemaTokenKind::NullabilitySuffix`.
  - AST metadata carriers remain `std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens`, `std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens`, `std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens`, and `std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens`.
  - frontend handoff bridge remains `BuildObjc3AstFromTokens(tokens)` and `sema_input.program = &result.program;`.
  - metadata bridge parity anchors remain `result.type_metadata_handoff` and `result.deterministic_type_metadata_handoff`.
- Required frontend Swift metadata-bridge commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m199_frontend_foreign_type_diagnostics_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m198_frontend_swift_metadata_bridge_contract.py -q`

## M197 frontend C++ interop shim strategy

Frontend C++ interop-shim contract relies on deterministic message-send parse boundaries, stable selector/argument AST packetization, and explicit parser-to-sema transport surfaces.

- Required frontend C++ interop-shim signals:
  - interop parse ingress remains `ParseMessageSendExpression()`.
  - call-form AST surfaces remain `Expr::Kind::MessageSend` with `selector` and `args`.
  - parser/AST bridge remains `BuildObjc3AstFromTokens(tokens)` and `result.program = std::move(parse_result.program);`.
  - sema ingress remains `sema_input.program = &result.program;`.
  - lowering-facing shim budget remains `max_message_send_args = options.lowering.max_message_send_args`.
- Required frontend C++ interop-shim commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m198_frontend_swift_metadata_bridge_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m197_frontend_cpp_interop_shim_contract.py -q`

## M196 frontend C interop headers and ABI alignment

Frontend C-interop header/ABI-alignment contract relies on deterministic C ABI wrapper declarations, stable compile-option/result alias surfaces, and explicit frontend-option normalization boundaries.

- Required frontend C-interop header/ABI-alignment signals:
  - C wrapper header anchor remains `#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u`.
  - C wrapper alias surfaces remain `typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;` and `typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;`.
  - wrapper ABI/layout guards remain `static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,` and companion compile-result/context checks.
  - frontend option normalization remains `BuildFrontendOptions(const objc3c_frontend_compile_options_t &options)` plus `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`.
  - pipeline ABI defaults remain `kRuntimeDispatchDefaultArgs = 4`, `kRuntimeDispatchMaxArgs = 16`, and `kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32"`.
- Required frontend C-interop header/ABI-alignment commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m197_frontend_cpp_interop_shim_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m196_frontend_c_interop_headers_abi_contract.py -q`

## M195 frontend system-extension conformance and policy

Frontend system-extension conformance/policy contract relies on deterministic compile-option validation, explicit fail-closed policy boundaries, and stable runtime-dispatch normalization guards.

- Required frontend system-extension conformance/policy signals:
  - language-policy guard remains `ValidateSupportedLanguageVersion(...)`.
  - compatibility-policy guard remains `ValidateSupportedCompatibilityMode(...)`.
  - fail-closed lowering-policy normalization remains `TryNormalizeObjc3LoweringContract(...)`.
  - policy defaults remain `kRuntimeDispatchDefaultArgs = 4`, `kRuntimeDispatchMaxArgs = 16`, and `kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32"`.
  - compile-option transport remains `frontend_options.lowering.max_message_send_args = options.max_message_send_args;` and `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`.
- Required frontend system-extension conformance/policy commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m196_frontend_c_interop_headers_abi_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py -q`

## M194 frontend atomics and memory-order mapping

Frontend atomics and memory-order mapping contract relies on deterministic compound-assignment token capture, replay-stable opcode lowering mapping, and fail-closed pipeline behavior.

- Required frontend atomics/memory-order signals:
  - assignment-token classification remains `IsAssignmentOperatorToken(TokenKind kind)`.
  - assignment-token decode remains `MatchAssignmentOperator(std::string &op)`.
  - bitwise/shift assignment forms remain explicit parser anchors:
    `if (Match(TokenKind::AmpersandEqual)) {`,
    `if (Match(TokenKind::PipeEqual)) {`,
    `if (Match(TokenKind::CaretEqual)) {`,
    `if (Match(TokenKind::LessLessEqual)) {`,
    and `if (Match(TokenKind::GreaterGreaterEqual)) {`.
  - compound assignment lowering mapping remains `TryGetCompoundAssignmentBinaryOpcode(...)` for `and`/`or`/`xor`/`shl`/`ashr`.
  - fail-closed pipeline posture remains `NoThrowFailClosed`.
  - lowering defaults remain `kRuntimeDispatchDefaultArgs = 4` and `kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32"`.
- Required frontend atomics/memory-order commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m194_frontend_atomics_memory_order_contract.py -q`

## M193 frontend SIMD/vector type lowering

Frontend SIMD/vector type lowering contract relies on deterministic parser acceptance for vector-friendly type spellings and stable AST metadata transport for vector base/lane shape.

- Required frontend SIMD/vector signals:
  - vector spelling parser entry remains `TryParseVectorTypeSpelling(...)`.
  - vector return metadata transport remains `fn.return_vector_spelling = true;`, `fn.return_vector_base_spelling = vector_base_spelling;`, and `fn.return_vector_lane_count = vector_lane_count;`.
  - vector parameter metadata transport remains `param.vector_spelling = true;`, `param.vector_base_spelling = vector_base_spelling;`, and `param.vector_lane_count = vector_lane_count;`.
  - AST vector metadata remains `bool vector_spelling = false;` + `unsigned vector_lane_count = 1;` on `FuncParam` and `bool return_vector_spelling = false;` + `unsigned return_vector_lane_count = 1;` on `FunctionDecl`.
  - accepted vector spellings remain `i32x2/i32x4/i32x8/i32x16` and `boolx2/boolx4/boolx8/boolx16`.
- Required frontend SIMD/vector commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m194_frontend_atomics_memory_order_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m193_frontend_simd_vector_lowering_contract.py -q`

## M203 frontend compile-time evaluation engine

Frontend compile-time evaluation engine contract relies on deterministic constant-expression folding surfaces and stable parser-to-sema value-provenance transport.

- Required frontend compile-time-eval signals:
  - constant-expression evaluator entry remains `EvalConstExpr(...)`.
  - global initializer folding surface remains `ResolveGlobalInitializerValues(...)`.
  - non-constant global initializer diagnostics remain fail-closed as `O3S210`.
  - artifact-level fail-closed lowering diagnostic remains `O3L300` for const-evaluation failure.
  - manifest semantic surface remains deterministic for evaluated globals/functions packets.
- Required frontend compile-time-eval commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m204_frontend_macro_diagnostics_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m203_frontend_compile_time_eval_contract.py -q`

