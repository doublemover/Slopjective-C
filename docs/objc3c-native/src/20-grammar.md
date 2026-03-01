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

## M168 frontend __block storage and escape parser/AST surface (M168-A001)

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

