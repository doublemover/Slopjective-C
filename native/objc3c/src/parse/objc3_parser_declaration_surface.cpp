#include "parse/objc3_parser_declaration_surface.h"

#include "ast/objc3_ast_decl_surface.h"

namespace objc3c::parse {

bool IsObjc3ActorClassDeclarationLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return index + 1u < tokens.size() &&
         tokens[index].kind == Objc3LexTokenKind::Identifier &&
         tokens[index].text == "actor" &&
         tokens[index + 1u].kind == Objc3LexTokenKind::Identifier &&
         tokens[index + 1u].text == "class";
}

bool IsObjc3RemovedOptionalTemplateAliasLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return index + 1u < tokens.size() &&
         tokens[index].kind == Objc3LexTokenKind::Identifier &&
         tokens[index].text == "optional" &&
         tokens[index + 1u].kind == Objc3LexTokenKind::Less;
}

bool IsObjc3TopLevelFunctionQualifierLead(Objc3LexTokenKind kind) {
  return kind == Objc3LexTokenKind::KwPure ||
         kind == Objc3LexTokenKind::KwExtern ||
         kind == Objc3LexTokenKind::KwAsync ||
         kind == Objc3LexTokenKind::KwFn;
}

GlobalDecl BuildObjc3GlobalLetDeclarationIdentity(
    const Objc3LexToken &name_token) {
  GlobalDecl decl;
  decl.name = name_token.text;
  decl.scope_owner_symbol =
      BuildObjcContainerScopeOwner("protocol", decl.name, false, "");
  decl.scope_path_lexicographic =
      BuildScopePathLexicographic(decl.scope_owner_symbol, "protocol:" + decl.name);
  decl.semantic_link_symbol = "protocol:" + decl.name;
  decl.line = name_token.line;
  decl.column = name_token.column;
  return decl;
}

}  // namespace objc3c::parse
