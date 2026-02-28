# Objc3c Native Frontend (Current Contract)

This document captures the currently implemented behavior for the native `objc3c` frontend (`native/objc3c/src/main.cpp`) and its command wrappers.

## Supported inputs

- `.objc3` files: custom lexer/parser + semantic checks + LLVM IR emission + object build.
- Any other extension (for example `.m`): libclang parse/diagnostics + symbol manifest + Objective-C object build.

## CLI usage

```text
objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [-fobjc-version=<N>] [--objc3-language-version <N>] [--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist] [--objc3-ir-object-backend <clang|llvm-direct>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>]
```

- Default `--out-dir`: `tmp/artifacts/compilation/objc3c-native`
- Default `--emit-prefix`: `module`
- Default `--clang`: `clang` (or explicit path)
- Default `--llc`: `llc` (or explicit path)
- Default language version: `3` (`-fobjc-version=<N>` or `--objc3-language-version <N>`)
- Default `--objc3-compat-mode`: `canonical`
- Default `--objc3-migration-assist`: `off`
- Default `--objc3-ir-object-backend`: `llvm-direct`
- Default `--objc3-max-message-args`: `4`
- Default `--objc3-runtime-dispatch-symbol`: `objc3_msgsend_i32`
- Native frontend language-version support is currently fail-closed to Objective-C `3` only; other version values return usage error exit `2`.

## C API parity runner usage (M142-E001)

```text
objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [--summary-out <path>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>] [--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist] [--objc3-ir-object-backend <clang|llvm-direct>] [--no-emit-manifest] [--no-emit-ir] [--no-emit-object]
```

- Binary path produced by native build scripts: `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- Default `--clang`: `clang` (or explicit path)
- Default `--llc`: `llc` (or explicit path)
- Default `--objc3-compat-mode`: `canonical`
- Default `--objc3-migration-assist`: `off`
- Default `--objc3-ir-object-backend`: `clang`
- Default summary output when `--summary-out` is omitted: `<out-dir>/<emit-prefix>.c_api_summary.json`
- Runner summary captures deterministic compatibility controls (`ir_object_backend`, `compatibility_mode`, `migration_assist`) and output paths (`diagnostics`, `manifest`, `ir`, `object`) for parity replay.
- For CLI/C API parity harness runs, use CLI backend override `--objc3-ir-object-backend clang` so both paths produce objects through the same compile backend.

## Artifact tmp-path governance (M143-D001)

- Source-mode parity workflows are tmp-governed by default:
  - `--work-dir` defaults to `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work`.
  - Derived outputs are rooted under `<work-dir>/<work_key>/{library,cli}`.
- `--work-key` is deterministic by default (derived from source + emit controls) and can be pinned explicitly for reproducible path contracts.
- Non-tmp source-mode work directories are rejected unless `--allow-non-tmp-work-dir` is set.

## LLVM capability discovery and backend routing (M144-E001)

- `--llvm-capabilities-summary <path>` points CLI routing to a deterministic capability summary packet.
- `--objc3-route-backend-from-capabilities` derives `--objc3-ir-object-backend` from summary capabilities (uses `llvm-direct` only when `llc --filetype=obj` is available).
- Capability probes are captured with `npm run check:objc3c:llvm-capabilities` into `tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json`.

## Driver shell split boundaries (M136-E001)

- Driver source wiring order is deterministic:
  - `src/driver/objc3_cli_options.cpp`
  - `src/driver/objc3_driver_shell.cpp`
  - `src/driver/objc3_frontend_options.cpp`
  - `src/driver/objc3_objc3_path.cpp`
  - `src/driver/objc3_objectivec_path.cpp`
  - `src/driver/objc3_compilation_driver.cpp`
- `objc3_driver_shell` owns shell-only responsibilities:
  - classify input kind (`.objc3` vs non-`.objc3`),
  - validate required tool paths (`clang` / `llc`) and input file presence.
- `objc3_frontend_options` owns CLI-to-frontend option mapping for `.objc3` lowering controls.
- `objc3_objc3_path` defines extracted `.objc3` path execution helpers during shell split rollout.
- `objc3_compilation_driver` owns top-level shell dispatch/orchestration and routes non-`.objc3` inputs to Objective-C path execution.
- `objc3_objectivec_path` owns Objective-C translation-unit parse, diagnostics normalization, symbol-manifest emission, and object compilation.
- CLI split boundaries remain stable; compatibility controls (`-fobjc-version`, `--objc3-language-version`,
  `--objc3-compat-mode`, `--objc3-migration-assist`) are parsed at the driver shell layer and validated fail-closed.

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

## M27 loop/control surface (`while`, `break`, `continue`)

Grammar status (implemented):

- `while (<expr>) { ... }` parses as a loop statement.
- `break;` and `continue;` parse as dedicated loop-control statements.
- Loop blocks are ordinary statement blocks and support nesting with lexical scopes.

Semantic status (implemented):

- `while` condition expressions must be bool-compatible (`bool`/`i32`) and reuse deterministic type-mismatch diagnostics (`O3S206`) if incompatible.
- `break` is legal only in an active loop context; misuse emits `O3S212`.
- `continue` is legal only in an active loop context; misuse emits `O3S213`.

Lowering/runtime status (implemented):

- `while` lowers to explicit LLVM CFG labels (`while_cond_*`, `while_body_*`, `while_end_*`).
- `break` lowers as an unconditional branch to loop end label.
- `continue` lowers as an unconditional branch to loop condition label.
- Nested loop control flow is tracked with a deterministic loop-label stack in lowering context.

## M29 do-while surface (`do { ... } while (...)`)

Grammar/semantics status (implemented):

- `do { ... } while (expr);` parses as a post-test loop statement.
- `do-while` condition expressions must be bool-compatible (`bool`/`i32`) and reuse deterministic type-mismatch diagnostics (`O3S206`) if incompatible.
- `break`/`continue` inside do-while bodies follow active-loop legality rules (`O3S212`, `O3S213`).

Lowering/runtime status (implemented):

- `do-while` lowers to explicit LLVM CFG labels (`do_body_*`, `do_cond_*`, `do_end_*`).
- `continue` targets the `do_cond_*` label and `break` targets `do_end_*`.

## M30 for-loop surface (`for (init; condition; step) { ... }`)

Grammar/semantics status (implemented):

- `for (init; condition; step) { ... }` parses with optional clauses:
  - `init`: empty, `let`, assignment, or expression.
  - `condition`: empty (`true` loop) or expression.
  - `step`: empty, assignment, or expression.
- `for` condition expressions (when present) must be bool-compatible (`bool`/`i32`) and reuse deterministic type-mismatch diagnostics (`O3S206`) if incompatible.
- Loop-scoped `let` declarations in the init clause are tracked in deterministic lexical scope.
- `break`/`continue` inside `for` bodies follow active-loop legality rules (`O3S212`, `O3S213`).

Lowering/runtime status (implemented):

- `for` lowers to explicit LLVM CFG labels (`for_cond_*`, `for_body_*`, `for_step_*`, `for_end_*`).
- `continue` targets the `for_step_*` label and `break` targets `for_end_*`.

## M31 switch/case/default surface (`switch (expr) { case ... default ... }`)

Grammar/semantics status (implemented):

- `switch (expr) { ... }` parses with `case <number|bool>:` and optional `default:` labels.
- `switch` condition expressions must be `i32`-compatible (`i32`/`bool`) and emit deterministic diagnostics (`O3S206`) when incompatible.
- Duplicate `case` values and duplicate `default` labels are rejected with deterministic semantic diagnostics (`O3S206`).
- `break` is legal in both loops and switches; `continue` still requires an active loop.

Lowering/runtime status (implemented):

- `switch` lowers to deterministic compare-and-branch chains with labels in the `switch_test_*`, `switch_case_*`, `switch_default_*`, `switch_end_*` families.
- `break` in switch arms targets `switch_end_*`.
- `continue` in a switch nested inside a loop resolves to the nearest loop continue label (for-loop: `for_step_*`, while/do-while: loop condition labels).

## M32 conditional-expression surface (`cond ? when_true : when_false`)

Grammar/semantics status (implemented):

- Right-associative conditional expressions are parsed in expression position: `cond ? when_true : when_false`.
- Condition expressions must be bool-compatible (`bool`/`i32`) and emit deterministic diagnostics (`O3S206`) when incompatible.
- Branch expressions must be scalar-compatible; incompatible branch families emit deterministic diagnostics (`O3S206`).

Lowering/runtime status (implemented):

- Conditional expressions lower to explicit compare/branch/merge label families (`cond_true_*`, `cond_false_*`, `cond_merge_*`).
- Lowering uses deterministic branch-local stores and merge loads for stable nested-conditional behavior.

## M33 compound-assignment surface (`+=`, `-=`, `*=`, `/=`)

Grammar/semantics status (implemented):

- Assignment statements support `ident += expr;`, `ident -= expr;`, `ident *= expr;`, and `ident /= expr;` in addition to `ident = expr;`.
- `for` init/step assignment clauses support the same assignment operator family (`=`, `+=`, `-=`, `*=`, `/=`).
- Compound-assignment targets must resolve to mutable local or global `i32` symbols; invalid targets continue to emit `O3S214`.
- Compound-assignment RHS expressions must be `i32`; target/RHS type drift emits deterministic `O3S206`.

Lowering/runtime status (implemented):

- `=` lowers as direct RHS store.
- `+=`, `-=`, `*=`, `/=` lower as deterministic load-op-store sequences (`add`, `sub`, `mul`, `sdiv`) against the target local slot.
- Compound assignments are supported in both statement position and `for` init/step lowering paths.

## M34 increment/decrement surface (`++`, `--`)

Grammar/semantics status (implemented):

- Update statements support both postfix and prefix forms on identifier targets: `ident++;`, `ident--;`, `++ident;`, `--ident;`.
- `for` init/step clauses support the same postfix/prefix update forms.
- Update targets must resolve to mutable local or global `i32` symbols; invalid targets continue to emit `O3S214`.
- Update target type drift emits deterministic `O3S206`.

Lowering/runtime status (implemented):

- `++` lowers as deterministic `load -> add 1 -> store`.
- `--` lowers as deterministic `load -> sub 1 -> store`.
- Update lowering is shared between statement position and `for` init/step clause lowering.

## M35 bitwise/shift surface (`&`, `|`, `^`, `<<`, `>>`, `~`)

Grammar/semantics status (implemented):

- Expressions support binary bitwise operators (`&`, `|`, `^`) and shift operators (`<<`, `>>`) with deterministic precedence tiers between equality/relational and logical operators.
- Unary bitwise-not (`~`) is parsed in unary position.
- Bitwise/shift operands must be `i32`; operand type drift emits deterministic `O3S206`.

Lowering/runtime status (implemented):

- Bitwise operators lower to deterministic LLVM integer ops (`and`, `or`, `xor`).
- Shift operators lower to deterministic LLVM shift ops (`shl`, `ashr`).
- Unary `~` lowers through deterministic xor-with-`-1` expression lowering.

## M36 modulo/remainder-assignment surface (`%`, `%=`)

Grammar/semantics status (implemented):

- `%` is supported in multiplicative expression position.
- `%=` is supported in assignment statements and `for` init/step assignment clauses.
- Modulo/remainder operands and `%=` targets/RHS values must be `i32`; type drift emits deterministic `O3S206`.

Lowering/runtime status (implemented):

- `%` lowers to deterministic LLVM signed remainder (`srem`).
- `%=` lowers through deterministic load-`srem`-store assignment lowering.

## M37 bitwise compound-assignment surface (`&=`, `|=`, `^=`, `<<=`, `>>=`)

Grammar/semantics status (implemented):

- Assignment statements support `ident &= expr;`, `ident |= expr;`, `ident ^= expr;`, `ident <<= expr;`, and `ident >>= expr;`.
- `for` init/step assignment clauses support the same bitwise compound-assignment family.
- Bitwise compound-assignment targets and RHS values must be `i32`; type drift emits deterministic `O3S206`.
- Invalid or unresolved assignment targets continue to emit deterministic `O3S214`.

Lowering/runtime status (implemented):

- `&=` lowers through deterministic load-`and`-store.
- `|=` lowers through deterministic load-`or`-store.
- `^=` lowers through deterministic load-`xor`-store.
- `<<=` lowers through deterministic load-`shl`-store.
- `>>=` lowers through deterministic load-`ashr`-store.

## M38 global assignment-target support (local+global mutable symbols)

Grammar/semantics status (implemented):

- Assignment/update/compound-assignment target resolution now checks lexical locals first, then module globals.
- `for` init/step assignment clauses use the same local-first, global-fallback target lookup order.
- Unresolved targets continue to emit deterministic `O3S214`.
- Assignment/update type checks remain unchanged (`O3S206`) and apply equally to global targets.

Lowering/runtime status (implemented):

- Assignment/update/compound-assignment stores lower to local allocas when a local target exists.
- If a local target is not found, lowering emits deterministic loads/stores against the global symbol (for example `@counter`).
- Local shadowing of globals is preserved by local-first pointer lookup.

## M39 global const-initializer symbol references (declaration-order resolution)

Grammar/semantics status (implemented):

- Global initializer constant expressions may reference previously declared globals by identifier.
- Resolution is declaration-order deterministic; forward references and self references are rejected.
- Invalid global const-reference initializer expressions emit deterministic `O3S210`.

Lowering/runtime status (implemented):

- Resolved global initializer values are folded before IR emission.
- `.ll` global declarations emit resolved scalar values in declaration order.
- Manifest `globals[].value` fields are emitted from the same resolved constant table used by lowering.

## M40 nil literal surface support (`nil`)

Grammar/semantics status (implemented):

- `nil` is recognized as a primary expression literal in `.objc3`.
- `nil` lowers/behaves as deterministic scalar zero (`i32 0`) in expression contexts.
- Existing scalar compatibility rules continue to apply when `nil` participates in arithmetic, comparison, assignment, and return expressions.

Lowering/runtime status (implemented):

- `nil` emits as immediate `0` in native lowering paths.
- Global initializers may use `nil` where constant expressions are allowed.

## M41 Objective-C literal aliases (`YES`, `NO`, `NULL`)

Grammar/semantics status (implemented):

- `YES` and `NO` are accepted as Objective-C boolean literal aliases.
- `NULL` is accepted as Objective-C null literal alias and behaves as scalar zero.
- Alias literals reuse existing scalar compatibility/type-checking paths used by `true`/`false`/`nil`.

Lowering/runtime status (implemented):

- `YES`/`NO` lower through the same deterministic bool-literal widening path.
- `NULL` lowers as immediate `0` (equivalent to `nil` in this scalar surface).
- Global initializer const-eval accepts alias literals through the same deterministic folding pipeline.

## M42 hex integer literal surface (`0x...`)

Grammar/semantics status (implemented):

- Primary expressions accept hexadecimal integer literals with `0x`/`0X` prefix.
- Hex literals participate in existing scalar typing/coercion behavior for arithmetic, comparison, assignment, and return expressions.
- Invalid numeric literal forms emit deterministic parser diagnostics (`O3P103`).

Lowering/runtime status (implemented):

- Hex literals lower as their deterministic folded scalar `i32` values.
- Global initializer constant-eval accepts hex literals and mixed literal/global expressions.
- Existing constant-eval rejection behavior for invalid ops (for example divide-by-zero) is preserved (`O3S210`).

## M43 binary integer literal surface (`0b...`)

Grammar/semantics status (implemented):

- Primary expressions accept binary integer literals with `0b`/`0B` prefix.
- Binary literals participate in existing scalar typing/coercion behavior for arithmetic, comparison, assignment, and return expressions.
- Invalid numeric literal forms emit deterministic parser diagnostics (`O3P103`).

Lowering/runtime status (implemented):

- Binary literals lower as deterministic folded scalar `i32` values.
- Global initializer constant-eval accepts binary literals and mixed literal/global expressions.
- Existing constant-eval rejection behavior for invalid ops (for example divide-by-zero) is preserved (`O3S210`).

## M44 octal integer literal surface (`0o...`)

Grammar/semantics status (implemented):

- Primary expressions accept octal integer literals with `0o`/`0O` prefix.
- Octal literals participate in existing scalar typing/coercion behavior for arithmetic, comparison, assignment, and return expressions.
- Invalid numeric literal forms emit deterministic parser diagnostics (`O3P103`).

Lowering/runtime status (implemented):

- Octal literals lower as deterministic folded scalar `i32` values.
- Global initializer constant-eval accepts octal literals and mixed literal/global expressions.
- Existing constant-eval rejection behavior for invalid ops (for example divide-by-zero) is preserved (`O3S210`).

## M45 integer digit separator surface (`_`)

Grammar/semantics status (implemented):

- Integer literals accept `_` separators between digits for decimal, binary, octal, and hex forms.
- Separator placement is validated deterministically (no leading, trailing, or consecutive separators).
- Malformed separator usage emits deterministic parser diagnostics (`O3P103`).

Lowering/runtime status (implemented):

- Separator literals normalize to their canonical digit sequence before parse/lowering.
- Lowering and const-eval treat separator literals identically to equivalent non-separated literals.
- Existing constant-eval rejection behavior for invalid ops (for example divide-by-zero) is preserved (`O3S210`).

## M46 block comment lexical surface (`/* ... */`)

Grammar/semantics status (implemented):

- Lexer trivia handling accepts C-style block comments (`/* ... */`) in addition to line comments.
- Block comments are elided from the token stream and do not affect semantic interpretation of surrounding code.
- Unterminated block comments emit deterministic lexical diagnostics (`O3L002`).

Lowering/runtime status (implemented):

- Lowering/runtime behavior is unchanged for valid programs; comment-elided and comment-free forms compile equivalently.
- Execution smoke coverage includes block-commented positive fixtures and unterminated-comment negative fixtures.

## M47 duplicate module declaration diagnostics

Grammar/semantics status (implemented):

- At most one `module <ident>;` declaration is accepted per translation unit.
- A second module declaration emits deterministic semantic diagnostics (`O3S200`) and does not replace the primary module identity.

Lowering/runtime status (implemented):

- Manifest and IR source filename continue to use the first valid module declaration.
- Duplicate module declarations fail before successful lowering artifacts are produced.

## M48 unary plus expression surface (`+expr`)

Grammar/semantics status (implemented):

- Unary plus is accepted in expression position (`+expr`) with recursive nesting (`+(+value)`).
- Unary plus shares arithmetic operand typing rules with unary minus; non-`i32` operands emit deterministic semantic diagnostics (`O3S206`).
- Missing unary-plus operands emit deterministic parser diagnostics (`O3P103`).

Lowering/runtime status (implemented):

- Unary plus lowers as deterministic arithmetic identity (`0 + expr`) in IR emission.
- Global constant initializer evaluation accepts unary plus in constant-expression chains.
- Execution/replay coverage includes unary-plus positive programs and deterministic compile-fail negatives.

## M49 nested block comment lexical guard

Grammar/semantics status (implemented):

- Block comments remain supported (`/* ... */`) but nested block comment openers are rejected.
- Encountering a nested block comment opener (`/*` inside an active block comment) emits deterministic lexical diagnostics (`O3L003`).
- Unterminated block comments continue to emit deterministic lexical diagnostics (`O3L002`).

Lowering/runtime status (implemented):

- Nested block comment lexical failures fail closed before parser/semantic/lowering stages.
- Recovery/execution regression suites include deterministic compile-fail fixtures for nested block comments.

## M50 stray block comment terminator lexical guard

Grammar/semantics status (implemented):

- Stray block comment terminators (`*/` outside an active block comment) are rejected by the lexer.
- Stray terminators emit deterministic lexical diagnostics (`O3L004`).
- Stray terminator lexical failures are emitted independently of parser-stage recovery diagnostics.

Lowering/runtime status (implemented):

- Stray terminator lexical failures fail closed before parser/semantic/lowering stages.
- Recovery/execution regression suites include deterministic compile-fail fixtures for stray terminators.

## M51 single-statement control-flow bodies

Grammar/semantics status (implemented):

- `if`/`else` branches accept either braced blocks or single statements.
- `else if` chains are accepted via single-statement `else` branches.
- `while`, `for`, and `do ... while` bodies accept either braced blocks or single statements.

Lowering/runtime status (implemented):

- Single-statement and braced control-flow bodies normalize to the same statement-list lowering path.
- Recovery/execution regression suites include deterministic fixtures for single-statement control-flow parsing and execution.

## M52 empty-statement control-flow support

Grammar/semantics status (implemented):

- Standalone empty statements (`;`) are accepted as no-op statements.
- Empty statements are accepted as single-body control-flow bodies (`if (...) ;`, `while (...) ;`, `for (...) ;`, `do ; while (...)`).
- Malformed control-flow with missing required body statements continues to emit deterministic parser diagnostics (`O3P103`).

Lowering/runtime status (implemented):

- Empty statements lower as no-op nodes with no semantic side effects.
- Recovery/execution regression suites include deterministic compile and execution fixtures for empty-statement control-flow paths.

## M53 nested block statement support

Grammar/semantics status (implemented):

- Standalone nested block statements (`{ ... }`) are accepted in statement position.
- Nested block statements introduce lexical sub-scopes for declaration/shadowing behavior.
- Malformed nested blocks with missing closing braces emit deterministic parser diagnostics (`O3P111`).

Lowering/runtime status (implemented):

- Nested block statement nodes lower through the native statement pipeline with lexical scope push/pop.
- Recovery/execution regression suites include deterministic fixtures for nested block statement scope and parse failures.

## M54 void return type and bare return support

Grammar/semantics status (implemented):

- Function return annotations accept `void` in addition to `i32`/`bool`.
- Return statements accept both `return expr;` and bare `return;`.
- `void` functions allow implicit fallthrough (no mandatory terminal return statement).
- Non-void functions reject bare `return;` with deterministic semantic diagnostics (`O3S211`).
- Void functions reject `return expr;` with deterministic semantic diagnostics (`O3S211`).

Lowering/runtime status (implemented):

- `void` functions lower as `define void @name(...)` with `ret void` fallthrough when not explicitly terminated.
- Bare `return;` in `void` functions lowers to `ret void`.
- Calls to `void` functions are emitted as `call void @name(...)` in statement position.
- `objc3c_entry` accepts `main` returning `void` and returns process exit code `0` in that case.

## M55 Objective-C `id` type annotation alias support

Grammar/semantics status (implemented):

- Function parameter annotations accept `id`.
- Function return annotations accept `id`.
- `id` annotations are treated as native scalar aliases in semantic validation paths.

Lowering/runtime status (implemented):

- `id`-annotated parameters/returns lower through the existing scalar ABI path.
- LLVM IR type emission remains deterministic (`i32`/`i1`/`void`) with `id` aliasing to scalar integer ABI lowering.

## M82 `id` parameter suffix support (`id<...>`, `id?`, `id!`)

Grammar/semantics status (implemented):

- Parameter annotations accept Objective-C style `id` suffix forms:
  - protocol-qualifier suffixes (`id<NSObject>`),
  - nullability suffixes (`id?`, `id!`).
- `id` suffixes are treated as annotation-only forms over the existing scalar alias semantics.
- Non-`id` generic/nullability parameter suffix usage remains deterministic semantic diagnostics (`O3S206`).

Lowering/runtime status (implemented):

- `id<...>`, `id?`, and `id!` parameter spellings lower through the same typed scalar ABI path as `id`.
- No ABI, calling convention, or runtime-dispatch surface changes are introduced by suffix forms.

## M83 `id` return suffix support (`id<...>`, `id?`, `id!`)

Grammar/semantics status (implemented):

- Function return annotations accept Objective-C style `id` suffix forms:
  - protocol-qualifier suffixes (`-> id<NSObject>`),
  - nullability suffixes (`-> id?`, `-> id!`).
- Return suffixes are accepted only for `id` return annotations.
- Non-`id` return suffix usage remains fail-closed.

Lowering/runtime status (implemented):

- `id<...>`, `id?`, and `id!` return annotations lower through the same scalar ABI path as unsuffixed `id`.
- No ABI or runtime-dispatch lowering changes are introduced by return suffix forms.

## M84 return suffix semanticization (`O3P114` -> `O3S206` for non-`id`/`Class` suffixes)

Grammar/semantics status (implemented):

- Return suffix metadata is parsed and retained uniformly after known return annotations.
- Unterminated generic return suffixes remain parser diagnostics (`O3P114`).
- Non-`id`/`Class` return suffix usage (`<...>`, `?`, `!`) is rejected at semantic stage with deterministic `O3S206`.

Lowering/runtime status (implemented):

- `id` return suffix spellings continue to lower through the same scalar ABI path as unsuffixed `id`.
- Non-`id`/`Class` suffix rejection is semantic-only and does not change ABI/runtime dispatch surfaces.

## M56 function prototype declarations and extern lowering support

Grammar/semantics status (implemented):

- Function declarations may be semicolon-terminated prototypes: `fn name(args) -> type;`.
- Compatible prototype + definition pairs for the same symbol are accepted.
- Incompatible prototype/definition signatures emit deterministic semantic diagnostics (`O3S206`).
- Multiple function definitions for the same symbol emit deterministic duplicate diagnostics (`O3S200`).

Lowering/runtime status (implemented):

- Prototype-only symbols lower as LLVM `declare` entries with typed ABI signatures.
- Definitions continue to lower as LLVM `define` entries.
- Calls to prototype-only symbols compile to object code and fail at link time if unresolved (expected native linker behavior).

## M57 Foundation scalar type alias support

Grammar/semantics status (implemented):

- Function parameter annotations accept `BOOL`, `NSInteger`, and `NSUInteger`.
- Function return annotations accept `BOOL`, `NSInteger`, and `NSUInteger`.
- `BOOL` aliases to native `bool`; `NSInteger`/`NSUInteger` alias to native scalar integer paths.

Lowering/runtime status (implemented):

- `BOOL` signatures lower through existing `i1` ABI paths.
- `NSInteger`/`NSUInteger` signatures lower through existing `i32` ABI paths.

## M85 `Class` type annotation alias support

Grammar/semantics status (implemented):

- Function parameter annotations accept `Class`.
- Function return annotations accept `Class`.
- `Class` annotations are treated as native scalar aliases in semantic validation paths.

Lowering/runtime status (implemented):

- `Class`-annotated parameters/returns lower through the existing scalar ABI path.
- LLVM IR type emission remains deterministic (`i32`/`i1`/`void`) with `Class` aliasing to scalar integer ABI lowering.

## M86 `Class` suffix support (`Class<...>`, `Class?`, `Class!`)

Grammar/semantics status (implemented):

- Parameter annotations accept `Class` suffix forms:
  - protocol-qualifier suffixes (`Class<NSObject>`),
  - nullability suffixes (`Class?`, `Class!`).
- Function return annotations accept `Class` suffix forms:
  - protocol-qualifier suffixes (`-> Class<NSObject>`),
  - nullability suffixes (`-> Class?`, `-> Class!`).
- Suffixes are accepted for `id` and `Class` annotation spellings.
- Non-`id`/`Class` suffix usage remains deterministic semantic diagnostics (`O3S206`).

Lowering/runtime status (implemented):

- `Class<...>`, `Class?`, and `Class!` spellings lower through the same scalar ABI path as unsuffixed `Class`.
- No ABI or runtime-dispatch lowering changes are introduced by `Class` suffix forms.

## M87 `SEL` type annotation alias support

Grammar/semantics status (implemented):

- Function parameter annotations accept `SEL`.
- Function return annotations accept `SEL`.
- `SEL` annotations are treated as native scalar aliases in semantic validation paths.
- Existing suffix acceptance boundaries remain unchanged: suffix forms are accepted for `id`/`Class` only.

Lowering/runtime status (implemented):

- `SEL`-annotated parameters/returns lower through the existing scalar ABI path.
- LLVM IR type emission remains deterministic (`i32`/`i1`/`void`) with `SEL` aliasing to scalar integer ABI lowering.

## M88 `Protocol` type annotation alias support

Grammar/semantics status (implemented):

- Function parameter annotations accept `Protocol`.
- Function return annotations accept `Protocol`.
- `Protocol` annotations are treated as native scalar aliases in semantic validation paths.
- Existing suffix acceptance boundaries remain unchanged: suffix forms are accepted for `id`/`Class` only.

Lowering/runtime status (implemented):

- `Protocol`-annotated parameters/returns lower through the existing scalar ABI path.
- LLVM IR type emission remains deterministic (`i32`/`i1`/`void`) with `Protocol` aliasing to scalar integer ABI lowering.

## M89 `instancetype` type annotation alias support

Grammar/semantics status (implemented):

- Function parameter annotations accept `instancetype`.
- Function return annotations accept `instancetype`.
- `instancetype` annotations are treated as native scalar aliases in semantic validation paths.
- Existing suffix acceptance boundaries remain unchanged: suffix forms are accepted for `id`/`Class` only.

Lowering/runtime status (implemented):

- `instancetype`-annotated parameters/returns lower through the existing scalar ABI path.
- LLVM IR type emission remains deterministic (`i32`/`i1`/`void`) with `instancetype` aliasing to scalar integer ABI lowering.

## M90 `instancetype` suffix frontend extension

Grammar/semantics status (implemented):

- Function parameter annotations accept `instancetype` generic/nullability suffix forms (`instancetype<...>`, `instancetype?`, `instancetype!`).
- Function return annotations accept `instancetype` generic/nullability suffix forms (`-> instancetype<...>`, `-> instancetype?`, `-> instancetype!`).
- Semantic suffix acceptance boundaries are extended from `id`/`Class` to `id`/`Class`/`instancetype`.
- Existing non-`id`/`Class`/`instancetype` suffix usage remains deterministic semantic diagnostics (`O3S206`).

Lowering/runtime status (implemented):

- `instancetype` suffix forms lower through the existing scalar ABI path.
- LLVM IR type emission remains deterministic (`i32`/`i1`/`void`) with `instancetype` suffix spellings aliasing to scalar integer ABI lowering.

## M91 return-suffix diagnostic boundary wording refresh

Grammar/semantics status (implemented):

- `O3S206` function return suffix diagnostics now explicitly spell the unsupported boundary as non-`id`/`Class`/`instancetype` return annotations.
- Diagnostic behavior remains fail-closed and deterministic for unsupported return suffixes on other annotation spellings.

Lowering/runtime status (implemented):

- No ABI or lowering behavior changes were introduced.
- Runtime behavior remains unchanged; this milestone is a deterministic diagnostic wording refinement only.

## M92 parameter-suffix diagnostic boundary wording refresh

Grammar/semantics status (implemented):

- `O3S206` parameter generic/nullability suffix diagnostics now explicitly spell the unsupported boundary as non-`id`/`Class`/`instancetype` parameter annotations.
- Parameter-suffix rejection behavior remains fail-closed and deterministic for unsupported suffixes on other annotation spellings.

Lowering/runtime status (implemented):

- No ABI or lowering behavior changes were introduced.
- Runtime behavior remains unchanged; this milestone is a deterministic diagnostic wording and coverage refinement only.

## M93 explicit `extern fn` declaration syntax support

Grammar/semantics status (implemented):

- Top-level explicit `extern fn` declarations are accepted as prototype declarations.
- `extern` must be followed by `fn`; invalid forms emit deterministic parser diagnostics (`O3P100`).
- `extern fn` declarations are declaration-only and must terminate with `;`; body-bearing forms emit deterministic parser diagnostics (`O3P104`).

Lowering/runtime status (implemented):

- `extern fn` declarations lower through the existing prototype declaration path.
- No ABI behavior changes were introduced; this milestone adds syntax and deterministic parser contracts only.

## M94 `extern`+`pure` qualifier-chain declaration syntax support

Grammar/semantics status (implemented):

- Top-level explicit qualifier-chain declarations accept both `extern pure fn` and `pure extern fn` prototype forms.
- Qualifier-chain declarations remain fail-closed for missing `fn`; malformed forms emit deterministic parser diagnostics (`O3P100`).
- Any declaration containing `extern` remains declaration-only and must terminate with `;`; body-bearing forms emit deterministic parser diagnostics (`O3P104`).

Lowering/runtime status (implemented):

- `extern pure fn` and `pure extern fn` declarations lower through the existing prototype declaration path.
- No ABI behavior changes were introduced; this milestone adds declaration syntax and deterministic parser contracts only.

## M95 qualifier duplicate diagnostics for function declarations

Grammar/semantics status (implemented):

- Accepted declaration qualifier forms remain `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Duplicate `pure` or `extern` qualifiers in declaration chains are rejected deterministically with parser diagnostics (`O3P100`).
- Over-chained qualifier sequences are rejected deterministically via duplicate-qualifier diagnostics while preserving accepted forms.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening and conformance expansion only.

## M96 qualifier placement diagnostics after `fn`

Grammar/semantics status (implemented):

- Misplaced qualifier tokens after `fn` are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages now include `unexpected qualifier 'pure' after 'fn'` and `unexpected qualifier 'extern' after 'fn'`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser placement-contract hardening and conformance expansion only.

## M97 qualifier placement diagnostics after function name

Grammar/semantics status (implemented):

- Qualifier tokens between a function identifier and `(` are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' after function name` and `unexpected qualifier 'extern' after function name`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser placement-contract hardening for function-name boundaries and conformance expansion only.

## M98 qualifier placement diagnostics after parameter list and return annotation

Grammar/semantics status (implemented):

- Qualifier tokens after `)` in function declarations are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' after parameter list` and
  `unexpected qualifier 'extern' after parameter list`.
- Qualifier tokens after parsed return annotations are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' after function return annotation` and
  `unexpected qualifier 'extern' after function return annotation`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser placement-contract hardening for post-parameter and post-return boundaries with conformance expansion only.

## M99 qualifier diagnostics in parameter and return type annotations

Grammar/semantics status (implemented):

- Qualifier tokens in parameter type annotation positions are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' in parameter type annotation` and
  `unexpected qualifier 'extern' in parameter type annotation`.
- Qualifier tokens in function return type annotation positions are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' in function return type annotation` and
  `unexpected qualifier 'extern' in function return type annotation`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for type-annotation qualifier placement with conformance expansion only.

## M100 qualifier diagnostics after parameter type annotations

Grammar/semantics status (implemented):

- Qualifier tokens after parsed parameter type annotations are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' after parameter type annotation` and
  `unexpected qualifier 'extern' after parameter type annotation`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Parameter separator contracts remain unchanged: parameter type annotations must be followed by `,` or `)`.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for parameter-type boundary qualifier placement with conformance expansion only.

## M101 qualifier diagnostics in parameter identifier positions

Grammar/semantics status (implemented):

- Qualifier tokens in parameter identifier positions are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' in parameter identifier position` and
  `unexpected qualifier 'extern' in parameter identifier position`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Parameter parse contracts remain unchanged: parameter identifiers must be identifier tokens and continue to require `:` + type annotation.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for parameter-identifier qualifier placement with conformance expansion only.

## M102 qualifier diagnostics after parameter names

Grammar/semantics status (implemented):

- Qualifier tokens after parameter names and before `:` are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' after parameter name` and
  `unexpected qualifier 'extern' after parameter name`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Parameter parse contracts remain unchanged: parameter identifiers continue to require immediate `:` separators and type annotations.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for parameter-name boundary qualifier placement with conformance expansion only.

## M103 qualifier diagnostics in statement positions

Grammar/semantics status (implemented):

- Qualifier tokens in statement positions are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' in statement position` and
  `unexpected qualifier 'extern' in statement position`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Statement parsing contracts remain unchanged for valid forms: `let`, `return`, control statements, and expression statements.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for statement-position qualifier placement with conformance expansion only.

## M104 qualifier diagnostics in expression positions

Grammar/semantics status (implemented):

- Qualifier tokens in expression positions are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' in expression position` and
  `unexpected qualifier 'extern' in expression position`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Expression parsing contracts remain unchanged for valid forms: literals, identifiers, grouped expressions, calls, and message sends.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for expression-position qualifier placement with conformance expansion only.

## M105 qualifier diagnostics in message selector positions

Grammar/semantics status (implemented):

- Qualifier tokens in Objective-C message selector positions are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' in message selector position`,
  `unexpected qualifier 'extern' in message selector position`,
  `unexpected qualifier 'pure' in keyword selector segment position`, and
  `unexpected qualifier 'extern' in keyword selector segment position`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Message-send parsing contracts remain unchanged for valid forms: selector head/keyword segments still require identifier tokens.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for message-selector qualifier placement with conformance expansion only.

## M106 message receiver diagnostic cascade control

Grammar/semantics status (implemented):

- Message receiver parse failures now suppress secondary generic `O3P113` receiver diagnostics when receiver parsing already emitted deterministic primary diagnostics.
- Receiver parse failures with no prior receiver diagnostics still emit `O3P113` with `invalid receiver expression in message send`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Message-send parsing contracts remain unchanged for valid forms and selector parsing behavior.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser diagnostic ordering/cascade hardening for message-receiver failures with conformance expansion only.

## M107 qualifier diagnostics in case-label expressions

Grammar/semantics status (implemented):

- Qualifier tokens in switch case-label expression positions are rejected deterministically with parser diagnostics (`O3P100`).
- Deterministic messages include `unexpected qualifier 'pure' in case label expression` and
  `unexpected qualifier 'extern' in case label expression`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.
- Switch parsing contracts remain unchanged for valid case labels (`number`, `true`, `false`) and default clauses.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; accepted declaration forms continue to lower through existing declaration paths.
- This milestone is parser contract hardening for case-label qualifier placement with conformance expansion only.

## M108 signed switch case labels

Grammar/semantics status (implemented):

- Switch case labels now accept signed numeric literal forms: `case -<number>:` and `case +<number>:`.
- Existing accepted case-label forms remain unchanged: `case <number>:`, `case true:`, and `case false:`.
- Malformed signed labels that omit numeric literals after `+`/`-` are rejected deterministically with parser diagnostics (`O3P103`) and token text `invalid case label expression`.
- Accepted declaration qualifier forms remain unchanged: `fn`, `pure fn`, `extern fn`, `extern pure fn`, and `pure extern fn`.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; switch lowering and control-flow behavior remain unchanged.
- This milestone is parser feature expansion for signed case-label forms with deterministic malformed-label diagnostics.

## M109 switch return-path analysis

Grammar/semantics status (implemented):

- Non-void functions now accept `switch`-terminated return paths when all case/default bodies guarantee return.
- Guaranteed-return detection for switches is conservative and requires:
  - a `default` case, and
  - every case/default body to satisfy existing guaranteed-return analysis.
- Switches without `default` remain non-exhaustive for return-path analysis and continue to trigger `O3S205` when no trailing guaranteed return exists.
- Existing return-path rules for `return`, block, and `if` remain unchanged.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract hardening for switch-aware guaranteed-return detection with conservative fail-closed behavior.

## M110 empty case fallthrough return analysis

Grammar/semantics status (implemented):

- Switch return-path analysis now treats empty `case` bodies as deterministic fallthrough links to the next case/default arm.
- Non-void functions are accepted when each switch arm guarantees return under these rules:
  - `default` exists,
  - each non-empty case/default body guarantees return under existing analysis, and
  - each empty case arm falls through to a subsequent arm that guarantees return.
- Non-empty case bodies that do not guarantee return remain conservative fail-closed (even if execution may fall through at runtime), preserving deterministic `O3S205` behavior.
- Existing return-path rules for `return`, block, and `if` remain unchanged.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for empty-case fallthrough chaining with conservative non-empty-case handling.

## M111 non-empty case fallthrough return analysis

Grammar/semantics status (implemented):

- Switch return-path analysis now accepts non-empty straight-line case bodies as deterministic fallthrough links to the next case/default arm.
- Straight-line case-body fallthrough chaining applies to statement sequences composed of:
  - `let`,
  - assignment,
  - expression statements,
  - empty statements, and
  - nested blocks composed of the same straight-line statement forms.
- Non-empty case bodies with break/control-flow statements (`break`, `continue`, `if`, loops, nested `switch`) remain conservative fail-closed for guaranteed-return analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under the expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for non-empty straight-line case-body fallthrough chaining.

## M112 conditional case fallthrough return analysis

Grammar/semantics status (implemented):

- Switch return-path analysis now accepts conditional case bodies when all conditional paths either:
  - guarantee return, or
  - continue to deterministic fallthrough into the next case/default arm.
- Conditional case-body analysis is currently expanded for `if` statements, including omitted-`else` forms (implicit fallthrough path).
- Break/control-flow escape paths inside conditional case bodies remain conservative fail-closed for guaranteed-return analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for conditional (`if`) case-body return/fallthrough chaining.

## M113 nested switch case fallthrough return analysis

Grammar/semantics status (implemented):

- Switch return-path analysis now treats nested `switch` statements in case bodies as chainable return-or-fallthrough steps.
- Nested `switch` statements that do not already guarantee return may continue into subsequent case-body statements and ultimately into outer case fallthrough paths.
- Break/control-flow escape paths in loop statements (`while`, `for`, `do-while`) remain conservative fail-closed in case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for nested-switch case-body chaining with conservative loop/control-flow handling.

## M114 static while-false case fallthrough analysis

Grammar/semantics status (implemented):

- Switch return-path analysis now treats case-body `while` statements with statically false conditions as chainable fallthrough steps.
- Static-false while detection currently accepts literal forms: `while (false)`, `while (0)`, and `while (nil)`.
- Non-static while conditions (including `true`, identifiers, and computed expressions) remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static while-false case-body chaining with conservative non-static while handling.

## M115 static for-false case fallthrough analysis

Grammar/semantics status (implemented):

- Switch return-path analysis now treats case-body `for` statements with statically false conditions as chainable fallthrough steps.
- Static-false for-condition detection currently accepts literal forms: `for (...; false; ...)`, `for (...; 0; ...)`, and `for (...; nil; ...)`.
- Non-static for conditions (including `true`, identifiers, and computed expressions) remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static for-false case-body chaining with conservative non-static for handling.

## M116 static do-while-false case fallthrough analysis

Grammar/semantics status (implemented):

- Switch return-path analysis now treats case-body `do-while` statements with statically false conditions as chainable fallthrough steps.
- Static-false do-while condition detection currently accepts literal forms: `do { ... } while (false)`, `while (0)`, and `while (nil)`.
- Non-static do-while conditions (including `true`, identifiers, and computed expressions) remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static do-while-false case-body chaining with conservative non-static do-while handling.

## M117 static unary condition folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds a deterministic unary-negation condition subset during static condition analysis.
- Supported folded form is the parser-normalized unary-not shape represented as `(<expr> == 0)` when `<expr>` has static truthiness.
- Static truthiness inputs currently include literal booleans, numeric literals, and `nil`.
- Non-literal or unresolved unary condition forms remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static unary-negation condition folding with conservative unresolved-form handling.

## M118 static binary condition folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds a deterministic binary condition subset during static condition analysis.
- Folded binary forms currently include `==` and `!=` comparisons when both sides are statically scalar-evaluable.
- Static scalar inputs currently include literal booleans, numeric literals, `nil`, and folded unary-negation results.
- Non-literal or unresolved binary condition forms remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static binary equality/inequality condition folding with conservative unresolved-form handling.

## M119 static relational condition folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds a deterministic relational condition subset during static condition analysis.
- Folded relational forms currently include `<`, `<=`, `>`, and `>=` when both sides are statically scalar-evaluable.
- Static scalar inputs currently include literal booleans, numeric literals, `nil`, and folded unary/binary condition results.
- Non-literal or unresolved relational condition forms remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static relational condition folding with conservative unresolved-form handling.

## M120 static logical condition folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds a deterministic logical condition subset during static condition analysis.
- Folded logical forms currently include `&&` and `||` when truthiness can be determined via static operands and short-circuit evaluation.
- Static scalar inputs currently include literal booleans, numeric literals, `nil`, and previously folded unary/binary/relational forms.
- Non-literal or unresolved logical condition forms remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static logical condition folding with conservative unresolved-form handling.

## M121 static conditional-operator folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds deterministic conditional (`?:`) expressions during static condition analysis.
- Conditional folding evaluates the static truthiness of the condition expression and then folds the selected branch.
- Non-literal or unresolved conditional-expression forms remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static conditional-expression folding with conservative unresolved-form handling.

## M122 static arithmetic folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds deterministic arithmetic expressions during static condition analysis.
- Arithmetic folding currently covers additive and multiplicative operators (`+`, `-`, `*`, `/`, `%`) when both operands are statically scalar-evaluable.
- Arithmetic folding is overflow-aware and remains conservative for divide/modulo-by-zero forms.
- Non-literal or unresolved arithmetic-expression forms remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static arithmetic-expression folding with conservative unresolved-form handling.

## M123 static bitwise/shift folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds deterministic bitwise/shift expressions during static condition analysis.
- Bitwise/shift folding currently covers `&`, `|`, `^`, `<<`, and `>>` when operands are statically scalar-evaluable.
- Shift folding enforces safe static constraints (non-negative inputs, valid shift counts, and non-overflowing left shifts).
- Non-literal or unresolved bitwise/shift-expression forms remain conservative fail-closed for case-body chaining analysis.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static bitwise/shift-expression folding with conservative unresolved-form handling.

## M124 static identifier-binding folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds identifier conditions when identifiers resolve to deterministic static scalar bindings.
- Identifier bindings are admitted only for top-level function `let` values that are statically scalar-evaluable and remain non-reassigned.
- Identifier binding folding is conservative against shadowing and reassignment: reassigned/shadowed/unresolved identifiers remain fail-closed.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static identifier-binding folding with conservative reassignment/shadowing handling.

## M125 static global-const binding folding in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now folds global identifier conditions when globals have deterministic constant initializers and remain non-assigned.
- Global binding folding reuses semantic constant-expression initialization and excludes any global assigned in function bodies.
- Global binding folding remains conservative for assigned/shadowed/unresolved global identifier forms.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static global-const identifier folding with conservative assigned/shadowed handling.

## M126 static if-condition branch selection in case chaining

Grammar/semantics status (implemented):

- Switch case-body chaining now performs static branch selection for `if` statements when condition truthiness is statically known.
- Statically-true `if` conditions evaluate only the `then` branch for case-body chaining.
- Statically-false `if` conditions evaluate only the `else` branch (or fallthrough when `else` is omitted) for case-body chaining.
- Unresolved/non-static `if` conditions remain conservative and require both branches to satisfy chaining constraints.
- Existing requirements remain: non-void switch return-path acceptance still requires `default` and all arms to guarantee return under expanded rules.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static `if` branch-selection in case-body chaining with conservative unresolved-form handling.

## M127 static if-condition branch selection in function return analysis

Grammar/semantics status (implemented):

- Function return-path analysis now performs static branch selection for top-level `if` statements when condition truthiness is statically known.
- Statically-true `if` conditions evaluate only the `then` branch for guaranteed-return proof.
- Statically-false `if` conditions evaluate only the `else` branch for guaranteed-return proof.
- Unresolved/non-static `if` conditions remain conservative and still require both branches to guarantee return.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static `if` branch-selection in function-level guaranteed-return analysis with conservative unresolved-form handling.

## M128 static switch-condition branch selection in function return analysis

Grammar/semantics status (implemented):

- Function return-path analysis now performs static branch selection for `switch` statements when switch condition values are statically known.
- Statically selected switch entry arms now drive guaranteed-return proof from the selected arm onward, including deterministic fallthrough chaining.
- If no matching arm exists, static selection falls back to `default` when present; no-match without `default` remains non-returning.
- Unresolved/non-static switch conditions remain conservative and still require `default` plus all arms to guarantee return.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for static switch-condition branch selection in function-level guaranteed-return analysis with conservative unresolved-form handling.

## M129 function-level loop return-path guarantees for statically-entered forms

Grammar/semantics status (implemented):

- Function return-path analysis now accepts `while` statements when loop entry is statically guaranteed (`true`) and loop bodies are proven always-returning.
- Function return-path analysis now accepts `for` statements when loop entry is statically guaranteed (`true` or omitted condition) and loop bodies are proven always-returning.
- Function return-path analysis now accepts `do-while` statements when loop bodies are proven always-returning (independent of condition value).
- Unresolved/non-static loop-entry conditions remain conservative and are not used to prove guaranteed return.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone extends semantic return-path analysis only.
- This milestone is semantic contract expansion for function-level loop return-path guarantees with conservative unresolved-loop handling.

## M131 loop-entry static-binding coverage for function return guarantees

Grammar/semantics status (implemented):

- Function return-path loop-entry guarantees are now conformance-covered for static top-level identifier bindings in `while` conditions.
- Function return-path loop-entry guarantees are now conformance-covered for static non-assigned global constant bindings in `for` conditions.
- Reassigned loop-entry identifiers remain conservative and continue to reject guaranteed-return proof when no alternate return path exists.

Lowering/runtime status (implemented):

- No lowering/runtime behavior changed; this milestone is conformance and contract coverage expansion for existing semantic behavior.

## Semantic checks currently enforced

- Duplicate module/global/function declarations: `O3S200`
- Duplicate parameters/local declarations in same scope: `O3S201`
- Undefined identifier: `O3S202`
- Unknown function call target: `O3S203`
- Function arity mismatch: `O3S204`
- Missing return path in function body: `O3S205`
  - Switch-aware for non-void functions when `default` exists and all switch arms guarantee return.
  - Empty `case` bodies are treated as fallthrough links to the next arm for guaranteed-return analysis.
  - Non-empty straight-line `case` bodies (`let`, assignment, expression, empty, nested straight-line blocks) are treated as fallthrough links to the next arm.
  - Conditional (`if`) case bodies are treated as chainable when all paths either return or continue to fallthrough.
  - Nested `switch` statements in case bodies are treated as chainable return-or-fallthrough steps.
  - `while` statements with statically false literal conditions (`false`, `0`, `nil`) are treated as fallthrough links to the next arm.
  - `for` statements with statically false literal conditions (`false`, `0`, `nil`) are treated as fallthrough links to the next arm.
  - `do-while` statements with statically false literal conditions (`false`, `0`, `nil`) are treated as fallthrough links to the next arm.
  - Unary-negation condition forms are folded when their operands have static truthiness (`bool`/numeric literals and `nil`).
  - Binary equality/inequality conditions (`==`, `!=`) are folded when both operands are statically scalar-evaluable.
  - Relational conditions (`<`, `<=`, `>`, `>=`) are folded when both operands are statically scalar-evaluable.
  - Logical conditions (`&&`, `||`) are folded when static truthiness can be determined through short-circuit evaluation.
  - Conditional (`?:`) expressions are folded when the condition truthiness and selected branch are statically evaluable.
  - Arithmetic expressions (`+`, `-`, `*`, `/`, `%`) are folded when operands are statically scalar-evaluable and arithmetic is safe.
  - Bitwise/shift expressions (`&`, `|`, `^`, `<<`, `>>`) are folded when operands are statically scalar-evaluable and shift operations are safe.
  - Identifier conditions are folded when they resolve to static non-reassigned top-level function `let` scalar bindings.
  - Global identifier conditions are folded when they resolve to static non-assigned constant global initializers.
  - `if` case-body statements use static branch selection when condition truthiness is statically known.
  - Function-level `if` statements use static branch selection when condition truthiness is statically known.
  - Unresolved function-level `if` statements remain conservative and require both branches to guarantee return.
  - Function-level `switch` statements use static condition branch selection when switch condition values are statically known.
  - Unresolved function-level `switch` conditions remain conservative and require `default` plus all arms to guarantee return.
  - Function-level `while` statements may guarantee return when loop entry is statically true and the loop body is always-returning.
  - Function-level `for` statements may guarantee return when loop entry is statically true (or condition omitted) and the loop body is always-returning.
  - Function-level `do-while` statements may guarantee return when the loop body is always-returning.
  - Function-level loop-entry guarantees are covered for static identifier/global binding conditions with conservative reassignment handling.
  - Unresolved function-level loop-entry conditions remain conservative for guaranteed-return analysis.
  - Non-empty `case` bodies with break/control-flow statements remain conservative fail-closed for guaranteed-return analysis.
- Type mismatch checks: `O3S206`
- Message receiver type mismatch: `O3S207`
- Message argument count limit exceeded: `O3S208`
- Message argument type mismatch in send: `O3S209`
- Global initializer must be constant expression: `O3S210`
- Return expression type mismatch against declared function return type: `O3S211`
- `break` outside loop is rejected: `O3S212`
- `continue` outside loop is rejected: `O3S213`
- assignment target must be mutable local or global symbol: `O3S214`
- migration assist in canonical compatibility mode rejects legacy Objective-C aliases (`YES`, `NO`, `NULL`): `O3S216`

## Semantic pipeline boundaries (M25-B007/B008)

M132-B001 freezes the stage contract in
`native/objc3c/src/pipeline/frontend_pipeline_contract.h` with concrete
`FrontendPipeline`, `StageResult`, and `DiagnosticsEnvelope` interfaces.

Deterministic stage order is now modeled explicitly as:

- Stage `lex`: tokenization + lexer diagnostics.
- Stage `parse`: AST construction + parser diagnostics.
- Stage `sema`: integration-surface build + semantic validation.
- Stage `lower`: native IR lowering + lowering metadata.
- Stage `emit`: diagnostics/manifest/object artifact emission.

Error propagation and diagnostics semantics are fail-closed and no-throw:

- Each stage returns a `StageResult` with `status`, `skip_reason`, and a
  stage-local `DiagnosticsEnvelope`.
- Any `error` or `fatal` severity marks stage failure.
- Downstream stages are reported as skipped with upstream-failure reason.
- The top-level pipeline exposes `error_model = NoThrowFailClosed`.

Every currently shipped `.objc3` stage behavior is mapped to contract fields:

- `sema_output.semantic_skipped` maps manifest
  `frontend.pipeline.semantic_skipped`.
- Stage diagnostics counts map
  `frontend.pipeline.stages.{lexer,parser,semantic}.diagnostics`.
- Semantic surface counters map
  `frontend.pipeline.semantic_surface.{declared_globals,declared_functions,resolved_global_symbols,resolved_function_symbols}`.
- Function signature surface counters map
  `frontend.pipeline.semantic_surface.function_signature_surface.{scalar_return_i32,scalar_return_bool,scalar_return_void,scalar_param_i32,scalar_param_bool}`.
- Lowering options/metadata map `lowering.{runtime_dispatch_symbol,runtime_dispatch_arg_slots,selector_global_ordering}`.
- Emit stage result captures diagnostics/manifest/object artifact write status and object compile exit status.

## Sema pass manager + diagnostics bus contract (M139-E001)

- Canonical sema contract types are defined in:
  - `native/objc3c/src/sema/objc3_sema_contract.h`
  - `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- Sema pass manager orchestration boundary:
  - `native/objc3c/src/sema/objc3_sema_pass_manager.h`
  - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
  - `RunObjc3SemaPassManager(...)` executes deterministic pass order (`BuildIntegrationSurface`, `ValidateBodies`, `ValidatePureContract`) and publishes pass diagnostics into the sema diagnostics bus.
- Semantic pass implementations remain extracted in:
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/sema/objc3_pure_contract.cpp`
  - `ValidatePureContractSemanticDiagnostics(...)` remains owned by `objc3_pure_contract.cpp` (fail-closed against regressions back into `objc3_semantic_passes.cpp`).
- Frontend diagnostics bus boundary:
  - `native/objc3c/src/parse/objc3_diagnostics_bus.h` owns `Objc3FrontendDiagnosticsBus` (`lexer`, `parser`, `semantic`) and deterministic transport into parsed-program diagnostics.
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` wires sema pass manager diagnostics via `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic` and finalizes with `TransportObjc3DiagnosticsToParsedProgram(...)`.
- Stage-contract packet remains anchored by `native/objc3c/src/pipeline/frontend_pipeline_contract.h` (`DiagnosticsEnvelope`, `SemaStageOutput`, and `kStageOrder` containing `StageId::Sema`).

## Frontend library boundary contract (M140-E001)

- Library compile entrypoints are pipeline-backed in:
  - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
  - `native/objc3c/src/libobjc3c_frontend/api.h`
- Reusable extracted compile-product boundary is defined in:
  - `native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.h`
  - `native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp`
  - `Objc3FrontendCompileProduct` carries `pipeline_result` + `artifact_bundle`.
- Deterministic sema type-metadata handoff surface:
  - `native/objc3c/src/sema/objc3_sema_contract.h`
  - `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
  - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- Lowering-to-IR replay boundary surface:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- Contract validation commands:
  - `python scripts/check_m140_frontend_library_boundary_contract.py`
  - `npm run check:compiler-closeout:m140`

## CMake targetization and linkage topology contract (M141-E001)

- Canonical target/link topology lives in `native/objc3c/CMakeLists.txt`:
  - Stage-forward links: `objc3c_parse -> objc3c_lex`, `objc3c_sema -> objc3c_parse`, `objc3c_lower -> objc3c_sema_type_system`, `objc3c_ir -> objc3c_lower`.
  - Semantic/type-system boundary target: `add_library(objc3c_sema_type_system INTERFACE)` and downstream consumption by lower/IR targets.
  - Runtime ABI split target: `add_library(objc3c_runtime_abi STATIC src/io/objc3_process.cpp)` linked by `objc3c_io`.
  - Aggregate executable wiring: `target_link_libraries(objc3c-native PRIVATE objc3c_driver)`.
- Driver entrypoint split:
  - `native/objc3c/src/main.cpp` delegates to `RunObjc3DriverMain(...)`.
  - `native/objc3c/src/driver/objc3_driver_main.cpp` owns CLI parse + compilation-driver dispatch.
- Build-surface parity:
  - `scripts/build_objc3c_native.ps1` includes `native/objc3c/src/driver/objc3_driver_main.cpp`.
- Contract validation commands:
  - `python scripts/check_m141_cmake_target_topology_contract.py`
  - `npm run check:compiler-closeout:m141`

## Frontend lowering parity harness contract (M142-E001)

- Deterministic parity harness entrypoint:
  - `scripts/check_objc3c_library_cli_parity.py`
  - Source mode executes both CLI (`--cli-bin`) and C API runner (`--c-api-bin`) from one `.objc3` input.
- C API runner surface:
  - `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
  - `objc3c-frontend-c-api-runner` forwards `--objc3-max-message-args` and `--objc3-runtime-dispatch-symbol` into C API compile options.
- Parity artifact dimensions compare:
  - `<emit-prefix>.diagnostics.json`
  - `<emit-prefix>.manifest.json`
  - `<emit-prefix>.ll`
  - `<emit-prefix>.obj`
- Object backend parity note:
  - CLI default object backend remains `llvm-direct`.
  - Harness parity flow pins CLI to `--cli-ir-object-backend clang` so CLI and C API object outputs share backend semantics.
  - CLI-only backend provenance sidecar `<emit-prefix>.object-backend.txt` is informational and intentionally excluded from parity digest dimensions.
- Contract validation commands:
  - `python scripts/check_m142_frontend_lowering_parity_contract.py`
  - `npm run check:compiler-closeout:m142`

## Artifact tmp-path governance contract (M143-D001)

- Source-mode parity replay is partitioned by deterministic `--work-key`:
  - Default `--work-key` derives from source path + emit controls + lowering/runtime-affecting options.
  - Source-mode output roots are `<work-dir>/<work-key>/library` and `<work-dir>/<work-key>/cli`.
- Tmp-path governance defaults are fail-closed:
  - `--work-dir` and derived output roots must remain under `tmp/`.
  - Explicit override requires `--allow-non-tmp-work-dir`.
- Replay safety contract for source mode:
  - Fails when expected generated artifacts for `<emit-prefix>` are missing after command execution.
  - Fails when stale `<emit-prefix>` artifacts are already present in target output roots prior to command execution.
- Lane-B sema/type-system replay artifacts are also tmp-governed:
  - `scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1` writes `summary.json` under `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/`.
  - Default run id is deterministic (`m143-sema-type-system-default`); explicit override remains available via `OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID`.
- Parser/AST-facing lane-A closeout coverage:
  - `test:objc3c:m143-artifact-governance` includes `tests/tooling/test_objc3c_parser_extraction.py` and `tests/tooling/test_objc3c_parser_ast_builder_extraction.py`.
- Lowering/LLVM IR/runtime-ABI lane-C closeout coverage:
  - `scripts/run_objc3c_lowering_regression_suite.ps1` uses deterministic default run id `m143-lane-c-lowering-regression-default` (override: `OBJC3C_NATIVE_LOWERING_RUN_ID`).
  - `scripts/check_objc3c_typed_abi_replay_proof.ps1` uses deterministic default run id `m143-lane-c-typed-abi-default` (override: `OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID`).
  - `scripts/check_objc3c_lowering_replay_proof.ps1` uses deterministic default proof run id `m143-lane-c-lowering-replay-proof-default` (override: `OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID`).

## LLVM capability discovery contract (M144-E001)

- Capability probe packet mode is pinned to `objc3c-llvm-capabilities-v2`:
  - `scripts/probe_objc3c_llvm_capabilities.py` publishes deterministic `clang`, `llc`, `llc_features`, and `sema_type_system_parity` capability fields.
- Driver capability summary routing is fail-closed:
  - `native/objc3c/src/driver/objc3_llvm_capability_routing.cpp` rejects missing/invalid summaries and blocks backend routing when parity readiness is unavailable.
  - `native/objc3c/src/driver/objc3_cli_options.cpp` exposes `--llvm-capabilities-summary` and `--objc3-route-backend-from-capabilities`.
- Source-mode CLI/C API parity can route backend from probe output:
  - `scripts/check_objc3c_library_cli_parity.py` consumes `--llvm-capabilities-summary` and enforces fail-closed routing prerequisites before executing CLI/C API runs.
- Contract validation commands:
  - `python scripts/check_m144_llvm_capability_discovery_contract.py`
  - `npm run check:compiler-closeout:m144`

## Direct LLVM object-emission matrix lane-B contract (M145-B001)

- Lane-B sema/type-system contract runner now includes explicit backend matrix coverage in:
  - `scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`
- Matrix behavior for semantic positive replay (`typed_i32_bool.objc3`):
  - `clang` replay remains required and deterministic.
  - `llvm-direct` replay is executed explicitly; when backend capability is unavailable, it must fail closed with deterministic backend diagnostics and absent object/backend artifacts.
  - forced missing-llc replay uses `--objc3-ir-object-backend llvm-direct --llc <missing-path>` and must fail closed with deterministic `llc executable not found` diagnostics.
- Matrix behavior for semantic negative replay:
  - Backend-invariant diagnostics are asserted across `clang` and `llvm-direct` runs for a deterministic negative fixture sample.
- Contract validation commands:
  - `python scripts/check_m145_direct_llvm_matrix_contract.py`
  - `npm run test:objc3c:m145-direct-llvm-matrix`
  - `npm run check:compiler-closeout:m145`

## Direct LLVM object-emission matrix lane-D contract (M145-D001)

- Lane-D validation extends M145 with fixture, determinism, conformance, and perf coverage:
  - Conformance fixture: `tests/conformance/lowering_abi/M145-D001.json`
  - Coverage map binding: `tests/conformance/COVERAGE_MAP.md` row `M145-D001 -> #4317 -> lowering_abi`
  - Lowering ABI manifest registration:
    `tests/conformance/lowering_abi/manifest.json`
- Fixture-level expectations:
  - Pins `--objc3-ir-object-backend llvm-direct` fail-closed behavior and diagnostic
    anchors `O3E001` / `O3E002`.
  - Ensures fail-closed object-emission contract coverage stays traceable from
    issue metadata through conformance fixture inventory.
- Lane-D validation commands:
  - `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`
  - `npm run check:compiler-closeout:m145`

## M25 Message-Send Contract Matrix

- Frontend grammar contract:
  - Bracketed send forms are accepted: unary selector (`[receiver ping]`) and keyword selector chains (`[receiver sum: 1 with: 2]`).
  - Message-send parse failures are surfaced through `O3P113` with deterministic replay ordering.
- Semantic contract:
  - Receiver expressions must be `i32`-compatible (`i32`/`bool`): `O3S207`.
  - Argument count cap is enforced by frontend option `--objc3-max-message-args` (default `4`): `O3S208`.
  - Message arguments must be `i32`-compatible (`i32`/`bool`): `O3S209`.
- Lowering contract:
  - Native IR emits runtime dispatch call `@<dispatch_symbol>(i32, ptr, i32...)` where slot count is configured by `--objc3-max-message-args`.
  - Dispatch symbol defaults to `objc3_msgsend_i32` and is configurable with `--objc3-runtime-dispatch-symbol`.
  - Unused dispatch argument slots are zero-filled by lowering.
  - Nil receiver sends short-circuit in native lowering: receiver `0` yields deterministic result `0` without runtime dispatch call.
  - Semantic typing keeps nil-receiver message-send results i32-compatible for arithmetic/comparison expression flows.
  - Direct syntactic `nil` receiver sends are elided to constant `0` without dispatch branch/call emission.
  - Immutable local bindings initialized to `nil` (for example `let receiver = nil; [receiver ping]`) are also compile-time elided.
  - Nil-bound identifier receiver elision is flow-sensitive at send sites: sends before reassignment can elide even when the identifier is reassigned later.
  - Numeric zero receivers (for example `[0 ping]`) are not compile-time elided; they retain runtime nil-check branch/call IR shape.
  - Mutable/reassigned receiver bindings retain runtime nil-check branch/call IR shape once the reassignment boundary is crossed.
  - Selector literals are interned and emitted as deterministic globals.
  - Selector global naming is canonicalized in lexicographic selector order for deterministic replay.
  - `.objc3` manifests record lowering integration metadata (`runtime_dispatch_symbol`, `runtime_dispatch_arg_slots`, `selector_global_ordering`).
- Diagnostics contract:
  - Diagnostics are normalized and deduplicated deterministically by location/code/message.
  - Receiver-type diagnostics (`O3S207`) are anchored to receiver expression locations.
- Tooling/CI contract:
  - `npm run test:objc3c:lane-e` is the aggregate gate for recovery, parser/diagnostics replay proof, perf/cache proof, lowering replay, execution smoke/replay proof, and M23/M24/M25 readiness checks.

## M224 sema/type-system release readiness

Sema/type-system GA readiness expectations (current enforced behavior):

- pass-manager execution order remains deterministic through `kObjc3SemaPassOrder` (`BuildIntegrationSurface`, `ValidateBodies`, `ValidatePureContract`).
- per-pass diagnostics remain deterministic and replay-stable: `CanonicalizePassDiagnostics(...)` is applied and parity asserts monotonic counters via `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- parity readiness remains fail-closed through `IsReadyObjc3SemaParityContractSurface(...)`, including deterministic diagnostics/type-metadata flags and symbol-count parity checks.
- pipeline-to-artifact evidence remains deterministic: semantic diagnostics are wired through `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;` and emitted in manifest `frontend.pipeline.sema_pass_manager` with `deterministic_semantic_diagnostics`, `deterministic_type_metadata_handoff`, and `parity_ready`.

Operator evidence sequence for sema/type-system readiness:

1. run sema extraction contract checks (`python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`).
2. run parser/sema integration contract checks (`python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`).
3. run M224 sema release contract test (`python -m pytest tests/tooling/test_objc3c_m224_sema_release_contract.py -q`).

## M225 sema/type roadmap seeding profile

To seed post-GA sema/type backlog items from shipped behavior, capture deterministic evidence in two packets:

### 1.1 Deterministic semantic diagnostics packet

- Pass-order and normalization anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics wiring anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest counters under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.

### 1.2 Deterministic type-metadata handoff packet

- Sema handoff and readiness anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity/type anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Backlog sizing surface from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counts (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).

Recommended seeding commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m225_sema_roadmap_seed_contract.py -q`

## M219 sema/type cross-platform parity profile

To prove sema/type parity across Windows/Linux/macOS before lane promotion, capture deterministic evidence packets for each replay run in stable platform order: `windows-msvc-x64`, `linux-clang-x64`, `macos-clang-arm64`.

### 1.1 Deterministic sema diagnostics cross-platform packet

- Source anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics handoff anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- Cross-platform diagnostics evidence packet names (deterministic tuple order):
  - `windows_sema_diagnostics_packet`
  - `linux_sema_diagnostics_packet`
  - `macos_sema_diagnostics_packet`

### 1.2 Deterministic type-metadata handoff cross-platform packet

- Source anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface metadata anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Cross-platform metadata handoff evidence packet names (deterministic tuple order):
  - `windows_type_metadata_handoff_packet`
  - `linux_type_metadata_handoff_packet`
  - `macos_type_metadata_handoff_packet`

Recommended cross-platform parity commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m219_sema_cross_platform_contract.py -q`

## M220 sema/type public-beta triage profile

For sema/type public-beta intake, triage, and patch loops, capture deterministic evidence in two replay-stable packets before promoting fixes to GA-bound lanes.

### 1.1 Deterministic semantic diagnostics intake packet

- Pass-order and diagnostics determinism anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics intake anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest intake counters under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.

### 1.2 Deterministic type-metadata triage/patch packet

- Sema handoff/parity anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest triage parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface patch-loop sizing anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).

Recommended public-beta triage loop commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m224_sema_release_contract.py -q`
4. `python -m pytest tests/tooling/test_objc3c_m220_sema_public_beta_contract.py -q`

## M221 sema/type GA blocker burn-down profile

To burn down sema/type GA blockers with deterministic, replay-stable evidence, capture two explicit packets before closing blocker state.

### 1.1 Deterministic semantic diagnostics blocker packet

- Pass-order and diagnostics determinism anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest blocker counters under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.

### 1.2 Deterministic type-metadata parity blocker packet

- Sema handoff/parity anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface blocker sizing anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).

Recommended burn-down commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m224_sema_release_contract.py -q`
4. `python -m pytest tests/tooling/test_objc3c_m221_sema_ga_blocker_contract.py -q`

## M218 sema/type RC provenance profile

For release-candidate automation and provenance attestation on the sema/type lane, capture deterministic evidence packets from replay-stable sema/type execution.

### 1.1 Deterministic semantic diagnostics RC evidence packet

- Pass-order and diagnostics determinism anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest RC diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- Deterministic release-candidate packet key for automation/attestation: `rc_sema_diagnostics_packet`.

### 1.2 Deterministic type-metadata RC provenance packet

- Sema handoff and parity anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest RC parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface provenance anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Deterministic release-candidate packet key for automation/attestation: `rc_type_metadata_handoff_packet`.

Recommended RC provenance commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m218_sema_rc_provenance_contract.py -q`

## M217 sema/type differential parity profile

To validate sema/type differential parity against baseline toolchains, capture deterministic evidence packets in stable tuple order: `native`, `baseline-clang`, `baseline-llvm-direct`.

### 1.1 Deterministic semantic diagnostics differential packet

- Pass-order and diagnostics determinism anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- Differential packet keys (deterministic tuple order):
  - `native_sema_diagnostics_packet`
  - `baseline_clang_sema_diagnostics_packet`
  - `baseline_llvm_direct_sema_diagnostics_packet`

### 1.2 Deterministic type-metadata handoff differential packet

- Sema handoff and parity anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface differential anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Differential packet keys (deterministic tuple order):
  - `native_type_metadata_handoff_packet`
  - `baseline_clang_type_metadata_handoff_packet`
  - `baseline_llvm_direct_type_metadata_handoff_packet`

Recommended differential parity commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m218_sema_rc_provenance_contract.py -q`
4. `python -m pytest tests/tooling/test_objc3c_m217_sema_differential_contract.py -q`

## M216 sema/type conformance suite profile

To operate deterministic sema/type conformance suite v1 replay, capture two evidence packets mapped to explicit spec sections.

Suite v1 spec-section packet map:

- `spec section 1.1 deterministic sema diagnostics` -> `m216_v1_sema_diagnostics_packet`
- `spec section 1.2 deterministic type-metadata handoff` -> `m216_v1_type_metadata_handoff_packet`

### 1.1 Deterministic sema diagnostics conformance packet

- Source anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- Deterministic suite-v1 evidence packet key: `m216_v1_sema_diagnostics_packet`.

### 1.2 Deterministic type-metadata handoff conformance packet

- Source anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Deterministic suite-v1 evidence packet key: `m216_v1_type_metadata_handoff_packet`.

Recommended conformance suite command sequence (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m216_sema_conformance_contract.py -q`

## M215 sema/type SDK packaging profile

For deterministic SDK/toolchain IDE consumption on the sema/type lane, capture two replay-stable evidence packets and ship them as the canonical packaging profile.

SDK packaging packet map:

- `sdk packet 1.1 deterministic sema diagnostics` -> `m215_sdk_sema_diagnostics_packet`
- `sdk packet 1.2 deterministic type-metadata handoff` -> `m215_sdk_type_metadata_handoff_packet`

### 1.1 Deterministic sema diagnostics SDK packaging packet

- Source anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- Deterministic SDK diagnostics packet key: `m215_sdk_sema_diagnostics_packet`.

### 1.2 Deterministic type-metadata handoff SDK packaging packet

- Source anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Deterministic SDK type-metadata packet key: `m215_sdk_type_metadata_handoff_packet`.

Recommended SDK packaging contract commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m215_sema_sdk_packaging_contract.py -q`

## M214 sema/type daemonized compiler profile

For persistent compiler/watch-mode execution on the sema/type lane, capture deterministic evidence packets per daemon cycle in stable order: `daemon-bootstrap`, `watch-incremental`, `watch-replay`.

Daemonized packet map:

- `daemon cycle 1.1 deterministic sema diagnostics` -> `m214_daemon_bootstrap_sema_diagnostics_packet`, `m214_watch_incremental_sema_diagnostics_packet`, `m214_watch_replay_sema_diagnostics_packet`
- `daemon cycle 1.2 deterministic type-metadata handoff` -> `m214_daemon_bootstrap_type_metadata_handoff_packet`, `m214_watch_incremental_type_metadata_handoff_packet`, `m214_watch_replay_type_metadata_handoff_packet`

### 1.1 Deterministic sema diagnostics daemonized packet

- Source anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- Daemonized diagnostics packet keys: `m214_daemon_bootstrap_sema_diagnostics_packet`, `m214_watch_incremental_sema_diagnostics_packet`, and `m214_watch_replay_sema_diagnostics_packet`.

### 1.2 Deterministic type-metadata handoff daemonized packet

- Source anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Daemonized type-metadata packet keys: `m214_daemon_bootstrap_type_metadata_handoff_packet`, `m214_watch_incremental_type_metadata_handoff_packet`, and `m214_watch_replay_type_metadata_handoff_packet`.

Recommended daemonized compiler commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m215_sema_sdk_packaging_contract.py -q`
4. `python -m pytest tests/tooling/test_objc3c_m214_sema_daemonized_contract.py -q`

## M213 sema/type debug-info fidelity profile

For deterministic sema/type debug-info fidelity across native debugger stacks, capture evidence packets in stable debug format order: `dwarf`, `pdb`.

Debug-info fidelity packet map:

- `debug format 1.1 deterministic sema diagnostics` -> `m213_dwarf_sema_diagnostics_packet`, `m213_pdb_sema_diagnostics_packet`
- `debug format 1.2 deterministic type-metadata + debug stepping fidelity` -> `m213_dwarf_type_metadata_stepping_packet`, `m213_pdb_type_metadata_stepping_packet`

### 1.1 Deterministic sema diagnostics debug-info packet

- Source anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- DWARF/PDB sema diagnostics packet keys: `m213_dwarf_sema_diagnostics_packet` and `m213_pdb_sema_diagnostics_packet`.

### 1.2 Deterministic type-metadata + debug stepping fidelity packet

- Source anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Debug stepping source-location anchors in manifest emission: `program.globals[i].line`, `program.globals[i].column`, `fn.line`, and `fn.column`.
- DWARF/PDB type-metadata + stepping packet keys: `m213_dwarf_type_metadata_stepping_packet` and `m213_pdb_type_metadata_stepping_packet`.

Recommended debug-info fidelity commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m214_sema_daemonized_contract.py -q`
4. `python -m pytest tests/tooling/test_objc3c_m213_sema_debug_fidelity_contract.py -q`

## M212 sema/type code-action profile

For compiler-driven code actions and refactoring safety, capture deterministic sema/type evidence packets from replay-stable sema execution before applying automated edits.

Code-action safety packet map:

- `code action packet 1.1 deterministic sema diagnostics` -> `m212_code_action_sema_diagnostics_packet`
- `code action packet 1.2 deterministic type-metadata handoff` -> `m212_code_action_type_metadata_handoff_packet`

### 1.1 Deterministic sema diagnostics code-action packet

- Source anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest diagnostics anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, and `deterministic_semantic_diagnostics`.
- Deterministic code-action sema packet key: `m212_code_action_sema_diagnostics_packet`.

### 1.2 Deterministic type-metadata handoff code-action packet

- Source anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Deterministic code-action type packet key: `m212_code_action_type_metadata_handoff_packet`.

Recommended code-action safety commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_sema_extraction.py -q`
2. `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`
3. `python -m pytest tests/tooling/test_objc3c_m213_sema_debug_fidelity_contract.py -q`
4. `python -m pytest tests/tooling/test_objc3c_m212_sema_code_action_contract.py -q`

## M211 sema/type LSP semantic profile

For semantic tokens/navigation support in LSP clients, capture deterministic sema/type evidence packets from parser token metadata, sema diagnostics ordering, and manifest source anchors before enabling editor integration.

LSP semantic packet map:

- `lsp packet 1.1 deterministic semantic-token metadata` -> `m211_lsp_semantic_token_metadata_packet`
- `lsp packet 1.2 deterministic navigation source-anchor` -> `m211_lsp_navigation_source_anchor_packet`

### 1.1 Deterministic semantic-token metadata packet

- Token metadata contract anchors: `Objc3SemaTokenMetadata` and `MakeObjc3SemaTokenMetadata(...)`.
- Parser capture anchors: `MakeSemaTokenMetadata(...)`, `Objc3SemaTokenKind::PointerDeclarator`, and `Objc3SemaTokenKind::NullabilitySuffix`.
- AST handoff anchors: `pointer_declarator_tokens`, `nullability_suffix_tokens`, `return_pointer_declarator_tokens`, and `return_nullability_suffix_tokens`.
- Deterministic semantic-token packet key: `m211_lsp_semantic_token_metadata_packet`.

### 1.2 Deterministic navigation source-anchor packet

- Sema diagnostics ordering anchor for stable locations: `CanonicalizePassDiagnostics(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest semantic-surface anchors for navigation indexing: `frontend.pipeline.semantic_surface`, `resolved_global_symbols`, and `resolved_function_symbols`.
- Manifest source-location anchors for go-to navigation: `program.globals[i].line`, `program.globals[i].column`, `fn.line`, and `fn.column`.
- Deterministic navigation packet key: `m211_lsp_navigation_source_anchor_packet`.

Recommended LSP semantic contract commands (sema/type lane):

1. `python -m pytest tests/tooling/test_objc3c_m211_frontend_lsp_contract.py -q`
2. `python -m pytest tests/tooling/test_objc3c_m211_sema_lsp_contract.py -q`

## M210 sema/type performance budgets and regression gates

For deterministic sema/type performance budgeting and regression gating, capture deterministic packet evidence from pass-manager budget counters, pipeline diagnostics transport, and manifest gate anchors before lane promotion.

Performance budget packet map:

- `budget packet 1.1 deterministic sema pass-manager budget counters` -> `m210_sema_pass_manager_budget_packet`
- `budget packet 1.2 deterministic sema/type regression gate anchors` -> `m210_sema_type_regression_gate_packet`

### 1.1 Deterministic sema pass-manager budget packet

- Source budget anchors: `kObjc3SemaPassOrder`, `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();`, `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`, and `IsMonotonicObjc3SemaDiagnosticsAfterPass(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest budget anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, `diagnostics_emitted_by_build`, `diagnostics_emitted_by_validate_bodies`, and `diagnostics_emitted_by_validate_pure_contract`.
- Deterministic sema budget packet key: `m210_sema_pass_manager_budget_packet`.

### 1.2 Deterministic sema/type regression gate anchor packet

- Source regression anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, `IsReadyObjc3SemaParityContractSurface(...)`, and `result.parity_surface.ready =`.
- Manifest regression-gate anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_monotonic`, `deterministic_semantic_diagnostics`, `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface regression anchors from `frontend.pipeline.semantic_surface`: `declared_globals`, `declared_functions`, `resolved_global_symbols`, and `resolved_function_symbols`.
- Deterministic sema/type regression packet key: `m210_sema_type_regression_gate_packet`.

Recommended M210 sema/type regression-gate validation command:

- `python -m pytest tests/tooling/test_objc3c_m210_sema_perf_regression_contract.py -q`

## M208 sema/type whole-module optimization controls

For deterministic sema/type whole-module optimization (WMO) controls, capture replay-stable packet evidence from sema pass ordering, integration-surface module shape, and manifest semantic-surface counters before enabling module-wide optimization heuristics.

Whole-module optimization control packet map:

- `wmo control packet 1.1 deterministic sema pass-order + integration surface` -> `m208_sema_pass_order_wmo_control_packet`
- `wmo control packet 1.2 deterministic type/symbol module-shape parity` -> `m208_type_symbol_module_shape_wmo_control_packet`

### 1.1 Deterministic sema pass-order + integration surface control packet

- Source control anchors: `kObjc3SemaPassOrder`, `BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);`, `ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);`, and `ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);`.
- Source diagnostics canonicalization anchor: `CanonicalizePassDiagnostics(...)`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Deterministic sema WMO control packet key: `m208_sema_pass_order_wmo_control_packet`.

### 1.2 Deterministic type/symbol module-shape parity control packet

- Source module-shape anchors: `BuildSemanticTypeMetadataHandoff(...)`, `result.parity_surface.globals_total = result.integration_surface.globals.size();`, `result.parity_surface.functions_total = result.integration_surface.functions.size();`, `result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();`, and `result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();`.
- Source readiness anchor: `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest WMO control anchors under `frontend.pipeline.sema_pass_manager`: `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Manifest semantic-surface module-shape anchors under `frontend.pipeline.semantic_surface`: `declared_globals`, `declared_functions`, `resolved_global_symbols`, and `resolved_function_symbols`.
- Deterministic type/symbol WMO control packet key: `m208_type_symbol_module_shape_wmo_control_packet`.

Recommended M208 sema/type whole-module optimization control command:

- `python -m pytest tests/tooling/test_objc3c_m208_sema_wmo_contract.py -q`

## M209 sema/type profile-guided optimization hooks

For deterministic sema/type profile-guided optimization (PGO) hooks, capture replay-stable packet evidence from sema pass diagnostics emission, type metadata handoff, and semantic-surface counters before changing optimization heuristics.

PGO hook packet map:

- `pgo hook packet 1.1 deterministic sema diagnostics emission profile` -> `m209_sema_diagnostics_emission_pgo_hook_packet`
- `pgo hook packet 1.2 deterministic type/symbol surface profile` -> `m209_type_symbol_surface_pgo_hook_packet`

### 1.1 Deterministic sema diagnostics emission profile hook packet

- Source hook anchors: `kObjc3SemaPassOrder`, `CanonicalizePassDiagnostics(...)`, `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`, and `result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest PGO hook anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_emitted_by_build`, `diagnostics_emitted_by_validate_bodies`, `diagnostics_emitted_by_validate_pure_contract`, and `diagnostics_monotonic`.
- Deterministic sema PGO hook packet key: `m209_sema_diagnostics_emission_pgo_hook_packet`.

### 1.2 Deterministic type/symbol surface profile hook packet

- Source hook anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, `result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();`, and `result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();`.
- Manifest type-metadata hook anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface hook anchors from `frontend.pipeline.semantic_surface`: `declared_globals`, `declared_functions`, `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Deterministic type/symbol PGO hook packet key: `m209_type_symbol_surface_pgo_hook_packet`.

Recommended M209 sema/type PGO regression gate command:

- `python -m pytest tests/tooling/test_objc3c_m209_sema_pgo_contract.py -q`

## M207 sema/type dispatch-specific optimization passes

For deterministic sema/type dispatch-specific optimization passes, capture replay-stable packet evidence from pass-manager dispatch ordering, message-send type/arity guard hooks, and manifest dispatch-control export surfaces.

Dispatch optimization packet map:

- `dispatch optimization packet 1.1 deterministic sema pass-manager dispatch hooks` -> `m207_sema_pass_dispatch_optimization_packet`
- `dispatch optimization packet 1.2 deterministic message-send type/arity optimization hooks` -> `m207_message_send_type_arity_optimization_packet`

### 1.1 Deterministic sema pass-manager dispatch optimization packet

- Source pass-dispatch anchors: `kObjc3SemaPassOrder`, `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `if (pass == Objc3SemaPassId::BuildIntegrationSurface) {`, `ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);`, and `CanonicalizePassDiagnostics(pass_diagnostics);`.
- Source pass diagnostics anchors: `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();` and `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest pass-manager anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, `diagnostics_emitted_by_build`, `diagnostics_emitted_by_validate_bodies`, and `diagnostics_emitted_by_validate_pure_contract`.
- Deterministic sema pass-dispatch packet key: `m207_sema_pass_dispatch_optimization_packet`.

### 1.2 Deterministic message-send type/arity optimization packet

- Source dispatch-option anchors: `Objc3SemanticValidationOptions`, `std::size_t max_message_send_args = 4;`, and `semantic_options.max_message_send_args = options.lowering.max_message_send_args;`.
- Source message-send hook anchors: `static ValueType ValidateMessageSendExpr(`, `if (receiver_type != ValueType::Unknown && !IsMessageI32CompatibleType(receiver_type)) {`, `if (expr->args.size() > max_message_send_args) {`, and `0, 0, options.max_message_send_args);`.
- Manifest dispatch-control anchors: `max_message_send_args`, `runtime_dispatch_symbol`, and `runtime_dispatch_arg_slots`.
- Deterministic message-send type/arity packet key: `m207_message_send_type_arity_optimization_packet`.

Recommended M207 sema/type dispatch-optimization validation command:

- `python -m pytest tests/tooling/test_objc3c_m207_sema_dispatch_optimizations_contract.py -q`

## M206 sema/type canonical optimization pipeline stage-1

For deterministic sema/type canonical optimization stage-1, capture replay-stable packet evidence from pass-level diagnostics canonicalization, canonical compatibility routing, and type-metadata handoff ordering.

Canonical optimization stage-1 packet map:

- `stage-1 packet 1.1 deterministic canonical sema diagnostics ordering hooks` -> `m206_canonical_sema_diagnostics_stage1_packet`
- `stage-1 packet 1.2 deterministic canonical type-metadata handoff hooks` -> `m206_canonical_type_metadata_stage1_packet`

### 1.1 Deterministic canonical sema diagnostics ordering packet

- Source canonical-routing anchors: `Objc3SemaCompatibilityMode::Canonical`, `kObjc3SemaPassOrder`, and `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`.
- Source canonical-diagnostics anchors: `CanonicalizePassDiagnostics(pass_diagnostics);`, `IsCanonicalPassDiagnostics(pass_diagnostics);`, `std::stable_sort(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);`, and `result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Manifest canonical-stage anchors under `frontend`: `compatibility_mode`, `frontend.pipeline.sema_pass_manager`, `deterministic_semantic_diagnostics`, and `diagnostics_monotonic`.
- Deterministic canonical sema diagnostics packet key: `m206_canonical_sema_diagnostics_stage1_packet`.

### 1.2 Deterministic canonical type-metadata handoff packet

- Source type-handoff anchors: `BuildSemanticTypeMetadataHandoff(...)`, `IsDeterministicSemanticTypeMetadataHandoff(...)`, `std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());`, and `std::sort(function_names.begin(), function_names.end());`.
- Source deterministic-validation anchors: `std::is_sorted(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end())`, `std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),`, and `result.deterministic_type_metadata_handoff =`.
- Source parity/readiness anchors: `result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();`, `result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();`, and `IsReadyObjc3SemaParityContractSurface(...)`.
- Manifest type-metadata anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface canonical-stage anchors from `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface` counters (`scalar_return_i32`, `scalar_return_bool`, `scalar_return_void`, `scalar_param_i32`, `scalar_param_bool`).
- Deterministic canonical type-metadata packet key: `m206_canonical_type_metadata_stage1_packet`.

Recommended M206 sema/type canonical optimization stage-1 validation command:

- `python -m pytest tests/tooling/test_objc3c_m206_sema_canonical_optimization_contract.py -q`

## M205 sema/type macro security policy enforcement

For deterministic sema/type macro-security policy enforcement, capture replay-stable packet evidence from migration-hint transport, canonical-only enforcement gating, fail-closed diagnostics, and manifest replay surfaces.

Macro security policy packet map:

- `macro policy packet 1.1 deterministic migration-hint transport hooks` -> `m205_macro_policy_migration_transport_packet`
- `macro policy packet 1.2 deterministic canonical macro-policy/type-surface hooks` -> `m205_macro_policy_canonical_type_surface_packet`

### 1.1 Deterministic migration-hint transport packet

- Source macro-hint capture anchors: `if (options_.migration_assist) {`, `++migration_hints_.legacy_yes_count;`, `++migration_hints_.legacy_no_count;`, and `++migration_hints_.legacy_null_count;`.
- Pipeline macro-hint transport anchors: `result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;`, `result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;`, `result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;`, `sema_input.migration_assist = options.migration_assist;`, `sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;`, `sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;`, and `sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;`.
- Source sema-input contract anchors: `bool migration_assist = false;`, `Objc3SemaMigrationHints migration_hints;`, and `if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {`.
- Deterministic migration-hint transport packet key: `m205_macro_policy_migration_transport_packet`.

### 1.2 Deterministic canonical macro-policy/type-surface packet

- Source canonical macro-policy anchors: `AppendMigrationAssistDiagnostics(input, pass_diagnostics);`, `append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");`, `append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");`, `append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");`, `"O3S216"`, and `CanonicalizePassDiagnostics(pass_diagnostics);`.
- Source deterministic type-surface anchors: `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, `result.deterministic_type_metadata_handoff =`, and `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`.
- Manifest macro-policy anchors under `frontend`: `migration_assist`, `migration_hints`, `legacy_yes`, `legacy_no`, `legacy_null`, and `legacy_total`.
- Manifest sema/type readiness anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_semantic_diagnostics`, `deterministic_type_metadata_handoff`, and `parity_ready`.
- Deterministic canonical macro-policy/type-surface packet key: `m205_macro_policy_canonical_type_surface_packet`.

Recommended M205 sema/type macro-security validation command:

- `python -m pytest tests/tooling/test_objc3c_m205_sema_macro_security_contract.py -q`

## M204 sema/type macro diagnostics and provenance

For deterministic sema/type macro diagnostics and provenance, capture replay-stable packet evidence from macro-hint intake, canonical sema diagnostics emission, and manifest provenance surfaces.

Macro diagnostics/provenance packet map:

- `macro diagnostics packet 1.1 deterministic canonical sema diagnostics hooks` -> `m204_macro_diagnostics_packet`
- `macro provenance packet 1.2 deterministic sema/type provenance hooks` -> `m204_macro_provenance_packet`

### 1.1 Deterministic canonical sema diagnostics packet

- Source macro-diagnostics anchors: `if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {`, `AppendMigrationAssistDiagnostics(input, pass_diagnostics);`, `"O3S216"`, and `CanonicalizePassDiagnostics(pass_diagnostics);`.
- Source macro-hint intake anchors: `if (options_.migration_assist) {`, `++migration_hints_.legacy_yes_count;`, `++migration_hints_.legacy_no_count;`, and `++migration_hints_.legacy_null_count;`.
- Pipeline diagnostics transport anchor: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`.
- Source deterministic sema diagnostics anchors: `result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;` and `result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;`.
- Deterministic canonical sema diagnostics packet key: `m204_macro_diagnostics_packet`.

### 1.2 Deterministic sema/type provenance packet

- Source sema-input provenance anchors: `bool migration_assist = false;`, `Objc3SemaMigrationHints migration_hints;`, `sema_input.migration_assist = options.migration_assist;`, `sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;`, `sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;`, and `sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;`.
- Source type/provenance determinism anchors: `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, `result.deterministic_type_metadata_handoff =`, and `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`.
- Manifest macro provenance anchors under `frontend`: `migration_assist`, `migration_hints`, `legacy_yes`, `legacy_no`, `legacy_null`, and `legacy_total`.
- Manifest sema/type provenance anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_semantic_diagnostics`, `deterministic_type_metadata_handoff`, and `parity_ready`.
- Deterministic sema/type provenance packet key: `m204_macro_provenance_packet`.

Recommended M204 sema/type macro diagnostics/provenance validation command:

- `python -m pytest tests/tooling/test_objc3c_m204_sema_macro_diagnostics_contract.py -q`

## M203 sema/type compile-time evaluation engine

For deterministic sema/type compile-time evaluation engine behavior, capture replay-stable packet evidence from sema global constant-expression resolution, static type-flow scalar evaluation, and lowering compile-time proof propagation.

Compile-time evaluation packet map:

- `compile-time eval packet 1.1 deterministic sema global const-expression hooks` -> `m203_sema_global_const_eval_packet`
- `compile-time eval packet 1.2 deterministic type/static-flow compile-time hooks` -> `m203_type_static_flow_compile_time_packet`

### 1.1 Deterministic sema global const-expression packet

- Source const-eval engine anchors: `static bool EvalConstExpr(const Expr *expr, int &value,`, `bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values)`, `if (!EvalConstExpr(global.value.get(), value, &resolved_global_values)) {`, and `MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression")`.
- Artifact const-eval failure anchor: `MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")`.
- Deterministic sema global const-eval packet key: `m203_sema_global_const_eval_packet`.

### 1.2 Deterministic type/static-flow compile-time packet

- Static analysis compile-time engine anchors: `using StaticScalarBindings = std::unordered_map<std::string, int>;`, `bool TryEvalStaticScalarValue(const Expr *expr, int &value, const StaticScalarBindings *bindings);`, `return TryEvalStaticArithmeticBinary(expr->op, lhs, rhs, value);`, `return TryEvalStaticBitwiseShiftBinary(expr->op, lhs, rhs, value);`, and `if (TryEvalStaticScalarValue(stmt->switch_stmt->condition.get(), static_switch_value, bindings)) {`.
- Lowering compile-time proof anchors: `bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {`, `bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {`, `ctx.const_value_ptrs[ptr] = assigned_const_value;`, and `lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);`.
- Deterministic type/static-flow compile-time packet key: `m203_type_static_flow_compile_time_packet`.

Recommended M203 sema/type compile-time evaluation command:

- `python -m pytest tests/tooling/test_objc3c_m203_sema_compile_time_eval_contract.py -q`

## O3S201..O3S216 behavior (implemented now)

- `O3S201`:
  - Duplicate parameter names within a single function parameter list.
  - Duplicate local `let` declaration names in the same lexical scope.
- `O3S202`:
  - Identifier lookup failed in local scopes and globals.
- `O3S203`:
  - Call target name does not match a declared function.
- `O3S204`:
  - Function exists but call argument count does not match declared arity.
- `O3S205`:
  - Function body is not proven to always return.
  - A body is accepted when an explicit `return` is guaranteed by control flow (including `if/else` where both branches always return).
- `O3S206`:
  - Function symbol used as a value expression.
  - Arithmetic/relational operands with non-`i32` type.
  - Equality between incompatible types, except `bool` compared with literal `0` or `1`.
  - Logical operands that are neither `bool` nor `i32`.
  - Call argument type mismatch against declared parameter types.
  - Message argument type mismatch.
  - `i32` arguments are accepted for `bool` parameters at call sites.
- `O3S207`:
  - Message receiver is not `i32`-compatible.
- `O3S208`:
  - Message-send expressions exceed configured arg cap (default `4`, configurable via `--objc3-max-message-args`).
- `O3S209`:
  - Message argument expression is not `i32`-compatible.
- `O3S211`:
  - Return expression type does not match the declared function return type.
  - `bool` return annotations accept `0`/`1` literals as bool-compatible returns.
- `O3S212`:
  - `break` used outside an active loop context.
- `O3S213`:
  - `continue` used outside an active loop context.
- `O3S214`:
  - Assignment target must be an existing mutable local or global symbol.
- `O3S216`:
  - migration assist requires canonical literal replacement for legacy Objective-C aliases (`YES`, `NO`, `NULL`) in canonical compatibility mode.

## M223 semantic migration operator guide

Compatibility/migration diagnostic behavior (implemented now):

- `compatibility_mode=canonical` + `migration_assist=false`
  - legacy aliases are still token-normalized for execution compatibility.
  - no migration-assist diagnostics are emitted.
- `compatibility_mode=canonical` + `migration_assist=true`
  - deterministic `O3S216` diagnostics are emitted for each encountered legacy alias family (`YES`, `NO`, `NULL`), including occurrence counts.
- `compatibility_mode=legacy` + `migration_assist=true`
  - migration-assist diagnostics are suppressed to preserve legacy-compatible operator flow while still recording migration hints in artifacts.

Operator triage workflow:

1. inspect `module.manifest.json` `frontend.migration_hints` counts.
2. inspect `module.diagnostics.txt` for `O3S216` rows (canonical + assist mode).
3. replace legacy aliases (`YES`->`true`, `NO`->`false`, `NULL`->`nil`) and replay.

Parser/lexer diagnostics currently emitted include:

- `O3L001` unexpected character
- `O3L002` unterminated block comment
- `O3L003` nested block comments are unsupported
- `O3L004` stray block comment terminator
- `O3P100` unsupported top-level statement
  - Also emitted for invalid declaration forms missing `fn` after `pure`/`extern`.
  - Also emitted for duplicate declaration qualifiers (`duplicate 'pure' qualifier in function declaration`, `duplicate 'extern' qualifier in function declaration`).
  - Also emitted for misplaced declaration qualifiers after `fn` (`unexpected qualifier 'pure' after 'fn'`, `unexpected qualifier 'extern' after 'fn'`).
  - Also emitted for misplaced declaration qualifiers after function identifiers (`unexpected qualifier 'pure' after function name`, `unexpected qualifier 'extern' after function name`).
  - Also emitted for misplaced declaration qualifiers after parameter lists (`unexpected qualifier 'pure' after parameter list`, `unexpected qualifier 'extern' after parameter list`).
  - Also emitted for misplaced declaration qualifiers after function return annotations (`unexpected qualifier 'pure' after function return annotation`, `unexpected qualifier 'extern' after function return annotation`).
  - Also emitted for misplaced qualifiers in parameter type annotations (`unexpected qualifier 'pure' in parameter type annotation`, `unexpected qualifier 'extern' in parameter type annotation`).
  - Also emitted for misplaced qualifiers in function return type annotations (`unexpected qualifier 'pure' in function return type annotation`, `unexpected qualifier 'extern' in function return type annotation`).
  - Also emitted for misplaced qualifiers after parameter type annotations (`unexpected qualifier 'pure' after parameter type annotation`, `unexpected qualifier 'extern' after parameter type annotation`).
  - Also emitted for misplaced qualifiers in parameter identifier positions (`unexpected qualifier 'pure' in parameter identifier position`, `unexpected qualifier 'extern' in parameter identifier position`).
  - Also emitted for misplaced qualifiers after parameter names (`unexpected qualifier 'pure' after parameter name`, `unexpected qualifier 'extern' after parameter name`).
  - Also emitted for misplaced qualifiers in statement positions (`unexpected qualifier 'pure' in statement position`, `unexpected qualifier 'extern' in statement position`).
  - Also emitted for misplaced qualifiers in expression positions (`unexpected qualifier 'pure' in expression position`, `unexpected qualifier 'extern' in expression position`).
  - Also emitted for misplaced qualifiers in message selector positions (`unexpected qualifier 'pure' in message selector position`, `unexpected qualifier 'extern' in message selector position`, `unexpected qualifier 'pure' in keyword selector segment position`, `unexpected qualifier 'extern' in keyword selector segment position`).
  - Also emitted for misplaced qualifiers in case-label expressions (`unexpected qualifier 'pure' in case label expression`, `unexpected qualifier 'extern' in case label expression`).
- `O3P101` invalid identifier in declaration positions
- `O3P102` missing `=`
- `O3P103` invalid expression
  - Also emitted for malformed switch case-label expressions, including signed-label forms missing numeric literals.
- `O3P104` missing `;`
  - Also emitted for `extern`-qualified declarations that do not terminate as prototypes.
- `O3P106` missing `(`
- `O3P107` missing `:`
- `O3P108` expected parameter type `i32`, `bool`, `BOOL`, `NSInteger`, `NSUInteger`, or `id`
  - Also accepts `Class`.
  - Also accepts `SEL`.
  - Also accepts `Protocol`.
  - Also accepts `instancetype`.
  - Also used for unsupported named parameter types and malformed generic suffix parse forms.
- `O3P109` missing `)`
- `O3P110` missing `{`
- `O3P111` missing `}`
- `O3P112` call target must be identifier
- `O3P113` invalid or unterminated message-send expression
  - Receiver fallback diagnostic is emitted only when receiver parsing fails without a prior primary receiver diagnostic.
- `O3P114` malformed or unsupported function return annotation type
  - Also accepts `Class`.
  - Also accepts `SEL`.
  - Also accepts `Protocol`.
  - Also accepts `instancetype`.
  - Also used for unterminated generic function return type suffix.

## Sema diagnostics bus contract (M139-E001)

- The frontend diagnostics bus is defined at:
  - `native/objc3c/src/parse/objc3_diagnostics_bus.h`
  - `Objc3FrontendDiagnosticsBus.{lexer,parser,semantic}` is the canonical deterministic diagnostics transport packet.
- Pipeline sema pass manager flow remains deterministic and fail-closed:
  - `RunObjc3SemaPassManager(...)` publishes each pass batch into `result.stage_diagnostics.semantic`.
  - `BuildIntegrationSurface`, `ValidateBodies`, and `ValidatePureContract` pass IDs run in deterministic order.
- Stage-local diagnostics are folded into final parsed-program diagnostics through:
  - `TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program)`
  - deterministic insert order: `lexer`, then `parser`, then `semantic`.

## Artifacts and exit codes

For `.objc3` input:

- Always writes:
  - `<prefix>.diagnostics.txt`
  - `<prefix>.diagnostics.json`
- On success writes:
  - `<prefix>.manifest.json` (module/frontend/lowering/globals/functions)
  - `<prefix>.ll` (emitted IR)
  - `<prefix>.obj` (compiled from IR)

For non-`.objc3` input:

- Always writes:
  - `<prefix>.diagnostics.txt`
  - `<prefix>.diagnostics.json`
- On success writes:
  - `<prefix>.manifest.json` (libclang symbol rows)
  - `<prefix>.obj` (compiled Objective-C object)

Exit codes:

- `0`: success
- `1`: parse/semantic/diagnostic failure
- `2`: CLI usage / missing input / invalid arg / missing explicit clang path
- `3`: clang compile step failed

## M223 lowering/IR metadata envelope

Native `.objc3` IR emission now includes deterministic frontend-profile metadata in addition to lowering boundary replay data:

- Prologue comment:
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
- Named LLVM metadata payload:
  - `!objc3.frontend = !{!0}`
  - `!0 = !{i32 <language_version>, !"compatibility_mode", i1 <migration_assist>, i64 <legacy_yes>, i64 <legacy_no>, i64 <legacy_null>, i64 <legacy_total>}`

Operator replay check (from repo root):

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata --emit-prefix module
```

Then inspect:

- `tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.manifest.json`

Both artifacts should present aligned compatibility/migration profile information for deterministic replay triage.

## M213 lowering/runtime debug-info fidelity profile

Lowering/runtime debug-info fidelity is captured as a deterministic packet rooted under `tmp/` to preserve replay-stable source mapping evidence.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/`
  - `tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/debug-metadata-markers.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `debug metadata markers` (required in debug metadata marker extracts):
  - `source_filename = "<module>.objc3"`
  - `"source":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `out << "source_filename = \"" << program_.module_name << ".objc3"\n\n";`
  - `manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";`
  - `manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]`
  - `<< ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and debug metadata marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, debug metadata marker, or source anchor is missing.

Debug-info fidelity capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.ll tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.manifest.json > tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/abi-ir-anchors.txt`
3. `rg -n "source_filename =|\"source\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":" tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.ll tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.manifest.json tmp/artifacts/compilation/objc3c-native/m213/lowering-runtime-debug-info-fidelity/module.diagnostics.json > tmp/reports/objc3c-native/m213/lowering-runtime-debug-info-fidelity/debug-metadata-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m213_lowering_debug_fidelity_contract.py -q`

## M212 lowering/runtime code-action profile

Lowering/runtime evidence for the refactor/code-action engine is captured as a deterministic packet rooted under `tmp/` so rewrite application is replay-stable and auditable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/`
  - `tmp/reports/objc3c-native/m212/lowering-runtime-code-action/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m212/lowering-runtime-code-action/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `rewrite markers` (required in rewrite marker extracts):
  - `@@ rewrite_scope:module`
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `"source":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";`
  - `manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]`
  - `<< ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";`
  - `<< ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and rewrite marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, rewrite marker, or source anchor is missing.

Code-action capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.ll tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json > tmp/reports/objc3c-native/m212/lowering-runtime-code-action/abi-ir-anchors.txt`
3. `@("@@ rewrite_scope:module") | Set-Content tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt; rg -n "runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp >> tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt; rg -n "\"source\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":" tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.diagnostics.json >> tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m212_lowering_code_action_contract.py -q`

## M211 lowering/runtime LSP semantic profile

Lowering/runtime semantic token and symbol-navigation evidence is captured as a deterministic packet rooted under `tmp/` for replay-stable LSP contract validation.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/`
  - `tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `symbol-navigation markers` (required in marker extracts):
  - `@@ lsp_profile:semantic_tokens_navigation`
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `"semantic_surface":`
  - `"declared_globals":`
  - `"declared_functions":`
  - `"resolved_global_symbols":`
  - `"resolved_function_symbols":`
  - `"globals":`
  - `"functions":`
  - `"name":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()`
  - `manifest << ",\"declared_functions\":" << manifest_functions.size()`
  - `manifest << ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()`
  - `manifest << ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()`
  - `manifest << "  \"globals\": [\n";`
  - `manifest << "  \"functions\": [\n";`
  - `manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]`
  - `<< ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";`
  - `manifest << "    {\"name\":\"" << fn.name << "\",\"params\":" << fn.params.size() << ",\"param_types\":[";`
  - `<< ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and symbol-navigation marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, symbol-navigation marker, or source anchor is missing.

LSP semantic profile capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.ll tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.manifest.json > tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/abi-ir-anchors.txt`
3. `@("@@ lsp_profile:semantic_tokens_navigation") | Set-Content tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt; rg -n "runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp >> tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt; rg -n "\"semantic_surface\":|\"declared_globals\":|\"declared_functions\":|\"resolved_global_symbols\":|\"resolved_function_symbols\":|\"globals\":|\"functions\":|\"name\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":" tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.manifest.json tmp/artifacts/compilation/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/module.diagnostics.json >> tmp/reports/objc3c-native/m211/lowering-runtime-lsp-semantic-profile/symbol-navigation-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m211_lowering_lsp_contract.py -q`

## M206 lowering/runtime canonical optimization pipeline stage-1

Lowering/runtime canonical optimization stage-1 evidence is captured as deterministic packet artifacts rooted under `tmp/` so replay checks stay stable across optimizer reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/`
  - `tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/canonical-optimization-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `canonical optimization stage-1 markers` (required in source-anchor extracts):
  - `runtime_dispatch_call_emitted_ = false;`
  - `runtime_dispatch_call_emitted_ = true;`
  - `receiver_is_compile_time_zero`
  - `receiver_is_compile_time_nonzero`
  - `FunctionMayHaveGlobalSideEffects`
  - `call_may_have_global_side_effects`
  - `global_proofs_invalidated`
  - `semantic_surface`
  - `function_signature_surface`
  - `scalar_return_i32`
  - `scalar_return_bool`
  - `scalar_return_void`
  - `scalar_param_i32`
  - `scalar_param_bool`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);`
  - `lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);`
  - `if (lowered.receiver_is_compile_time_zero) {`
  - `if (lowered.receiver_is_compile_time_nonzero) {`
  - `const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);`
  - `if (call_may_have_global_side_effects) {`
  - `ctx.global_proofs_invalidated = true;`
  - `manifest_functions.reserve(program.functions.size())`
  - `std::unordered_set<std::string> manifest_function_names`
  - `if (manifest_function_names.insert(fn.name).second)`
  - `manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()`
  - `<< ",\"declared_functions\":" << manifest_functions.size()`
  - `<< ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()`
  - `<< ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()`
  - `<< ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32`
  - `<< ",\"scalar_return_bool\":" << scalar_return_bool`
  - `<< ",\"scalar_return_void\":" << scalar_return_void << ",\"scalar_param_i32\":" << scalar_param_i32`
  - `<< ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and canonical optimization source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, canonical optimization marker, or source anchor is missing.

Canonical optimization stage-1 capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1 --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.ll tmp/artifacts/compilation/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/module.manifest.json > tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/abi-ir-anchors.txt`
3. `rg -n "runtime_dispatch_call_emitted_|receiver_is_compile_time_zero|receiver_is_compile_time_nonzero|FunctionMayHaveGlobalSideEffects|call_may_have_global_side_effects|global_proofs_invalidated|manifest_functions\.reserve\(program\.functions\.size\(\)\)|manifest_function_names|function_signature_surface|scalar_return_i32|scalar_return_bool|scalar_return_void|scalar_param_i32|scalar_param_bool|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m206/lowering-runtime-canonical-optimization-stage-1/canonical-optimization-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m206_lowering_canonical_optimization_contract.py -q`

## M204 lowering/runtime macro diagnostics and provenance

Lowering/runtime macro diagnostics and provenance evidence is captured as deterministic packet artifacts rooted under `tmp/` so pragma diagnostics metadata and lowering replay boundaries remain auditable and stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/`
  - `tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/macro-diagnostics-provenance-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `macro diagnostics/provenance markers` (required in source-anchor extracts):
  - `MakeDiag(...)`
  - `error:<line>:<column>: <message> [<code>]`
  - `ConsumeLanguageVersionPragmas(diagnostics)`
  - `ConsumeLanguageVersionPragmaDirective(...)`
  - `O3L005`
  - `O3L006`
  - `O3L007`
  - `O3L008`
  - `first_line`
  - `first_column`
  - `last_line`
  - `last_column`
  - `ParseDiagSortKey(...)`
  - `"severity"`
  - `"line"`
  - `"column"`
  - `"code"`
  - `"message"`
  - `"raw"`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";`
  - `ConsumeLanguageVersionPragmas(diagnostics);`
  - `ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));`
  - `diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));`
  - `language_version_pragma_contract_.first_line = line;`
  - `language_version_pragma_contract_.first_column = column;`
  - `language_version_pragma_contract_.last_line = line;`
  - `language_version_pragma_contract_.last_column = column;`
  - `result.language_version_pragma_contract.first_line = pragma_contract.first_line;`
  - `result.language_version_pragma_contract.first_column = pragma_contract.first_column;`
  - `result.language_version_pragma_contract.last_line = pragma_contract.last_line;`
  - `result.language_version_pragma_contract.last_column = pragma_contract.last_column;`
  - `manifest << "    \"language_version_pragma_contract\":{\"seen\":"`
  - `<< ",\"first_line\":" << pipeline_result.language_version_pragma_contract.first_line`
  - `<< ",\"first_column\":" << pipeline_result.language_version_pragma_contract.first_column`
  - `<< ",\"last_line\":" << pipeline_result.language_version_pragma_contract.last_line`
  - `<< ",\"last_column\":" << pipeline_result.language_version_pragma_contract.last_column << "},\n";`
  - `const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);`
  - `out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line`
  - `<< ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""`
  - `<< EscapeJsonString(key.message) << "\",\"raw\":\"" << EscapeJsonString(diagnostics[i]) << "\"}";`
  - `WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and macro diagnostics/provenance source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, macro diagnostics/provenance marker, or source anchor is missing.

Macro diagnostics/provenance capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.ll tmp/artifacts/compilation/objc3c-native/m204/lowering-runtime-macro-diagnostics/module.manifest.json > tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/abi-ir-anchors.txt`
3. `rg -n "MakeDiag\(|error:|ConsumeLanguageVersionPragmas\(diagnostics\)|ConsumeLanguageVersionPragmaDirective\(|O3L005|O3L006|O3L007|O3L008|first_line|first_column|last_line|last_column|ParseDiagSortKey\(|\"severity\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/lex/objc3_lexer.cpp native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/io/objc3_diagnostics_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m204/lowering-runtime-macro-diagnostics/macro-diagnostics-provenance-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m204_lowering_macro_diagnostics_contract.py -q`

## M203 lowering/runtime compile-time evaluation engine

Lowering/runtime compile-time evaluation engine evidence is captured as deterministic packet artifacts rooted under `tmp/` so constant-evaluation lowering and runtime dispatch fast-path replay remains auditable and stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/`
  - `tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/compile-time-eval-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `compile-time evaluation markers` (required in source-anchor extracts):
  - `TryGetCompileTimeI32ExprInContext`
  - `IsCompileTimeNilReceiverExprInContext`
  - `IsCompileTimeKnownNonNilExprInContext`
  - `has_assigned_const_value`
  - `has_assigned_nil_value`
  - `has_clause_const_value`
  - `has_let_const_value`
  - `const_value_ptrs`
  - `nil_bound_ptrs`
  - `nonzero_bound_ptrs`
  - `global_proofs_invalidated`
  - `receiver_is_compile_time_zero`
  - `receiver_is_compile_time_nonzero`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `const bool has_assigned_const_value =`
  - `op == "=" && value_expr != nullptr && TryGetCompileTimeI32ExprInContext(value_expr, ctx, assigned_const_value);`
  - `const bool has_assigned_nil_value = op == "=" && value_expr != nullptr && IsCompileTimeNilReceiverExprInContext(value_expr, ctx);`
  - `ctx.const_value_ptrs.erase(ptr);`
  - `const bool has_clause_const_value = TryGetCompileTimeI32ExprInContext(clause.value.get(), ctx, clause_const_value);`
  - `const bool has_let_const_value = TryGetCompileTimeI32ExprInContext(let->value.get(), ctx, let_const_value);`
  - `bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {`
  - `bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {`
  - `if (expr->op == "&&" || expr->op == "||") {`
  - `if (expr->op == "<<" || expr->op == ">>") {`
  - `bool IsCompileTimeKnownNonNilExprInContext(const Expr *expr, const FunctionContext &ctx) const {`
  - `lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);`
  - `lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);`
  - `if (lowered.receiver_is_compile_time_zero) {`
  - `if (lowered.receiver_is_compile_time_nonzero) {`
  - `ctx.global_proofs_invalidated = true;`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and compile-time-evaluation source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, compile-time evaluation marker, or source anchor is missing.

Compile-time evaluation engine capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.ll tmp/artifacts/compilation/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/module.manifest.json > tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/abi-ir-anchors.txt`
3. `rg -n "TryGetCompileTimeI32ExprInContext|IsCompileTimeNilReceiverExprInContext|IsCompileTimeKnownNonNilExprInContext|has_assigned_const_value|has_assigned_nil_value|has_clause_const_value|has_let_const_value|const_value_ptrs|nil_bound_ptrs|nonzero_bound_ptrs|global_proofs_invalidated|receiver_is_compile_time_zero|receiver_is_compile_time_nonzero|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m203/lowering-runtime-compile-time-eval-engine/compile-time-eval-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m203_lowering_compile_time_eval_contract.py -q`

## M205 lowering/runtime macro security policy enforcement

Lowering/runtime macro-security policy enforcement evidence is captured as deterministic packet artifacts rooted under `tmp/` so pragma-policy diagnostics and lowering replay boundaries remain auditable and stable across reruns.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/`
  - `tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/macro-security-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `macro-security markers` (required in source-anchor extracts):
  - `ConsumeLanguageVersionPragmas(diagnostics)`
  - `ConsumeLanguageVersionPragmaDirective(...)`
  - `LanguageVersionPragmaPlacement::kNonLeading`
  - `O3L005`
  - `O3L006`
  - `O3L007`
  - `O3L008`
  - `frontend.language_version_pragma_contract`
  - `directive_count`
  - `duplicate`
  - `non_leading`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `ConsumeLanguageVersionPragmas(diagnostics);`
  - `ConsumeLanguageVersionPragmaDirective(diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false))`
  - `if (placement == LanguageVersionPragmaPlacement::kNonLeading) {`
  - `diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));`
  - `diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));`
  - `result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;`
  - `result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;`
  - `result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;`
  - `manifest << "    \"language_version_pragma_contract\":{\"seen\":"`
  - `<< ",\"directive_count\":" << pipeline_result.language_version_pragma_contract.directive_count`
  - `<< ",\"duplicate\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")`
  - `<< ",\"non_leading\":"`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and macro-security source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, macro-security marker, or source anchor is missing.

Macro-security capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.ll tmp/artifacts/compilation/objc3c-native/m205/lowering-runtime-macro-security/module.manifest.json > tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/abi-ir-anchors.txt`
3. `rg -n "ConsumeLanguageVersionPragmas\(diagnostics\)|ConsumeLanguageVersionPragmaDirective\(|LanguageVersionPragmaPlacement::kNonLeading|O3L005|O3L006|O3L007|O3L008|language_version_pragma_contract|directive_count|duplicate|non_leading|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/lex/objc3_lexer.cpp native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m205/lowering-runtime-macro-security/macro-security-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m205_lowering_macro_security_contract.py -q`

## M207 lowering/runtime dispatch-specific optimization passes

Lowering/runtime dispatch-specific optimization pass evidence is captured as deterministic packet artifacts rooted under `tmp/` so nil-elision, non-nil fast-path routing, and usage-driven runtime-dispatch declaration emission remain replay-stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/`
  - `tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.diagnostics.json`
  - `tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/dispatch-optimization-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `dispatch optimization markers` (required in source-anchor extracts):
  - `runtime_dispatch_call_emitted_ = false;`
  - `runtime_dispatch_call_emitted_ = true;`
  - `receiver_is_compile_time_zero`
  - `receiver_is_compile_time_nonzero`
  - `msg_nil_`
  - `msg_dispatch_`
  - `phi i32 [0, %`
  - `FunctionMayHaveGlobalSideEffects`
  - `call_may_have_global_side_effects`
  - `global_proofs_invalidated`
  - `runtime_dispatch_symbol`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `if (runtime_dispatch_call_emitted_) {`
  - `lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);`
  - `lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);`
  - `if (lowered.receiver_is_compile_time_zero) {`
  - `if (lowered.receiver_is_compile_time_nonzero) {`
  - `const std::string nil_label = NewLabel(ctx, "msg_nil_");`
  - `const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");`
  - `ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +`
  - `const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);`
  - `if (call_may_have_global_side_effects) {`
  - `ctx.global_proofs_invalidated = true;`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.
  - ABI/IR anchor extracts and dispatch optimization source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, dispatch optimization marker, or source anchor is missing.

Dispatch optimization capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.ll tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.manifest.json > tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/abi-ir-anchors.txt`
3. `rg -n "runtime_dispatch_call_emitted_|receiver_is_compile_time_zero|receiver_is_compile_time_nonzero|msg_nil_|msg_dispatch_|phi i32 \[0, %|FunctionMayHaveGlobalSideEffects|call_may_have_global_side_effects|global_proofs_invalidated|Objc3LoweringIRBoundaryReplayKey\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/dispatch-optimization-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m207_lowering_dispatch_optimizations_contract.py -q`

## M208 lowering/runtime whole-module optimization controls

Lowering/runtime whole-module optimization (WMO) controls are captured as deterministic packet artifacts rooted under `tmp/` so module-shape and runtime-dispatch surfaces remain replay-stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/`
  - `tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.manifest.json`
  - `tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/wmo-control-source-anchors.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `whole-module control markers` (required in source-anchor extracts):
  - `max_message_send_args`
  - `semantic_surface`
  - `declared_functions`
  - `resolved_function_symbols`
  - `runtime_dispatch_arg_slots`
  - `selector_global_ordering`
- `source anchors`:
  - `manifest_functions.reserve(program.functions.size())`
  - `std::unordered_set<std::string> manifest_function_names`
  - `if (manifest_function_names.insert(fn.name).second)`
  - `manifest << "    \"max_message_send_args\":" << options.lowering.max_message_send_args << ",\n";`
  - `manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()`
  - `<< ",\"declared_functions\":" << manifest_functions.size()`
  - `<< ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()`
  - `if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {`
  - `error = "invalid lowering contract max_message_send_args: "`
  - `boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;`
  - `boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;`
  - `if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {`
  - `lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");`
  - `call << "  " << dispatch_value << " = call i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32 "`
- `closure criteria`:
  - rerunning identical source + lowering/runtime options preserves byte-identical `module.ll` and `module.manifest.json`.
  - ABI/IR anchors and WMO control source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, whole-module control marker, or source anchor is missing.

WMO control capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.ll tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.manifest.json > tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/abi-ir-anchors.txt`
3. `rg -n "manifest_functions\\.reserve\\(program\\.functions\\.size\\(\\)\\)|manifest_function_names|max_message_send_args|semantic_surface|declared_functions|resolved_function_symbols|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp > tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/wmo-control-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m208_lowering_wmo_contract.py -q`

## M209 lowering/runtime profile-guided optimization hooks

Lowering/runtime LLVM profile-guided optimization (PGO) hook evidence is captured as deterministic packet artifacts rooted under `tmp/` so profile surfaces remain replay-stable.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/`
  - `tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.manifest.json`
  - `tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/pgo-hook-source-anchors.txt`
- `PGO hook ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `!0 = !{i32 <language_version>, !"compatibility_mode", i1 <migration_assist>, i64 <legacy_yes>, i64 <legacy_no>, i64 <legacy_null>, i64 <legacy_total>}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `source anchors`:
  - `Objc3IRFrontendMetadata ir_frontend_metadata;`
  - `ir_frontend_metadata.language_version = options.language_version;`
  - `ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);`
  - `ir_frontend_metadata.migration_assist = options.migration_assist;`
  - `ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;`
  - `ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;`
  - `ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;`
  - `out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";`
  - `out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)`
  - `out << "!objc3.frontend = !{!0}\n";`
  - `out << "!0 = !{i32 " << static_cast<unsigned>(frontend_metadata_.language_version) << ", !\""`
  - `out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";`
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `<< "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args`
  - `<< ",\"selector_global_ordering\":\"lexicographic\"},\n";`
- `closure criteria`:
  - rerunning identical source + lowering/runtime options preserves byte-identical `module.ll` and `module.manifest.json`.
  - PGO hook ABI/IR anchors and source-anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, or source anchor is missing.

PGO hook capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|!0 = !{|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.ll tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.manifest.json > tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/abi-ir-anchors.txt`
3. `rg -n "Objc3IRFrontendMetadata ir_frontend_metadata;|ir_frontend_metadata\\.language_version = options\\.language_version;|ir_frontend_metadata\\.compatibility_mode = CompatibilityModeName\\(options\\.compatibility_mode\\);|ir_frontend_metadata\\.migration_assist = options\\.migration_assist;|ir_frontend_metadata\\.migration_legacy_yes = pipeline_result\\.migration_hints\\.legacy_yes_count;|ir_frontend_metadata\\.migration_legacy_no = pipeline_result\\.migration_hints\\.legacy_no_count;|ir_frontend_metadata\\.migration_legacy_null = pipeline_result\\.migration_hints\\.legacy_null_count;|Objc3LoweringIRBoundaryReplayKey\\(|invalid lowering contract runtime_dispatch_symbol|runtime_dispatch_symbol=|runtime_dispatch_arg_slots=|selector_global_ordering=lexicographic" native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/pgo-hook-source-anchors.txt`
4. `python -m pytest tests/tooling/test_objc3c_m209_lowering_pgo_contract.py -q`

## M210 lowering/runtime performance budgets and regression gates

Lowering/LLVM/runtime perf regression evidence is captured as a deterministic packet rooted under `tmp/` so throughput budget and cache-proof gates fail closed.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/`
  - `tmp/artifacts/objc3c-native/perf-budget/<run_id>/`
  - `tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.manifest.json`
  - `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/perf-regression-gates.txt`
- `ABI/IR anchors` (persist verbatim in each packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `perf regression gate markers` (required in gate extracts):
  - `tmp/artifacts/objc3c-native/perf-budget`
  - `summary.json`
  - `defaultMaxElapsedMs`
  - `defaultPerFixtureBudgetMs`
  - `cache_hit=(true|false)`
  - `dispatch_fixture_count`
  - `max_elapsed_ms`
  - `total_elapsed_ms`
  - `budget_breached`
  - `cache_proof`
  - `status`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
  - `$perfRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/perf-budget"`
  - `$summaryPath = Join-Path $runDir "summary.json"`
  - `$defaultMaxElapsedMs = 4000`
  - `$defaultPerFixtureBudgetMs = 150`
  - `$matches = [regex]::Matches($OutputText, "(?m)^cache_hit=(true|false)\s*$")`
  - `throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"`
  - `dispatch_fixture_count = $dispatchFixtureCount`
- `closure criteria`:
  - rerunning the same source + lowering options must preserve byte-identical `module.ll` and `module.manifest.json` plus stable perf-budget summary gate markers.
  - perf-budget packets remain fail-closed when `status != "PASS"`, `budget_breached == true`, or cache-proof gates drift.
  - closure remains open if any required packet artifact, ABI/IR anchor, perf regression gate marker, or source anchor is missing.

Performance-budget capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression --emit-prefix module`
2. `npm run test:objc3c:perf-budget`
3. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.ll tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.manifest.json > tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/abi-ir-anchors.txt`
4. `rg -n "tmp/artifacts/objc3c-native/perf-budget|summary.json|defaultMaxElapsedMs|defaultPerFixtureBudgetMs|cache_hit=|dispatch_fixture_count|max_elapsed_ms|total_elapsed_ms|budget_breached|cache_proof|status" scripts/check_objc3c_native_perf_budget.ps1 tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json > tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/perf-regression-gates.txt`
5. `python -m pytest tests/tooling/test_objc3c_m210_lowering_perf_regression_contract.py -q`

## M214 lowering/runtime daemonized compiler profile

Lowering/runtime daemon/watch mode evidence is captured as deterministic packet artifacts under `tmp/` for incremental replay validation.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/`
  - `tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.manifest.json`
  - `tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in daemonized packet artifacts):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `incremental replay markers` (required in daemon/watch evidence extracts):
  - `@@ cycle:cycle-001`
  - `@@ cycle:cycle-002`
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `incremental_cycle_id`
  - `run1_sha256`
  - `run2_sha256`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +`
  - `manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol`
- `closure criteria`:
  - cycle-001 and cycle-002 captures from identical source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - ABI/IR anchor extracts and incremental replay marker extracts remain stable across daemon/watch reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, incremental replay marker, or source anchor is missing.

Daemonized capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001 --emit-prefix module`
2. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002 --emit-prefix module`
3. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.ll tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-001/module.manifest.json tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.ll tmp/artifacts/compilation/objc3c-native/m214/lowering-runtime-daemonized-compiler/cycle-002/module.manifest.json > tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/abi-ir-anchors.txt`
4. `@("@@ cycle:cycle-001", "@@ cycle:cycle-002") | Set-Content tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt; rg -n "runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp >> tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt; rg -n "\"incremental_cycle_id\":|\"run1_sha256\":|\"run2_sha256\":" tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json >> tmp/reports/objc3c-native/m214/lowering-runtime-daemonized-compiler/incremental-replay-markers.txt`
5. `python -m pytest tests/tooling/test_objc3c_m214_lowering_daemonized_contract.py -q`

## M215 lowering/runtime SDK packaging profile

Lowering/runtime SDK packaging evidence is captured as a deterministic packet for IDE-facing toolchains under `tmp/`.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/`
  - `tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.diagnostics.json`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.obj`
  - `tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.object-backend.txt`
  - `tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/ide-consumable-artifact-markers.txt`
- `ABI/IR anchors` (persist verbatim in each SDK packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `IDE-consumable artifact markers` (required in SDK packet marker extracts):
  - `"schema_version": "1.0.0"`
  - `"diagnostics": [`
  - `"severity":`
  - `"line":`
  - `"column":`
  - `"code":`
  - `"message":`
  - `"raw":`
  - `"module":`
  - `"frontend":`
  - `"lowering":`
  - `"globals":`
  - `"functions":`
  - `"runtime_dispatch_symbol":`
  - `"runtime_dispatch_arg_slots":`
  - `clang`
  - `llvm-direct`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());`
  - `WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest_json);`
  - `const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll`, `module.manifest.json`, `module.diagnostics.json`, and `module.object-backend.txt`.
  - ABI/IR anchor extracts and IDE-consumable marker extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, source anchor, or IDE-consumable marker is missing.

SDK packaging capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.ll tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json > tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/abi-ir-anchors.txt`
3. `rg -n "\"schema_version\":|\"diagnostics\":|\"severity\":|\"line\":|\"column\":|\"code\":|\"message\":|\"raw\":|\"module\":|\"frontend\":|\"lowering\":|\"globals\":|\"functions\":|\"runtime_dispatch_symbol\":|\"runtime_dispatch_arg_slots\":|clang|llvm-direct" tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.diagnostics.json tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.object-backend.txt > tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/ide-consumable-artifact-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m215_lowering_sdk_packaging_contract.py -q`

## M216 lowering/runtime conformance suite profile

Lowering/runtime conformance suite evidence is captured as deterministic packet artifacts under `tmp/`.

- `packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/`
  - `tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.manifest.json`
  - `tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/abi-ir-anchors.txt`
  - `tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/conformance-matrix-markers.txt`
- `ABI/IR anchors` (persist verbatim in the packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `conformance-matrix markers` (required in matrix summary evidence):
  - `suite.status`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `spec_section_map`
- `source anchors`:
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
  - `Require-Range "CRPT-" 1 6`
  - `Require-Range "CAN-" 1 7`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - conformance matrix marker rows and ABI/IR anchor extracts remain stable across reruns.
  - closure remains open if any required packet artifact, ABI/IR anchor, source anchor, or conformance-matrix marker is missing.

Conformance suite capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite --emit-prefix module`
2. `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`
3. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.ll tmp/artifacts/compilation/objc3c-native/m216/lowering-runtime-conformance-suite/module.manifest.json > tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/abi-ir-anchors.txt`
4. `rg -n "\"suite\":|\"status\":|\"matrix\":|\"total_cases\":|\"failed_cases\":|\"spec_section_map\"" tmp/artifacts/conformance-suite/<target>/summary.json > tmp/reports/objc3c-native/m216/lowering-runtime-conformance-suite/conformance-matrix-markers.txt`
5. `python -m pytest tests/tooling/test_objc3c_m216_lowering_conformance_contract.py -q`

## M217 lowering/runtime differential parity profile

Lowering/runtime differential parity is captured as a deterministic packet versus baseline toolchains under `tmp/`.

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/`
- `packet toolchain roots`:
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native/`
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-clang/`
  - `tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-llvm-direct/`
- `packet artifacts` (required in each toolchain root):
  - `module.ll`
  - `module.manifest.json`
- `diff marker reports`:
  - `tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/ir-diff-markers.txt`
  - `tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/manifest-diff-markers.txt`
- `ABI/IR anchors` (persist verbatim in native and baseline packets):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `differential source markers` (source anchors to include in parity packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `diff markers` (required deterministic parity rows):
  - `@@ anchor:lowering_ir_boundary`
  - `@@ anchor:frontend_profile`
  - `@@ anchor:objc3.frontend`
  - `@@ anchor:runtime_dispatch_declare`
  - `@@ anchor:manifest.lowering.runtime_dispatch_symbol`
- `closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical packet artifacts inside each toolchain root.
  - native and baseline toolchains may differ in non-anchor payloads, but ABI/IR anchors and diff marker rows must remain stable across reruns.
  - closure remains open if any required toolchain packet artifact, ABI/IR anchor, differential source marker, or diff marker report is missing.

Differential parity capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native --emit-prefix module`
2. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-clang --emit-prefix module --cli-ir-object-backend clang`
3. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/baseline-llvm-direct --emit-prefix module --cli-ir-object-backend llvm-direct`
4. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native/module.ll > tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/ir-diff-markers.txt`
5. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m217/lowering-runtime-differential-parity/native/module.manifest.json > tmp/reports/objc3c-native/m217/lowering-runtime-differential-parity/manifest-diff-markers.txt`
6. `python -m pytest tests/tooling/test_objc3c_m217_lowering_differential_contract.py -q`

## M218 lowering/runtime RC provenance profile

Release-candidate lowering/runtime provenance is captured as a deterministic packet rooted under `tmp/`.

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.manifest.json`
  - `tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt`
  - `tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/attestation-markers.txt`
- `ABI/IR anchors` (persist verbatim in each RC packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `attestation markers` (contract markers to include in RC packet attestation notes):
  - `runtime_dispatch_symbol=`
  - `selector_global_ordering=lexicographic`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `RC provenance closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - replay and attestation marker reports stay stable across reruns (no added/removed markers).
  - closure remains open if any required packet artifact, ABI/IR anchor, replay marker, or attestation marker is missing.

RC provenance capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.ll > tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.manifest.json >> tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt`
4. `rg -n "Objc3LoweringIRBoundaryReplayKey|invalid lowering contract runtime_dispatch_symbol|runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/attestation-markers.txt`
5. `python -m pytest tests/tooling/test_objc3c_m218_lowering_rc_provenance_contract.py -q`

## M219 lowering/runtime cross-platform parity profile

Cross-platform lowering/runtime parity evidence is captured as deterministic packet artifacts under `tmp/` across windows/linux/macos.

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/`
- `platform packet roots`:
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/windows/`
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/linux/`
  - `tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/macos/`
- `packet artifacts` (required for each platform root):
  - `module.ll`
  - `module.manifest.json`
- `replay marker reports`:
  - `tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/windows-replay-markers.txt`
  - `tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/linux-replay-markers.txt`
  - `tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/macos-replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in each platform packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors that must be present in packet notes across windows/linux/macos):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `cross-platform parity closure criteria`:
  - rerunning the same source + lowering options on each platform produces byte-identical `module.ll` and `module.manifest.json` within that platform.
  - replay marker token sets must match across windows/linux/macos (ordering may differ only by tool output line-number prefixes).
  - closure remains open if any required platform packet artifact, ABI/IR anchor, or replay marker is missing.

Cross-platform capture commands (run per platform worker):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform> --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>/module.ll > tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>-replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>/module.manifest.json >> tmp/reports/objc3c-native/m219/lowering-runtime-cross-platform-parity/<platform>-replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m219_lowering_cross_platform_contract.py -q`

## M220 lowering/runtime public-beta triage profile

Public-beta lowering/runtime triage must ship as deterministic packet evidence rooted under `tmp/`:

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.manifest.json`
  - `tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in each beta triage packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `patch-loop closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - replay markers stay stable across reruns (no added/removed lines, no reordered anchors).
  - closure remains open if any ABI/IR anchor or replay marker is missing.

Public-beta triage capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.ll > tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.manifest.json >> tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m220_lowering_public_beta_contract.py -q`

## M221 lowering/runtime GA blocker burn-down profile

GA-blocker burn-down evidence for lowering/runtime should be captured as a deterministic packet rooted under `tmp/`:

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.manifest.json`
  - `tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in triage packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `GA blocker closure signal`:
  - identical source + lowering options produce byte-identical packet artifacts and stable replay markers.

Burn-down capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.ll > tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.manifest.json >> tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m221_lowering_ga_blocker_contract.py -q`

## M224 lowering/LLVM IR/runtime ABI release readiness

GA readiness evidence for native `.objc3` lowering remains deterministic and fail-closed:

- IR replay markers must remain present and aligned:
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
- Manifest lowering packet must mirror the runtime boundary contract:
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- Runtime ABI declaration remains symbol-aligned with the lowering contract when dispatch calls are emitted:
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
- Boundary normalization remains fail-closed for invalid symbols:
  - `invalid lowering contract runtime_dispatch_symbol`
- Determinism expectation for GA:
  - identical source + lowering options produce byte-identical `module.ll` and `module.manifest.json`.

Operator evidence sequence:

1. Generate artifacts in a deterministic tmp root:

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness --emit-prefix module
```

2. Validate marker alignment in:
  - `tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness/module.manifest.json`
3. Run contract guard:
  - `python -m pytest tests/tooling/test_objc3c_m224_lowering_release_contract.py -q`

## M225 lowering/runtime roadmap seeding profile

Post-1.0 backlog seeding for lowering/runtime 1.1/1.2 should record deterministic artifact evidence plus source-anchored ABI/IR signals:

- `1.1 artifact evidence capture`:
  - `tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding/module.manifest.json`
  - extract and persist:
    - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
    - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
    - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
    - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `1.2 ABI/IR signal extraction`:
  - replay-key source marker: `Objc3LoweringIRBoundaryReplayKey(...)`
  - IR ABI declaration marker: `declare i32 @` + `runtime_dispatch_symbol`
  - lowering normalization marker: `invalid lowering contract runtime_dispatch_symbol`

Roadmap-seeding commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding --emit-prefix module`
2. `python -m pytest tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py -q`

## Recovery fixture layout (`tests/tooling/fixtures/native/recovery`)

Current recovery fixtures are partitioned as:

```text
tests/tooling/fixtures/native/recovery/
  positive/
    bool_param_flow_relational.objc3
    comparison_logic.objc3
    function_return_annotation_bool.objc3
    function_return_annotation_bool.objc3-ir.expect.txt
    hello.objc3
    main_bool_entrypoint.objc3
    main_bool_entrypoint.objc3-ir.expect.txt
    lowering_dispatch/
      msgsend_lookup_basic.m
      msgsend_lookup_basic.dispatch-ir.expect.txt
      msgsend_lookup_two_args.m
      msgsend_lookup_two_args.dispatch-ir.expect.txt
    message_send_basic.objc3
    message_send_bool_compatible.objc3
    message_send_four_args.objc3
    message_send_keywords.objc3
    relational_logical_bool_literals.objc3
    return_paths_ok.objc3
    typed_bool_param_i32_expr_call.objc3
    typed_bool_param_i32_expr_call.objc3-ir.expect.txt
    typed_bool_return_literal_edges.objc3
    typed_bool_return_literal_edges.objc3-ir.expect.txt
    typed_i32_bool.objc3
    typed_i32_bool.objc3-ir.expect.txt
    typed_i32_return_from_bool_expr.objc3
    typed_i32_return_from_bool_expr.objc3-ir.expect.txt
  negative/
    neg_bool_fn_value.objc3
    neg_i32_param_from_bool_call.objc3
    neg_param_nullable.objc3
    neg_param_type_generic.objc3
    neg_rel_arg_mismatch.objc3
    neg_typed_bool_return_from_i32_call.objc3
    negative_arity_mismatch.objc3
    negative_message_arg_function.objc3
    negative_message_missing_keyword_colon.objc3
    negative_message_receiver_function.objc3
    negative_message_too_many_args.objc3
    negative_message_unterminated.objc3
    negative_missing_return.objc3
    negative_return_annotation_type_mismatch.objc3
    negative_return_annotation_unsupported_type.objc3
    negative_type_mismatch.objc3
    negative_undefined_symbol.objc3
```

`scripts/check_objc3c_native_recovery_contract.ps1` discovers fixtures recursively under `positive/` and `negative/` and enforces contract behavior per class (compile success/object+manifest artifacts for positive fixtures, deterministic failure diagnostics for negative fixtures).

`scripts/run_objc3c_lowering_regression_suite.ps1` also supports per-fixture dispatch IR assertions for Objective-C fixtures:

- Place `<fixture>.dispatch-ir.expect.txt` next to the `.m` file.
- One unique non-comment token per line (`#` and blank lines ignored; duplicate tokens fail validation).
- Optional dispatch fixture roots are discovered in deterministic order when present:
  - `tests/tooling/fixtures/native/recovery/positive/lowering_dispatch`
  - `tests/tooling/fixtures/native/dispatch/positive`
- The suite emits replay artifacts `module.dispatch.ll` for run1/run2 under the case directory and requires:
  - clang emit success for both runs,
  - deterministic `module.dispatch.ll` bytes across replay,
  - every token to appear in both emitted IR files.
- For `.objc3` fixtures, place `<fixture>.objc3-ir.expect.txt` next to the source file to assert deterministic native-lowered IR markers (`module.ll`) across replay runs.

## M26 Lane-E execution smoke harness contract (`scripts/check_objc3c_native_execution_smoke.ps1`)

Execution smoke validates a native `.objc3` compile-link-run path and fails closed on the first contract breach.

- Harness preconditions:
  - Native compiler executable must exist at `artifacts/bin/objc3c-native.exe` (or be buildable via `scripts/build_objc3c_native.ps1`).
  - Runtime shim source must exist at `tests/tooling/runtime/objc3_msgsend_i32_shim.c`.
  - `clang` must be invokable (override with `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`).
- Positive fixture flow:
  - Discovers `*.objc3` recursively under `tests/tooling/fixtures/native/execution/positive` in deterministic sorted order.
  - Requires `<fixture>.exitcode.txt` sidecar with one integer expected process exit code.
  - Compiles via `objc3c-native.exe` and requires `module.obj`.
  - When positive `.meta.json` explicitly sets `execution.requires_runtime_shim`, harness verifies `module.ll` parity:
    - `true`: lowered IR must include both runtime dispatch declaration and call for the configured symbol.
    - `false`: lowered IR must include neither runtime dispatch declaration nor call for the configured symbol.
  - Links with `clang <module.obj> tests/tooling/runtime/objc3_msgsend_i32_shim.c -o module.exe`.
  - Runs `module.exe` and requires `run_exit == expected_exit`.
- Negative fixture flow:
  - Discovers `*.objc3` recursively under `tests/tooling/fixtures/native/execution/negative` in deterministic sorted order.
  - Requires `<fixture>.meta.json` sidecar.
  - Sidecar contract currently consumed by the harness:
    - `fixture`: filename that must match the fixture.
    - `expect_failure.stage`: `compile`, `link`, or `run`.
    - `expect_failure.required_diagnostic_tokens`: required stage-diagnostic tokens (case-sensitive matching).
    - `execution.requires_runtime_shim`: informational flag documenting whether a successful execution path would require the shim.
    - `execution.runtime_dispatch_symbol`: optional symbol name (defaults to `objc3_msgsend_i32`).
  - Stage behavior:
    - `compile`: requires non-zero compile exit and expected tokens in compile diagnostics.
    - `link`: requires compile success + non-zero link exit + expected tokens in link diagnostics.
    - `run`: requires compile/link success + non-zero run exit + expected tokens in run diagnostics.
- Output contract:
  - Per-run artifacts: `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/`.
  - Per-case logs: compile/link/run logs and compile out-dir artifacts.
  - Summary JSON: `summary.json` with `status`, `total`, `passed`, `failed`, and per-case result rows.

## Runtime shim contract (`objc3_msgsend_i32`)

- `.objc3` message-send lowering emits direct calls with configurable slot count:
  - `declare i32 @objc3_msgsend_i32(i32, ptr, i32, ..., i32)`
  - Slot count equals `--objc3-max-message-args` (default `4`).
- Native execution smoke expects a C-ABI compatible runtime shim symbol named exactly `objc3_msgsend_i32`, equivalent to:

```c
int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3);
```

- Call-shape contract:
  - `receiver` is lowered as `i32`.
  - `selector` is a pointer to a lowered selector literal.
  - Dispatch argument slots are passed as `i32`; unused slots are zero-filled by lowering.
  - When lowered receiver value is `0` (`nil`), native IR short-circuits message send result to `0`.
  - Link-time runtime-dispatch symbol resolution still applies because emitted IR keeps a non-nil dispatch branch.
  - For direct syntactic `nil` receiver sends that lower to constant `0`, runtime-dispatch declaration/call emission is omitted.
  - For immutable local bindings initialized to `nil`, runtime-dispatch declaration/call emission is omitted when no other non-elided sends are present.
  - For flow-sensitive nil-bound send sites prior to reassignment, runtime-dispatch call emission is omitted.
  - Explicit `= nil` reassignment regains nil-bound elision eligibility for subsequent identifier receiver sends.
  - Nil-provenance elision eligibility propagates through `let`/`for let` alias bindings when their initializer expression is nil-proven.
  - Conditional receiver expressions are nil-elided when condition evaluation is compile-time known and selects a nil-proven branch.
  - Immutable global identifiers with nil-proven initializers are nil-elided under the same dispatch omission contract.
  - Mutable global identifiers are excluded from global nil elision and continue to lower through runtime nil-check branch scaffolding.
  - Runtime-dispatch declaration emission is now usage-driven: it is emitted only when lowering emits at least one non-elided runtime-dispatch call.
  - Compile-time non-nil receivers lower through a direct runtime-dispatch call path without `msg_nil/msg_dispatch` branch scaffolding.
  - Current non-nil fast-path eligibility includes non-zero integer literals, `YES`, unary constant-expression receivers, short-circuit logical constant-expression receivers, compile-time non-zero global constant identifiers, and flow-sensitive local bindings proven compile-time non-zero at the send site.
  - Global identifier proofing is mutation-gated: only receiver globals with no detected write forms in function bodies (`=`, compound assignment, `++`/`--`, and `for` init/step assignment/update clauses) are eligible for compile-time non-zero fast-path lowering.
  - Call boundaries invalidate global receiver proof state only when the callee is effect-summary impure; pure-call boundaries retain global nil/non-zero proofs and preserve post-call fast-path/elision eligibility.
  - `pure fn` annotations are supported for prototypes and definitions: pure-annotated external prototypes are treated as side-effect-free call boundaries, while impure pure-annotated definitions fail during semantic validation with deterministic diagnostic `O3S215` (`pure contract violation: function '<name>' declared 'pure' has side effects (cause: <cause-token>; cause-site:<line>:<column>; detail:<detail-token>@<line>:<column>)`) emitted at the definition location.
  - Deterministic pure-contract cause tokens currently include `global-write`, `message-send`, `impure-callee:<name>`, and `unannotated-extern-call:<name>`.
  - For call-derived causes (`impure-callee:*`, `unannotated-extern-call:*`), `cause-site` coordinates bind to the callee identifier token location.
  - `detail` carries the propagated leaf impurity token/site so transitive call-derived failures expose the underlying root cause deterministically.
  - Compile-time non-nil proofing uses local constant-expression tracking for literals, identifiers bound to compile-time constants, conditionals, unary canonical forms, and supported binary operators.
  - Logical `&&`/`||` constant evaluation follows short-circuit semantics so skipped branches are not required to be compile-time evaluable for receiver non-nil proofing.
  - Flow-sensitive non-zero proofs can be regained after explicit `=` writes to compile-time non-zero values and are invalidated by subsequent writes that are not compile-time non-zero proofs.
  - Numeric zero receiver sends keep runtime-dispatch declaration/call emission.
  - Reassignments to non-nil or runtime-unknown values keep runtime-dispatch declaration/call emission.
- Determinism assumption for smoke:
  - For identical inputs, shim behavior should be deterministic and stable.
  - Harness assertions are based on process exit code equality (positive fixtures) and deterministic link diagnostics token matching (negative fixtures).

## Execution fixture layout (`tests/tooling/fixtures/native/execution`)

Execution smoke fixture roots and sidecars:

```text
tests/tooling/fixtures/native/execution/
  positive/
    <name>.objc3
    <name>.exitcode.txt
    [optional] <name>.meta.json
  negative/
    <name>.objc3
    <name>.meta.json
```

Metadata conventions consumed by the current harness:

- Positive sidecar (`<name>.exitcode.txt`):
  - Single integer exit code (parsed as `int`).
- Optional positive sidecar (`<name>.meta.json`):
  - `fixture`: must match fixture filename.
  - `execution.native_compile_args`: optional string array appended to native compiler invocation.
  - `execution.requires_runtime_shim` (optional): when present, execution smoke enforces `module.ll` dispatch declaration/call parity for the configured dispatch symbol.
  - `execution.runtime_dispatch_symbol` (optional): runtime dispatch symbol for parity checks (defaults to `objc3_msgsend_i32`).
- Negative sidecar (`<name>.meta.json`):
  - `expect_failure.stage`: `compile`, `link`, or `run`.
  - `expect_failure.required_diagnostic_tokens`: deterministic stage-diagnostic token list.
  - `execution.requires_runtime_shim`: informational flag for runtime dependency.
  - `execution.runtime_dispatch_symbol` (optional): expected runtime symbol.

Current repository state on 2026-02-27:

- `tests/tooling/fixtures/native/execution/positive` includes baseline, assignment, do-while, and for-loop smoke fixtures with `.exitcode.txt` sidecars.
- `tests/tooling/fixtures/native/execution/negative` includes compile/link-stage fixtures with `.meta.json` sidecars.

## Build and compile commands

From repo root:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3
```

Native driver option for `.objc3` frontend behavior:
- `--objc3-max-message-args <0-16>`
  - Default: `4`
  - Tightens the semantic arg-cap check for bracketed message-send expressions.
- `--objc3-runtime-dispatch-symbol <symbol>`
  - Default: `objc3_msgsend_i32`
  - Rebinds the runtime dispatch call target used by message-send lowering (`[A-Za-z_.$][A-Za-z0-9_.$]*`).

Direct script equivalents:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3
```

## Driver shell split validation commands (M136-E001)

Use one `.objc3` input and one non-`.objc3` Objective-C input to validate both shell branches:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/objc3 --emit-prefix module_objc3
npm run compile:objc3c -- tests/tooling/fixtures/native/recovery/positive/lowering_dispatch/msgsend_lookup_basic.m --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/objectivec --emit-prefix module_objc
```

Expected success surface:

- `.objc3` compile writes diagnostics + manifest + `module_objc3.ll` + `module_objc3.obj`.
- Objective-C compile writes diagnostics + manifest + `module_objc.obj`.
- Both invocations exit `0` on success.

## Lexer extraction contract artifacts (M137-E001)

`scripts/check_objc3c_lexer_extraction_token_contract.ps1` emits deterministic validation artifacts under:

- `tmp/artifacts/objc3c-native/lexer-extraction-token-contract/<run_id>/`
  - `summary.json` (contract id, pass/fail counters, check rows, and per-case replay evidence)
  - run logs and replay output directories for positive/negative lexer fixtures

Commands:

```powershell
npm run test:objc3c:lexer-extraction-token-contract
npm run test:objc3c:lexer-parity
npm run check:compiler-closeout:m137
```

## Parser/AST extraction validation artifacts (M138-E001)

`npm run test:objc3c:parser-replay-proof` writes deterministic replay-proof outputs under:

- `tmp/artifacts/objc3c-native/parser-replay-proof/<proof_run_id>/summary.json`

Parser/AST extraction surface validation commands:

```powershell
npm run test:objc3c:parser-ast-extraction
npm run check:compiler-closeout:m138
```

`npm run check:compiler-closeout:m138` fail-closes on parser + AST builder + docs/CI/release wiring drift via:

- `python scripts/check_m138_parser_ast_contract.py`
- `python -m pytest tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py -q`

## Sema pass-manager + diagnostics bus validation artifacts (M139-E001)

Sema extraction and diagnostics-bus validation commands:

```powershell
npm run test:objc3c:sema-pass-manager-diagnostics-bus
npm run check:compiler-closeout:m139
```

`npm run test:objc3c:sema-pass-manager-diagnostics-bus` writes deterministic pass-manager diagnostics-bus proof artifacts under:

- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`

Deterministic run-id behavior:

- Default run id is fixed to `m143-sema-type-system-default`.
- Optional override remains `OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID` (validated token syntax).

`npm run check:compiler-closeout:m139` fail-closes on sema pass-manager and diagnostics-bus wiring drift via:

- `python scripts/check_m139_sema_pass_manager_contract.py`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`
- `python -m pytest tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_pure_contract_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py -q`

## Frontend library boundary validation artifacts (M140-E001)

M140 extraction and boundary closeout commands:

```powershell
npm run test:objc3c:m140-boundary-contract
npm run check:compiler-closeout:m140
```

`npm run check:compiler-closeout:m140` fail-closes on frontend library boundary drift via:

- `python scripts/check_m140_frontend_library_boundary_contract.py`
- `python -m pytest tests/tooling/test_objc3c_frontend_library_entrypoint_extraction.py tests/tooling/test_objc3c_m140_boundary_contract.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`

## CMake targetization and linkage topology validation artifacts (M141-E001)

M141 target-topology validation commands:

```powershell
npm run test:objc3c:m141-target-topology
npm run check:compiler-closeout:m141
```

`npm run check:compiler-closeout:m141` fail-closes on targetization/linkage-topology drift via:

- `python scripts/check_m141_cmake_target_topology_contract.py`
- `python -m pytest tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_cmake_target_topology.py tests/tooling/test_objc3c_process_io_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`

## Frontend lowering parity harness artifacts (M142-E001)

Parity harness replay commands:

```powershell
npm run check:objc3c:library-cli-parity:source
npm run check:compiler-closeout:m142
```

`npm run check:objc3c:library-cli-parity:source` writes deterministic parity outputs under:

- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/library/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/cli/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/summary.json`

Where `<work_key>` is deterministic (derived from source + emit/lowering/runtime controls when not passed explicitly), effective replay roots are:

- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/<work_key>/library/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/<work_key>/cli/`

Expected compared artifacts per side (`emit_prefix=module` default):

- `module.diagnostics.json`
- `module.manifest.json`
- `module.ll`
- `module.obj`

Object backend note for harness replay:

- CLI emits backend provenance sidecar `module.object-backend.txt` (`clang` or `llvm-direct`).
- M142 parity dimensions exclude `module.object-backend.txt`; it is a provenance note, not a compared artifact payload.
- Source-mode parity command pins `--cli-ir-object-backend clang` so CLI and C API object outputs are backend-aligned.

`npm run check:compiler-closeout:m142` fail-closes on parity harness source/docs/package drift via:

- `python scripts/check_m142_frontend_lowering_parity_contract.py`
- `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_frontend_lowering_parity_contract.py tests/tooling/test_objc3c_sema_cli_c_api_parity_surface.py -q`

## Artifact tmp-path governance artifacts (M143-D001)

Tmp-governed parity governance commands:

```powershell
npm run check:objc3c:library-cli-parity:source:m143
npm run check:compiler-closeout:m143
```

`npm run check:objc3c:library-cli-parity:source:m143` writes replay-governed outputs under:

- `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work/<work-key>/library/`
- `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work/<work-key>/cli/`
- `tmp/artifacts/compilation/objc3c-native/m143/library-cli-parity/summary.json`

Governance contract notes:

- Work roots are deterministic via `--work-key` (or default-derived deterministic key when omitted).
- Source mode fail-closes when stale `<emit-prefix>` outputs are detected in target work roots.
- Source mode fail-closes when expected generated parity artifacts are missing after command execution.
- Tmp-path policy is default-enforced; non-tmp work roots require explicit opt-in.
- Lane-C lowering/runtime artifact roots remain under `tmp/artifacts/objc3c-native/`:
  - `tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json`
  - `tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json`
  - `tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json`
- Lane-C deterministic default run ids:
  - `m143-lane-c-lowering-regression-default` (`OBJC3C_NATIVE_LOWERING_RUN_ID`)
  - `m143-lane-c-typed-abi-default` (`OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID`)
  - `m143-lane-c-lowering-replay-proof-default` (`OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID`)

`npm run check:compiler-closeout:m143` fail-closes on tmp-governance source/docs/package drift via:

- `python scripts/check_m143_artifact_tmp_governance_contract.py`
- `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py tests/tooling/test_objc3c_m143_artifact_tmp_governance_contract.py tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py tests/tooling/test_check_m143_artifact_tmp_governance_contract.py -q`

## LLVM capability discovery artifacts (M144-E001)

Capability discovery and routing validation commands:

```powershell
npm run check:objc3c:llvm-capabilities
npm run check:objc3c:library-cli-parity:source:m144
npm run check:compiler-closeout:m144
```

Capability probe summary output:

- `tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json`

Capability-routed source-mode parity output:

- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work/<work-key>/library/`
- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work/<work-key>/cli/`
- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/summary.json`

`npm run check:compiler-closeout:m144` fail-closes on capability discovery source/docs/package drift via:

- `python scripts/check_m144_llvm_capability_discovery_contract.py`
- `python -m pytest tests/tooling/test_probe_objc3c_llvm_capabilities.py tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_m144_llvm_capability_discovery_contract.py tests/tooling/test_check_m144_llvm_capability_discovery_contract.py -q`

## Direct LLVM object-emission lane-B matrix artifacts (M145-B001)

Lane-B sema/type-system direct-LLVM matrix validation commands:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run check:compiler-closeout:m145
```

`npm run test:objc3c:m145-direct-llvm-matrix` writes matrix artifacts under:

- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke_llvm_direct/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke_llvm_direct_forced_missing_llc/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/negative_backend_matrix/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`

Fail-closed matrix markers captured in `summary.json` checks:

- `runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes`
- `runtime.positive.matrix.llvm_direct_forced_missing_llc.fail_closed_marker`
- `runtime.negative.matrix.backend.exit_codes.<fixture>`

## Direct LLVM object-emission lane-D validation artifacts (M145-D001)

Lane-D conformance/perf validation commands:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run check:compiler-closeout:m145
```

Lane-D artifact roots:

- Conformance aggregation output:
  - `tmp/artifacts/conformance_suite/summary.json`
- Perf-budget summary output:
  - `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
- Coverage sources tied to lane-D fixture:
  - `tests/conformance/COVERAGE_MAP.md`
  - `tests/conformance/lowering_abi/manifest.json`
  - `tests/conformance/lowering_abi/M145-D001.json`

## Execution smoke commands (M26 lane-E)

```powershell
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

- Direct script equivalent path:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1
```

- Optional toolchain override:
  - `OBJC3C_NATIVE_EXECUTION_CLANG_PATH=<clang executable>`

## Deterministic contract commands

From repo root:

```powershell
npm run test:objc3c
npm run test:objc3c:matrix
npm run test:objc3c:perf-budget
npm run test:objc3c:diagnostics-replay-proof
npm run test:objc3c:parser-replay-proof
npm run test:objc3c:parser-extraction-ast-builder-contract
npm run test:objc3c:parser-ast-extraction
npm run test:objc3c:sema-pass-manager-diagnostics-bus
npm run test:objc3c:lowering-replay-proof
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:driver-shell-split
npm run test:objc3c:lexer-extraction-token-contract
npm run test:objc3c:lexer-parity
npm run proof:objc3c
npm run test:objc3c:lane-e
npm run check:compiler-closeout:m137
npm run check:compiler-closeout:m138
npm run check:compiler-closeout:m139
npm run test:objc3c:m140-boundary-contract
npm run check:compiler-closeout:m140
npm run test:objc3c:m141-target-topology
npm run check:compiler-closeout:m141
npm run test:objc3c:m142-lowering-parity
npm run check:objc3c:library-cli-parity:source
npm run check:compiler-closeout:m142
npm run test:objc3c:m143-artifact-governance
npm run check:objc3c:library-cli-parity:source:m143
npm run check:compiler-closeout:m143
npm run test:objc3c:m144-llvm-capability-discovery
npm run check:objc3c:llvm-capabilities
npm run check:objc3c:library-cli-parity:source:m144
npm run check:compiler-closeout:m144
npm run test:objc3c:m145-direct-llvm-matrix
npm run check:compiler-closeout:m145
```

Driver shell split regression spot-check (M136-E001):

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/tests-objc3 --emit-prefix module_objc3
npm run compile:objc3c -- tests/tooling/fixtures/native/recovery/positive/lowering_dispatch/msgsend_lookup_basic.m --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/tests-objectivec --emit-prefix module_objc
```

- Validates both shell branches (`.objc3` frontend path and non-`.objc3` Objective-C path) using deterministic fixture inputs.
- Keeps lane-E proof artifacts isolated under `tmp/artifacts/objc3c-native/m136-driver-shell/`.

- `npm run test:objc3c`
  - Runs `scripts/check_objc3c_native_recovery_contract.ps1`.
  - Verifies deterministic replay for baseline cases and all recovery fixtures.
  - Verifies `.objc3` manifest frontend pipeline/integration-surface contract fields.
  - Also exposed as `npm run check:compiler-impl-recovery`.
- `npm run test:objc3c:matrix`
  - Runs `scripts/run_objc3c_native_fixture_matrix.ps1`.
  - Executes all recovery fixtures and writes a per-run summary JSON under `tmp/artifacts/objc3c-native/fixture-matrix/<run_id>/summary.json`.
  - Current observed behavior on 2026-02-27 in this workspace: exits zero with `status: PASS`.
- `npm run test:objc3c:perf-budget`
  - Runs `scripts/check_objc3c_native_perf_budget.ps1`.
  - Enforces total elapsed compile budget across positive recovery fixtures (default `4000` ms, override via `OBJC3C_NATIVE_PERF_MAX_MS` or `-MaxElapsedMs`).
  - Runs fail-closed cache proof:
    - Run 1 (unique cache key): requires exactly one `cache_hit=false` marker.
    - Run 2 (same key): requires exactly one `cache_hit=true` marker.
    - Hashes for emitted artifacts (`.obj`, `.manifest.json`, `.diagnostics.txt`, `.ll` for `.objc3`) must match between miss/hit runs.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json` with `cache_proof` evidence.
- `npm run test:objc3c:driver-shell-split`
  - Runs `scripts/check_objc3c_driver_shell_split_contract.ps1`.
  - Verifies `main.cpp` shell boundary: parse + exit-code mapping + delegation-only contract into `driver/*`.
  - Runs a deterministic two-pass smoke compile over `tests/tooling/fixtures/native/driver_split/smoke_compile_driver_shell_split.objc3`.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/driver-shell-split/<run_id>/summary.json`.
- `npm run test:objc3c:lexer-extraction-token-contract`
  - Runs `scripts/check_objc3c_lexer_extraction_token_contract.ps1`.
  - Verifies lexer subsystem extraction surfaces (`lex/*`, pipeline wiring, `TokenKind` contract markers).
  - Replays positive/negative lexer fixtures and enforces deterministic diagnostics/artifact contracts.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/lexer-extraction-token-contract/<run_id>/summary.json`.
- `npm run test:objc3c:lexer-parity`
  - Runs `python -m pytest tests/tooling/test_objc3c_lexer_parity.py -q`.
  - Verifies lexer extraction parity contract surfaces:
    - lexer module files exist,
    - pipeline consumes lexer header boundary,
    - CMake registers lexer target/source wiring.
- `npm run check:compiler-closeout:m137`
  - Runs `python scripts/check_m137_lexer_contract.py`.
  - Runs `npm run test:objc3c:lexer-extraction-token-contract` and `npm run test:objc3c:lexer-parity`.
  - Enforces fail-closed M137 lexer/token contract wiring across build/docs/CI/release surfaces.
- `npm run check:compiler-closeout:m138`
  - Runs `python scripts/check_m138_parser_ast_contract.py`.
  - Runs `npm run test:objc3c:parser-extraction-ast-builder-contract` and `npm run test:objc3c:parser-ast-extraction`.
  - Enforces fail-closed M138 parser extraction + AST builder contract wiring across build/docs/CI/release surfaces.
- `npm run check:compiler-closeout:m139`
  - Runs `python scripts/check_m139_sema_pass_manager_contract.py`.
  - Runs `npm run test:objc3c:sema-pass-manager-diagnostics-bus`.
  - Enforces fail-closed M139 sema pass-manager + diagnostics-bus contract wiring across build/docs/CI/release surfaces.
- `npm run test:objc3c:m140-boundary-contract`
  - Runs `python -m pytest tests/tooling/test_objc3c_frontend_library_entrypoint_extraction.py tests/tooling/test_objc3c_m140_boundary_contract.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`.
  - Verifies extracted frontend library entrypoint wiring, sema type-metadata handoff determinism, and lowering-to-IR boundary replay markers.
- `npm run check:compiler-closeout:m140`
  - Runs `python scripts/check_m140_frontend_library_boundary_contract.py`.
  - Runs `npm run test:objc3c:m140-boundary-contract`.
  - Enforces fail-closed M140 frontend-library boundary contract wiring across source/docs/package surfaces.
- `npm run test:objc3c:m141-target-topology`
  - Runs `python -m pytest tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_cmake_target_topology.py tests/tooling/test_objc3c_process_io_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`.
  - Verifies deterministic stage-target linkage topology across CMake driver, sema/type-system boundary, lower/IR/runtime-ABI targets, and aggregate executable wiring.
- `npm run check:compiler-closeout:m141`
  - Runs `python scripts/check_m141_cmake_target_topology_contract.py`.
  - Runs `npm run test:objc3c:m141-target-topology`.
  - Enforces fail-closed M141 CMake targetization/linkage-topology contract wiring across source/docs/package/workflow surfaces.
- `npm run test:objc3c:m142-lowering-parity`
  - Runs `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_frontend_lowering_parity_contract.py tests/tooling/test_objc3c_sema_cli_c_api_parity_surface.py -q`.
  - Verifies source-mode CLI/C API parity harness execution surfaces, C API runner contract snippets, and M142 docs/package wiring.
- `npm run check:objc3c:library-cli-parity:source`
  - Runs `python scripts/check_objc3c_library_cli_parity.py --source ... --cli-bin artifacts/bin/objc3c-native.exe --c-api-bin artifacts/bin/objc3c-frontend-c-api-runner.exe --cli-ir-object-backend clang`.
  - Executes CLI and C API runner on one source input and compares deterministic diagnostics/manifest/IR/object digest surfaces.
  - Writes replay artifacts under `tmp/artifacts/objc3c-native/m142/library-cli-parity/`.
- `npm run check:compiler-closeout:m142`
  - Runs `python scripts/check_m142_frontend_lowering_parity_contract.py`.
  - Runs `npm run test:objc3c:m142-lowering-parity`.
  - Enforces fail-closed M142 parity harness wiring across source/docs/package/workflow surfaces.
- `npm run test:objc3c:m143-artifact-governance`
  - Runs `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py tests/tooling/test_objc3c_m143_artifact_tmp_governance_contract.py tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py tests/tooling/test_check_m143_artifact_tmp_governance_contract.py -q`.
  - Verifies tmp-governed default output paths, parser/AST lane-A coverage wiring, sema/type-system lane-B governance, lowering/LLVM IR/runtime-ABI lane-C governance, source-mode work-root governance, and M143 docs/package wiring.
- `npm run check:objc3c:library-cli-parity:source:m143`
  - Runs `python scripts/check_objc3c_library_cli_parity.py --source ... --summary-out tmp/artifacts/compilation/objc3c-native/m143/library-cli-parity/summary.json ...`.
  - Enforces deterministic replay roots and fail-closed stale/missing artifact checks under tmp-governed paths.
- `npm run check:compiler-closeout:m143`
  - Runs `python scripts/check_m143_artifact_tmp_governance_contract.py`.
  - Runs `npm run test:objc3c:m143-artifact-governance`.
  - Enforces fail-closed M143 tmp-governance wiring across source/docs/package/workflow surfaces.
- `npm run test:objc3c:m144-llvm-capability-discovery`
  - Runs `python -m pytest tests/tooling/test_probe_objc3c_llvm_capabilities.py tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_m144_llvm_capability_discovery_contract.py tests/tooling/test_check_m144_llvm_capability_discovery_contract.py -q`.
  - Verifies capability probe packet behavior, fail-closed backend routing extraction, and M144 docs/package wiring.
- `npm run check:objc3c:llvm-capabilities`
  - Runs `python scripts/probe_objc3c_llvm_capabilities.py --summary-out tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json`.
  - Produces deterministic capability summary packet used by M144 routed parity workflows.
- `npm run check:compiler-closeout:m144`
  - Runs `python scripts/check_m144_llvm_capability_discovery_contract.py`.
  - Runs `npm run test:objc3c:m144-llvm-capability-discovery`.
  - Enforces fail-closed M144 capability discovery wiring across source/docs/package/workflow surfaces.
- `npm run test:objc3c:m145-direct-llvm-matrix`
  - Runs `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`.
  - Runs `python -m pytest tests/tooling/test_check_m145_direct_llvm_matrix_contract.py -q`.
  - Verifies lane-B sema/type-system direct LLVM object-emission matrix coverage, including forced missing-llc fail-closed behavior.
  - Verifies matrix checks such as `runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes` and backend-invariant negative diagnostics.
- `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`
  - Runs `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_conformance_suite.ps1`.
  - Runs `npm run test:objc3c:perf-budget`.
  - Verifies lane-D conformance/perf anchors for `M145-D001` and fail-closed matrix coverage.
- `npm run check:compiler-closeout:m145`
  - Runs `python scripts/check_m145_direct_llvm_matrix_contract.py`.
  - Runs `npm run test:objc3c:m145-direct-llvm-matrix`.
  - Runs `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`.
  - Runs `python scripts/spec_lint.py --glob "docs/contracts/direct_llvm_emission_expectations.md"`.
  - Enforces fail-closed M145 direct LLVM object-emission matrix wiring across lane-B sema/type-system, lane-C runtime-ABI, and lane-D validation/conformance/perf surfaces.
- `npm run proof:objc3c`
  - Runs `scripts/run_objc3c_native_compile_proof.ps1`.
  - Replays `tests/tooling/fixtures/native/hello.objc3` twice and writes `artifacts/compilation/objc3c-native/proof_20260226/digest.json` on success.
- `npm run test:objc3c:lane-e`
  - Runs lane-E deterministic validation chain:
    - `npm run test:objc3c`
    - `npm run test:objc3c:diagnostics-replay-proof`
    - `npm run test:objc3c:parser-replay-proof`
    - `npm run test:objc3c:sema-pass-manager-diagnostics-bus`
    - `npm run test:objc3c:driver-shell-split`
    - `npm run test:objc3c:lexer-extraction-token-contract`
    - `npm run test:objc3c:lexer-parity`
    - `npm run test:objc3c:perf-budget`
    - `npm run test:objc3c:lowering-regression`
    - `npm run test:objc3c:lowering-replay-proof`
    - `npm run test:objc3c:typed-abi-replay-proof`
    - `npm run test:objc3c:execution-smoke`
    - `npm run test:objc3c:execution-replay-proof`
    - `npm run check:compiler-execution:m23`
    - `npm run check:compiler-execution:m24`
    - `npm run check:compiler-execution:m25:lane-e`
  - Fails closed if any stage exits non-zero.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_diagnostics_replay_proof.ps1`
  - Runs diagnostics regression twice with distinct run IDs.
  - Canonicalizes deterministic per-fixture result surface and asserts SHA256 equality across replay.
  - Writes proof summary under `tmp/artifacts/objc3c-native/diagnostics-replay-proof/<proof_run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_replay_proof.ps1`
  - Replays dedicated malformed loop/control + assignment (including compound-assignment/update and `%=` parser negatives) + bitwise/shift + modulo + do-while + for + for-step + switch + conditional parser negatives (`negative_loop_control_parser_*.objc3`, `negative_assignment_parser_*.objc3`, `negative_bitwise_parser_*.objc3`, `negative_modulo_parser_*.objc3`, `negative_do_while_parser_*.objc3`, `negative_for_parser_*.objc3`, `negative_for_step_parser_*.objc3`, `negative_switch_parser_*.objc3`, `negative_conditional_parser_*.objc3`) twice per fixture.
  - Enforces deterministic non-zero exit codes and deterministic `module.diagnostics.txt` / `module.diagnostics.json` hashes and code sets across replay.
  - Enforces expected fixture diagnostic headers and parser-only expected codes (`O3P*`).
  - Writes proof summary under `tmp/artifacts/objc3c-native/parser-replay-proof/<proof_run_id>/summary.json`.
- `npm run test:objc3c:parser-extraction-ast-builder-contract`
  - Runs `scripts/check_objc3c_parser_extraction_ast_builder_contract.ps1`.
  - Verifies parser extraction boundaries and AST builder scaffold markers in `parse/*`, `ast/*`, pipeline wiring, and CMake parser target registration.
  - Replays one positive parser scaffold fixture and selected negative parser fixtures with deterministic diagnostics/artifact assertions.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/parser-extraction-ast-builder-contract/<run_id>/summary.json`.
- `npm run test:objc3c:sema-pass-manager-diagnostics-bus`
  - Runs `scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`.
  - Runs `python -m pytest tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_pure_contract_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py -q`.
  - Verifies sema module extraction boundaries, parser-contract integration, pure-contract extraction boundary, and pipeline diagnostics-bus type surfaces.
  - Replays positive/negative sema fixtures and enforces deterministic diagnostics/artifact contracts for pass-manager + diagnostics-bus extraction.
  - Writes per-run summary JSON under `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`.
  - Uses deterministic default run id `m143-sema-type-system-default` unless `OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID` is explicitly provided.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1`
  - Replays all recovery fixtures (positive and negative) twice per fixture.
  - Includes optional Objective-C dispatch fixture roots when present (`recovery/positive/lowering_dispatch`, then `dispatch/positive`).
  - Enforces deterministic diagnostics for every fixture, deterministic manifests/IR for positive fixtures, and fail-closed artifact absence for negative fixtures.
  - For fixtures with `<fixture>.dispatch-ir.expect.txt`, emits replay IR via `clang -S -emit-llvm` and enforces deterministic dispatch IR marker matches across replay.
  - For fixtures with `<fixture>.objc3-ir.expect.txt`, enforces deterministic native-lowered IR marker matches across replay for emitted `module.ll`.
  - Optional clang override: `OBJC3C_NATIVE_LOWERING_CLANG_PATH=<clang executable>`.
  - Uses deterministic default run id `m143-lane-c-lowering-regression-default` unless `OBJC3C_NATIVE_LOWERING_RUN_ID` is explicitly provided.
  - Writes per-run summary under `tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json` plus stable latest summary at `tmp/artifacts/objc3c-native/lowering-regression/latest-summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1`
  - Replays typed-signature `.objc3` fixtures twice and enforces deterministic `module.ll` plus required typed ABI marker token presence from adjacent `.objc3-ir.expect.txt` files.
  - Uses deterministic default run id `m143-lane-c-typed-abi-default` unless `OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID` is explicitly provided.
  - Writes proof summary under `tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
  - Runs execution smoke twice with distinct run IDs.
  - Canonicalizes summary surfaces (excluding run-path entropy) and enforces SHA256 equality across replay.
  - Writes proof summary under `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lowering_replay_proof.ps1`
  - Runs lowering regression twice with distinct run IDs.
  - Captures each run's deterministic summary and asserts SHA256 equality across replay.
  - Uses deterministic default proof run id `m143-lane-c-lowering-replay-proof-default` unless `OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID` is explicitly provided.
  - Writes proof summary under `tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json`.

Direct script equivalents:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_recovery_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_native_fixture_matrix.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_perf_budget.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_native_compile_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_extraction_ast_builder_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_driver_shell_split_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lexer_extraction_token_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1
python scripts/check_m137_lexer_contract.py
python scripts/check_m139_sema_pass_manager_contract.py
python scripts/check_m142_frontend_lowering_parity_contract.py
python scripts/check_m143_artifact_tmp_governance_contract.py
python scripts/check_m144_llvm_capability_discovery_contract.py
python scripts/check_m145_direct_llvm_matrix_contract.py
python -m pytest tests/tooling/test_objc3c_lexer_parity.py -q
python scripts/check_m23_execution_readiness.py
python scripts/check_m24_execution_readiness.py
```

## M223 operator quickstart (docs+CI parity)

For deterministic day-to-day operator usage, run this minimal sequence from repo root:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m223/quickstart --emit-prefix module
npm run test:objc3c:m222-compatibility-migration
python scripts/build_objc3c_native_docs.py --check
```

Operational intent:

- `build:objc3c-native` verifies native toolchain wiring and executable output.
- `compile:objc3c` verifies deterministic compile artifact generation under `tmp/`.
- `test:objc3c:m222-compatibility-migration` verifies compatibility/migration contract surfaces.
- `build_objc3c_native_docs.py --check` verifies generated docs are in sync with source fragments.

## M223 validation/perf triage sequence

When validating release-facing behavior after compiler/runtime changes, run this ordered triage:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Expected evidence roots:

- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`

Fail-closed operator guidance:

1. treat non-zero exit in the above sequence as a hard stop.
2. inspect failing summary JSON first, then per-case logs under the same run root.
3. do not interpret replay/perf regressions without comparing both run1/run2 evidence packets.

## M224 validation/conformance/perf release readiness

For M224 release-readiness operators, run this fail-closed order from repo root:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Fail-closed release criteria:

1. any non-zero exit code in sequence is a hard stop; do not execute later commands.
2. `test:objc3c:m145-direct-llvm-matrix:lane-d` must execute both `scripts/check_conformance_suite.ps1` and `npm run test:objc3c:perf-budget`; treat either missing as release-blocking.
3. release-ready evidence requires both `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json` and `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`; missing summary evidence is a failure.

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m224_validation_release_contract.py -q
```

## M225 validation/perf roadmap seeding runbook

From repo root, run this deterministic order and stop immediately on the first non-zero exit:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Evidence packet fields for next-cycle milestone seeding:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m225_validation_roadmap_seed_contract.py -q
```

## M221 validation/perf GA blocker burn-down runbook

From repo root, run this deterministic blocker-burn sequence and fail closed on first non-zero exit:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

GA blocker evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m221_validation_ga_blocker_contract.py -q
```

## M220 validation/perf public-beta triage runbook

Public-beta triage loop requires deterministic validation/perf replay packets and strict command ordering.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Public-beta evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m220_validation_public_beta_contract.py -q
```

## M219 validation/perf cross-platform parity runbook

Cross-platform parity validation runs the same deterministic command sequence on Windows, Linux, and macOS.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Cross-platform parity evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `platform`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `platform`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `platform`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `platform`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m219_validation_cross_platform_contract.py -q
```

## M218 validation/perf RC provenance runbook

RC provenance validation runs deterministic test commands and captures attestable evidence packets.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

RC provenance evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `attestation_sha256`
  - `provenance.bundle_id`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `attestation_sha256`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `attestation_sha256`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `attestation_sha256`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m218_validation_rc_provenance_contract.py -q
```

## M217 validation/perf differential runbook

Differential testing runbook compares deterministic execution evidence against baseline toolchains.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Differential evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `baseline_delta_ms`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `baseline_diff_count`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `baseline_diff_count`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `baseline_diff_count`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m217_validation_differential_contract.py -q
```

## M216 validation/perf conformance suite runbook

Conformance suite v1 validation runs deterministic command order and records spec-mapped evidence packets.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Conformance suite evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `spec_section_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `spec_section_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `spec_section_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `spec_section_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m216_validation_conformance_contract.py -q
```

## M215 validation/perf SDK packaging runbook

SDK packaging validation runbook ensures deterministic evidence for IDE-consumable toolchain artifacts.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

SDK packaging evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `sdk_bundle_id`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `sdk_bundle_id`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `sdk_bundle_id`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `sdk_bundle_id`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m215_validation_sdk_packaging_contract.py -q
```

## M214 validation/perf daemonized compiler runbook

Daemon/watch validation runbook verifies deterministic incremental behavior and replay evidence.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Daemonized evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `incremental_cycle_id`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `incremental_cycle_id`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `incremental_cycle_id`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `incremental_cycle_id`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m214_validation_daemonized_contract.py -q
```

## M213 validation/perf debug-info fidelity runbook

Debug-fidelity validation runbook verifies deterministic evidence for DWARF/PDB emission and source-level stepping.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Debug-fidelity evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `debug_symbol_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `debug_symbol_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `debug_symbol_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `debug_symbol_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m213_validation_debug_fidelity_contract.py -q
```

## M212 validation/perf code-action runbook

Code-action/refactor validation runbook verifies deterministic rewrite-safety evidence.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Code-action evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `rewrite_safety_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `rewrite_safety_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `rewrite_safety_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `rewrite_safety_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m212_validation_code_action_contract.py -q
```

## M211 validation/perf LSP semantic runbook

LSP semantic-token/navigation validation runbook verifies deterministic editor-facing evidence.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

LSP semantic evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `semantic_token_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `semantic_token_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `semantic_token_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `semantic_token_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m211_validation_lsp_contract.py -q
```

## M210 validation/perf performance-budget regression runbook

Performance-budget regression gating runbook verifies deterministic budget and replay surfaces before integration promotion.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Performance-regression evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m210_validation_perf_regression_contract.py -q
```

## M209 validation/perf profile-guided optimization runbook

Profile-guided optimization (PGO) validation runbook verifies deterministic profile evidence surfaces before enabling optimization-driven policy changes.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

PGO evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m209_validation_pgo_contract.py -q
```

## M208 validation/perf whole-module optimization runbook

Whole-module optimization (WMO) validation runbook verifies deterministic module-shape and optimization-control evidence before promotion.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Whole-module optimization evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m208_validation_wmo_contract.py -q
```

## M207 validation/perf dispatch-specific optimization runbook

Dispatch-specific optimization validation runbook verifies deterministic dispatch-surface evidence before optimization pass promotion.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Dispatch-optimization evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m207_validation_dispatch_optimizations_contract.py -q
```

## M206 validation/perf canonical optimization pipeline stage-1 runbook

Canonical optimization stage-1 validation runbook verifies deterministic optimization-surface evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Canonical optimization stage-1 evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m206_validation_canonical_optimization_contract.py -q
```

## M205 validation/perf macro security policy runbook

Macro security policy validation runbook verifies deterministic fail-closed directive enforcement evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Macro-security evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m205_validation_macro_security_contract.py -q
```

## M204 validation/perf macro diagnostics and provenance runbook

Macro diagnostics/provenance validation runbook verifies deterministic diagnostic packet and source-location evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Macro-diagnostics evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m204_validation_macro_diagnostics_contract.py -q
```

## M203 validation/perf compile-time evaluation runbook

Compile-time evaluation validation runbook verifies deterministic constant-evaluation evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Compile-time-eval evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m203_validation_compile_time_eval_contract.py -q
```

## Current limitations (implemented behavior only)

- Top-level `.objc3` declarations currently include `module`, `let`, `fn`, `pure fn`, declaration-only `extern fn`, declaration-only `extern pure fn`, and declaration-only `pure extern fn`.
- Parameters and explicit function return annotations support `i32`, `bool`, `BOOL`, `NSInteger`, `NSUInteger`, `void` (return only), `id` (scalar alias), `Class` (scalar alias), `SEL` (scalar alias), `Protocol` (scalar alias), and `instancetype` (scalar alias).
- Function declarations can be full definitions (`{ ... }`) or semicolon-terminated prototypes, including explicit extern-qualified prototype declarations (`extern fn`, `extern pure fn`, `pure extern fn`).
- `id`/`Class`/`instancetype` parameter suffix forms (`id<...>`, `id?`, `id!`, `Class<...>`, `Class?`, `Class!`, `instancetype<...>`, `instancetype?`, `instancetype!`) are accepted and lowered as scalar aliases; non-`id`/`Class`/`instancetype` suffix usage remains deterministic `O3S206` diagnostics.
- `id`/`Class`/`instancetype` return suffix forms (`-> id<...>`, `-> id?`, `-> id!`, `-> Class<...>`, `-> Class?`, `-> Class!`, `-> instancetype<...>`, `-> instancetype?`, `-> instancetype!`) are accepted and lowered as scalar aliases; non-`id`/`Class`/`instancetype` return suffix usage remains deterministic `O3S206` diagnostics.
- Statements currently include `let`, assignment (`ident = expr;` plus `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=` forms), update operators (`ident++`, `ident--`, `++ident`, `--ident`), `return`, `if`/`else`, `do-while`, `for`, `switch`, `while`, `break`, `continue`, and expression statements.
- Standalone empty statements (`;`) are accepted and treated as no-op statements.
- Standalone nested block statements (`{ ... }`) are accepted with lexical scope boundaries.
- Control-flow bodies for `if`/`else`, `while`, `for`, and `do-while` may be either braced blocks or single statements.
- Expressions are limited to numeric/bool literals, identifiers, identifier-target calls, bracketed message-send expressions, unary `!`/`+`/`-`/`~`, arithmetic (including `%`), bitwise/shift, relational/equality operators, logical `&&`/`||`, conditional (`?:`) expressions, and grouping.
- Calls are identifier-only targets (no chained calls or non-identifier call expressions).
- Lowering emits typed function ABI signatures (`bool` as `i1`, `i32` as `i32`) with boundary casts to preserve expression-level `i32` evaluation.
- Message-send lowering is emitted as direct native IR calls to runtime dispatch shim `@objc3_msgsend_i32`.
- Message-send lowering supports up to four explicit arguments by default and can be lowered via `--objc3-max-message-args`.
- Selector literal globals are canonically named in lexicographic selector order for deterministic replay.
- Global initializers must be compile-time constant expressions over literals/operators and may reference previously declared globals.
- `if` conditions are lowered as non-zero truthiness checks; there is no dedicated condition-type diagnostic.
- Lexer comment support is limited to `// ...` and non-nested `/* ... */`.
- Duplicate `module` declarations are rejected with deterministic `O3S200` diagnostics.
# libobjc3c_frontend Library API (Embedding Contract)

This document defines the current public embedding API exposed by `native/objc3c/src/libobjc3c_frontend/api.h`.

## Public Surface

- Primary header: `native/objc3c/src/libobjc3c_frontend/api.h`
- Version header: `native/objc3c/src/libobjc3c_frontend/version.h`
- Optional C shim header: `native/objc3c/src/libobjc3c_frontend/c_api.h`

`api.h` exposes a C ABI usable from C and C++ with an opaque context type (`objc3c_frontend_context_t`).

## Stability

- stability boundary: exported symbols, enums, and struct layouts in `api.h`.
- ABI growth rule: append-only struct evolution; reserved fields remain reserved.
- Embedding rule: zero-initialize option/result structs to keep reserved fields deterministic.
- Current compile behavior is stable and intentional for now: compile entrypoints are scaffolded and return `OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR` after argument validation.

## Compatibility and versioning

- Semantic version source: `OBJC3C_FRONTEND_VERSION_{MAJOR,MINOR,PATCH}`.
- Current version string macro: `OBJC3C_FRONTEND_VERSION_STRING` (`"0.1.0"` in this workspace).
- ABI version source: `OBJC3C_FRONTEND_ABI_VERSION` (`1u` in this workspace).
- Compatibility window (inclusive): `OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION` through `OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION`.
- Startup check: call `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` before invoking compile entrypoints.

```c
#include "libobjc3c_frontend/api.h"

int objc3c_frontend_startup_check(void) {
  if (!objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)) {
    return 0;
  }

  const objc3c_frontend_version_t v = objc3c_frontend_version();
  return v.abi_version == objc3c_frontend_abi_version();
}
```

## M224 integration/release-readiness (1.0 ABI/version gates)

- Gate intent: fail closed on ABI/version drift before a 1.0 cut.
- Required startup/version invariants:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` must pass before compile entrypoints are used.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()` must remain true.
  - `OBJC3C_FRONTEND_ABI_VERSION` must stay inside the inclusive compatibility window
    `OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION` through
    `OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION`.
- Deterministic M224 integration gate:
  - `npm run check:objc3c:m224-integration-release-readiness`
  - This gate chains existing deterministic checks for M222 compatibility migration,
    library/CLI parity golden replay, and M224 tooling wiring.

## M225 integration roadmap seeding

- Gate intent: export ABI/version and deterministic gate evidence into 1.1/1.2 planning intake.
### 1.1 ABI/version continuity planning intake
- Preserve these intake invariants as seeded evidence for 1.1 planning:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` remains the required startup guard.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()` remains true.
  - `OBJC3C_FRONTEND_ABI_VERSION` stays within
    `OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION` through
    `OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain the planning intake anchors.
### 1.2 Gate-evidence planning intake
- Deterministic export gate:
  - `npm run check:objc3c:m225-roadmap-seeding`
- Exported evidence chain for 1.2 planning intake:
  - Replays `check:objc3c:m224-integration-release-readiness` as the baseline deterministic ABI/version gate.
  - Runs M225 roadmap-seeding contracts for frontend, sema/type, lowering/runtime, validation/perf, and integration wiring:
    `tests/tooling/test_objc3c_m225_frontend_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_sema_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_validation_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_integration_roadmap_seed_contract.py`.

## M221 integration GA blocker burn-down

- Gate intent: fail closed on unresolved GA blockers by chaining release-readiness and M221 lane contracts.
### 1.1 GA blocker integration chain
- Deterministic blocker gate:
  - `npm run check:objc3c:m221-ga-blocker-burndown`
- Chain order:
  - replays `check:objc3c:m225-roadmap-seeding`.
  - enforces all M221 lane contract surfaces:
    `tests/tooling/test_objc3c_m221_frontend_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_sema_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_lowering_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_validation_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_integration_ga_blocker_contract.py`.
### 1.2 ABI/version continuity constraints
- Keep startup/version invariants unchanged while burning down GA blockers:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain release anchors.

## M220 integration public-beta triage loop

- Gate intent: formalize public-beta intake/triage/patch loop with deterministic lane-contract replay.
### 1.1 Public-beta integration chain
- Deterministic triage gate:
  - `npm run check:objc3c:m220-public-beta-triage`
- Chain order:
  - replays `check:objc3c:m221-ga-blocker-burndown`.
  - enforces all M220 lane contracts:
    `tests/tooling/test_objc3c_m220_frontend_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_sema_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_lowering_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_validation_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_integration_public_beta_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve release guard invariants through beta loop execution:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain integration anchors.

## M219 integration cross-platform parity matrix

- Gate intent: enforce deterministic cross-platform parity replay across Windows, Linux, and macOS.
### 1.1 Cross-platform integration chain
- Deterministic parity gate:
  - `npm run check:objc3c:m219-cross-platform-parity`
- Chain order:
  - replays `check:objc3c:m220-public-beta-triage`.
  - enforces all M219 lane contracts:
    `tests/tooling/test_objc3c_m219_frontend_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_sema_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_lowering_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_validation_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_integration_cross_platform_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through parity runs:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain parity anchors.

## M218 integration RC automation and provenance

- Gate intent: enforce deterministic RC automation and provenance attestation chain across all lanes.
### 1.1 RC integration chain
- Deterministic RC gate:
  - `npm run check:objc3c:m218-rc-provenance`
- Chain order:
  - replays `check:objc3c:m219-cross-platform-parity`.
  - enforces all M218 lane contracts:
    `tests/tooling/test_objc3c_m218_frontend_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_sema_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_lowering_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_validation_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_integration_rc_provenance_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through RC automation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain provenance anchors.

## M217 integration differential testing matrix

- Gate intent: enforce deterministic differential testing chain against baseline toolchains.
### 1.1 Differential integration chain
- Deterministic differential gate:
  - `npm run check:objc3c:m217-differential-parity`
- Chain order:
  - replays `check:objc3c:m218-rc-provenance`.
  - enforces all M217 lane contracts:
    `tests/tooling/test_objc3c_m217_frontend_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_sema_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_lowering_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_validation_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_integration_differential_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through differential replay:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain differential anchors.

## M216 integration conformance suite v1

- Gate intent: enforce deterministic Objective-C 3 conformance suite v1 mapping against spec sections.
### 1.1 Conformance integration chain
- Deterministic conformance gate:
  - `npm run check:objc3c:m216-conformance-suite-v1`
- Chain order:
  - replays `check:objc3c:m217-differential-parity`.
  - enforces all M216 lane contracts:
    `tests/tooling/test_objc3c_m216_frontend_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_sema_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_lowering_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_validation_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_integration_conformance_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through conformance suite execution:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain conformance anchors.

## M215 integration SDK/toolchain packaging

- Gate intent: enforce deterministic SDK/toolchain packaging evidence for IDE consumption.
### 1.1 SDK packaging integration chain
- Deterministic SDK gate:
  - `npm run check:objc3c:m215-sdk-packaging`
- Chain order:
  - replays `check:objc3c:m216-conformance-suite-v1`.
  - enforces all M215 lane contracts:
    `tests/tooling/test_objc3c_m215_frontend_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_sema_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_lowering_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_validation_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_integration_sdk_packaging_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through SDK packaging validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain packaging anchors.

## M214 integration daemonized compiler/watch mode

- Gate intent: enforce deterministic daemon/watch mode evidence across all lanes.
### 1.1 Daemonized integration chain
- Deterministic daemonized gate:
  - `npm run check:objc3c:m214-daemonized-watch`
- Chain order:
  - replays `check:objc3c:m215-sdk-packaging`.
  - enforces all M214 lane contracts:
    `tests/tooling/test_objc3c_m214_frontend_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_sema_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_lowering_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_validation_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_integration_daemonized_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through daemonized/watch validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain daemonized anchors.

## M213 integration debug-info fidelity

- Gate intent: enforce deterministic debug-info fidelity evidence across all lanes.
### 1.1 Debug-fidelity integration chain
- Deterministic debug-fidelity gate:
  - `npm run check:objc3c:m213-debug-fidelity`
- Chain order:
  - replays `check:objc3c:m214-daemonized-watch`.
  - enforces all M213 lane contracts:
    `tests/tooling/test_objc3c_m213_frontend_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_sema_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_lowering_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_validation_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_integration_debug_fidelity_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through debug-fidelity validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain debug-fidelity anchors.

## M212 integration refactor/code-action engine

- Gate intent: enforce deterministic code-action/refactor evidence across all lanes.
### 1.1 Code-action integration chain
- Deterministic code-action gate:
  - `npm run check:objc3c:m212-code-action`
- Chain order:
  - replays `check:objc3c:m213-debug-fidelity`.
  - enforces all M212 lane contracts:
    `tests/tooling/test_objc3c_m212_frontend_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_sema_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_lowering_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_validation_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_integration_code_action_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through code-action validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain code-action anchors.

## M211 integration LSP semantic tokens and navigation

- Gate intent: enforce deterministic LSP semantic-token/navigation evidence across all lanes.
### 1.1 LSP integration chain
- Deterministic LSP gate:
  - `npm run check:objc3c:m211-lsp-semantics`
- Chain order:
  - replays `check:objc3c:m212-code-action`.
  - enforces all M211 lane contracts:
    `tests/tooling/test_objc3c_m211_frontend_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_sema_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_lowering_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_validation_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_integration_lsp_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through LSP semantic validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain LSP anchors.

## M210 integration performance budgets and regression gates

- Gate intent: enforce deterministic performance-budget and regression-gate evidence across all lanes.
### 1.1 Performance-regression integration chain
- Deterministic performance-regression gate:
  - `npm run check:objc3c:m210-performance-regression`
- Chain order:
  - replays `check:objc3c:m211-lsp-semantics`.
  - enforces all M210 lane contracts:
    `tests/tooling/test_objc3c_m210_frontend_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_sema_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_lowering_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_validation_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_integration_perf_regression_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through performance-regression validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain regression-gate anchors.

## M209 integration profile-guided optimization hooks

- Gate intent: enforce deterministic profile-guided optimization hook evidence across all lanes.
### 1.1 PGO integration chain
- Deterministic PGO hook gate:
  - `npm run check:objc3c:m209-pgo-hooks`
- Chain order:
  - replays `check:objc3c:m210-performance-regression`.
  - enforces all M209 lane contracts:
    `tests/tooling/test_objc3c_m209_frontend_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_sema_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_lowering_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_validation_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_integration_pgo_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through PGO-hook validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain PGO anchors.

## M208 integration whole-module optimization controls

- Gate intent: enforce deterministic whole-module optimization control evidence across all lanes.
### 1.1 WMO integration chain
- Deterministic WMO gate:
  - `npm run check:objc3c:m208-whole-module-optimization`
- Chain order:
  - replays `check:objc3c:m209-pgo-hooks`.
  - enforces all M208 lane contracts:
    `tests/tooling/test_objc3c_m208_frontend_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_sema_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_lowering_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_validation_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_integration_wmo_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through WMO validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain WMO anchors.

## M207 integration dispatch-specific optimization passes

- Gate intent: enforce deterministic dispatch-specific optimization evidence across all lanes.
### 1.1 Dispatch-optimization integration chain
- Deterministic dispatch-optimization gate:
  - `npm run check:objc3c:m207-dispatch-optimizations`
- Chain order:
  - replays `check:objc3c:m208-whole-module-optimization`.
  - enforces all M207 lane contracts:
    `tests/tooling/test_objc3c_m207_frontend_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_sema_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_lowering_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_validation_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_integration_dispatch_optimizations_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through dispatch-optimization validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain dispatch-optimization anchors.

## M206 integration canonical optimization pipeline stage-1

- Gate intent: enforce deterministic canonical-optimization stage-1 evidence across all lanes.
### 1.1 Canonical-optimization stage-1 chain
- Deterministic canonical-optimization stage-1 gate:
  - `npm run check:objc3c:m206-canonical-optimization-stage1`
- Chain order:
  - replays `check:objc3c:m207-dispatch-optimizations`.
  - enforces all M206 lane contracts:
    `tests/tooling/test_objc3c_m206_frontend_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_sema_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_lowering_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_validation_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_integration_canonical_optimization_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through canonical-optimization validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain canonical-optimization anchors.

## M205 integration macro security policy enforcement

- Gate intent: enforce deterministic macro-security policy evidence across all lanes.
### 1.1 Macro-security integration chain
- Deterministic macro-security gate:
  - `npm run check:objc3c:m205-macro-security`
- Chain order:
  - replays `check:objc3c:m206-canonical-optimization-stage1`.
  - enforces all M205 lane contracts:
    `tests/tooling/test_objc3c_m205_frontend_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_sema_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_lowering_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_validation_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_integration_macro_security_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through macro-security validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain macro-security anchors.

## M204 integration macro diagnostics and provenance

- Gate intent: enforce deterministic macro-diagnostics/provenance evidence across all lanes.
### 1.1 Macro-diagnostics integration chain
- Deterministic macro-diagnostics gate:
  - `npm run check:objc3c:m204-macro-diagnostics`
- Chain order:
  - replays `check:objc3c:m205-macro-security`.
  - enforces all M204 lane contracts:
    `tests/tooling/test_objc3c_m204_frontend_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_sema_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_lowering_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_validation_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_integration_macro_diagnostics_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through macro-diagnostics validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain macro-diagnostics anchors.

## M203 integration compile-time evaluation engine

- Gate intent: enforce deterministic compile-time-eval evidence across all lanes.
### 1.1 Compile-time-eval integration chain
- Deterministic compile-time-eval gate:
  - `npm run check:objc3c:m203-compile-time-eval`
- Chain order:
  - replays `check:objc3c:m204-macro-diagnostics`.
  - enforces all M203 lane contracts:
    `tests/tooling/test_objc3c_m203_frontend_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_sema_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_lowering_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_validation_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_integration_compile_time_eval_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through compile-time-eval validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain compile-time-eval anchors.

## Current call contract

- `objc3c_frontend_context_create()` returns `NULL` on allocation failure.
- `objc3c_frontend_context_destroy(ctx)` releases context resources.
- `objc3c_frontend_compile_file(...)` and `objc3c_frontend_compile_source(...)`:
  - Return `OBJC3C_FRONTEND_STATUS_USAGE_ERROR` when `context`, `options`, or `result` is `NULL`.
  - For non-null pointers, currently return `OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR` and set context error text to a scaffolded-entrypoint message.
- `objc3c_frontend_copy_last_error(ctx, buffer, buffer_size)`:
  - Returns required byte count including the trailing NUL.
  - Supports size probing with `buffer == NULL` or `buffer_size == 0`.
  - NUL-terminates written buffers when `buffer_size > 0`.

## Minimal embedding example (current API reality)

```c
#include <stdio.h>
#include <string.h>

#include "libobjc3c_frontend/api.h"

int main(void) {
  if (!objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)) {
    return 2;
  }

  objc3c_frontend_context_t *ctx = objc3c_frontend_context_create();
  if (ctx == NULL) {
    return 3;
  }

  objc3c_frontend_compile_options_t opts;
  memset(&opts, 0, sizeof(opts));
  opts.input_path = "example.objc3";
  opts.out_dir = "artifacts/compilation/objc3c-native";

  objc3c_frontend_compile_result_t result;
  memset(&result, 0, sizeof(result));

  const objc3c_frontend_status_t status = objc3c_frontend_compile_file(ctx, &opts, &result);
  if (status == OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR) {
    char err[512];
    (void)objc3c_frontend_copy_last_error(ctx, err, sizeof(err));
    fprintf(stderr, "frontend unavailable yet: %s\n", err);
  }

  objc3c_frontend_context_destroy(ctx);
  return status == OBJC3C_FRONTEND_STATUS_OK ? 0 : 1;
}
```

For pure C environments that prefer `*_c_*` symbol names, use `c_api.h`; it forwards to the same underlying ABI and behavior.
