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

