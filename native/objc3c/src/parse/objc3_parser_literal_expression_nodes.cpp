#include "parse/objc3_parser_expression_nodes.h"

#include <utility>

#include "parse/objc3_parser_profile_helpers.h"

namespace objc3c::parse {

std::unique_ptr<Expr> BuildObjc3NumericLiteralExpr(
    int value,
    const Objc3LexToken &token) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::Number;
  literal->number = value;
  literal->line = token.line;
  literal->column = token.column;
  return literal;
}

std::unique_ptr<Expr> BuildObjc3BoolLiteralExpr(
    bool value,
    const Objc3LexToken &token) {
  return BuildObjc3BoolLiteralExprAt(value, token.line, token.column);
}

std::unique_ptr<Expr> BuildObjc3BoolLiteralExprAt(
    bool value,
    unsigned line,
    unsigned column) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::BoolLiteral;
  literal->bool_value = value;
  literal->line = line;
  literal->column = column;
  return literal;
}

std::unique_ptr<Expr> BuildObjc3NilLiteralExpr(const Objc3LexToken &token) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::NilLiteral;
  literal->line = token.line;
  literal->column = token.column;
  return literal;
}

std::unique_ptr<Expr> BuildObjc3IdentifierExpr(
    const std::string &identifier,
    const Objc3LexToken &token) {
  auto expr = std::make_unique<Expr>();
  expr->kind = Expr::Kind::Identifier;
  expr->line = token.line;
  expr->column = token.column;
  expr->ident = identifier;
  return expr;
}

std::unique_ptr<Expr> BuildObjc3TypedKeyPathLiteralExpr(
    const Objc3LexToken &keypath_token,
    const Objc3LexToken &root_token,
    std::vector<std::string> components) {
  auto expr = std::make_unique<Expr>();
  expr->kind = Expr::Kind::Identifier;
  expr->ident = "__objc3_keypath_literal";
  expr->line = keypath_token.line;
  expr->column = keypath_token.column;
  expr->typed_keypath_literal_enabled = true;
  expr->typed_keypath_root_is_self = root_token.text == "self";
  expr->typed_keypath_root_name = root_token.text;
  expr->typed_keypath_components = std::move(components);
  expr->typed_keypath_literal_profile = BuildTypedKeyPathLiteralProfile(
      expr->typed_keypath_root_name,
      expr->typed_keypath_root_is_self,
      expr->typed_keypath_components);
  expr->typed_keypath_literal_is_normalized =
      !expr->typed_keypath_components.empty();
  return expr;
}

}  // namespace objc3c::parse
