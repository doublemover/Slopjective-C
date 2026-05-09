#include "parse/objc3_parser_declaration_surface.h"

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

}  // namespace objc3c::parse
