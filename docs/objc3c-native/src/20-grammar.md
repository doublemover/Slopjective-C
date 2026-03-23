<!-- markdownlint-disable-file MD041 -->

## Supported `.objc3` grammar (implemented today)

```ebnf
program         = { module_decl | global_let | function_decl } EOF ;

module_decl     = "module" ident ";" ;
global_let      = "let" ident "=" expr ";" ;
function_decl   = "fn" ident "(" [ param { "," param } ] ")" [ throws_clause ] [ "->" return_type ] [ throws_clause ] ( block | ";" ) ;
throws_clause   = "throws" ;
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

## M146 frontend @interface/@implementation grammar

Frontend parser/AST support now accepts Objective-C container declarations:

- `@interface <Name> [: <Super>] ... @end`
- `@implementation <Name> ... @end`
- `@end`

M146 parser surface details:

- Lexer contract emits dedicated tokens:
  - `KwAtInterface`
  - `KwAtImplementation`
  - `KwAtEnd`
- Parser top-level routes:
  - `ParseObjcInterfaceDecl()`
  - `ParseObjcImplementationDecl()`
- Parser class-method surface:
  - `ParseObjcMethodDecl(...)` supports `-` and `+` method markers.
  - `@interface` requires `;` method declarations.
  - `@implementation` accepts `;` declarations and braced method bodies (balanced-brace recovery).

Deterministic recovery/diagnostic anchors:

- unsupported `@` directives fail closed (`unsupported '@' directive '@<name>'`).
- missing container terminators fail closed:
  - `missing '@end' after @interface`
  - `missing '@end' after @implementation`

Recommended M146 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m146_frontend_interface_implementation_contract.py -q`

## M147 frontend @protocol/@category grammar

Frontend parser/AST support now accepts Objective-C protocol and category declaration surfaces:

- `@protocol <Name> [<InheritedA, InheritedB>] ... @end`
- `@protocol <Name>;` forward declaration
- `@interface <Name> (<CategoryName>) [<AdoptedA, AdoptedB>] ... @end`
- `@implementation <Name> (<CategoryName>) ... @end`

M147 parser surface details:

- Lexer contract emits dedicated token:
  - `KwAtProtocol`
- Parser top-level route:
  - `ParseObjcProtocolDecl()`
- Parser composition/category helpers:
  - `ParseObjcProtocolCompositionClause(...)` supports `<A, B>` lists.
  - `ParseObjcCategoryClause(...)` supports named and anonymous category clauses.

Deterministic recovery/diagnostic anchors:

- invalid protocol/category identifiers fail closed:
  - `invalid Objective-C protocol identifier`
  - `invalid Objective-C protocol composition identifier`
  - `missing ')' after Objective-C category name`
- missing container terminators fail closed:
  - `missing '@end' after @protocol`

Recommended M147 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m147_frontend_protocol_category_contract.py -q`

## M148 frontend selector-normalized method declaration grammar

Frontend parser/AST method-declaration support now captures selector pieces and emits a canonical selector spelling for
both declarations and definitions.

M148 parser surface details:

- Selector canonicalization helper:
  - `BuildNormalizedObjcSelector(...)`
- Method parser behavior (`ParseObjcMethodDecl(...)`):
  - captures each selector piece in `selector_pieces`
  - records parameter linkage per piece (`parameter_name`)
  - emits deterministic canonical selector text in `selector`
  - marks canonicalization completion with `selector_is_normalized`

Deterministic grammar intent:

- method declarations and definitions share the same selector-piece normalization path
- canonical selector text is derived from parsed pieces instead of ad-hoc string concatenation

Recommended M148 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m148_frontend_selector_normalization_contract.py -q`

## M149 frontend @property grammar and attribute parsing

Frontend parser/AST now accepts Objective-C property declarations within container bodies and captures attribute /
accessor-modifier metadata in a deterministic structure.

M149 parser surface details:

- Lexer contract emits dedicated token:
  - `KwAtProperty`
- Property parser helpers:
  - `ParseObjcPropertyDecl(...)`
  - `ParseObjcPropertyAttributes(...)`
  - `ParseObjcPropertyAttributeValueText()`
  - `ApplyObjcPropertyAttributes(...)`
- Container integration:
  - `@protocol`, `@interface`, and `@implementation` now accept `@property ...;` entries.
  - parsed properties are stored in `properties` vectors on container AST nodes.

Deterministic recovery/diagnostic anchors:

- invalid property attribute/name failures:
  - `invalid Objective-C @property attribute`
  - `invalid Objective-C @property identifier`
- malformed attribute list / declaration termination:
  - `missing ')' after Objective-C @property attribute list`
  - `missing ';' after Objective-C @property declaration`

Recommended M149 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m149_frontend_property_attribute_contract.py -q`

## M150 frontend object pointer declarators, nullability, lightweight generics parse

Frontend parser/AST type parsing now accepts nominal object-pointer spellings for parameter, return, and property type
annotations while preserving pointer/nullability/generic suffix contracts.

M150 parser/AST surface details:

- function return type parse (`ParseFunctionReturnType(...)`) accepts identifier-based object pointer base spellings.
- parameter type parse (`ParseParameterType(...)`) accepts identifier-based object pointer base spellings.
- object-pointer AST markers:
  - `object_pointer_type_spelling`
  - `object_pointer_type_name`
  - `return_object_pointer_type_spelling`
  - `return_object_pointer_type_name`
- property type handoff preserves object pointer markers through `CopyPropertyTypeFromParam(...)`.

Deterministic grammar intent:

- object pointer spellings may carry generic suffix text, pointer declarators, and nullability suffix tokens.
- vector-type spellings remain parsed through the existing vector branch.

Recommended M150 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m150_frontend_object_pointer_nullability_generics_contract.py -q`

## M265 frontend Part 3 type source closure

Lane A now freezes the truthful parser-owned Part 3 type surface before the
later M265 issues add runnable optional semantics.

Currently live in the frontend:

- protocol `@required` / `@optional` partitions
- object-pointer type spellings carried through parameter, return, and property
  annotations
- nullability suffix carriers (`?` / `!`) on those object-pointer spellings
- pragmatic generic suffix carriers such as `id<Protocol>*?`
- optional binding forms `if let`, `if var`, `guard let`, and `guard var`
- optional sends written as `[receiver? selector]`
- nil-coalescing expressions written with `??`
- typed key-path literals such as `@keypath(self, title)`

Current native lowering boundary:

- validated single-component typed key-path literals now lower into retained
  descriptor artifacts and stable nonzero handles on the native path
- broader typed key-path application/runtime behavior still remains deferred to
  later `M265` lane-D work

Deterministic current behavior:

- contiguous `?.` now tokenizes as one punctuator and lowers by desugaring
  into the same optional-send/nil-short-circuit path used by bracketed
  optional sends
- `if let` / `guard let` are parser-owned source forms in this lane and lower
  into deterministic frontend control-flow scaffolding for later `M265` sema
  and lowering work
- optional sends are admitted as parser-owned message-send forms and published
  through the frontend semantic surface
- optional-member access is admitted as parser-owned sugar and published
  truthfully through the frontend semantic surface
- `??` and `@keypath(...)` are admitted as parser-owned source forms and
  published through the frontend semantic surface
- typed key-path literals now remain source/sema surfaces while the validated
  single-component subset also lowers natively as retained descriptor handles

Recommended frontend contract checks:

- `python scripts/check_m265_a001_optionals_nullability_pragmatic_generics_and_key_path_source_closure_contract_and_architecture_freeze.py`
- `python scripts/check_m265_a002_frontend_support_for_optional_sends_binds_coalescing_and_typed_key_paths_core_feature_implementation.py`

## M265 Part 3 lowering boundary

Current implementation status (`M265-C001`):

- optional binding, optional send, and nil-coalescing sites now publish a
  lowering-owned packet in addition to the earlier source/sema packets
- optional bindings and `guard` bindings lower natively with one
  single-evaluation control-flow model
- optional sends now lower natively with single-evaluation nil short-circuit
  behavior; selector arguments are not evaluated on the nil arm
- `?.` optional-member access now lowers natively by desugaring into the same
  optional-send/nil-short-circuit path
- nil-coalescing `??` now lowers natively under the same single-evaluation
  short-circuit model
- ordinary sends still fail closed for nullable receivers unless the receiver
  has been proven nonnull or optional-send syntax is used
- typed key-path literals now lower natively for the validated
  single-component subset by emitting retained descriptor artifacts and stable
  runtime handles
- generic-metadata replay evidence now survives alongside those emitted
  key-path artifacts so erased type-surface proof remains visible in manifests
  and object artifacts
- generic Objective-C method declarations written as `- <T> ...` remain
  reserved for a future revision and continue to diagnose explicitly

Current fail-closed boundary:

- typed key-path application/runtime evaluation still remains deferred to later
  `M265` lane-D work
- multi-component typed key-path member chains still fail closed until later
  executable key-path lowering work
- broader Part 3 lowering/runtime work still remains in later `M265` / `M266`
  milestones

Published packet:

- `frontend.pipeline.semantic_surface.objc_part3_optional_keypath_lowering_contract`
- `frontend.pipeline.semantic_surface.objc_part3_optional_keypath_runtime_helper_contract`

Current runtime-helper boundary (`M265-D002`):

- optional sends and optional-member access continue to execute through the
  public runtime selector lookup and dispatch entrypoints
- lowering still owns nil short-circuit semantics; the runtime helper boundary
  does not add a separate optional-send entrypoint
- validated typed key-path sites now hand runtime-facing stable descriptor
  handles plus retained `objc3.runtime.keypath_descriptors` payloads into a
  private runtime registry
- the runtime now exposes private testing helpers for validated single-component
  key-path handles without widening the stable public runtime header
- unsupported typed key-path shapes and non-ObjC optional-member access remain
  compile-time fail-closed diagnostics rather than runtime fallbacks

Current cross-module preservation boundary (`M265-D003`):

- provider `module.runtime-import-surface.json` artifacts preserve both the
  lowering packet and the runtime-helper packet for the runnable optional/
  key-path subset
- imported manifests aggregate those facts into the imported semantic-rules and
  cross-module orchestration summaries
- linked multi-image programs keep imported typed key-path runtime metadata
  visible after import-surface ingestion and startup registration

## M265 type-surface executable conformance gate (M265-E001)

`objc3c-type-surface-executable-conformance-gate/m265-e001-v1`

This lane-E gate freezes the currently runnable Part 3 type-surface slice as a
single integrated claim instead of treating the upstream `A002`/`B003`/`C003`/
`D003` proofs as unrelated evidence.

- Evidence model:
  `a002-b003-c003-d003-summary-chain`
- The gate must keep optional bindings, optional sends, nil coalescing, typed
  key-path artifacts, and cross-module preservation synchronized as one
  executable surface.
- The integrated native fixture must continue to prove the truthful supported
  counts for bindings, sends, coalescing, and typed key-path literals on the
  `llvm-direct` path.
- `M265-E002` is the next issue.

## M265 runnable optionals, generics, and key-path matrix (M265-E002)

`objc3c-runnable-type-surface-closeout/m265-e002-v1`

This closeout matrix proves the currently supported runnable Part 3 slice
without widening it.

- runtime rows cover optional-send short-circuiting, optional binding and
  refinement flow, optional-member access, and validated typed key-path
  execution
- pragmatic generic annotations remain erased at runtime, so this matrix keeps
  them truthful through a preserved metadata/replay row instead of inventing a
  fake runtime behavior claim
- `M266-A001` is the next issue after `M265` closeout

## M266 frontend pattern grammar and guard surface

The current Part 5 frontend boundary is still intentionally narrow, but it now
admits more real syntax than the initial `M266-A001` freeze.

- `guard let` / `guard var` remains admitted through the existing parser-owned
  optional-binding surface
- `guard` now also accepts comma-separated boolean clauses after optional
  bindings, for example `guard let ready = maybeValue, true else { ... }`
- statement-form `match (...) { ... }` is now admitted as a frontend-owned
  control-flow carrier
- the currently supported `match` pattern slice is:
  - wildcard `_`
  - literal integer, `true`, `false`, and `nil`
  - binding patterns `let name` / `var name`
  - result-case patterns such as `.Ok(let value)` and `.Err(let error)`
- source-only `defer { ... }` statements are now admitted as a frontend/sema-owned Part 5 control-flow surface
- native emit paths still fail closed for runnable `defer` lowering with `O3S221` until later lane-C/lane-D work lands
- expression-form `match` arms using `=>`, guarded patterns using `where`, and
  type-test patterns using contextual `is` still fail closed with targeted
  parser diagnostics
- the frontend publishes this widened boundary in
  `frontend.pipeline.semantic_surface.objc_part5_control_flow_source_closure`
  so later `M266` sema/lowering/runtime work can widen one deterministic Part 5
  contract instead of rebuilding grammar truth from docs

Recommended frontend contract check:

- `python scripts/check_m266_a002_frontend_pattern_grammar_and_guard_surface_completion_core_feature_implementation.py`

Recommended lowering contract check:

- `python scripts/check_m266_c001_control_flow_safety_lowering_contract_and_architecture_freeze.py`

## M266 control-flow semantic model

The sema-owned Part 5 packet now records the current live-versus-deferred truth
at `frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model`.

- live today:
  - guard refinement after `guard let` / `guard var`
  - bool-compatible validation for comma-separated `guard` condition clauses
  - fail-closed `guard ... else` scope-exit enforcement
  - statement-form `match` case-local binding scopes
  - result-case pattern case-local binding scopes
  - live bool/result-case exhaustiveness plus catch-all exhaustiveness
  - `break` / `continue` legality restrictions
- still deferred today:
  - runnable guard short-circuit lowering
  - runnable statement-form `match` dispatch lowering
  - runnable `defer` cleanup lowering/runtime execution
  - result payload typing beyond the current binding-scope surface

M266-C001 lowering note:

- the frontend now publishes `frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract`
- this packet truthfully freezes the current lowering boundary:
  - `guard` is admitted in frontend/sema and remains fail-closed in native IR lowering
  - statement-form `match` is admitted in frontend/sema and remains fail-closed in native IR lowering
  - source-only `defer { ... }` is admitted in frontend/sema and remains fail-closed in native IR lowering
- current native fail-closed probes terminate with deterministic `O3L300` lowering diagnostics instead of silently widening the runnable surface

Recommended semantic contract check:

- `python scripts/check_m266_b001_control_flow_and_pattern_semantic_model_contract_and_architecture_freeze.py`

## M266 cleanup and unwind integration contract

`M266-D001` freezes the current runtime/toolchain boundary for the runnable
Part 5 cleanup slice without pretending that a broader public unwind runtime
API already exists.

- native `defer` cleanup execution currently rides through the existing
  `M266-C002` lowering path into ordinary native objects and executables
- executable probes consume the emitted linker-response sidecar plus the
  runtime archive path recorded in the registration/link artifacts
- the runtime side remains intentionally private:
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
  - `objc3_runtime_copy_memory_management_state_for_testing`
- this issue does not claim a new public cleanup-stack ABI, exception runtime,
  or generalized unwind personality surface
- `M266-D002` widens runnable cleanup/unwind execution beyond this frozen
  boundary:
  - ordinary lexical exit executes deferred cleanups through native executable
    proof
  - guard-mediated early return executes deferred cleanups through native
    executable proof
  - nested-scope return unwind executes inner-to-outer cleanup ordering
    through native executable proof
  - the runtime side still remains intentionally private and continues to use
    the same autoreleasepool helper cluster as the cleanup carrier
- `M266-E001` is the next issue after this runtime implementation lands

Recommended runtime contract check:

- `python scripts/check_m266_d001_cleanup_and_unwind_integration_contract_and_architecture_freeze.py`

## M266 control-flow execution gate (E001)

`objc3c-part5-control-flow-execution-gate/m266-e001-v1`

Lane E now freezes one integrated executable claim for the currently runnable
Part 5 control-flow slice. The gate consumes the existing `A002`, `B003`,
`C003`, and `D002` evidence chain plus one integrated native happy-path probe
instead of widening the language with a synthetic closeout matrix.

- supported runnable slice:
  - boolean-clause `guard` short-circuit with exiting `else`
  - statement-form `match` over the currently supported exhaustive forms
  - lexical `defer` registration with LIFO cleanup execution on ordinary exit
    and return unwind
- canonical operator proof remains the emitted manifest/IR/object triplet plus
  the private runtime cleanup carrier already proven by `M266-D002`
- explicit non-goals remain:
  - expression-form `match`
  - guarded patterns using `where`
  - type-test patterns
  - a public cleanup/unwind runtime ABI
  - result-payload runtime ABI beyond the current fail-closed lowering
    boundary
- `M266-E002` is the next issue

## M266 runnable control-flow matrix and docs (E002)

`objc3c-part5-runnable-control-flow-matrix/m266-e002-v1`

The milestone closeout matrix now publishes the exact runnable Part 5 evidence
slice without widening it. Closeout rows consume the same native artifact and
runtime evidence already proven by `M266-D002` and `M266-E001`.

- published runnable rows cover:
  - ordinary lexical `defer` cleanup execution
  - guard-mediated early return cleanup execution
  - nested-scope return unwind ordering
  - one integrated native `guard` + supported statement-form `match` + `defer`
    program
- this remains a closeout matrix and docs sync only
- unsupported forms remain unsupported:
  - expression-form `match`
  - guarded patterns using `where`
  - type-test patterns
  - public cleanup/unwind ABI widening
- `M267-A001` is the next issue

## M151 frontend symbol graph and scope-resolution parser surface

Frontend parser/AST now emits deterministic scope-owner and scope-path symbol metadata for Objective-C container/member
declarations and top-level functions.

M151 parser/AST surface details:

- scope helper anchors:
  - `BuildScopePathLexicographic(...)`
  - `BuildObjcContainerScopeOwner(...)`
  - `BuildObjcMethodScopePathSymbol(...)`
  - `BuildObjcPropertyScopePathSymbol(...)`
- function scope markers:
  - `fn->scope_owner_symbol = "global";`
  - `fn->scope_path_lexicographic = BuildScopePathLexicographic(...)`
- Objective-C container scope markers:
  - `decl->scope_owner_symbol = BuildObjcContainerScopeOwner(...)`
  - `decl->scope_path_lexicographic = BuildScopePathLexicographic(...)`
- Objective-C member scope markers:
  - `method.scope_owner_symbol = decl->scope_owner_symbol;`
  - `method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);`
  - `property.scope_owner_symbol = decl->scope_owner_symbol;`
  - `property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);`

Deterministic grammar intent:

- symbol/scope metadata is attached at parse time and remains replay-stable for sema/lowering handoff.
- scope path packets are lexicographically normalized via `BuildScopePathLexicographic(...)`.

Recommended M151 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m151_frontend_symbol_graph_scope_resolution_contract.py -q`

## M152 frontend class-protocol-category semantic-linking parser surface

Frontend parser/AST now emits deterministic semantic-link packets connecting interfaces, implementations, categories,
and protocol composition lists.

M152 parser/AST surface details:

- semantic-link helper anchors:
  - `BuildProtocolSemanticLinkTargetsLexicographic(...)`
  - `BuildObjcCategorySemanticLinkSymbol(...)`
- protocol linking markers:
  - `decl->semantic_link_symbol = "protocol:" + decl->name;`
  - `decl->inherited_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(...)`
- interface linking markers:
  - `decl->adopted_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(...)`
  - `decl->semantic_link_symbol = BuildObjcContainerScopeOwner("interface", ...)`
  - `decl->semantic_link_super_symbol = "interface:" + decl->super_name;`
  - `decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(...)`
- implementation linking markers:
  - `decl->semantic_link_symbol = BuildObjcContainerScopeOwner("implementation", ...)`
  - `decl->semantic_link_interface_symbol = BuildObjcContainerScopeOwner("interface", ...)`
  - `decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(...)`

Deterministic grammar intent:

- protocol link targets are normalized as sorted unique `protocol:<name>` packets.
- class/category links remain explicit AST fields for sema handoff and deterministic replay.

Recommended M152 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m152_frontend_class_protocol_category_linking_contract.py -q`

## M153 frontend method lookup-override-conflict parser surface

Frontend parser/AST now emits deterministic method lookup, override lookup, and conflict packets for Objective-C
protocol/interface/implementation declarations.

M153 parser/AST surface details:

- method lookup helper anchors:
  - `BuildObjcMethodLookupSymbol(...)`
  - `BuildObjcMethodOverrideLookupSymbol(...)`
  - `BuildObjcMethodConflictLookupSymbol(...)`
  - `BuildObjcMethodLookupSymbolsLexicographic(...)`
  - `BuildObjcMethodOverrideLookupSymbolsLexicographic(...)`
  - `BuildObjcMethodConflictLookupSymbolsLexicographic(...)`
- parser assignment anchors:
  - `AssignObjcMethodLookupOverrideConflictSymbols(...)`
  - `FinalizeObjcMethodLookupOverrideConflictPackets(...)`
  - `method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);`
  - `method.override_lookup_symbol = override_owner_symbol + "::" + BuildObjcMethodOverrideLookupSymbol(method);`
  - `method.conflict_lookup_symbol = BuildObjcMethodConflictLookupSymbol(method);`
- container packet anchors:
  - `decl->semantic_link_symbol = "protocol:" + decl->name;`
  - `decl->method_lookup_symbols_lexicographic`
  - `decl->override_lookup_symbols_lexicographic`
  - `decl->conflict_lookup_symbols_lexicographic`

Deterministic grammar intent:

- lookup/override/conflict packets are attached per method and replay-stable for sema handoff.
- container-level packet vectors are normalized as sorted unique symbols.

Recommended M153 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m153_frontend_method_lookup_override_conflict_contract.py -q`

## M154 frontend property-synthesis-ivar-binding parser surface

Frontend parser/AST now emits deterministic property synthesis and ivar binding packets for Objective-C
`@implementation` property declarations.

M154 parser/AST surface details:

- property synthesis/ivar binding helper anchors:
  - `BuildObjcPropertySynthesisSymbol(...)`
  - `BuildObjcIvarBindingSymbol(...)`
  - `BuildObjcPropertySynthesisSymbolsLexicographic(...)`
  - `BuildObjcIvarBindingSymbolsLexicographic(...)`
- parser assignment anchors:
  - `AssignObjcPropertySynthesisIvarBindingSymbols(...)`
  - `FinalizeObjcPropertySynthesisIvarBindingPackets(...)`
  - `property.property_synthesis_symbol = synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);`
  - `property.ivar_binding_symbol = synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);`
- implementation packet anchors:
  - `decl->semantic_link_symbol = BuildObjcContainerScopeOwner("implementation", ...)`
  - `decl->property_synthesis_symbols_lexicographic`
  - `decl->ivar_binding_symbols_lexicographic`

Deterministic grammar intent:

- property synthesis and default ivar binding packets are attached per `@implementation` property declaration.
- implementation packet vectors are normalized as sorted unique symbols for sema handoff/replay.

Recommended M154 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m154_frontend_property_synthesis_ivar_binding_contract.py -q`

## M155 frontend id/Class/SEL/object-pointer typecheck parser surface

Frontend parser/AST now emits deterministic typecheck-family symbols for `id`, `Class`, `SEL`, and named object-pointer
spellings across function returns, method returns, parameters, and properties.

M155 parser/AST surface details:

- typecheck family helper anchors:
  - `BuildObjcTypecheckParamFamilySymbol(...)`
  - `BuildObjcTypecheckReturnFamilySymbol(...)`
- parser assignment anchors:
  - `fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);`
  - `param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);`
  - `target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;`
  - `target.typecheck_family_symbol = source.typecheck_family_symbol;`
- AST typecheck-family carriers:
  - `typecheck_family_symbol`
  - `return_typecheck_family_symbol`
  - `sel_spelling`
  - `return_sel_spelling`

Deterministic grammar intent:

- typecheck-family packets normalize parser spellings into stable symbols (`id`, `Class`, `SEL`, `object-pointer:<name>`).
- property/method type handoff preserves these packets through `CopyPropertyTypeFromParam(...)` and
  `CopyMethodReturnTypeFromFunctionDecl(...)`.

Recommended M155 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m155_frontend_id_class_sel_object_pointer_typecheck_contract.py -q`

## M156 frontend message-send selector-lowering parser surface

Frontend parser/AST now emits deterministic message-send form packets and selector-lowering metadata for bracketed
message-send expressions.

M156 parser/AST surface details:

- message-send helper anchors:
  - `BuildMessageSendFormSymbol(...)`
  - `BuildMessageSendSelectorLoweringSymbol(...)`
- parser assignment anchors:
  - `message->message_send_form = Expr::MessageSendForm::Keyword;`
  - `message->message_send_form = Expr::MessageSendForm::Unary;`
  - `message->message_send_form_symbol = BuildMessageSendFormSymbol(message->message_send_form);`
  - `message->selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message->selector_lowering_pieces);`
  - `message->selector_lowering_is_normalized = true;`
- AST message-send carriers:
  - `MessageSendForm`
  - `message_send_form`
  - `message_send_form_symbol`
  - `selector_lowering_pieces`
  - `selector_lowering_symbol`
  - `selector_lowering_is_normalized`

Deterministic grammar intent:

- message-send parse shape captures unary vs keyword form as a stable parser symbol.
- selector lowering symbol packets are derived from parsed selector pieces instead of raw token replay.

Recommended M156 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m156_frontend_message_send_selector_lowering_contract.py -q`

## M157 frontend dispatch ABI marshalling parser/AST surface

Frontend parser/AST now emits deterministic dispatch ABI marshalling packets for bracketed message-send expressions.

M157 parser/AST surface details:

- dispatch-ABI helper anchors:
  - `ComputeDispatchAbiArgumentPaddingSlots(...)`
  - `BuildDispatchAbiMarshallingSymbol(...)`
- parser assignment anchors:
  - `message->dispatch_abi_receiver_slots_marshaled = 1u;`
  - `message->dispatch_abi_selector_slots_marshaled = 1u;`
  - `message->dispatch_abi_argument_value_slots_marshaled = static_cast<unsigned>(message->args.size());`
  - `message->dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;`
  - `message->dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(...)`
  - `message->dispatch_abi_total_slots_marshaled = ...`
  - `message->dispatch_abi_marshalling_symbol = BuildDispatchAbiMarshallingSymbol(...)`
  - `message->dispatch_abi_marshalling_is_normalized = true;`
- AST dispatch-ABI carriers:
  - `dispatch_abi_receiver_slots_marshaled`
  - `dispatch_abi_selector_slots_marshaled`
  - `dispatch_abi_argument_value_slots_marshaled`
  - `dispatch_abi_argument_padding_slots_marshaled`
  - `dispatch_abi_argument_total_slots_marshaled`
  - `dispatch_abi_total_slots_marshaled`
  - `dispatch_abi_runtime_arg_slots`
  - `dispatch_abi_marshalling_symbol`
  - `dispatch_abi_marshalling_is_normalized`

Deterministic grammar intent:

- marshalling slot packets are derived directly from parsed receiver/selector/argument shape.
- argument padding and total slots are normalized against a stable frontend runtime slot budget.

Recommended M157 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m157_frontend_dispatch_abi_marshalling_contract.py -q`

## M158 frontend nil-receiver semantics/foldability parser/AST surface

Frontend parser/AST now emits deterministic nil-receiver semantics/foldability packets for
message-send expressions with compile-time nil receivers.

M158 parser/AST surface details:

- nil-receiver helper anchor:
  - `BuildNilReceiverFoldingSymbol(...)`
- parser assignment anchors:
  - `message->nil_receiver_semantics_enabled = message->receiver->kind == Expr::Kind::NilLiteral;`
  - `message->nil_receiver_foldable = message->nil_receiver_semantics_enabled;`
  - `message->nil_receiver_requires_runtime_dispatch = !message->nil_receiver_foldable;`
  - `message->nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(...)`
  - `message->nil_receiver_semantics_is_normalized = true;`
- AST nil-receiver carriers:
  - `nil_receiver_semantics_enabled`
  - `nil_receiver_foldable`
  - `nil_receiver_requires_runtime_dispatch`
  - `nil_receiver_folding_symbol`
  - `nil_receiver_semantics_is_normalized`

Deterministic grammar intent:

- nil receiver detection is derived from parsed receiver expression kind (`NilLiteral`).
- foldability and runtime-dispatch requirements are normalized as stable parser-owned packet fields.

Recommended M158 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m158_frontend_nil_receiver_semantics_foldability_contract.py -q`

## M159 frontend super-dispatch and method-family parser/AST surface

Frontend parser/AST now emits deterministic super-dispatch and method-family packets for
message-send expressions.

M159 parser/AST surface details:

- super-dispatch helper anchors:
  - `IsSuperDispatchReceiver(...)`
  - `BuildSuperDispatchSymbol(...)`
- method-family helper anchors:
  - `ClassifyMethodFamilyFromSelector(...)`
  - `BuildMethodFamilySemanticsSymbol(...)`
- parser assignment anchors:
  - `message->super_dispatch_enabled = IsSuperDispatchReceiver(*message->receiver);`
  - `message->super_dispatch_requires_class_context = message->super_dispatch_enabled;`
  - `message->super_dispatch_symbol = BuildSuperDispatchSymbol(...)`
  - `message->super_dispatch_semantics_is_normalized = true;`
  - `message->method_family_name = ClassifyMethodFamilyFromSelector(message->selector);`
  - `message->method_family_returns_retained_result = ...`
  - `message->method_family_returns_related_result = message->method_family_name == "init";`
  - `message->method_family_semantics_symbol = BuildMethodFamilySemanticsSymbol(...)`
  - `message->method_family_semantics_is_normalized = true;`
- AST super-dispatch carriers:
  - `super_dispatch_enabled`
  - `super_dispatch_requires_class_context`
  - `super_dispatch_symbol`
  - `super_dispatch_semantics_is_normalized`
- AST method-family carriers:
  - `method_family_name`
  - `method_family_returns_retained_result`
  - `method_family_returns_related_result`
  - `method_family_semantics_symbol`
  - `method_family_semantics_is_normalized`

Deterministic grammar intent:

- super dispatch detection is derived from receiver spelling (`Identifier == "super"`).
- method-family classification is selector-prefix normalized (`init`, `copy`, `mutableCopy`, `new`, `none`).

Recommended M159 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m159_frontend_super_dispatch_method_family_contract.py -q`

## M160 frontend runtime-shim host-link parser/AST surface

Frontend parser/AST now emits deterministic runtime-shim host-link packets for
message-send expressions.

M160 parser/AST surface details:

- runtime-shim helper anchors:
  - `kRuntimeShimHostLinkDispatchSymbol`
  - `BuildRuntimeShimHostLinkSymbol(...)`
- parser assignment anchors:
  - `message->runtime_shim_host_link_required = message->nil_receiver_requires_runtime_dispatch;`
  - `message->runtime_shim_host_link_elided = !message->runtime_shim_host_link_required;`
  - `message->runtime_shim_host_link_declaration_parameter_count = message->dispatch_abi_runtime_arg_slots + 2u;`
  - `message->runtime_dispatch_bridge_symbol = kRuntimeShimHostLinkDispatchSymbol;`
  - `message->runtime_shim_host_link_symbol = BuildRuntimeShimHostLinkSymbol(...)`
  - `message->runtime_shim_host_link_is_normalized = true;`
- AST runtime-shim carriers:
  - `runtime_shim_host_link_required`
  - `runtime_shim_host_link_elided`
  - `runtime_shim_host_link_declaration_parameter_count`
  - `runtime_dispatch_bridge_symbol`
  - `runtime_shim_host_link_symbol`
  - `runtime_shim_host_link_is_normalized`

Deterministic grammar intent:

- runtime-shim requirement/elision derives from parser-owned nil-receiver foldability classification.
- runtime dispatch bridge symbol is pinned to parser-owned canonical spelling (`objc3_msgsend_i32`).
- declaration parameter count is normalized as `runtime_dispatch_arg_slots + 2`.

Recommended M160 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m160_frontend_runtime_shim_host_link_contract.py -q`

## M161 frontend ownership qualifier parser/AST surface

Frontend parser/AST now captures Objective-C ownership qualifier spellings and
token metadata on declaration type annotations.

M161 parser/AST surface details:

- ownership qualifier helper anchors:
  - `IsOwnershipQualifierSpelling(...)`
  - `BuildOwnershipQualifierSymbol(...)`
- parser capture anchors:
  - `MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier)`
  - prefix and suffix qualifier capture in `ParseParameterType(...)`
  - prefix and suffix qualifier capture in `ParseFunctionReturnType(...)`
- AST ownership qualifier carriers:
  - `param.has_ownership_qualifier`
  - `param.ownership_qualifier_spelling`
  - `param.ownership_qualifier_symbol`
  - `param.ownership_qualifier_tokens`
  - `fn.has_return_ownership_qualifier`
  - `fn.return_ownership_qualifier_spelling`
  - `fn.return_ownership_qualifier_symbol`
  - `fn.return_ownership_qualifier_tokens`

Deterministic grammar intent:

- accepted ownership qualifier spellings are pinned to parser-owned canonical tokens:
  `__strong`, `__weak`, `__autoreleasing`, and `__unsafe_unretained`.
- symbol packetization is deterministic and role-aware:
  `ownership-qualifier:<spelling>` for parameter/property spellings and
  `return-ownership-qualifier:<spelling>` for return type spellings.
- qualifier metadata is replay-stable through `Objc3SemaTokenKind::OwnershipQualifier`.

Recommended M161 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m161_frontend_ownership_qualifier_parser_contract.py -q`

## M162 frontend retain/release ownership-operation profile surface

Frontend parser/AST now derives deterministic ownership-operation profile hints
from ownership qualifiers so lowering can insert retain/release operations
without reclassifying qualifier spellings.

M162 parser/AST surface details:

- ownership-operation helper anchors:
  - `BuildParamOwnershipOperationProfile(...)`
  - `BuildReturnOwnershipOperationProfile(...)`
- parser assignment anchors:
  - `param.ownership_insert_retain`
  - `param.ownership_insert_release`
  - `param.ownership_insert_autorelease`
  - `param.ownership_operation_profile`
  - `fn.return_ownership_insert_retain`
  - `fn.return_ownership_insert_release`
  - `fn.return_ownership_insert_autorelease`
  - `fn.return_ownership_operation_profile`
- AST transfer anchors:
  - `CopyMethodReturnTypeFromFunctionDecl(...)`
  - `CopyPropertyTypeFromParam(...)`

Deterministic grammar intent:

- qualifier-to-operation-profile mapping is parser-owned and fail-closed:
  - `__strong` -> retain/release profile
  - `__weak` -> weak-side-table profile
  - `__autoreleasing` -> autorelease-bridge profile
  - `__unsafe_unretained` -> unsafe-unretained profile
- parsed ownership-operation hints are copied into method/property surfaces so
  lane-B/lane-C consumers use a single deterministic source of truth.

Recommended M162 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m162_frontend_retain_release_parser_contract.py -q`

## M163 frontend autorelease-pool scope grammar surface

Frontend parser/AST now accepts Objective-C autorelease-pool scope statements
using `@autoreleasepool { ... }` and emits deterministic scope metadata for
downstream lifetime analysis.

M163 parser/AST surface details:

- lexer/token anchors:
  - `Objc3LexTokenKind::KwAtAutoreleasePool`
  - lexer directive match for `@autoreleasepool`
- parser statement anchors:
  - `Match(TokenKind::KwAtAutoreleasePool)`
  - `BuildAutoreleasePoolScopeSymbol(...)`
  - `stmt->block_stmt->is_autoreleasepool_scope`
  - `stmt->block_stmt->autoreleasepool_scope_symbol`
  - `stmt->block_stmt->autoreleasepool_scope_depth`
- statement recovery anchor:
  - `At(TokenKind::KwAtAutoreleasePool)` in `SynchronizeStatement()`

Deterministic grammar intent:

- each parsed autorelease-pool block receives parser-owned deterministic scope
  metadata (`serial`, `depth`) without introducing a new statement kind.
- scope metadata is carried on `BlockStmt` so sema/lowering can adopt the
  feature incrementally while existing block traversal remains valid.

Recommended M163 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m163_frontend_autorelease_pool_parser_contract.py -q`

## M164 frontend weak/unowned parser/AST surface

Frontend parser/AST now derives deterministic weak/unowned lifetime metadata
from ownership qualifiers and property attributes so downstream lanes can
enforce safety diagnostics without reclassifying ownership spellings.

M164 parser/AST surface details:

- lifetime-profile helper anchors:
  - `BuildWeakUnownedLifetimeProfile(...)`
  - `BuildPropertyWeakUnownedLifetimeProfile(...)`
- parser assignment anchors:
  - `param.ownership_is_weak_reference`
  - `param.ownership_is_unowned_reference`
  - `param.ownership_lifetime_profile`
  - `fn.return_ownership_is_weak_reference`
  - `fn.return_ownership_is_unowned_reference`
  - `fn.return_ownership_runtime_hook_profile`
  - `property.ownership_is_weak_reference`
  - `property.ownership_is_unowned_reference`
  - `property.ownership_lifetime_profile`
- attribute/consistency anchors:
  - `property.is_unowned`
  - `property.has_weak_unowned_conflict`

Deterministic grammar intent:

- weak/unowned classification remains parser-owned:
  - `__weak` -> weak reference + side-table runtime profile.
  - `__unsafe_unretained` -> unowned reference + unsafe runtime profile.
  - `@property (..., unowned)` -> unowned reference + safe runtime profile.
- property-level consistency is replay-stable through
  `property.has_weak_unowned_conflict` so lane-B diagnostics can stay
  deterministic.

Recommended M164 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m164_frontend_weak_unowned_parser_contract.py -q`

## M165 frontend ARC diagnostics/fix-it parser/AST surface

Frontend parser/AST now derives deterministic ARC diagnostic/fix-it hint
profiles from ownership qualifiers and weak/unowned consistency signals so
downstream semantic diagnostics can stay policy-driven and replay-stable.

M165 parser/AST surface details:

- ARC diagnostic helper anchor:
  - `BuildArcDiagnosticFixitProfile(...)`
- parser assignment anchors:
  - `param.ownership_arc_diagnostic_candidate`
  - `param.ownership_arc_fixit_hint`
  - `fn.return_ownership_arc_diagnostic_profile`
  - `fn.return_ownership_arc_fixit_hint`
  - `property.ownership_arc_diagnostic_profile`
  - `property.ownership_arc_fixit_available`
- consistency anchor:
  - `property.has_weak_unowned_conflict`

Deterministic grammar intent:

- parser emits deterministic diagnostic/fix-it profiles without firing semantic
  diagnostics directly; lane-B consumes these carrier fields.
- common ARC misuse patterns map to stable parser-owned profile tags:
  - `__unsafe_unretained` -> unsafe-unretained diagnostic candidate + fix-it.
  - `__autoreleasing` -> autoreleasing-transfer/misuse diagnostic candidate + fix-it.
  - weak+unowned property attribute conflict -> deterministic conflict diagnostic + fix-it.

Recommended M165 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m165_frontend_arc_diagnostics_fixit_parser_contract.py -q`

## M166 frontend block literal syntax and capture-set parser/AST surface

Frontend parser/AST now accepts Objective-C block literal expression syntax and
derives deterministic lexical capture sets from block-local declarations and
identifier uses.

M166 parser/AST surface details:

- block-literal parser anchors:
  - `ParseBlockLiteralExpression(...)`
  - `if (Match(TokenKind::Caret)) {`
- capture-set derivation anchors:
  - `CollectBlockLiteralExprIdentifiers(...)`
  - `CollectBlockLiteralStmtIdentifiers(...)`
  - `BuildBlockLiteralCaptureSet(...)`
  - `BuildBlockLiteralCaptureProfile(...)`
- AST carrier anchors:
  - `Expr::Kind::BlockLiteral`
  - `block_parameter_names_lexicographic`
  - `block_capture_names_lexicographic`
  - `block_capture_profile`
  - `block_literal_is_normalized`

Deterministic grammar intent:

- block literal parsing remains parser-owned:
  - `^{ ... }` parses as a block literal expression.
  - `^(type name, ...) { ... }` parses block parameter names and body shape.
- capture-set emission is replay-stable:
  - capture candidates derive from identifier uses minus block parameters and
    block-local declarations.
  - capture names are published in lexicographic stable order.

Recommended M166 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m166_frontend_block_literal_capture_parser_contract.py -q`

## M167 frontend block ABI invoke-trampoline parser/AST surface (M167-A001)

Frontend parser/AST now emits deterministic block ABI carrier metadata used by
downstream sema/lowering passes for invoke trampoline planning.

M167 parser/AST surface details:

- ABI helper anchors:
  - `BuildBlockLiteralAbiLayoutProfile(...)`
  - `BuildBlockLiteralAbiDescriptorSymbol(...)`
  - `BuildBlockLiteralInvokeTrampolineSymbol(...)`
- parser assignment anchors:
  - `block_abi_invoke_argument_slots`
  - `block_abi_capture_word_count`
  - `block_abi_layout_profile`
  - `block_abi_descriptor_symbol`
  - `block_invoke_trampoline_symbol`
  - `block_abi_has_invoke_trampoline`
  - `block_abi_layout_is_normalized`

Deterministic grammar intent:

- parser derives replay-stable block ABI metadata from block literal shape:
  - invoke argument slots mirror parsed block parameter count.
  - capture word count mirrors deterministic capture cardinality.
  - descriptor/invoke symbols are stable from source coordinates and shape.

Recommended M167 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m167_frontend_block_abi_invoke_trampoline_parser_contract.py -q`

## M168 frontend \_\_block storage and escape parser/AST surface (M168-A001)

Frontend parser/AST now emits deterministic mutable-capture storage and escape
profile carrier metadata for block literals.

M168 parser/AST surface details:

- storage/escape helper anchors:
  - `BuildBlockStorageEscapeProfile(...)`
  - `BuildBlockStorageByrefLayoutSymbol(...)`
- parser assignment anchors:
  - `block_storage_mutable_capture_count`
  - `block_storage_byref_slot_count`
  - `block_storage_requires_byref_cells`
  - `block_storage_escape_analysis_enabled`
  - `block_storage_escape_to_heap`
  - `block_storage_escape_profile`
  - `block_storage_byref_layout_symbol`
  - `block_storage_escape_profile_is_normalized`

Deterministic grammar intent:

- parser derives replay-stable storage/escape metadata from block literal shape:
  - mutable-capture and byref-slot counts mirror deterministic capture count.
  - escape profile and byref-layout symbol remain stable from source coordinates
    and block capture shape.
  - normalized storage/escape flags remain tied to block-literal normalization
    and deterministic capture-set derivation.

Recommended M168 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m168_frontend_block_storage_escape_parser_contract.py -q`

## M169 frontend block copy/dispose helper parser/AST surface (M169-A001)

Frontend parser/AST now emits deterministic block copy/dispose helper carrier
metadata derived from block capture/storage shape.

M169 parser/AST surface details:

- copy/dispose helper anchors:
  - `BuildBlockCopyDisposeProfile(...)`
  - `BuildBlockCopyHelperSymbol(...)`
  - `BuildBlockDisposeHelperSymbol(...)`
- parser assignment anchors:
  - `block_copy_helper_required`
  - `block_dispose_helper_required`
  - `block_copy_dispose_profile`
  - `block_copy_helper_symbol`
  - `block_dispose_helper_symbol`
  - `block_copy_dispose_profile_is_normalized`

Deterministic grammar intent:

- parser derives replay-stable helper metadata from block literal shape:
  - copy/dispose helper enablement follows deterministic capture/byref counts.
  - helper symbol names remain stable from source coordinates and capture shape.
  - copy/dispose profile normalization remains tied to normalized storage/escape
    metadata and stable helper enablement parity.

Recommended M169 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m169_frontend_block_copy_dispose_helper_parser_contract.py -q`

## M170 frontend block determinism/perf baseline parser/AST surface (M170-A001)

Frontend parser/AST now emits deterministic block determinism/perf baseline
metadata so validation and release lanes can replay a stable baseline profile.

M170 parser/AST surface details:

- determinism/perf baseline anchors:
  - `BuildBlockDeterminismPerfBaselineWeight(...)`
  - `BuildBlockDeterminismPerfBaselineProfile(...)`
- parser assignment anchors:
  - `block_determinism_perf_baseline_weight`
  - `block_determinism_perf_baseline_profile`
  - `block_determinism_perf_baseline_profile_is_normalized`

Deterministic grammar intent:

- parser derives replay-stable block baseline metrics from canonical block shape:
  - baseline weight uses deterministic parameter/capture/body counts plus helper
    enablement.
  - baseline profile remains normalized only when capture and copy/dispose
    metadata are normalized.
  - profile tiering (`light`/`medium`/`heavy`) is deterministic from baseline
    weight thresholds.

Recommended M170 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m170_frontend_block_determinism_perf_baseline_parser_contract.py -q`

## M261 executable block source closure (M261-A001)

`M261-A001` freezes the truthful parser/AST boundary for executable block
literals before any runnable block lowering exists.

- contract id
  `objc3c-executable-block-source-closure/m261-a001-v1`

Current source-closure details:

- parser admission remains block-literal syntax only:
  - `ParseBlockLiteralExpression()` accepts `^(...) { ... }` block literal
    expressions.
  - block literal bodies are consumed through the shared `ParseBlock()` helper
    so the brace-owned block surface stays deterministic with every other
    statement block.
- AST ownership remains source-profile only:
  - block literals carry capture-set, ABI-layout, storage-escape,
    copy/dispose, and determinism baseline profiles on `Expr`.
  - those profiles are replay-stable evidence for later `M261` runtime issues,
    not a claim that blocks are runnable yet.
- truthful fail-closed rule:
  - semantic validation still rejects block literals with `O3S221` because the
    runnable block runtime has not landed yet.
- explicit non-goals in this freeze:
  - no block pointer declarator spellings yet.
  - no explicit `__block` byref storage spellings yet.
  - no block runtime lowering, invocation, heap promotion, or helper emission
    yet.

Recommended M261 lane-A contract check:

- `python scripts/check_m261_a001_executable_block_source_closure_contract_and_architecture_freeze.py`
- `M261-A002` is the next issue.

## M261 block source model completion (M261-A002)

`M261-A002` upgrades the frozen `M261-A001` block-literal surface into a real
source-only frontend capability.

- contract id
  `objc3c-executable-block-source-model-completion/m261-a002-v1`

Current source-model details:

- source-only frontend positive path:
  - `objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
    may now admit block literals through sema.
  - the emitted manifest publishes
    `objc_block_source_model_completion_surface`.
- truthful source model now includes:
  - deterministic parameter-signature entries with explicit-typed and implicit
    parameter counts.
  - deterministic capture-inventory entries with the current truthful storage
    class: by-value readonly only.
  - deterministic invoke-surface entries for the descriptor and invoke
    trampoline symbols.
- native emit path remains fail-closed:
  - `objc3c-native.exe` still rejects runnable block lowering with `O3S221`.
- explicit non-goals in this implementation:
  - no explicit `__block` byref spelling yet.
  - no copy/dispose helper emission yet.
  - no runnable block object/runtime invocation yet.

Recommended M261 lane-A implementation check:

- `python scripts/check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py`
- `M261-B001` is the next issue.

## M261 block source storage annotations (M261-A003)

`M261-A003` expands the truthful source-only block model so later runtime lanes
can distinguish byref candidates, helper intent, and heap-promotion-relevant
escape shapes without pretending runnable block lowering already exists.

- contract id
  `objc3c-executable-block-source-storage-annotation/m261-a003-v1`

Current source-annotation details:

- source-only frontend positive path:
  - `objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
    may now admit block literals and publish
    `objc_block_source_storage_annotation_surface`.
- truthful storage/helper/escape annotations now include:
  - mutated captured bindings classified as byref-capture candidates before any
    explicit `__block` spelling exists.
  - copy/dispose helper intent bits derived from the byref-candidate inventory.
  - deterministic escape-shape classes:
    `expression-site`, `global-initializer`, `binding-initializer`,
    `assignment-value`, `return-value`, `call-argument`, and
    `message-argument`.
  - heap-promotion candidate truth for every non-`expression-site` block
    literal.
- emitted/native boundary details:
  - the native IR boundary now carries
    `; executable_block_source_storage_annotations = ...`.
  - runnable native emit paths still fail closed with `O3S221`.
- explicit non-goals in this implementation:
  - no explicit `__block` storage spelling yet.
  - no copy/dispose helper lowering yet.
  - no runnable block object lowering, helper emission, or heap promotion yet.

Recommended M261 lane-A expansion check:

- `python scripts/check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py`
- `M261-B001` is the next issue.

## M261 block runtime semantic rules (M261-B001)

`M261-B001` freezes the current semantic-rule boundary for blocks before lane-B
begins implementing runnable capture legality, byref behavior, helper
generation, or invocation semantics.

- contract id
  `objc3c-executable-block-runtime-semantic-rules/m261-b001-v1`

Current semantic-rule details:

- source-only frontend admission remains the only supported positive path:
  - `objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
    may admit block literals through sema for manifest projection.
- current semantic truth is intentionally limited to:
  - deterministic capture inventory and source-owned byref/helper/escape-shape
    annotations.
  - block literals classified as function-shaped source values only.
  - native emit paths fail closed before runnable block semantics land.
- emitted/native boundary details:
  - the native IR boundary now carries
    `; executable_block_runtime_semantic_rules = ...`.
  - native emit paths still reject block literals with `O3S221`.
- explicit non-goals in this freeze:
  - no runnable capture legality beyond deterministic source metadata checks.
  - no runnable byref storage lowering or heap-promotion behavior.
  - no copy/dispose helper lowering or helper emission.
  - no runnable block invocation or block-object execution semantics.

Recommended M261 lane-B freeze check:

- `python scripts/check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py`
- `M261-B002` is the next issue.

## M261 capture legality, escape classification, and invocation typing (M261-B002)

`M261-B002` upgrades the source-only block semantic surface into a real
compiler capability while preserving the same native fail-closed runtime
boundary from `M261-B001`.

- contract id
  `objc3c-executable-block-capture-legality-escape-and-invocation/m261-b002-v1`

Current source-only semantic details:

- undefined captures in block literals are now rejected during block-body
  validation with `O3S202`.
- local block bindings now carry callable signatures so block invocations are
  type-checked and mismatches fail with `O3S206`.
- truthful mutable-capture, byref, escape, and copy/dispose helper counts are
  preserved in the lowering handoff instead of expanding every capture into a
  mutable/byref requirement.
- native emit paths still reject block literals with `O3S221`.

Recommended M261 lane-B implementation check:

- `python scripts/check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py`
- `M261-B003` is the next issue.

## M261 byref mutation, copy-dispose eligibility, and object-capture ownership semantics (M261-B003)

`M261-B003` keeps blocks on the same source-only execution boundary while
making helper eligibility and mutation legality ownership-sensitive.

- contract id
  `objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1`

Current source-only semantic details:

- mutating a captured `__weak` or `__unsafe_unretained` object now fails closed
  with `O3S206`.
- owned object captures now promote copy/dispose helper eligibility even when a
  block has no byref slots.
- weak and unowned object captures remain non-owning and do not force helper
  generation by themselves.
- native emit paths still reject block literals with `O3S221`.

Recommended M261 lane-B implementation check:

- `python scripts/check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py`
- `M261-C001` is the next issue.

## M261 block lowering ABI and artifact boundary (M261-C001)

`M261-C001` freezes the truthful lane-C lowering boundary required for
runnable block objects without claiming that runnable block emission already
exists.

- contract id
  `objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1`

Current boundary details:

- source-only manifests already publish the lowering surfaces later runnable
  block emission must preserve:
  - `objc_block_literal_capture_lowering_surface`
  - `objc_block_abi_invoke_trampoline_lowering_surface`
  - `objc_block_storage_escape_lowering_surface`
  - `objc_block_copy_dispose_lowering_surface`
- truthful surface split on the current owned-capture corpus:
  - capture and invoke surfaces are deterministic.
  - storage-escape and copy/dispose surfaces are still source-only helper and
    escape profiles and are not required to be deterministic yet.
- emitted/native boundary details:
  - the native IR boundary now carries
    `; executable_block_lowering_abi_artifact_boundary = ...`.
  - native emit paths still reject block literals with `O3S221`.
- helper symbol policy is still truthful and narrow:
  - invoke thunks, byref cells, and copy/dispose helpers are source-modeled
    lowering surfaces only.
  - lane-C has not yet emitted runnable block object records or helper bodies.
- explicit non-goals in this freeze:
  - no emitted block object records yet.
  - no emitted invoke-thunk bodies yet.
  - no emitted byref cell storage yet.
  - no emitted copy/dispose helper bodies yet.
  - no runnable block execution yet.

Recommended M261 lane-C freeze check:

- `python scripts/check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py`
- `M261-C002` is the next issue.

## M261 executable block object and invoke-thunk lowering (M261-C002)

`M261-C002` turns the frozen `M261-C001` boundary into one runnable native
slice: stack block objects, readonly scalar captures, and direct local block
invocation now lower, link, and execute.

- contract id
  `objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1`

Current runnable slice:

- supported positive path:
  - one local block literal bound to a local name
  - up to four parameters
  - readonly scalar captures
  - one emitted stack block object
  - one emitted internal invoke thunk
  - one direct local invocation path through the stored thunk pointer
- emitted/native proof details:
  - the native IR boundary now also carries
    `; executable_block_object_invoke_thunk_lowering = ...`.
  - native compile/link/run over
    `m261_executable_block_object_invoke_thunk_positive.objc3` now succeeds
    with exit `15`.
  - the object backend remains `llvm-direct`.
- explicitly deferred to `M261-C003`:
  - byref-cell lowering
  - copy/dispose helper bodies
  - owned object capture runtime lowering
  - escaping heap-promoted block values
  - those cases still fail closed with `O3S221`.

Recommended M261 lane-C implementation check:

- `python scripts/check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py`
- `M261-C003` is the next issue.

## M261 byref-cell, copy-helper, and dispose-helper lowering (M261-C003)

`M261-C003` expands the runnable `M261-C002` block-lowering slice to admit
local byref mutation plus ownership-sensitive captures through emitted stack
byref cells and emitted helper bodies.

- contract id
  `objc3c-executable-block-byref-helper-lowering/m261-c003-v1`

Current runnable slice:

- supported positive path:
  - one local block literal bound to a local name
  - up to four parameters
  - readonly scalar captures
  - local byref mutation through emitted byref-cell storage
  - owned object captures through emitted copy/dispose helpers
  - weak and unowned object captures through emitted pointer-capture storage
- emitted/native proof details:
  - the native IR boundary now also carries
    `; executable_block_byref_helper_lowering = ...`.
  - manifest lowering surfaces now publish deterministic byref layout symbols
    for the runnable local byref slice.
  - native compile/link/run now succeeds for:
    - `m261_executable_block_object_invoke_thunk_positive.objc3` with exit `15`
    - `m261_byref_cell_copy_dispose_runtime_positive.objc3` with exit `14`
    - `m261_owned_object_capture_runtime_positive.objc3` with exit `11`
    - `m261_nonowning_object_capture_runtime_positive.objc3` with exit `9`
  - byref/object helper lowering keeps the object backend on `llvm-direct`.
- explicitly deferred to `M261-C004`:
  - escaping block heap-promotion/runtime hook lowering
  - first-class escaping block values
  - runtime-managed block copy/allocation semantics outside the local
    nonescaping slice

Recommended M261 lane-C implementation check:

- `python scripts/check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py`
- `M261-C004` is the next issue.

## M261 heap-promotion and escaping-block runtime hook lowering (M261-C004)

`M261-C004` widens the runnable `M261-C003` block slice to admit readonly
scalar block values that escape through call arguments or return values.

- contract id
  `objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1`

Current runnable slice:

- supported positive path:
  - local block literals with readonly scalar captures
  - escaping through call arguments or return values
  - heap-promotion through the private runtime hook
    `objc3_runtime_promote_block_i32`
  - direct subsequent invocation through the private runtime hook
    `objc3_runtime_invoke_block_i32`
- emitted/native proof details:
  - the native IR boundary now also carries
    `; executable_block_escape_runtime_hook_lowering = ...`.
  - native compile/link/run now succeeds for:
    - `m261_escaping_block_runtime_hook_argument_positive.objc3` with exit `14`
    - `m261_escaping_block_runtime_hook_return_positive.objc3` with exit `0`
  - object emission stays on `llvm-direct`.
- still fail-closed:
  - byref-forwarded escaping blocks
  - escaping owned-object capture blocks
  - runtime-managed block copy/allocation semantics outside the readonly scalar
    slice
  - those cases still fail closed with `O3L300`.

Recommended M261 lane-C implementation check:

- `python scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py`
- `M261-D001` is the next issue.

## M261 block runtime API and object layout (M261-D001)

`M261-D001` freezes the truthful runtime boundary that the current runnable
block lowering path uses.

- contract id
  `objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1`

Current frozen boundary:

- stable/public surface:
  - the public runtime header still does not publish block helper entrypoints
- private/runtime-only surface:
  - `objc3_runtime_promote_block_i32`
  - `objc3_runtime_invoke_block_i32`
  - private runtime block records keep copied storage bytes, the invoke
    function pointer, and the retain count
  - the emitted IR now carries
    `; runtime_block_api_object_layout = ...`
- still explicitly deferred:
  - public block-object ABI
  - generalized runtime-managed block allocation/copy/dispose
  - byref-forwarded escaping blocks
  - owned-object escaping block lifetimes

Recommended M261 lane-D contract check:

- `python scripts/check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py`
- `M261-D002` is the next issue.

## M261 block object allocation, copy-dispose, and invoke support (M261-D002)

`M261-D002` turns the frozen D001 block helper boundary into a live runtime
capability for promoted block records.

- contract id
  `objc3c-runtime-block-allocation-copy-dispose-invoke-support/m261-d002-v1`

Current live runtime behavior:

- promotion copies block storage into aligned runtime-owned buffers
- pointer-capture block promotion preserves copy/dispose helper pointers
- promotion runs the copy helper before the runtime block handle is published
- final runtime release runs the dispose helper before the runtime record is
  erased
- runtime block invocation now supports pointer-capture promoted blocks as well
  as the earlier readonly-scalar slice
- the emitted IR now carries
  `; runtime_block_allocation_copy_dispose_invoke_support = ...`

Still explicitly deferred:

- byref-forwarded escaping blocks
- runtime-reentrant ownership helper interop
- public block-object ABI or public block runtime helper declarations

Recommended M261 lane-D implementation check:

- `python scripts/check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py`
- `M261-D003` is the next issue.

## M261 byref forwarding, heap promotion, and ownership interop for escaping blocks (M261-D003)

`M261-D003` closes the main runtime gap for escaping pointer-capture blocks.

- contract id
  `objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1`

Current live runtime behavior:

- escaping pointer-capture block promotion now rewrites capture slots onto
  runtime-owned heap cells before publishing the block handle
- escaped byref captures mutate those runtime-owned forwarding cells across
  repeated invokes after the source frame returns
- owned-capture copy/dispose helpers now run against runtime-owned capture cells
  instead of borrowed stack-cell addresses
- emitted IR now carries
  `; runtime_block_byref_forwarding_heap_promotion_ownership_interop = ...`

Still explicitly deferred:

- no public block-object ABI
- no public block runtime helper declarations
- no outer stack-cell forwarding bridge back into a still-live caller frame

Recommended M261 lane-D expansion check:

- `python scripts/check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py`
- `M261-E001` is the next issue.

## M261 runnable block-runtime gate (M261-E001)

`M261-E001` freezes the first truthful lane-E gate for runnable blocks above
the retained source, sema, lowering, and runtime proof chain.

- contract id
  `objc3c-runnable-block-runtime-gate/m261-e001-v1`

Current gate claims:

- lane-E consumes the existing `A003/B003/C004/D003` proof chain rather than
  metadata-only summaries
- supported runnable slice includes:
  - nonescaping byref and owned-capture blocks through emitted helper bodies
  - escaping readonly-scalar blocks through private runtime promotion/invoke
    hooks
  - escaping pointer-capture blocks through runtime-owned forwarding cells and
    helper interop after the source frame returns
- emitted IR now carries:
  - `; runnable_block_runtime_gate = ...`
  - `!objc3.objc_runnable_block_runtime_gate`

Still explicitly deferred:

- no public block-object ABI
- no public stable block runtime helper declarations
- no generalized foreign block ABI interop
- no caller-frame forwarding bridge back into still-live outer stack cells

Recommended M261 lane-E gate check:

- `python scripts/check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py`
- `M261-E002` is the next issue.

## M261 runnable block execution matrix and docs (M261-E002)

`M261-E002` closes the current M261 block-runtime tranche with one truthful
execution matrix over the already-landed runnable slice.

- contract id
  `objc3c-runnable-block-execution-matrix/m261-e002-v1`

Current closeout claims:

- lane-E consumes the retained `A003/B003/C004/D003/E001` chain and then proves
  integrated executable behavior through real native fixtures:
  - `m261_owned_object_capture_runtime_positive.objc3` with exit `11`
  - `m261_nonowning_object_capture_runtime_positive.objc3` with exit `9`
  - `m261_byref_cell_copy_dispose_runtime_positive.objc3` with exit `14`
  - `m261_escaping_block_runtime_hook_argument_positive.objc3` with exit `14`
  - `m261_escaping_block_runtime_hook_return_positive.objc3` with exit `0`
- the retained `M261-D003` runtime probe still proves escaping pointer-capture
  forwarding and helper interop:
  - copy helper count after promotion = `1`
  - second invoke result = `25`
  - dispose count after final release = `1`
- emitted IR now carries:
  - `; runnable_block_execution_matrix = ...`
  - `!objc3.objc_runnable_block_execution_matrix`

Still explicitly deferred:

- no public block-object ABI
- no public stable block runtime helper declarations
- no generalized foreign block ABI interop
- no caller-frame forwarding bridge back into still-live outer stack cells

Recommended M261 lane-E closeout check:

- `python scripts/check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py`
- `M262-A001` is the next issue.

## M262 ARC source surface and mode boundary (M262-A001)

`M262-A001` freezes the truthful ARC-adjacent frontend boundary before ARC
automation begins.

- contract id
  `objc3c-arc-source-mode-boundary-freeze/m262-a001-v1`
- current source surface preserved:
  - ownership qualifiers, weak/unowned summaries, `@autoreleasepool` profiling,
    and ARC fix-it metadata remain parser/sema-visible
  - runtime-backed ownership/property work from `M260` remains the current
    executable baseline instead of generalized ARC automation
- current mode boundary:
  - the native driver still rejects `-fobjc-arc`
  - executable function/method ownership qualifiers still fail closed with
    `O3S221`
- emitted IR now carries:
  - `; arc_source_mode_boundary = ...`
  - `!objc3.objc_arc_source_mode_boundary`
- still explicitly deferred:
  - no user-visible `-fobjc-arc` / `-fno-objc-arc` mode split
  - no automatic ARC cleanup or retain/release insertion for general executable
    functions/methods
  - no claim that executable ownership-qualified functions or methods are
    runnable yet

Recommended M262 lane-A freeze check:

- `python scripts/check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py`
- `M262-A002` is the next issue.

## M262 ARC mode handling for methods, properties, returns, and block captures (M262-A002)

`M262-A002` turns the frozen ARC-adjacent source boundary into a real explicit
ARC mode for a narrow runnable slice.

- contract id
  `objc3c-arc-mode-handling/m262-a002-v1`
- explicit mode behavior:
  - the native driver now accepts `-fobjc-arc` and `-fno-objc-arc`
  - frontend manifests and emitted IR now carry the selected ARC mode
- runnable ARC-mode surface:
  - ownership-qualified method and function signatures are admitted under
    `-fobjc-arc`
  - ownership-qualified property surfaces compile under explicit ARC mode
  - block captures over ownership-qualified values compile under explicit ARC
    mode
- non-ARC boundary still remains fail-closed:
  - the same executable ownership-qualified method/function signatures still
    terminate in `O3S221` without ARC mode
- emitted IR now carries:
  - `; arc_mode_handling = ...`
  - `!objc3.objc_arc_mode_handling`
- still explicitly deferred:
  - no generalized ARC cleanup or retain/release synthesis
  - no claim of full ARC lifetime automation
  - no claim that forbidden ARC forms are complete yet

Recommended M262 lane-A implementation check:

- `python scripts/check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py`
- `M262-B001` is the next issue.

## M262 ARC semantic rules and forbidden forms (M262-B001)

`M262-B001` freezes the semantic legality boundary that remains truthful after
explicit ARC mode handling is live.

- contract id
  `objc3c-arc-semantic-rules/m262-b001-v1`
- explicit ARC mode does not yet imply generalized ARC inference:
  - ownership-qualified executable signatures are admitted only when they are
    spelled explicitly
  - property ownership conflicts still fail closed
  - atomic ownership-aware properties still fail closed
- emitted IR now carries:
  - `; arc_semantic_rules = ...`
  - `!objc3.objc_arc_semantic_rules`
- still explicitly deferred:
  - no implicit retain/release inference
  - no lifetime extension semantics
  - no method-family ARC semantics yet

Recommended M262 lane-B freeze check:

- `python scripts/check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py`
- `M262-B002` is the next issue.

## M262 implicit retain-release inference and lifetime-extension semantics (M262-B002)

`M262-B002` promotes the supported ARC slice from explicit-only ownership
spelling to semantic strong-owned inference for unqualified object signatures.

- contract id
  `objc3c-arc-inference-lifetime/m262-b002-v1`
- explicit ARC inference behavior:
  - unqualified object parameters now infer strong-owned retain/release
    behavior under `-fobjc-arc`
  - unqualified object returns now infer strong-owned retain/release behavior
    under `-fobjc-arc`
  - unqualified object property surfaces now infer a strong-owned lifetime
    profile under `-fobjc-arc`
  - the same source remains a zero-inference baseline without ARC mode
- emitted IR now carries:
  - `; arc_inference_lifetime = ...`
  - `!objc3.objc_arc_inference_lifetime`
  - `; frontend_objc_retain_release_operation_lowering_profile = ...`
- current proof scope:
  - ARC-enabled manifests show nonzero ownership-qualified, retain-insertion,
    and release-insertion accounting for the supported fixture
  - non-ARC manifests preserve zero inferred ownership/release activity for the
    same fixture
  - the block-bearing fixture still traverses the existing block escape path
- still explicitly deferred:
  - no full ARC cleanup synthesis
  - no weak/autorelease-return/property-synthesis/block-interaction ARC
    semantics yet

Recommended M262 lane-B implementation check:

- `python scripts/check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py`
- `M262-B003` is the next issue.

## M262 weak, autorelease-return, property-synthesis, and block-interaction ARC semantics (M262-B003)

`M262-B003` closes the current semantic interaction inventory that sits above
the `B002` inference baseline.

- contract id
  `objc3c-arc-interaction-semantics/m262-b003-v1`
- supported ARC interaction behavior:
  - attribute-only strong properties now publish strong-owned synthesized
    accessor ownership packets under ARC mode
  - attribute-only weak properties now publish weak synthesized accessor
    ownership packets under ARC mode
  - explicit `__autoreleasing` returns remain profiled through autorelease
    insertion accounting
  - owned block captures keep nonzero retain/release behavior under ARC mode
  - weak/unowned block captures stay non-owning under ARC mode
- emitted IR now carries:
  - `; arc_interaction_semantics = ...`
  - `!objc3.objc_arc_interaction_semantics`
- still explicitly deferred:
  - no generalized ARC cleanup insertion
  - no full method-family automation
  - no broader cross-module ARC interactions yet

Recommended M262 lane-B expansion check:

- `python scripts/check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py`
- `M262-C001` is the next issue.

## M262 ARC lowering ABI and cleanup model (M262-C001)

Lane C now freezes the current ARC lowering boundary without claiming the later
retain/release insertion and cleanup-scheduling work is already complete.

Current implementation status (`M262-C001`):

- lowering now publishes one canonical contract:
  - `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
- the frozen boundary is defined as:
  - ARC semantic packets from sema plus unwind-cleanup accounting plus private
    runtime helper entrypoints
- the current private helper boundary is explicit:
  - `objc3_runtime_retain_i32`
  - `objc3_runtime_release_i32`
  - `objc3_runtime_autorelease_i32`
  - `objc3_runtime_load_weak_current_property_i32`
  - `objc3_runtime_store_weak_current_property_i32`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
- emitted IR now carries:
  - `; arc_lowering_abi_cleanup_model = ...`
- still explicitly deferred:
  - no general ARC cleanup-scope insertion yet
  - no generalized weak load/store lowering yet
  - no automatic autorelease-return rewrite pipeline yet

Recommended M262 lane-C freeze check:

- `python scripts/check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py`
- `M262-C002` is the next issue.

## M262 ARC automatic retain release autorelease insertion (M262-C002)

Lane C now consumes the frozen ARC lowering boundary plus the semantic
insertion packets and emits real retain, release, and autorelease helper calls
for the supported runnable slice.

Current implementation status (`M262-C002`):

- lowering now publishes one canonical contract:
  - `objc3c-arc-automatic-insertion/m262-c002-v1`
- the supported automatic-insertion boundary is defined as:
  - ARC semantic insertion packets from `M262-A002`, `M262-B002`, and
    `M262-B003`
  - the frozen lowering/helper ABI boundary from `M262-C001`
  - runtime helper insertion only for the supported runnable function and
    method parameter/return paths
- emitted IR now carries:
  - `; arc_automatic_insertions = ...`
  - `!objc3.objc_arc_automatic_insertions`
- supported ARC helper placement now materializes:
  - retain-on-entry for ARC-owned runnable parameters
  - release-on-exit for tracked ARC-owned storage
  - autorelease-return lowering for supported autoreleasing returns
- still explicitly deferred:
  - no generalized local ARC cleanup stack
  - no exception-cleanup widening
  - no cross-module ARC optimization

Recommended M262 lane-C implementation check:

- `python scripts/check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py`
- `M262-C003` is the next issue.

## M262 ARC cleanup emission weak load-store lowering and lifetime extension hooks (M262-C003)

`M262-C003` extends the supported ARC lowering slice from helper insertion into
real cleanup scheduling and weak/runtime hook continuity for the currently
supported executable subset.

Current implementation status (`M262-C003`):

- lowering now publishes one canonical contract:
  - `objc3c-arc-cleanup-weak-lifetime-hooks/m262-c003-v1`
- the supported `C003` boundary is defined as:
  - scope-exit cleanup emission for ARC-owned tracked storage
  - implicit-exit cleanup emission for supported `void` and scalar-return
    functions/methods after the live `M262-C002` insertion path
  - deterministic pending block dispose-helper unwinding on scope exit
  - continued weak current-property lowering through
    `objc3_runtime_load_weak_current_property_i32` and
    `objc3_runtime_store_weak_current_property_i32`
- emitted IR now carries:
  - `; arc_cleanup_weak_lifetime_hooks = ...`
  - `!objc3.objc_arc_cleanup_weak_lifetime_hooks`
- the supported happy path now materially proves:
  - scope-local escaping block storage is disposed before control rejoins the
    merge block
  - tracked ARC-owned storage is released before the final return on supported
    exits
  - implicit `void` exits drain both block-dispose helpers and tracked ARC
    cleanup before `ret void`
  - weak current-property lowering remains runtime-hooked instead of degrading
    into summary-only metadata
- still explicitly deferred:
  - no generalized weak local-storage lowering
  - no exception cleanup stack
  - no cross-module ARC optimization

Recommended M262 lane-C implementation check:

- `python scripts/check_m262_c003_cleanup_emission_weak_load_store_lowering_and_lifetime_extension_hooks_core_feature_implementation.py`
- `M262-C004` is the next issue.

## M262 ARC and block-interaction lowering with autorelease-return conventions (M262-C004)

`M262-C004` closes the supported escaping-block plus autoreleasing-return edge
inventory for the runnable ARC slice.

Current implementation status (`M262-C004`):

- lowering now publishes one canonical contract:
  - `objc3c-arc-block-autorelease-return-lowering/m262-c004-v1`
- the supported `C004` boundary is defined as:
  - escaping block promotion through the retained `M261-C004` runtime-hook path
  - terminal branch cleanup emission that does not consume sibling-branch ARC
    cleanup state during code generation
  - autoreleasing returns that still perform the required cleanup/dispose work
    after block interaction
- emitted IR now carries:
  - `; arc_block_autorelease_return_lowering = ...`
  - `!objc3.objc_arc_block_autorelease_return_lowering`
- the supported happy path now materially proves:
  - escaping block promotion remains live under `-fobjc-arc`
  - branch-local cleanup emission does not erase later cleanup obligations
  - autoreleasing returns still emit `objc3_runtime_autorelease_i32` and the
    corresponding release/dispose cleanup on both branch paths
- still explicitly deferred:
  - no generalized method-family ARC automation
  - no public ARC runtime ABI
  - no cross-module ARC optimization

Recommended M262 lane-C implementation check:

- `python scripts/check_m262_c004_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion.py`
- `M262-D001` is the next issue.

## M262 runtime ARC helper API surface (M262-D001)

`M262-D001` freezes the truthful private runtime helper ABI that the current
ARC lowering slice already consumes.

Current implementation status (`M262-D001`):

- lane D now freezes one canonical runtime/helper contract:
  - `objc3c-runtime-arc-helper-api-surface-freeze/m262-d001-v1`
- the supported `D001` boundary is defined as:
  - the public runtime header still exposes only registration, lookup,
    dispatch, and testing snapshots
  - ARC helper entrypoints remain private to
    `objc3_runtime_bootstrap_internal.h`
  - `objc3_runtime_push_autoreleasepool_scope` remains a private helper
  - private ARC helper entrypoints and autoreleasepool hooks remain internal
    runtime ABI
- the frozen helper surface currently covers:
  - `objc3_runtime_retain_i32`
  - `objc3_runtime_release_i32`
  - `objc3_runtime_autorelease_i32`
  - `objc3_runtime_read_current_property_i32`
  - `objc3_runtime_write_current_property_i32`
  - `objc3_runtime_exchange_current_property_i32`
  - `objc3_runtime_load_weak_current_property_i32`
  - `objc3_runtime_store_weak_current_property_i32`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
- emitted IR now carries:
  - `; runtime_arc_helper_api_surface = ...`
  - `!objc3.objc_runtime_arc_helper_api_surface`
- still explicitly deferred:
  - no public ARC runtime header widening
  - no user-facing ARC helper ABI

Recommended M262 lane-D freeze check:

- `python scripts/check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py`
- `M262-D002` is the next issue.

## M262 runtime ARC helper runtime support (M262-D002)

`M262-D002` proves the private ARC helper ABI frozen by `M262-D001` is a live
compiler/runtime capability for the supported ARC property/weak and
autorelease-return slice.

Current implementation status (`M262-D002`):

- lane D now proves one canonical runtime-support contract:
  - `objc3c-runtime-arc-helper-runtime-support/m262-d002-v1`
- the supported `D002` boundary is defined as:
  - ARC-generated weak current-property access lowers through the private
    runtime helper entrypoints and emits object code successfully
  - ARC-generated autorelease-return paths lower through the private runtime
    helper entrypoints, link against the native runtime library, and execute
    successfully
  - the helper ABI remains private to the bootstrap-internal runtime surface
- emitted IR now carries:
  - `; runtime_arc_helper_runtime_support = ...`
  - `!objc3.objc_runtime_arc_helper_runtime_support`
- the live helper/runtime support currently covers:
  - `objc3_runtime_retain_i32`
  - `objc3_runtime_release_i32`
  - `objc3_runtime_autorelease_i32`
  - `objc3_runtime_load_weak_current_property_i32`
  - `objc3_runtime_store_weak_current_property_i32`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
- still explicitly deferred:
  - no public ARC runtime header widening
  - no debug or ownership instrumentation hooks yet

Recommended M262 lane-D runtime-support check:

- `python scripts/check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py`
- `M262-D003` is the next issue.

## M262 ARC ownership debug instrumentation and runtime validation hooks (M262-D003)

`M262-D003` extends the live helper/runtime boundary with a private ARC debug
snapshot surface for the supported runnable slice.

Current implementation status (`M262-D003`):

- lane D now proves one canonical ARC debug contract:
  - `objc3c-runtime-arc-debug-instrumentation/m262-d003-v1`
- the supported `D003` boundary is defined as:
  - retain/release/autorelease helper traffic publishes deterministic counters
    and last-value state
  - current-property, weak current-property, and autoreleasepool helper traffic
    publishes deterministic counters and last property context
  - the debug surface remains private to the bootstrap-internal runtime header
- emitted IR now carries:
  - `; runtime_arc_debug_instrumentation = ...`
  - `!objc3.objc_runtime_arc_debug_instrumentation`
- the live ARC debug surface currently covers:
  - retain/release/autorelease helper call counts
  - autoreleasepool push/pop helper call counts
  - current-property read/write/exchange helper call counts
  - weak current-property load/store helper call counts
  - last helper values plus last property name/owner context
- still explicitly deferred:
  - no public ARC debug ABI
  - no user-facing ownership tracing hooks
  - no broader ARC runtime completeness claim beyond the supported runnable
    slice

Recommended M262 lane-D ARC-debug check:

- `python scripts/check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py`
- `M262-E001` is the next issue.

## M262 runnable ARC runtime gate (M262-E001)

Contract id: `objc3c-runnable-arc-runtime-gate/m262-e001-v1`

`M262-E001` freezes the current runnable ARC lane-E gate above the
`A002/B003/C004/D003` proof chain. It consumes the existing ARC mode-handling,
interaction, lowering, and runtime proofs and publishes the gate boundary
through `!objc3.objc_runnable_arc_runtime_gate`.

The gate is intentionally narrow:

- it does not widen ARC source or semantic behavior
- it does not add new lowering or runtime helper behavior
- it does not claim conformance-matrix or operator closeout coverage yet

`M262-E002` is the next issue.

## M262 runnable ARC closeout matrix and runbook (M262-E002)

Contract id: `objc3c-runnable-arc-closeout/m262-e002-v1`

`M262-E002` closes the current ARC tranche by consuming the
`A002/B003/C004/D003/E001` proof chain, three ARC-positive execution-smoke
rows, and the operator runbook.

The closeout is intentionally narrow:

- it does not widen ARC source or semantic behavior
- it does not add new lowering or runtime helper behavior
- it treats property/runtime behavior as a private `M262-D003` probe-backed row
- it hands off to `M263-A001`

## M171 frontend lightweight generics constraint parser/AST surface (M171-A001)

Frontend parser/AST now emits deterministic lightweight-generic constraint
profiles for parameter and return type annotations.

M171 parser/AST surface details:

- lightweight-generic anchors:
  - `BuildLightweightGenericConstraintProfile(...)`
  - `IsLightweightGenericConstraintProfileNormalized(...)`
- parser assignment anchors:
  - `lightweight_generic_constraint_profile`
  - `lightweight_generic_constraint_profile_is_normalized`
  - `return_lightweight_generic_constraint_profile`
  - `return_lightweight_generic_constraint_profile_is_normalized`

Deterministic grammar intent:

- parser computes generic-instantiation validity from object-pointer spelling,
  suffix termination, and pointer declarator participation.
- profile normalization is fail-closed for malformed/unterminated generic
  suffixes.

Recommended M171 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m171_frontend_lightweight_generics_parser_contract.py -q`

## M172 frontend nullability-flow parser/AST surface (M172-A001)

Frontend parser/AST now emits deterministic nullability-flow profiles for
parameter and return type annotations.

M172 parser/AST surface details:

- nullability-flow anchors:
  - `BuildNullabilityFlowProfile(...)`
  - `IsNullabilityFlowProfileNormalized(...)`
- parser assignment anchors:
  - `nullability_flow_profile`
  - `nullability_flow_profile_is_normalized`
  - `return_nullability_flow_profile`
  - `return_nullability_flow_profile_is_normalized`

Deterministic grammar intent:

- parser derives flow precision from object-pointer spelling and nullability
  suffix presence.
- nullability-flow profile normalization is fail-closed when nullability
  suffixes appear without object-pointer type spellings.

Recommended M172 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m172_frontend_nullability_flow_parser_contract.py -q`

## M173 frontend protocol-qualified object type parser/AST surface (M173-A001)

Frontend parser/AST now emits deterministic protocol-qualified object type
profiles for parameter and return type annotations.

M173 parser/AST surface details:

- protocol-qualified object type anchors:
  - `BuildProtocolQualifiedObjectTypeProfile(...)`
  - `IsProtocolQualifiedObjectTypeProfileNormalized(...)`
- parser assignment anchors:
  - `protocol_qualified_object_type_profile`
  - `protocol_qualified_object_type_profile_is_normalized`
  - `return_protocol_qualified_object_type_profile`
  - `return_protocol_qualified_object_type_profile_is_normalized`

Deterministic grammar intent:

- parser computes protocol-composition validity from object-pointer spelling,
  suffix termination, and pointer declarator participation.
- profile normalization is fail-closed for malformed/unterminated protocol
  composition suffixes.

Recommended M173 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m173_frontend_protocol_qualified_object_type_parser_contract.py -q`

## M174 frontend variance and bridged-cast parser/AST surface (M174-A001)

Frontend parser/AST now emits deterministic variance/bridged-cast profiles for
parameter/property/return type annotations.

M174 parser/AST surface details:

- variance/bridge-cast anchors:
  - `BuildVarianceBridgeCastProfile(...)`
  - `IsVarianceBridgeCastProfileNormalized(...)`
- parser assignment anchors:
  - `variance_bridge_cast_profile`
  - `variance_bridge_cast_profile_is_normalized`
  - `return_variance_bridge_cast_profile`
  - `return_variance_bridge_cast_profile_is_normalized`

Deterministic grammar intent:

- parser derives variance marker safety (`__covariant/__contravariant`) and
  bridged-cast marker consistency from deterministic suffix/qualifier packets.
- profile normalization is fail-closed when marked variance/bridge packets are
  emitted without object-pointer type spellings or with conflicting marker
  combinations.

Recommended M174 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m174_frontend_variance_bridge_cast_parser_contract.py -q`

## M175 frontend generic metadata emission and ABI checks parser/AST surface (M175-A001)

Frontend parser/AST now emits deterministic generic-metadata/ABI profiles for
parameter/property/return type annotations.

M175 parser/AST surface details:

- generic-metadata ABI anchors:
  - `BuildGenericMetadataAbiProfile(...)`
  - `IsGenericMetadataAbiProfileNormalized(...)`
- parser assignment anchors:
  - `generic_metadata_abi_profile`
  - `generic_metadata_abi_profile_is_normalized`
  - `return_generic_metadata_abi_profile`
  - `return_generic_metadata_abi_profile_is_normalized`

Deterministic grammar intent:

- parser derives generic metadata emission readiness from object-pointer spelling,
  generic suffix termination, and top-level generic argument-slot accounting.
- profile normalization is fail-closed for malformed generic packets that would
  destabilize ABI-side metadata interpretation.

Recommended M175 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m175_frontend_generic_metadata_abi_parser_contract.py -q`

## M176 frontend module map ingestion and import graph parser/AST surface (M176-A001)

Frontend parser/AST now emits deterministic module-import-graph profiles for
parameter/property/return type annotations.

M176 parser/AST surface details:

- module import-graph anchors:
  - `BuildModuleImportGraphProfile(...)`
  - `IsModuleImportGraphProfileNormalized(...)`
- parser assignment anchors:
  - `module_import_graph_profile`
  - `module_import_graph_profile_is_normalized`
  - `return_module_import_graph_profile`
  - `return_module_import_graph_profile_is_normalized`

Deterministic grammar intent:

- parser derives import-edge candidates from generic suffix packets and module
  namespace segments from object-pointer spellings.
- profile normalization is fail-closed for malformed generic packets that would
  destabilize deterministic import-graph handoff.

Recommended M176 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m176_frontend_module_import_graph_parser_contract.py -q`

## M177 frontend namespace collision and shadowing diagnostics parser/AST surface (M177-A001)

Frontend parser/AST now emits deterministic namespace-collision/shadowing
profiles for parameter/property/return type annotations.

M177 parser/AST surface details:

- namespace collision/shadowing anchors:
  - `BuildNamespaceCollisionShadowingProfile(...)`
  - `IsNamespaceCollisionShadowingProfileNormalized(...)`
- parser assignment anchors:
  - `namespace_collision_shadowing_profile`
  - `namespace_collision_shadowing_profile_is_normalized`
  - `return_namespace_collision_shadowing_profile`
  - `return_namespace_collision_shadowing_profile_is_normalized`

Deterministic grammar intent:

- parser derives namespace collision and shadowing risk from namespace-segment
  shape, generic suffix packets, and pointer declarator participation.
- profile normalization is fail-closed for malformed packets that would
  destabilize downstream diagnostic routing.

Recommended M177 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m177_frontend_namespace_collision_shadowing_parser_contract.py -q`

## M178 frontend public/private API partition parser/AST surface (M178-A001)

Frontend parser/AST now emits deterministic public/private API partition
profiles for parameter/property/return type annotations.

M178 parser/AST surface details:

- public/private API partition anchors:
  - `BuildPublicPrivateApiPartitionProfile(...)`
  - `IsPublicPrivateApiPartitionProfileNormalized(...)`
- parser assignment anchors:
  - `public_private_api_partition_profile`
  - `public_private_api_partition_profile_is_normalized`
  - `return_public_private_api_partition_profile`
  - `return_public_private_api_partition_profile_is_normalized`

Deterministic grammar intent:

- parser derives visibility-partition readiness from namespace-segment shape,
  generic suffix packets, and pointer declarator participation.
- profile normalization is fail-closed for private-partition packets that would
  destabilize downstream visibility-boundary diagnostics.

Recommended M178 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m178_frontend_public_private_api_partition_parser_contract.py -q`

## M179 frontend incremental module cache and invalidation parser/AST surface (M179-A001)

Frontend parser/AST now emits deterministic incremental-module-cache/invalidation
profiles for parameter/property/return type annotations.

M179 parser/AST surface details:

- incremental module-cache/invalidation anchors:
  - `BuildIncrementalModuleCacheInvalidationProfile(...)`
  - `IsIncrementalModuleCacheInvalidationProfileNormalized(...)`
- parser assignment anchors:
  - `incremental_module_cache_invalidation_profile`
  - `incremental_module_cache_invalidation_profile_is_normalized`
  - `return_incremental_module_cache_invalidation_profile`
  - `return_incremental_module_cache_invalidation_profile_is_normalized`

Deterministic grammar intent:

- parser derives cache-key readiness and invalidation-on-shape-change state from
  object-pointer spelling, generic suffix packets, namespace-segment shape, and
  pointer declarator participation.
- profile normalization is fail-closed for malformed packets that would
  destabilize incremental module-cache replay and invalidation handoff.

Recommended M179 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m179_frontend_incremental_module_cache_parser_contract.py -q`

## M180 frontend cross-module conformance suite parser/AST surface (M180-A001)

Frontend parser/AST now emits deterministic cross-module conformance suite
profiles for parameter/property/return type annotations.

M180 parser/AST surface details:

- cross-module conformance suite anchors:
  - `BuildCrossModuleConformanceProfile(...)`
  - `IsCrossModuleConformanceProfileNormalized(...)`
- parser assignment anchors:
  - `cross_module_conformance_profile`
  - `cross_module_conformance_profile_is_normalized`
  - `return_cross_module_conformance_profile`
  - `return_cross_module_conformance_profile_is_normalized`
- parser transfer/copy anchors:
  - `CopyMethodReturnTypeFromFunctionDecl(...)`
  - `CopyPropertyTypeFromParam(...)`
  - `target.return_cross_module_conformance_profile_is_normalized = source.return_cross_module_conformance_profile_is_normalized;`
  - `target.return_cross_module_conformance_profile = source.return_cross_module_conformance_profile;`
  - `target.cross_module_conformance_profile_is_normalized = source.cross_module_conformance_profile_is_normalized;`
  - `target.cross_module_conformance_profile = source.cross_module_conformance_profile;`

Deterministic grammar intent:

- parser derives cross-module boundary engagement and conformance-surface
  readiness from object-pointer spelling, generic suffix packets,
  namespace-segment shape, and pointer declarator participation.
- parameter/profile normalization remains deterministic across direct parameter
  parsing and property/method type transfer surfaces, including normalized-flag
  transfer anchors.
- profile normalization is fail-closed for malformed packets that would
  destabilize cross-module conformance replay and handoff.

Recommended M180 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m180_frontend_cross_module_conformance_parser_contract.py -q`

## M181 frontend throws declarations and propagation parser/AST surface (M181-A001)

Frontend parser/AST now emits deterministic throws-declaration profiles for
function and Objective-C method declarations, with explicit propagation anchors
across parser transfer surfaces.

M181 parser/AST surface details:

- throws declaration/profile anchors:
  - `BuildThrowsDeclarationProfile(...)`
  - `IsThrowsDeclarationProfileNormalized(...)`
  - `AtThrowsClauseKeyword()`
  - `ParseOptionalThrowsClause(FunctionDecl &fn)`
  - `ParseOptionalThrowsClause(Objc3MethodDecl &method)`
  - `FinalizeThrowsDeclarationProfile(FunctionDecl &fn, bool has_return_annotation)`
  - `FinalizeThrowsDeclarationProfile(Objc3MethodDecl &method)`
- parser assignment anchors:
  - `throws_declared`
  - `throws_declaration_profile`
  - `throws_declaration_profile_is_normalized`
- parser transfer/copy anchors:
  - `CopyMethodReturnTypeFromFunctionDecl(...)`
  - `target.throws_declared = source.throws_declared;`
  - `target.throws_declaration_profile = source.throws_declaration_profile;`

Deterministic grammar intent:

- `throws` is recognized as a declaration-tail modifier for function and
  Objective-C method declarations.
- throws packet normalization is fail-closed for malformed declaration-shape
  combinations that would destabilize parser-to-sema propagation.
- throws profile emission remains deterministic across direct parse surfaces and
  parser transfer anchors.

Recommended M181 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m181_frontend_throws_parser_contract.py -q`

## M182 frontend result-like control-flow/lowering preparatory parser/AST surface (M182-A001)

Frontend parser/AST now emits deterministic result-like control-flow profile
packets on function and Objective-C method declarations so lowering can consume
a stable preparatory lane-A handoff.

M182 parser/AST surface details:

- result-like profile anchors:
  - `BuildResultLikeProfile(...)`
  - `IsResultLikeProfileNormalized(...)`
  - `CollectResultLikeExprProfile(...)`
  - `CollectResultLikeStmtProfile(...)`
  - `BuildResultLikeProfileFromBody(...)`
  - `BuildResultLikeProfileFromOpaqueBody(...)`
  - `FinalizeResultLikeProfile(FunctionDecl &fn)`
  - `FinalizeResultLikeProfile(Objc3MethodDecl &method)`
- parser assignment anchors:
  - `result_like_profile`
  - `result_like_profile_is_normalized`
  - `deterministic_result_like_lowering_handoff`
  - `result_like_sites`
  - `result_success_sites`
  - `result_failure_sites`
  - `result_branch_sites`
  - `result_payload_sites`
  - `result_normalized_sites`
  - `result_branch_merge_sites`
  - `result_contract_violation_sites`
- parser transfer/copy anchors:
  - `CopyMethodReturnTypeFromFunctionDecl(...)`
  - `target.result_like_profile = source.result_like_profile;`
  - `target.deterministic_result_like_lowering_handoff = source.deterministic_result_like_lowering_handoff;`

Deterministic grammar intent:

- return and control-flow statement surfaces are reduced into stable
  result-like site counters for lowering replay preparation.
- parser profile normalization is fail-closed and invariant-checked for
  `normalized_sites + branch_merge_sites == result_like_sites`.
- Objective-C method declarations with opaque implementation bodies still
  emit deterministic preparatory packets through explicit opaque-body anchors.

Recommended M182 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m182_frontend_result_like_parser_contract.py -q`

## M183 frontend NSError-bridging parser/AST surface (M183-A001)

Frontend parser/AST now emits deterministic `NSError`-bridging convention
profiles on function and Objective-C method declaration surfaces so lowering can
consume a stable lane-A handoff packet for bridge-path analysis.

M183 parser/AST surface details:

- NSError-bridging profile anchors:
  - `BuildNSErrorBridgingProfile(...)`
  - `IsNSErrorBridgingProfileNormalized(...)`
  - `IsNSErrorTypeSpelling(...)`
  - `IsNSErrorOutParameterSite(...)`
  - `IsFailableCallSymbol(...)`
  - `CountFailableCallSitesInExpr(...)`
  - `BuildNSErrorBridgingProfileFromParameters(...)`
  - `BuildNSErrorBridgingProfileFromFunction(...)`
  - `BuildNSErrorBridgingProfileFromOpaqueBody(...)`
  - `FinalizeNSErrorBridgingProfile(FunctionDecl &fn)`
  - `FinalizeNSErrorBridgingProfile(Objc3MethodDecl &method)`
- parser assignment anchors:
  - `ns_error_bridging_profile`
  - `ns_error_bridging_profile_is_normalized`
  - `deterministic_ns_error_bridging_lowering_handoff`
  - `ns_error_bridging_sites`
  - `ns_error_parameter_sites`
  - `ns_error_out_parameter_sites`
  - `ns_error_bridge_path_sites`
  - `failable_call_sites`
  - `ns_error_bridging_normalized_sites`
  - `ns_error_bridge_boundary_sites`
  - `ns_error_bridging_contract_violation_sites`
- parser transfer/copy anchors:
  - `CopyMethodReturnTypeFromFunctionDecl(...)`
  - `target.ns_error_bridging_profile = source.ns_error_bridging_profile;`
  - `target.deterministic_ns_error_bridging_lowering_handoff = source.deterministic_ns_error_bridging_lowering_handoff;`

Deterministic grammar intent:

- parser detects `NSError` parameter and out-parameter conventions directly from
  parameter type spelling and pointer declarator evidence.
- failable call spelling evidence is reduced into deterministic packet counters
  for bridge-path preparation.
- profile normalization is fail-closed and invariant-checked for
  `normalized_sites + bridge_boundary_sites == ns_error_bridging_sites`.

Recommended M183 frontend contract check:

- `python -m pytest tests/tooling/test_objc3c_m183_frontend_ns_error_bridging_parser_contract.py -q`

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
  - `Objc3SemaTokenKind::OwnershipQualifier`
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
  1. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  1. `python -m pytest tests/tooling/test_objc3c_m216_frontend_conformance_contract.py -q`
  1. `python -m pytest tests/tooling/test_objc3c_m215_frontend_sdk_packaging_contract.py -q`

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

Frontend foreign-type import diagnostics contract relies on deterministic parser fail-closed syntax boundaries, stable foreign/object-pointer spelling carrier fields into AST/sema, and semantic enforcement of generic-suffix allowlists.

- Required frontend foreign-type diagnostics signals:
  - malformed parameter-type annotations remain parser fail-closed `O3P108` (invalid/missing type token or unterminated parameter generic suffix syntax).
  - malformed return-type annotations remain parser fail-closed `O3P114`.
  - parser spelling carriers remain explicit: `param.id_spelling`, `param.class_spelling`, `param.instancetype_spelling`, `param.object_pointer_type_spelling`, `param.object_pointer_type_name`, `fn.return_id_spelling`, `fn.return_class_spelling`, `fn.return_instancetype_spelling`, `fn.return_object_pointer_type_spelling`, and `fn.return_object_pointer_type_name`.
  - semantic generic-suffix allowlist enforcement remains `O3S206` via `SupportsGenericParamTypeSuffix` / `SupportsGenericReturnTypeSuffix` (`id` / `Class` / `instancetype`).
  - AST carrier continuity remains `bool id_spelling = false;`, `bool class_spelling = false;`, `bool instancetype_spelling = false;`, `bool object_pointer_type_spelling = false;`, and return equivalents.
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

## M192 frontend inline asm + intrinsic governance packetization

Frontend inline-asm/intrinsic governance contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for inline asm and privileged intrinsic gating.

- Required frontend inline-asm/intrinsic governance signals:
  - parser symbol classifiers remain `IsInlineAsmCallSymbol(...)`, `IsIntrinsicCallSymbol(...)`, and `IsPrivilegedIntrinsicCallSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3InlineAsmIntrinsicGovernanceProfile`.
  - parser profile serialization remains `BuildInlineAsmIntrinsicGovernanceProfile(...)`.
  - parser profile invariant gate remains `IsInlineAsmIntrinsicGovernanceProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeInlineAsmIntrinsicGovernanceProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeInlineAsmIntrinsicGovernanceProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;` and `method.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;`.
  - AST carrier anchors remain `bool inline_asm_intrinsic_governance_profile_is_normalized = false;`, `bool deterministic_inline_asm_intrinsic_governance_handoff = false;`, and `std::string inline_asm_intrinsic_governance_profile;` on function/method declarations.
- Required frontend inline-asm/intrinsic governance commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m192_frontend_inline_asm_intrinsic_parser_contract.py -q`

## M191 frontend unsafe pointer-arithmetic extension gating

Frontend unsafe pointer-extension gating relies on deterministic parser-owned profile synthesis for unsafe ownership qualifiers, raw pointer type spellings, and pointer-arithmetic operation sites.

- Required frontend unsafe pointer-extension gating signals:
  - parser profile packet carrier remains `struct Objc3UnsafePointerExtensionProfile`.
  - parser profile serialization remains `BuildUnsafePointerExtensionProfile(...)`.
  - parser profile invariant gate remains `IsUnsafePointerExtensionProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeUnsafePointerExtensionProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeUnsafePointerExtensionProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;` and `method.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;`.
  - AST carrier anchors remain `bool unsafe_pointer_extension_profile_is_normalized = false;`, `bool deterministic_unsafe_pointer_extension_handoff = false;`, and `std::string unsafe_pointer_extension_profile;` on function/method declarations.
- Required frontend unsafe pointer-extension gating commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m193_frontend_simd_vector_lowering_contract.py -q`
  4. `python -m pytest tests/tooling/test_objc3c_m191_frontend_unsafe_pointer_arithmetic_parser_contract.py -q`

## M190 frontend concurrency replay-proof and race-guard packetization

Frontend concurrency replay/race-guard contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for replay proof, race guard, task handoff, and actor isolation surfaces.

- Required frontend concurrency replay/race-guard signals:
  - parser symbol classifiers remain `IsConcurrencyReplaySymbol(...)`, `IsReplayProofSymbol(...)`, `IsRaceGuardSymbol(...)`, `IsTaskHandoffSymbol(...)`, and `IsActorIsolationSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3ConcurrencyReplayRaceGuardProfile`.
  - parser profile serialization remains `BuildConcurrencyReplayRaceGuardProfile(...)`.
  - parser profile invariant gate remains `IsConcurrencyReplayRaceGuardProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeConcurrencyReplayRaceGuardProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeConcurrencyReplayRaceGuardProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;` and `method.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;`.
  - AST carrier anchors remain `bool concurrency_replay_race_guard_profile_is_normalized = false;`, `bool deterministic_concurrency_replay_race_guard_handoff = false;`, and `std::string concurrency_replay_race_guard_profile;` on function/method declarations.
- Required frontend concurrency replay/race-guard commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m190_frontend_concurrency_replay_parser_contract.py -q`

## M189 frontend task-runtime interop and cancellation packetization

Frontend task-runtime/cancellation contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for runtime hooks, cancellation checks, cancellation handlers, and suspension points.

- Required frontend task-runtime/cancellation signals:
  - parser symbol classifiers remain `IsTaskRuntimeHookSymbol(...)`, `IsCancellationCheckSymbol(...)`, `IsCancellationHandlerSymbol(...)`, and `IsSuspensionPointSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3TaskRuntimeCancellationProfile`.
  - parser profile serialization remains `BuildTaskRuntimeCancellationProfile(...)`.
  - parser profile invariant gate remains `IsTaskRuntimeCancellationProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeTaskRuntimeCancellationProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeTaskRuntimeCancellationProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.task_runtime_interop_sites = profile.task_runtime_interop_sites;` and `method.task_runtime_interop_sites = profile.task_runtime_interop_sites;`.
  - AST carrier anchors remain `bool task_runtime_cancellation_profile_is_normalized = false;`, `bool deterministic_task_runtime_cancellation_handoff = false;`, and `std::string task_runtime_cancellation_profile;` on function/method declarations.
- Required frontend task-runtime/cancellation commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m189_frontend_task_runtime_cancellation_parser_contract.py -q`

## M188 frontend actor-isolation and sendability packetization

Frontend actor-isolation/sendability contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for actor declarations, actor hops, sendable annotations, and non-sendable crossings.

- Required frontend actor-isolation/sendability signals:
  - parser symbol classifiers remain `IsActorIsolationDeclSymbol(...)`, `IsActorHopSymbol(...)`, `IsSendableAnnotationSymbol(...)`, and `IsNonSendableCrossingSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3ActorIsolationSendabilityProfile`.
  - parser profile serialization remains `BuildActorIsolationSendabilityProfile(...)`.
  - parser profile invariant gate remains `IsActorIsolationSendabilityProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeActorIsolationSendabilityProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeActorIsolationSendabilityProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;` and `method.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;`.
  - AST carrier anchors remain `bool actor_isolation_sendability_profile_is_normalized = false;`, `bool deterministic_actor_isolation_sendability_handoff = false;`, and `std::string actor_isolation_sendability_profile;` on function/method declarations.
- Required frontend actor-isolation/sendability commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m188_frontend_actor_isolation_sendability_parser_contract.py -q`

## M187 frontend await lowering and suspension packetization

Frontend await-lowering/suspension contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for await keywords, suspension points, resume surfaces, state-machine hooks, and continuation surfaces.

- Required frontend await-lowering/suspension signals:
  - parser symbol classifiers remain `IsAwaitKeywordSymbol(...)`, `IsAwaitSuspensionPointSymbol(...)`, `IsAwaitResumeSymbol(...)`, `IsAwaitStateMachineSymbol(...)`, and `IsAwaitContinuationSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3AwaitSuspensionProfile`.
  - parser profile serialization remains `BuildAwaitSuspensionProfile(...)`.
  - parser profile invariant gate remains `IsAwaitSuspensionProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeAwaitSuspensionProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeAwaitSuspensionProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.await_suspension_sites = profile.await_suspension_sites;` and `method.await_suspension_sites = profile.await_suspension_sites;`.
  - AST carrier anchors remain `bool await_suspension_profile_is_normalized = false;`, `bool deterministic_await_suspension_handoff = false;`, and `std::string await_suspension_profile;` on function/method declarations.
- Required frontend await-lowering/suspension commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m187_frontend_await_suspension_parser_contract.py -q`

## M186 frontend async grammar and continuation packetization

Frontend async-grammar/continuation contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for async keyword/function surfaces and continuation allocation/resume/suspend state-machine surfaces.

- Required frontend async-grammar/continuation signals:
  - parser symbol classifiers remain `IsAsyncKeywordSymbol(...)`, `IsAsyncFunctionSymbol(...)`, `IsContinuationAllocationSymbol(...)`, `IsContinuationResumeSymbol(...)`, `IsContinuationSuspendSymbol(...)`, and `IsAsyncStateMachineSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3AsyncContinuationProfile`.
  - parser profile serialization remains `BuildAsyncContinuationProfile(...)`.
  - parser profile invariant gate remains `IsAsyncContinuationProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeAsyncContinuationProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeAsyncContinuationProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.async_continuation_sites = profile.async_continuation_sites;` and `method.async_continuation_sites = profile.async_continuation_sites;`.
  - AST carrier anchors remain `bool async_continuation_profile_is_normalized = false;`, `bool deterministic_async_continuation_handoff = false;`, and `std::string async_continuation_profile;` on function/method declarations.
- Required frontend async-grammar/continuation commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m186_frontend_async_continuation_parser_contract.py -q`

## M185 frontend error diagnostics UX and recovery packetization

Frontend error-diagnostics/recovery contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for diagnostic emits, recovery anchors, recovery boundaries, and fail-closed diagnostics.

- Required frontend error-diagnostics/recovery signals:
  - parser symbol classifiers remain `IsErrorDiagnosticSymbol(...)`, `IsRecoveryAnchorSymbol(...)`, `IsRecoveryBoundarySymbol(...)`, and `IsFailClosedDiagnosticSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3ErrorDiagnosticsRecoveryProfile`.
  - parser profile serialization remains `BuildErrorDiagnosticsRecoveryProfile(...)`.
  - parser profile invariant gate remains `IsErrorDiagnosticsRecoveryProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeErrorDiagnosticsRecoveryProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeErrorDiagnosticsRecoveryProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;` and `method.error_diagnostics_recovery_sites = profile.error_diagnostics_recovery_sites;`.
  - AST carrier anchors remain `bool error_diagnostics_recovery_profile_is_normalized = false;`, `bool deterministic_error_diagnostics_recovery_handoff = false;`, and `std::string error_diagnostics_recovery_profile;` on function/method declarations.
- Required frontend error-diagnostics/recovery commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m185_frontend_error_diagnostics_recovery_parser_contract.py -q`

## M184 frontend unwind safety and cleanup emission packetization

Frontend unwind-safety/cleanup-emission contract relies on deterministic parser-owned symbol classification and replay-stable AST profile packet transport for exceptional exits, cleanup actions, cleanup scopes, and cleanup resume surfaces.

- Required frontend unwind-safety/cleanup-emission signals:
  - parser symbol classifiers remain `IsExceptionalExitSymbol(...)`, `IsCleanupActionSymbol(...)`, `IsCleanupScopeSymbol(...)`, and `IsCleanupResumeSymbol(...)`.
  - parser profile packet carrier remains `struct Objc3UnwindCleanupProfile`.
  - parser profile serialization remains `BuildUnwindCleanupProfile(...)`.
  - parser profile invariant gate remains `IsUnwindCleanupProfileNormalized(...)`.
  - function declaration finalization remains `FinalizeUnwindCleanupProfile(FunctionDecl &fn)`.
  - Objective-C method declaration finalization remains `FinalizeUnwindCleanupProfile(Objc3MethodDecl &method)`.
  - parser profile transport remains `fn.unwind_cleanup_sites = profile.unwind_cleanup_sites;` and `method.unwind_cleanup_sites = profile.unwind_cleanup_sites;`.
  - AST carrier anchors remain `bool unwind_cleanup_profile_is_normalized = false;`, `bool deterministic_unwind_cleanup_handoff = false;`, and `std::string unwind_cleanup_profile;` on function/method declarations.
- Required frontend unwind-safety/cleanup-emission commands (run in order):
  1. `npm run test:objc3c:parser-ast-extraction`
  2. `npm run test:objc3c:parser-extraction-ast-builder-contract`
  3. `python -m pytest tests/tooling/test_objc3c_m184_frontend_unwind_cleanup_parser_contract.py -q`

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

## M267 frontend Part 6 error source closure

The frontend now freezes one truthful Part 6 source surface in
`frontend.pipeline.semantic_surface.objc_part6_error_source_closure`.

- live parser/AST/source-only surfaces:
  - `throws` declarations during frontend-only validation on functions and Objective-C methods
  - deterministic result-like carrier profiling on function and method bodies
  - `NSError` bridging profiling remains emitted as deterministic frontend state
- `try`, `throw`, and `do/catch` remain reserved fail-closed parser constructs:
  - `try` expressions
  - `throw` statements
  - `do { ... } catch { ... }`
- the current frontend runner may admit `throws` declarations for source-only
  validation when both IR and object emission are disabled
- runnable propagation, `try`, `throw`, `do/catch`, and native error ABI remain
  deferred to later `M267` sema/lowering/runtime issues

## M267 frontend canonical bridge-marker completion

The frontend now accepts the canonical Part 6 declaration markers:

- `__attribute__((objc_nserror))`
- `__attribute__((objc_status_code(success: ..., error_type: ..., mapping: ...)))`

Current implementation boundary:

- function and Objective-C method declarations normalize these markers into the
  Part 6 frontend semantic surface
- the semantic surface now counts canonical `objc_nserror` and
  `objc_status_code(...)` markers and the required status-code clauses
- malformed `objc_status_code(...)` payloads fail closed in the parser
- runtime `try` lowering, bridge temporaries, and status-to-error execution
  remain deferred to later `M267` issues

## M268 frontend async source closure

The frontend now owns one truthful Part 7 source boundary for async syntax.

- `async fn` is admitted as a parser-owned declaration form
- Objective-C methods now admit a parser-owned `async` declaration modifier
- `await <expr>` is admitted as a parser-owned expression marker
- callable declarations now admit:
  - `__attribute__((objc_executor(main)))`
  - `__attribute__((objc_executor(global)))`
  - `__attribute__((objc_executor(named("..."))))`

Current implementation status:

- this is source closure only
- no continuation ABI, suspension cleanup, executor runtime, or runnable async
  behavior is claimed by `M268-A001`

## M268 frontend async semantic packet

The frontend now publishes a dedicated Part 7 source packet at
`frontend.pipeline.semantic_surface.objc_part7_async_source_closure`.

- the packet counts parser-owned `async` declaration sites
- the packet counts parser-owned `await` expression sites
- the packet records canonical `objc_executor(...)` attribute use
- the packet remains source-only and does not claim runnable continuation or
  executor runtime behavior

## M268 async effect and suspension semantic model

The semantic pipeline now publishes a dedicated Part 7 semantic packet at
`frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model`.

- the packet carries live async-continuation legality counts derived from sema
  integration summaries
- the packet carries live await-suspension legality counts derived from sema
  integration summaries
- the packet records the current actor-isolation, task-cancellation, and
  concurrency-replay guard profile handoff that already feeds later Part 7 work
- the packet is ready for later lowering and runtime expansion, but it still
  does not claim runnable async frame lowering, suspension cleanup, or executor
  runtime execution behavior

## M269 task, executor, and cancellation source closure

The frontend now freezes one truthful source boundary for task-runtime and
cancellation-oriented callable code.

- no dedicated `task` or `cancel` keyword is claimed by this issue
- the admitted source surface remains existing `async fn`, `await <expr>`, and
  canonical `objc_executor(...)` callable attributes
- parser-owned symbol profiling now serves as the deterministic source contract
  for task-runtime hooks, cancellation checks, cancellation handlers, and
  suspension-point identifiers
- the happy path is proven through the frontend C API runner with
  `--no-emit-ir --no-emit-object`, not through runnable task scheduling yet

Current implementation status:

- this issue freezes source closure only
- runnable task creation, executor hops, scheduler ownership, and cancellation
  execution semantics remain later `M269` work

## M269 frontend task-group and cancellation packet

The frontend now publishes a dedicated task-runtime source packet at
`frontend.pipeline.semantic_surface.objc_part7_task_group_and_cancellation_source_closure`.

- the packet counts task creation call sites admitted by the current frontend
  source model
- the packet counts the supported task-group scope, add-task, wait-next, and
  cancel-all call surface
- the packet counts cancellation checks and cancellation handlers carried by the
  same callable source surface
- the packet remains source-only and does not claim runnable task allocation,
  scheduler execution, or live cancellation runtime behavior

## M270 actor, isolation, and sendable source closure

The frontend now freezes one truthful source boundary for actor, isolation, and
sendability markers.

- no dedicated `actor`, `sendable`, or `nonisolated` keyword is claimed by this issue
- the admitted source surface remains the existing parser-owned symbol and
  attribute profiling already carried by the Part 7 async semantic packet
- parser-owned symbol profiling remains the deterministic source contract for
  actor-isolation declarations, actor hops, sendable markers, and non-sendable
  crossings
- the happy path is proven through the frontend C API runner with
  `--no-emit-ir --no-emit-object`, not through live actor-member or runtime
  behavior yet

Current implementation status:

- this issue freezes source closure only
- actor-member semantics, isolation diagnostics, cross-actor legality, and
  runnable actor runtime behavior remain later `M270` work

## M270 actor-member and isolation annotation surface

The frontend now admits one dedicated actor-member source surface and publishes
it under
`frontend.pipeline.semantic_surface.objc_part7_actor_member_and_isolation_source_closure`.

- `actor class` is admitted as a contextual parser surface rather than a
  reserved lexer keyword
- actor interfaces now carry methods and properties through the normal
  interface source closure
- callable `__attribute__((objc_nonisolated))` is admitted on actor methods
- actor methods continue to carry existing `async` and
  `__attribute__((objc_executor(...)))` spellings
- the emitted semantic packet publishes deterministic counts for actor
  interfaces, actor methods, actor properties, nonisolated annotations,
  executor annotations on actor members, async actor methods, and actor member
  metadata sites

Current implementation status:

- this issue completes the frontend/source-model surface only
- actor-member legality, cross-actor diagnostics, sendability enforcement, and
  runnable actor runtime behavior remain later `M270` work

## M270 actor isolation and sendable semantic model

The semantic pipeline now publishes a dedicated packet at
`frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendable_semantic_model`.

- the packet consumes the existing `M270-A002` actor-member source packet and
  the already-live aggregated actor/sendability sema counters
- the packet freezes one truthful sema model for:
  - actor-member source dependency
  - parser-owned actor-isolation/sendability profile normalization
  - fail-closed strict-concurrency selection/reporting
- current support remains a deterministic sema/accounting boundary rather than
  a full actor runtime or broad cross-actor legality implementation

Current implementation status:

- this issue freezes one dedicated actor/sendability sema packet
- dedicated actor-isolation diagnostics, sendability enforcement, and runnable
  actor/executor runtime behavior remain later `M270` work

## M269 task executor and cancellation semantic model

The frontend now publishes a dedicated semantic packet at
`frontend.pipeline.semantic_surface.objc_part7_task_executor_and_cancellation_semantic_model`.

- the packet consumes the existing `M269-A002` task-group/cancellation source
  packet rather than widening the source boundary again
- the packet freezes live semantic ownership for task lifetime legality,
  executor-affinity legality, cancellation observation legality, and structured
  task-group legality
- runnable lowering, executor runtime behavior, and scheduler runtime behavior
  remain deferred to later `M269` issues

## M269 structured task and cancellation semantics

The frontend now publishes a dedicated semantic packet at
`frontend.pipeline.semantic_surface.objc_part7_structured_task_and_cancellation_semantics`.

- task-runtime, task-group, and cancellation calls now fail closed outside async
  functions and methods with `O3S227`
- task-group add, wait-next, and cancel-all calls now fail closed without a
  task-group scope in the same async callable with `O3S228`
- detached task creation now fails closed when mixed with structured task-group
  callables in the same async callable with `O3S229`
- cancellation handlers and cancel-all calls now fail closed without a
  cancellation check in the same async callable with `O3S230`

## M269 executor hop and affinity compatibility completion

The frontend now publishes a dedicated semantic packet at
`frontend.pipeline.semantic_surface.objc_part7_executor_hop_and_affinity_compatibility_completion`.

- async task callables now fail closed without `objc_executor(...)` affinity
  with `O3S231`
- detached task creation now fails closed under `objc_executor(main)` with
  `O3S232`
- runnable executor-hop lowering and scheduler-backed execution remain deferred
  to later `M269` issues

## M269 task runtime lowering contract

The frontend now publishes a dedicated lowering packet at
`frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract`.

- the packet consumes the existing `M269-B001`, `M269-B002`, and `M269-B003`
  semantic packets and lowers them into explicit replay-stable lane contracts
  for actor isolation, task runtime interop, and concurrency replay guards
- emitted IR now carries replay keys and frontend lowering profiles for task
  creation, task-group artifacts, executor-hop boundaries, cancellation polls,
  and scheduler-visible replay handoff counts
- this issue freezes the lowering handoff only; native task spawn, executor
  hop, cancellation runtime entrypoints, and task-group ABI completion remain
  later `M269` work

## M269 executor hop, cancellation, and task spawning lowering

`M269-C002` turns the retained lowering packet into a live helper-backed IR
rewrite for the currently recognized task-runtime symbol family.

- supported task symbols such as `task_spawn_child`, `detached_task_create`,
  `task_group_wait_next`, `task_group_cancel_all`, and
  `task_runtime_cancelled_value` now lower through private runtime helpers
- awaited task-group `wait_next` sites also emit an explicit
  `objc3_runtime_executor_hop_i32` handoff before the existing continuation
  helper path resumes the current async callable
- the helper ABI remains private to
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- current native end-to-end issue proof is still constrained by older
  `O3S260` / `O3L300` front-door gates, so the issue-local runtime probe proves
  the helper cluster directly while IR lowering remains source-anchored

## M269 task-group and runtime ABI completion

`M269-C003` publishes the dedicated ABI/artifact packet for the helper-backed
Part 7 task-runtime slice.

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_task_group_and_runtime_abi_completion`
- emitted IR now carries:
  - `; part7_task_runtime_abi_completion = ...`
  - `!objc3.objc_part7_task_runtime_abi_completion = !{!93}`
- the packet preserves the helper list, task-group helper count, and the
  private runtime snapshot symbol
- the runtime proof surface remains private and continues to use
  `objc3_runtime_copy_task_runtime_state_for_testing`

## M269 scheduler and executor runtime contract

`M269-D001` freezes the truthful private runtime boundary above the helper
slice already landed in `M269-C002` and `M269-C003`.

- the contract remains private to
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- the helper cluster is:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`
- the private testing snapshot remains
  `objc3_runtime_copy_task_runtime_state_for_testing`
- emitted IR carries:
  - `; part7_scheduler_executor_runtime_contract = ...`
  - `!objc3.objc_part7_scheduler_executor_runtime_contract = !{!94}`
- the public runtime header still does not widen
- this issue does not yet claim a general scheduler implementation or
  cross-module task runtime behavior

## M269 live task runtime and executor implementation

`M269-D002` turns the same private helper cluster into a truthful live runtime
execution boundary for the supported Part 7 task slice.

- emitted IR now carries:
  - `; part7_live_task_runtime_integration = ...`
  - `!objc3.objc_part7_live_task_runtime_integration = !{!95}`
- the linked runtime probe proves executable traffic through:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`
  - `objc3_runtime_copy_task_runtime_state_for_testing`
- the public runtime header still does not widen
- retained runtime-metadata export gates can still fail closed earlier with
  `O3S260` / `O3L300`, so D002 proves the live helper-backed execution path
  directly rather than claiming broader front-door scheduler completeness

## M269 task runtime cancellation and autorelease hardening

`M269-D003` hardens the same live task-runtime slice around reset-stable replay
and autoreleasepool/cancellation edges.

- emitted IR now carries:
  - `; part7_task_runtime_hardening = ...`
  - `!objc3.objc_part7_task_runtime_hardening = !{!96}`
- the linked runtime probe validates deterministic two-pass replay across:
  - `objc3_runtime_reset_for_testing`
  - `objc3_runtime_copy_task_runtime_state_for_testing`
  - `objc3_runtime_copy_memory_management_state_for_testing`
  - `objc3_runtime_copy_arc_debug_state_for_testing`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
- the supported task helper cluster remains private and live
- broader front-door metadata-export gating still remains outside this issue

## M269 task and executor conformance gate

`M269-E001` freezes the runnable task/executor milestone gate on top of the
already-landed source, sema, lowering, ABI, and hardened runtime slices.

- the gate consumes `M269-A002`, `M269-B003`, `M269-C003`, and `M269-D003`
  summaries rather than introducing a second task-runtime proof model
- the truthful runnable proof remains the hardened `M269-D003` live runtime
  probe and its deterministic snapshot/replay evidence
- lane-E keeps the driver/manifest/frontend publication anchors explicit so the
  current fail-closed front-door behavior stays visible to operators
- the next issue is `M269-E002`

## M269 runnable task and executor matrix closeout (M269-E002)

`M269-E002` closes the current task/executor milestone truthfully by replaying
`M269-A002` through `M269-E001` and publishing one explicit runnable matrix for
the already-landed Part 7 task/runtime slice.

- the closeout rows are:
  - task creation source closure
  - executor-affinity legality
  - task runtime ABI/helper publication
  - hardened cancellation/autorelease replay
  - lane-E conformance gate
- the matrix does not widen the supported surface beyond the current helper-
  backed task/runtime slice
- the next issue is `M270-A001`

## M268 await suspension and resume semantics

The semantic pipeline now enforces live Part 7 await-placement legality and
publishes a dedicated packet at
`frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics`.

- `await` now fails closed outside an async function or Objective-C method
- the packet records live counts for async callable sites, await expression
  sites, illegal await sites, and await sites admitted inside async callables
- this issue still does not claim runnable async frame layout, resume lowering,
  suspension cleanup, or executor runtime execution

## M268 async diagnostics and compatibility completion

The semantic pipeline now publishes a dedicated compatibility packet at
`frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion`.

- `objc_executor(...)` now fails closed on non-async functions and methods
- async function prototypes now fail closed until continuation lowering lands
- async throws functions now fail closed until async error propagation lands
- this issue still does not claim runnable async frame layout, suspension
  cleanup, or executor runtime execution

## M268 continuation ABI and async lowering contract

The lowering pipeline now publishes a dedicated contract packet at
`frontend.pipeline.semantic_surface.objc_part7_continuation_abi_and_async_lowering_contract`.

- async continuation lowering replay keys now flow through emitted manifests and
  IR metadata
- await suspension lowering replay keys now flow through emitted manifests and
  IR metadata
- the packet freezes the current lowering counts for async entry points, await
  suspension points, and continuation handoff readiness
- this issue still does not claim runnable async frame layout, resume cleanup,
  or executor runtime execution

## M268 native async lowering slice

The native lowering path now publishes a dedicated implementation packet at
`frontend.pipeline.semantic_surface.objc_part7_async_function_await_and_continuation_lowering`.

- supported async functions and async Objective-C methods now emit runnable
  LLVM IR and object files for the current non-suspending happy path
- `await` currently lowers by reusing the operand direct-call path rather than
  materializing continuation allocation, suspend/resume helpers, or a state
  machine
- the emitted packet records that the current slice remains direct-call-only:
  - `continuation_allocation_sites = 0`
  - `continuation_resume_sites = 0`
  - `continuation_suspend_sites = 0`
  - `async_state_machine_sites = 0`
  - `await_resume_sites = 0`
  - `await_state_machine_sites = 0`
  - `await_continuation_sites = 0`
- continuation allocation, suspension cleanup, resume cleanup, and executor
  runtime scheduling remain later `M268` work

## M268 async cleanup integration slice

The native lowering path now publishes a dedicated integration packet at
`frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration`.

- the current non-suspending async slice reuses the already-live
  autoreleasepool scope hooks and defer-cleanup lowering
- the supported proof fixture shows one deterministic composition shape in
  emitted IR:
  - push autoreleasepool scope
  - lower the awaited operand through a direct call
  - pop the autoreleasepool scope
  - emit the deferred cleanup call before returning
- this issue still does not claim continuation-frame cleanup, suspension resume
  cleanup, or executor runtime scheduling

## M268 continuation and runtime-helper contract

The runtime/lowering boundary now publishes a dedicated helper packet through
emitted IR:

- `; part7_continuation_runtime_helper = ...`
- `!objc3.objc_part7_continuation_runtime_helper = !{!91}`

This freezes the first truthful private Part 7 helper cluster for:

- logical continuation-handle allocation
- executor handoff recording
- resume traffic

Current implementation status:

- the helper ABI is private to `objc3_runtime_bootstrap_internal.h`
- the runtime probe can allocate a logical continuation handle, hand it off to
  an executor tag, resume it, and inspect the published testing snapshot
- the published testing snapshot entrypoint is
  `objc3_runtime_copy_async_continuation_state_for_testing`
- compiled async code still uses the direct-call slice from `M268-C002` and
  now has a dedicated helper ABI available for later live integration work
- live suspension frames, state machines, and executor scheduling remain later
  `M268` work

## M268 live continuation runtime integration

The supported non-suspending async slice now publishes one executable runtime
integration boundary through emitted IR:

- `; part7_live_continuation_runtime_integration = ...`
- `!objc3.objc_part7_live_continuation_runtime_integration = !{!92}`

Current implementation status:

- supported `await` sites in async functions and async Objective-C methods now
  execute through:
  - `objc3_runtime_allocate_async_continuation_i32`
  - `objc3_runtime_handoff_async_continuation_to_executor_i32`
  - `objc3_runtime_resume_async_continuation_i32`
- the happy path is still non-suspending and direct-call-only
- emitted object artifacts link against the existing runtime-support archive and
  runtime probes can observe deterministic helper traffic
- live suspension frames, executor scheduling, and cross-module runnable async
  claims remain later `M268` work

## M268 async executable conformance gate (M268-E001)

Lane E freezes one truthful end-to-end gate over the currently runnable Part 7
slice.

Current implementation status:

- the gate fail-closes over the upstream evidence chain from:
  - `M268-A002`
  - `M268-B003`
  - `M268-C003`
  - `M268-D002`
- the canonical gate fixture remains
  `tests/tooling/fixtures/native/m268_d002_live_continuation_runtime_integration_positive.objc3`
- lane-E consumes the emitted manifest/IR/object artifact triplet from the
  native CLI and frontend publication path instead of inventing a separate Part
  7 proof channel
- the gate proves the current runnable async/await slice only: parser-owned
  async syntax, fail-closed legality, direct-call await lowering, cleanup
  integration, and live continuation-helper execution on the supported
  non-suspending path
- matrix expansion and broader suspension/runtime claims are deferred to
  `M268-E002`

## M268 runnable async and await matrix closeout (M268-E002)

The milestone closeout matrix now publishes the exact runnable Part 7 evidence
surface for the currently supported async/await slice.

Current implementation status:

- the closeout consumes the canonical proof chain from:
  - `M268-A002`
  - `M268-B003`
  - `M268-C003`
  - `M268-D002`
  - `M268-E001`
- matrix rows stay bound to the already-landed fixtures and evidence; this issue
  does not introduce a second runtime probe family
- the current runnable rows cover:
  - async function entry on the direct-call slice
  - async Objective-C method entry on the direct-call slice
  - await lowering on the non-suspending path
  - cleanup integration through the existing autoreleasepool/defer lowering
  - live continuation helper allocation, handoff, and resume traffic
- broader suspension-frame, state-machine, executor-runtime, and cross-module
  runnable async claims remain later work beyond this closeout
