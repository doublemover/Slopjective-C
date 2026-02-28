# Objc3c Native Frontend (Current Contract)

This document captures the currently implemented behavior for the native `objc3c` frontend (`native/objc3c/src/main.cpp`) and its command wrappers.

## Supported inputs

- `.objc3` files: custom lexer/parser + semantic checks + LLVM IR emission + object build.
- Any other extension (for example `.m`): libclang parse/diagnostics + symbol manifest + Objective-C object build.

## CLI usage

```text
objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>]
```

- Default `--out-dir`: `artifacts/compilation/objc3c-native`
- Default `--emit-prefix`: `module`
- Default `--clang`: `clang` (or explicit path)
- Default `--objc3-max-message-args`: `4`
- Default `--objc3-runtime-dispatch-symbol`: `objc3_msgsend_i32`

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
- CLI flags, defaults, and exit code semantics remain unchanged by the split.

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

## O3S201..O3S213 behavior (implemented now)

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
npm run test:objc3c:lowering-replay-proof
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:driver-shell-split
npm run proof:objc3c
npm run test:objc3c:lane-e
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
- `npm run proof:objc3c`
  - Runs `scripts/run_objc3c_native_compile_proof.ps1`.
  - Replays `tests/tooling/fixtures/native/hello.objc3` twice and writes `artifacts/compilation/objc3c-native/proof_20260226/digest.json` on success.
- `npm run test:objc3c:lane-e`
  - Runs lane-E deterministic validation chain:
    - `npm run test:objc3c`
    - `npm run test:objc3c:diagnostics-replay-proof`
    - `npm run test:objc3c:parser-replay-proof`
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
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1`
  - Replays all recovery fixtures (positive and negative) twice per fixture.
  - Includes optional Objective-C dispatch fixture roots when present (`recovery/positive/lowering_dispatch`, then `dispatch/positive`).
  - Enforces deterministic diagnostics for every fixture, deterministic manifests/IR for positive fixtures, and fail-closed artifact absence for negative fixtures.
  - For fixtures with `<fixture>.dispatch-ir.expect.txt`, emits replay IR via `clang -S -emit-llvm` and enforces deterministic dispatch IR marker matches across replay.
  - For fixtures with `<fixture>.objc3-ir.expect.txt`, enforces deterministic native-lowered IR marker matches across replay for emitted `module.ll`.
  - Optional clang override: `OBJC3C_NATIVE_LOWERING_CLANG_PATH=<clang executable>`.
  - Writes per-run summary under `tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json` plus stable latest summary at `tmp/artifacts/objc3c-native/lowering-regression/latest-summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1`
  - Replays typed-signature `.objc3` fixtures twice and enforces deterministic `module.ll` plus required typed ABI marker token presence from adjacent `.objc3-ir.expect.txt` files.
  - Writes proof summary under `tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
  - Runs execution smoke twice with distinct run IDs.
  - Canonicalizes summary surfaces (excluding run-path entropy) and enforces SHA256 equality across replay.
  - Writes proof summary under `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lowering_replay_proof.ps1`
  - Runs lowering regression twice with distinct run IDs.
  - Captures each run's deterministic summary and asserts SHA256 equality across replay.
  - Writes proof summary under `tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json`.

Direct script equivalents:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_recovery_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_native_fixture_matrix.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_perf_budget.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_native_compile_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_driver_shell_split_contract.ps1
python scripts/check_m23_execution_readiness.py
python scripts/check_m24_execution_readiness.py
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
