<!-- markdownlint-disable-file MD041 -->

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

## Frontend sema/type CLI-C-API parity contract (M142-B001)

- Lane-B sema/type parity surface remains deterministic through:
  - `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
  - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Manifest parity anchors under `frontend.pipeline.sema_pass_manager`:
  - pass diagnostics counters: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, `diagnostics_emitted_by_build`, `diagnostics_emitted_by_validate_bodies`, `diagnostics_emitted_by_validate_pure_contract`, `diagnostics_monotonic`, and `diagnostics_total`.
  - type-system parity counters: `deterministic_semantic_diagnostics`, `deterministic_type_metadata_handoff`, `ready`, `parity_ready`, `globals_total`, `functions_total`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
  - sema/type extension parity counters: `deterministic_atomic_memory_order_mapping`, `atomic_memory_order_mapping_total`, `atomic_relaxed_ops`, `atomic_acquire_ops`, `atomic_release_ops`, `atomic_acq_rel_ops`, `atomic_seq_cst_ops`, `atomic_unmapped_ops`, `deterministic_vector_type_lowering`, `vector_type_lowering_total`, `vector_return_annotations`, `vector_param_annotations`, `vector_i32_annotations`, `vector_bool_annotations`, `vector_lane2_annotations`, `vector_lane4_annotations`, `vector_lane8_annotations`, `vector_lane16_annotations`, and `vector_unsupported_annotations`.
- Contract validation command:
  - `python -m pytest tests/tooling/test_objc3c_m142_sema_cli_c_api_parity_contract.py -q`

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

## M202 sema/type derive/synthesis pipeline

For deterministic sema/type derive/synthesis pipeline behavior, capture replay-stable packet evidence from sema pass-manager derive hooks, type-metadata synthesis ordering, and manifest parity/readiness surfaces.

Derive/synthesis packet map:

- `derive packet 1.1 deterministic sema integration-surface derive hooks` -> `m202_sema_integration_surface_derive_packet`
- `synthesis packet 1.2 deterministic type-metadata synthesis hooks` -> `m202_type_metadata_synthesis_packet`

### 1.1 Deterministic sema integration-surface derive packet

- Source pass-manager derive anchors: `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);`, `ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);`, and `ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);`.
- Source integration-surface derive anchors: `Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,`, and `surface.built = true;`.
- Pipeline derive transport anchors: `Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);`, `result.integration_surface = std::move(sema_result.integration_surface);`, and `result.sema_parity_surface = sema_result.parity_surface;`.
- Deterministic integration-surface derive packet key: `m202_sema_integration_surface_derive_packet`.

### 1.2 Deterministic type-metadata synthesis packet

- Source type-synthesis anchors: `Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {`, `handoff.global_names_lexicographic.reserve(surface.globals.size());`, `std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());`, `std::sort(function_names.begin(), function_names.end());`, and `handoff.functions_lexicographic.reserve(function_names.size());`.
- Source synthesis determinism anchors: `bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {`, `return std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),`, `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, `result.deterministic_type_metadata_handoff =`, and `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`.
- Source parity/readiness synthesis anchors: `IsReadyObjc3SemaParityContractSurface(const Objc3SemaParityContractSurface &surface)`, `result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();`, `result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();`, and `result.parity_surface.ready =`.
- Manifest synthesis anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Semantic-surface synthesis anchors under `frontend.pipeline.semantic_surface`: `resolved_global_symbols` and `resolved_function_symbols`.
- Deterministic type-metadata synthesis packet key: `m202_type_metadata_synthesis_packet`.

Recommended M202 sema/type derive/synthesis validation command:

- `python -m pytest tests/tooling/test_objc3c_m202_sema_derive_synthesis_contract.py -q`

## M201 sema/type macro expansion architecture and isolation

For deterministic sema/type macro-expansion architecture and isolation, capture replay-stable packet evidence from lexer literal-expansion hooks, pipeline transport handoffs, and canonical-only sema isolation gates.

Macro expansion architecture/isolation packet map:

- `macro expansion packet 1.1 deterministic lexer expansion architecture hooks` -> `m201_sema_type_macro_expansion_architecture_packet`
- `macro expansion packet 1.2 deterministic sema isolation hooks` -> `m201_sema_type_macro_expansion_isolation_packet`

### 1.1 Deterministic lexer expansion architecture packet

- Source lexer expansion anchors: `} else if (ident == "YES") {`, `} else if (ident == "NO") {`, `} else if (ident == "NULL") {`, `kind = TokenKind::KwTrue;`, `kind = TokenKind::KwFalse;`, `kind = TokenKind::KwNil;`, `++migration_hints_.legacy_yes_count;`, `++migration_hints_.legacy_no_count;`, and `++migration_hints_.legacy_null_count;`.
- Pipeline transport anchors: `const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();`, `result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;`, `result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;`, `result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;`, `sema_input.migration_assist = options.migration_assist;`, `sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;`, `sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;`, and `sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;`.
- Source sema-input contract anchors: `bool migration_assist = false;` and `Objc3SemaMigrationHints migration_hints;`.
- Deterministic lexer expansion architecture packet key: `m201_sema_type_macro_expansion_architecture_packet`.

### 1.2 Deterministic sema isolation packet

- Source sema isolation anchors: `inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =`, `if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {`, `ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);`, `AppendMigrationAssistDiagnostics(input, pass_diagnostics);`, and `CanonicalizePassDiagnostics(pass_diagnostics);`.
- Manifest macro-isolation anchors under `frontend`: `manifest << "    \"migration_assist\":`, `manifest << "    \"migration_hints\":{\"legacy_yes\":`, and `legacy_total()`.
- Deterministic sema macro-expansion isolation packet key: `m201_sema_type_macro_expansion_isolation_packet`.

Recommended M201 sema/type macro expansion architecture/isolation validation command:

- `python -m pytest tests/tooling/test_objc3c_m201_sema_macro_expansion_contract.py -q`

## M200 sema/type interop integration suite and packaging

For deterministic sema/type interop integration suite and packaging, capture replay-stable packet evidence from pass-manager architecture ordering, pass-level isolation boundaries, and manifest packaging surfaces.

Interop integration suite + packaging packet map:

- `interop packet 1.1 deterministic sema/type interop architecture anchors` -> `m200_sema_type_interop_architecture_packet`
- `interop packet 1.2 deterministic sema/type interop isolation + packaging anchors` -> `m200_sema_type_interop_isolation_packaging_packet`

### 1.1 Deterministic sema/type interop architecture packet

- Source pass-architecture anchors: `inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =`, `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `if (pass == Objc3SemaPassId::BuildIntegrationSurface) {`, `result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);`, `ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);`, and `ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);`.
- Source deterministic-type anchors: `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, `result.deterministic_type_metadata_handoff =`, and `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`.
- Deterministic interop architecture packet key: `m200_sema_type_interop_architecture_packet`.

### 1.2 Deterministic sema/type interop isolation + packaging packet

- Source pass-isolation anchors: `CanonicalizePassDiagnostics(pass_diagnostics);`, `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();`, `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`, and `result.parity_surface.ready =`.
- Pipeline interop transport anchors: `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`, `Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);`, `result.integration_surface = std::move(sema_result.integration_surface);`, and `result.sema_parity_surface = sema_result.parity_surface;`.
- Manifest packaging anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, `diagnostics_emitted_by_build`, `diagnostics_emitted_by_validate_bodies`, `diagnostics_emitted_by_validate_pure_contract`, `deterministic_semantic_diagnostics`, `deterministic_type_metadata_handoff`, and `parity_ready`.
- Manifest packaging anchors under `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface`.
- Deterministic interop isolation + packaging packet key: `m200_sema_type_interop_isolation_packaging_packet`.

Recommended M200 sema/type interop integration suite and packaging command:

- `python -m pytest tests/tooling/test_objc3c_m200_sema_interop_packaging_contract.py -q`

## M199 sema/type foreign type import diagnostics

For deterministic sema/type foreign-type import diagnostics, capture replay-stable packet evidence from foreign-type spelling architecture predicates, type-metadata handoff continuity, and pass-manager diagnostic isolation.

Foreign type import diagnostics packet map:

- `foreign import packet 1.1 deterministic sema/type foreign-type architecture anchors` -> `m199_sema_type_foreign_import_architecture_packet`
- `foreign import packet 1.2 deterministic sema foreign-type diagnostic isolation anchors` -> `m199_sema_foreign_import_diagnostic_isolation_packet`

### 1.1 Deterministic sema/type foreign-type architecture packet

- Source foreign-type architecture anchors: `static bool SupportsGenericParamTypeSuffix(const FuncParam &param) {`, `return param.id_spelling || param.class_spelling || param.instancetype_spelling;`, `static bool SupportsGenericReturnTypeSuffix(const FunctionDecl &fn) {`, `return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;`, and `HasInvalidParamTypeSuffix(param)`.
- Source type-metadata handoff anchors: `info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));`, `metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;`, and `metadata.param_has_invalid_type_suffix.size() == metadata.arity;`.
- Deterministic foreign-type architecture packet key: `m199_sema_type_foreign_import_architecture_packet`.

### 1.2 Deterministic sema foreign-type diagnostic isolation packet

- Source foreign-type diagnostic anchors: `ValidateReturnTypeSuffixes(fn, diagnostics);`, `ValidateParameterTypeSuffixes(fn, diagnostics);`, `"type mismatch: generic parameter type suffix '" + suffix +`, `"type mismatch: pointer parameter type declarator '" + token.text +`, `"type mismatch: nullability parameter type suffix '" + token.text +`, `"type mismatch: unsupported function return type suffix '" + suffix +`, `"type mismatch: unsupported function return type declarator '" + token.text +`, and `"O3S206"`.
- Source pass-manager diagnostic-isolation anchors: `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `CanonicalizePassDiagnostics(pass_diagnostics);`, `input.diagnostics_bus.PublishBatch(pass_diagnostics);`, `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();`, and `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`.
- Deterministic foreign-type diagnostic-isolation packet key: `m199_sema_foreign_import_diagnostic_isolation_packet`.

Recommended M199 sema/type foreign type import diagnostics command:

- `python -m pytest tests/tooling/test_objc3c_m199_sema_foreign_type_diagnostics_contract.py -q`

## M198 sema/type swift metadata bridge

For deterministic sema/type Swift metadata-bridge behavior, capture replay-stable packet evidence from parser token-metadata capture, semantic suffix validation hooks, and pass-manager isolation handoff surfaces.

Swift metadata bridge packet map:

- `swift metadata packet 1.1 deterministic sema/type bridge architecture anchors` -> `m198_sema_type_swift_metadata_architecture_packet`
- `swift metadata packet 1.2 deterministic sema/type bridge isolation anchors` -> `m198_sema_type_swift_metadata_isolation_packet`

### 1.1 Deterministic sema/type swift metadata bridge architecture packet

- Source parser metadata-capture anchors: `static Objc3SemaTokenMetadata MakeSemaTokenMetadata(Objc3SemaTokenKind kind, const Token &token) {`, `return MakeObjc3SemaTokenMetadata(kind, token.text, token.line, token.column);`, `MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));`, and `MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));`.
- Source AST metadata-carrier anchors: `std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;`, `std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;`, `std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;`, and `std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;`.
- Source sema validation anchors: `ValidateReturnTypeSuffixes(fn, diagnostics);`, `ValidateParameterTypeSuffixes(fn, diagnostics);`, `for (const auto &token : param.pointer_declarator_tokens) {`, `for (const auto &token : param.nullability_suffix_tokens) {`, `for (const auto &token : fn.return_pointer_declarator_tokens) {`, and `for (const auto &token : fn.return_nullability_suffix_tokens) {`.
- Source type-metadata bridge anchors: `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`, `metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;`, and `metadata.param_has_invalid_type_suffix.size() == metadata.arity;`.
- Deterministic sema/type Swift metadata bridge architecture packet key: `m198_sema_type_swift_metadata_architecture_packet`.

### 1.2 Deterministic sema/type swift metadata bridge isolation packet

- Source pass-manager isolation anchors: `inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =`, `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `CanonicalizePassDiagnostics(pass_diagnostics);`, `input.diagnostics_bus.PublishBatch(pass_diagnostics);`, `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();`, and `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`.
- Pipeline bridge transport anchors: `sema_input.program = &result.program;`, `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`, `Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);`, and `result.sema_parity_surface = sema_result.parity_surface;`.
- Manifest sema/type parity anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_type_metadata_handoff`, `parity_ready`, `type_metadata_global_entries`, and `type_metadata_function_entries`.
- Deterministic sema/type Swift metadata bridge isolation packet key: `m198_sema_type_swift_metadata_isolation_packet`.

Recommended M198 sema/type Swift metadata bridge validation command:

- `python -m pytest tests/tooling/test_objc3c_m198_sema_swift_metadata_bridge_contract.py -q`

## M197 sema/type C++ interop shim strategy

For deterministic sema/type C++ interop shim strategy behavior, capture replay-stable packet evidence from C ABI shim forwarding into the C++ frontend pipeline and sema/type isolation boundaries.

C++ interop shim packet map:

- `interop shim packet 1.1 deterministic sema/type C++ interop architecture anchors` -> `m197_sema_type_cpp_interop_shim_architecture_packet`
- `interop shim packet 1.2 deterministic sema/type C++ interop isolation anchors` -> `m197_sema_type_cpp_interop_shim_isolation_packet`

### 1.1 Deterministic sema/type C++ interop shim architecture packet

- Source C ABI shim anchors: `Optional C ABI shim for non-C++ embedding environments.`, `typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;`, `static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,`, and `extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(`.
- Source C++ frontend pipeline bridge anchors: `Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(`, `product.pipeline_result = RunObjc3FrontendPipeline(source, options);`, `product.artifact_bundle = BuildObjc3FrontendArtifacts(input_path, product.pipeline_result, options);`, `Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);`, and `Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);`.
- Source sema/type metadata handoff anchors: `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, `result.deterministic_type_metadata_handoff =`, and `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`.
- Deterministic C++ interop shim architecture packet key: `m197_sema_type_cpp_interop_shim_architecture_packet`.

### 1.2 Deterministic sema/type C++ interop shim isolation packet

- Source sema pass-isolation anchors: `inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =`, `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `CanonicalizePassDiagnostics(pass_diagnostics);`, `input.diagnostics_bus.PublishBatch(pass_diagnostics);`, `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();`, `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`, and `result.parity_surface.ready =`.
- Pipeline sema transport-isolation anchors: `sema_input.program = &result.program;`, `sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy`, `sema_input.migration_assist = options.migration_assist;`, `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`, and `result.sema_parity_surface = sema_result.parity_surface;`.
- Build/link isolation anchors: `add_library(objc3c_sema_type_system INTERFACE)`, `target_link_libraries(objc3c_lower PUBLIC`, `target_link_libraries(objc3c_ir PUBLIC`, `add_library(objc3c_runtime_abi STATIC`, `target_link_libraries(objc3c_frontend PUBLIC`, and `objc3c_runtime_abi`.
- Deterministic C++ interop shim isolation packet key: `m197_sema_type_cpp_interop_shim_isolation_packet`.

Recommended M197 sema/type C++ interop shim strategy validation command:

- `python -m pytest tests/tooling/test_objc3c_m197_sema_cpp_interop_shim_contract.py -q`

## M196 sema/type C interop headers and ABI alignment

For deterministic sema/type C interop headers and ABI alignment behavior, capture replay-stable packet evidence from C wrapper ABI identity surfaces and sema pass-manager isolation boundaries.

C interop headers + ABI alignment packet map:

- `c interop packet 1.1 deterministic sema/type C interop headers ABI architecture anchors` -> `m196_sema_type_c_interop_headers_abi_architecture_packet`
- `c interop packet 1.2 deterministic sema/type C interop headers ABI isolation anchors` -> `m196_sema_type_c_interop_headers_abi_isolation_packet`

### 1.1 Deterministic sema/type C interop headers ABI architecture packet

- Source C interop header ABI anchors: `#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u`, `typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;`, and `typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;`.
- Source C wrapper ABI-alignment guard anchors: `static_assert(std::is_same_v<objc3c_frontend_c_context_t, objc3c_frontend_context_t>,`, `static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,`, and `static_assert(std::is_same_v<objc3c_frontend_c_compile_result_t, objc3c_frontend_compile_result_t>,`.
- Source frontend option-normalization anchors: `Objc3FrontendOptions BuildFrontendOptions(const objc3c_frontend_compile_options_t &options) {`, `frontend_options.compatibility_mode =`, `frontend_options.migration_assist = options.migration_assist != 0;`, and `frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;`.
- Source pipeline ABI contract anchors: `inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;`, `inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;`, and `inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";`.
- Deterministic sema/type C interop headers ABI architecture packet key: `m196_sema_type_c_interop_headers_abi_architecture_packet`.

### 1.2 Deterministic sema/type C interop headers ABI isolation packet

- Source sema pass-isolation anchors: `inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =`, `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `CanonicalizePassDiagnostics(pass_diagnostics);`, `input.diagnostics_bus.PublishBatch(pass_diagnostics);`, `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();`, `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`, `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, and `result.parity_surface.ready =`.
- Pipeline sema transport-isolation anchors: `sema_input.program = &result.program;`, `sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy`, `sema_input.migration_assist = options.migration_assist;`, `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`, `Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);`, and `result.sema_parity_surface = sema_result.parity_surface;`.
- Build/link runtime ABI isolation anchors: `add_library(objc3c_sema_type_system INTERFACE)`, `add_library(objc3c_runtime_abi STATIC`, `target_link_libraries(objc3c_frontend PUBLIC`, and `objc3c_runtime_abi`.
- Deterministic sema/type C interop headers ABI isolation packet key: `m196_sema_type_c_interop_headers_abi_isolation_packet`.

Recommended M196 sema/type C interop headers and ABI alignment validation command:

- `python -m pytest tests/tooling/test_objc3c_m196_sema_c_interop_headers_abi_contract.py -q`

## M195 sema/type system-extension conformance and policy

For deterministic sema/type system-extension conformance and policy behavior, capture replay-stable packet evidence from compile-option policy gates, sema pass-manager architecture contracts, and manifest isolation surfaces.

System-extension conformance + policy packet map:

- `system-extension packet 1.1 deterministic sema/type conformance architecture anchors` -> `m195_sema_type_system_extension_conformance_architecture_packet`
- `system-extension packet 1.2 deterministic sema/type policy isolation anchors` -> `m195_sema_type_system_extension_policy_isolation_packet`

### 1.1 Deterministic sema/type conformance architecture packet

- Source compile-option policy anchors: `static bool ValidateSupportedLanguageVersion(uint8_t requested_language_version, std::string &error) {`, `static bool ValidateSupportedCompatibilityMode(uint8_t requested_compatibility_mode, std::string &error) {`, and `if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {`.
- Source sema input-contract anchors: `Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;`, `bool migration_assist = false;`, `Objc3SemaDiagnosticsBus diagnostics_bus;`, and `inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder =`.
- Pipeline sema transport anchors: `if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {`, `sema_input.validation_options = semantic_options;`, `sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy`, `sema_input.migration_assist = options.migration_assist;`, `sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;`, and `Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);`.
- Source sema/type architecture anchors: `result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);`, `result.deterministic_type_metadata_handoff =`, and `IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);`.
- Deterministic sema/type conformance architecture packet key: `m195_sema_type_system_extension_conformance_architecture_packet`.

### 1.2 Deterministic sema/type policy isolation packet

- Source sema pass-isolation anchors: `for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {`, `CanonicalizePassDiagnostics(pass_diagnostics);`, `input.diagnostics_bus.PublishBatch(pass_diagnostics);`, `result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();`, `result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();`, and `result.parity_surface.ready =`.
- Manifest policy-isolation anchors under `frontend.pipeline.sema_pass_manager`: `diagnostics_after_build`, `diagnostics_after_validate_bodies`, `diagnostics_after_validate_pure_contract`, `deterministic_semantic_diagnostics`, `deterministic_type_metadata_handoff`, and `parity_ready`.
- Manifest policy-isolation anchors under `frontend.pipeline.semantic_surface`: `resolved_global_symbols`, `resolved_function_symbols`, and `function_signature_surface`.
- Deterministic sema/type policy isolation packet key: `m195_sema_type_system_extension_policy_isolation_packet`.

Recommended M195 sema/type system-extension conformance and policy validation command:

- `python -m pytest tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py -q`

## M194 sema/type atomics and memory-order mapping

For deterministic sema/type atomics and memory-order mapping behavior, capture replay-stable packet evidence from assignment-operator memory-order classification, sema pass-manager parity transport, and manifest isolation surfaces.

Atomics memory-order packet map:

- `atomics packet 1.1 deterministic sema/type memory-order architecture anchors` -> `m194_sema_type_atomic_memory_order_architecture_packet`
- `atomics packet 1.2 deterministic sema/type memory-order isolation anchors` -> `m194_sema_type_atomic_memory_order_isolation_packet`

### 1.1 Deterministic sema/type memory-order architecture packet

- Source sema contract anchors: `enum class Objc3SemaAtomicMemoryOrder : std::uint8_t {` and `struct Objc3AtomicMemoryOrderMappingSummary {`.
- Source sema mapping anchors: `MapAssignmentOperatorToAtomicMemoryOrder(...)`, `FormatAtomicMemoryOrderMappingHint(...)`, and `BuildAtomicMemoryOrderMappingSummary(...)`.
- Source sema pass-manager architecture anchors: `result.atomic_memory_order_mapping = BuildAtomicMemoryOrderMappingSummary(*input.program);` and `result.deterministic_atomic_memory_order_mapping = result.atomic_memory_order_mapping.deterministic;`.
- Deterministic sema/type atomics architecture packet key: `m194_sema_type_atomic_memory_order_architecture_packet`.

### 1.2 Deterministic sema/type memory-order isolation packet

- Source sema pass-manager isolation anchors: `result.parity_surface.atomic_memory_order_mapping = result.atomic_memory_order_mapping;`, `result.parity_surface.deterministic_atomic_memory_order_mapping = result.deterministic_atomic_memory_order_mapping;`, and `result.parity_surface.ready =`.
- Source sema parity contract anchors: `Objc3AtomicMemoryOrderMappingSummary atomic_memory_order_mapping;` and `bool deterministic_atomic_memory_order_mapping = false;`.
- Manifest isolation anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_atomic_memory_order_mapping`, `atomic_memory_order_mapping_total`, `atomic_relaxed_ops`, `atomic_acquire_ops`, `atomic_release_ops`, `atomic_acq_rel_ops`, `atomic_seq_cst_ops`, and `atomic_unmapped_ops`.
- Deterministic sema/type atomics isolation packet key: `m194_sema_type_atomic_memory_order_isolation_packet`.

Recommended M194 sema/type atomics and memory-order mapping validation command:

- `python -m pytest tests/tooling/test_objc3c_m194_sema_atomics_memory_order_contract.py -q`

## M193 sema/type SIMD/vector type lowering

For deterministic sema/type SIMD/vector type-lowering behavior, capture replay-stable packet evidence from vector-aware type metadata transport, semantic diagnostics isolation, and sema parity-surface replay fields.

SIMD/vector packet map:

- `simd vector packet 1.1 deterministic sema/type vector architecture anchors` -> `m193_sema_type_simd_vector_architecture_packet`
- `simd vector packet 1.2 deterministic sema/type vector isolation anchors` -> `m193_sema_type_simd_vector_isolation_packet`

### 1.1 Deterministic sema/type vector architecture packet

- Source sema contract anchors: `struct Objc3VectorTypeLoweringSummary {`, `std::vector<bool> param_is_vector;`, `std::vector<std::string> param_vector_base_spelling;`, `std::vector<unsigned> param_vector_lane_count;`, `bool return_is_vector = false;`, and `unsigned return_vector_lane_count = 1;`.
- Source semantic type-system anchors: `struct SemanticTypeInfo {`, `MakeSemanticTypeFromParam(...)`, `MakeSemanticTypeFromFunctionReturn(...)`, `MakeSemanticTypeFromFunctionInfoParam(...)`, `IsSameSemanticType(...)`, and `SemanticTypeName(...)`.
- Source sema behavior anchors: `RecordVectorTypeLoweringAnnotation(...)`, `BuildVectorTypeLoweringSummary(...)`, `existing.return_is_vector == fn.return_vector_spelling`, and `type mismatch: incompatible function signature for`.
- Source diagnostics anchors: `type mismatch: expected '`, `type mismatch: assignment to '`, and `type mismatch: return expression in function '`.
- Deterministic sema/type SIMD/vector architecture packet key: `m193_sema_type_simd_vector_architecture_packet`.

### 1.2 Deterministic sema/type vector isolation packet

- Source sema pass-manager anchors: `result.vector_type_lowering = BuildVectorTypeLoweringSummary(result.integration_surface);`, `result.deterministic_vector_type_lowering = result.vector_type_lowering.deterministic;`, `result.parity_surface.vector_type_lowering = result.vector_type_lowering;`, and `result.parity_surface.deterministic_vector_type_lowering = result.deterministic_vector_type_lowering;`.
- Source sema parity contract anchors: `Objc3VectorTypeLoweringSummary vector_type_lowering;`, `bool deterministic_vector_type_lowering = false;`, and `surface.deterministic_vector_type_lowering`.
- Manifest isolation anchors under `frontend.pipeline.sema_pass_manager`: `deterministic_vector_type_lowering`, `vector_type_lowering_total`, `vector_return_annotations`, `vector_param_annotations`, `vector_i32_annotations`, `vector_bool_annotations`, `vector_lane2_annotations`, `vector_lane4_annotations`, `vector_lane8_annotations`, `vector_lane16_annotations`, and `vector_unsupported_annotations`.
- Vector lane contract remains deterministic as parser-accepted `2/4/8/16` lane spellings.
- Deterministic sema/type SIMD/vector isolation packet key: `m193_sema_type_simd_vector_isolation_packet`.

Recommended M193 sema/type SIMD/vector type lowering validation command:

- `python -m pytest tests/tooling/test_objc3c_m193_sema_simd_vector_lowering_contract.py -q`

## M146 sema/type @interface/@implementation parity contract (M146-B001)

M146-B extends sema/type metadata to track Objective-C interface/implementation declarations and selector-level coherence.

Sema/type contract markers:

- `Objc3MethodInfo`
- `Objc3InterfaceInfo`
- `Objc3ImplementationInfo`
- `Objc3InterfaceImplementationSummary`
- `interfaces_total`
- `implementations_total`
- `type_metadata_interface_entries`
- `type_metadata_implementation_entries`
- `deterministic_interface_implementation_handoff`

Semantic coherence diagnostics (fail-closed):

- missing interface declaration for implementation
- duplicate interface selector / duplicate implementation selector
- incompatible method signature for selector

Sema/type metadata handoff contract:

- interface metadata packet: `handoff.interfaces_lexicographic`
- implementation metadata packet: `handoff.implementations_lexicographic`
- deterministic summary packet: `interface_implementation_summary`

Recommended M146 sema contract check:

- `python -m pytest tests/tooling/test_objc3c_m146_sema_interface_implementation_contract.py -q`

## M147 sema/type @protocol/@category composition contract (M147-B001)

M147-B extends sema/type metadata and deterministic parity surfaces for protocol-composition suffixes and
Objective-C category-context method composition tracking.

Sema/type contract markers:

- `Objc3ProtocolCategoryCompositionSummary`
- `param_has_protocol_composition`
- `param_protocol_composition_lexicographic`
- `param_has_invalid_protocol_composition`
- `return_has_protocol_composition`
- `return_protocol_composition_lexicographic`
- `return_has_invalid_protocol_composition`
- `protocol_composition_sites_total`
- `category_composition_sites_total`
- `deterministic_protocol_category_composition_handoff`

Semantic coherence diagnostics (fail-closed):

- malformed protocol composition suffix
- empty protocol composition suffix
- invalid protocol identifier
- duplicate protocol identifier
- incompatible function signature for composition drift
- incompatible method signature for selector composition drift

Sema/type metadata handoff contract:

- protocol/category summary packet: `protocol_category_composition_summary`
- function packet composition fields: `param_protocol_composition_lexicographic`
- method packet composition fields: `return_protocol_composition_lexicographic`

Recommended M147 sema contract check:

- `python -m pytest tests/tooling/test_objc3c_m147_sema_protocol_category_contract.py -q`

## M148 sema/type selector-normalized method declaration contract (M148-B001)

M148-B extends sema/type metadata and pass-manager parity surfaces for selector-normalized Objective-C
method declarations consumed from selector-piece grammar.

Sema/type contract markers:

- `Objc3SelectorNormalizationSummary`
- `selector_normalization_summary`
- `selector_normalization_methods_total`
- `selector_normalization_normalized_methods_total`
- `selector_contract_normalized`
- `selector_piece_count`
- `selector_parameter_piece_count`
- `selector_has_parameter_linkage_mismatch`
- `deterministic_selector_normalization_handoff`

Deterministic semantic diagnostics (fail-closed):

- selector normalization requires selector pieces
- selector normalization mismatch
- selector normalization flag mismatch
- selector arity mismatch
- selector parameter linkage mismatch

Sema/type metadata handoff contract:

- normalization summary packet: `handoff.selector_normalization_summary`
- method packet selector fields: `selector_normalized`, `selector_piece_count`, `selector_parameter_piece_count`
- parity packet gate: `result.parity_surface.deterministic_selector_normalization_handoff`

Recommended M148 sema contract check:

- `python -m pytest tests/tooling/test_objc3c_m148_sema_selector_normalization_contract.py -q`

## M149 sema/type @property attribute and accessor modifier contract (M149-B001)

M149-B extends sema/type metadata and pass-manager parity surfaces for Objective-C `@property` declarations,
attribute packs, and accessor selector modifiers.

Sema/type contract markers:

- `Objc3PropertyAttributeSummary`
- `Objc3PropertyInfo`
- `property_attribute_summary`
- `property_attribute_entries_total`
- `property_attribute_contract_violations_total`
- `has_accessor_selector_contract_violation`
- `has_invalid_attribute_contract`
- `deterministic_property_attribute_handoff`

Deterministic semantic diagnostics (fail-closed):

- unknown `@property` attribute
- duplicate `@property` attribute
- invalid `getter` / `setter` selector contracts
- conflicting attribute families (`readonly/readwrite`, `atomic/nonatomic`, ownership modifiers)
- incompatible property signature between interface and implementation

Sema/type metadata handoff contract:

- property summary packet: `handoff.property_attribute_summary`
- interface/implementation property packets: `properties_lexicographic`
- parity packet gate: `result.parity_surface.deterministic_property_attribute_handoff`

Recommended M149 sema contract check:

- `python -m pytest tests/tooling/test_objc3c_m149_sema_property_attribute_contract.py -q`
