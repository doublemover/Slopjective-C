#include "parse/objc3_parser_expression_nodes.h"

#include <utility>

namespace objc3c::parse {

std::unique_ptr<Expr> BuildObjc3BinaryExpr(
    const Objc3LexToken &op,
    std::unique_ptr<Expr> lhs,
    std::unique_ptr<Expr> rhs) {
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::Binary;
  node->op = op.text;
  node->line = op.line;
  node->column = op.column;
  node->left = std::move(lhs);
  node->right = std::move(rhs);
  return node;
}

std::unique_ptr<Expr> BuildObjc3ConditionalExpr(
    const Objc3LexToken &question,
    std::unique_ptr<Expr> condition,
    std::unique_ptr<Expr> when_true,
    std::unique_ptr<Expr> when_false) {
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::Conditional;
  node->line = question.line;
  node->column = question.column;
  node->left = std::move(condition);
  node->right = std::move(when_true);
  node->third = std::move(when_false);
  return node;
}

std::unique_ptr<Expr> BuildObjc3MatchExpression(
    const Objc3LexToken &match_token,
    std::unique_ptr<Expr> scrutinee,
    std::vector<Expr::MatchExpressionArm> arms) {
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::MatchExpression;
  node->line = match_token.line;
  node->column = match_token.column;
  node->match_expression_scrutinee = std::move(scrutinee);
  node->match_expression_arms = std::move(arms);
  return node;
}

std::unique_ptr<Expr> BuildObjc3CollectionLiteralExpr(
    Expr::CollectionLiteralKind kind,
    const Objc3LexToken &token,
    std::vector<std::unique_ptr<Expr>> keys,
    std::vector<std::unique_ptr<Expr>> values) {
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::CollectionLiteral;
  node->collection_literal_kind = kind;
  node->line = token.line;
  node->column = token.column;
  node->collection_keys = std::move(keys);
  node->collection_values = std::move(values);
  return node;
}

std::unique_ptr<Expr> BuildObjc3IndexAccessExpr(
    const Objc3LexToken &open,
    std::unique_ptr<Expr> collection,
    std::unique_ptr<Expr> index) {
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::IndexAccess;
  node->line = open.line;
  node->column = open.column;
  node->left = std::move(collection);
  node->right = std::move(index);
  return node;
}

}  // namespace objc3c::parse
