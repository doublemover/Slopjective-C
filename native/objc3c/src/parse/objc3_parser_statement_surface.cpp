#include "parse/objc3_parser_statement_surface.h"

#include "parse/objc3_parser_expression_surface.h"

namespace objc3c::parse {

bool IsObjc3IdentifierColonStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return index + 1u < tokens.size() &&
         tokens[index].kind == Objc3LexTokenKind::Identifier &&
         tokens[index + 1u].kind == Objc3LexTokenKind::Colon;
}

bool IsObjc3IdentifierAssignmentStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return index + 1u < tokens.size() &&
         tokens[index].kind == Objc3LexTokenKind::Identifier &&
         IsObjc3AssignmentOperatorToken(tokens[index + 1u].kind);
}

bool IsObjc3IdentifierUpdateStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return index + 1u < tokens.size() &&
         tokens[index].kind == Objc3LexTokenKind::Identifier &&
         IsObjc3UpdateOperatorToken(tokens[index + 1u].kind);
}

bool IsObjc3PrefixUpdateStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return index + 1u < tokens.size() &&
         IsObjc3UpdateOperatorToken(tokens[index].kind) &&
         tokens[index + 1u].kind == Objc3LexTokenKind::Identifier;
}

}  // namespace objc3c::parse
