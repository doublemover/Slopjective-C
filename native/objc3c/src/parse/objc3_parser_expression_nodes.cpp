#include "parse/objc3_parser_expression_nodes.h"

#include <sstream>
#include <utility>

namespace objc3c::parse {
namespace {

const char *TryOperatorKindSpelling(Expr::TryOperatorKind kind) {
  switch (kind) {
  case Expr::TryOperatorKind::Propagate:
    return "try";
  case Expr::TryOperatorKind::Optional:
    return "try?";
  case Expr::TryOperatorKind::Forced:
    return "try!";
  case Expr::TryOperatorKind::None:
  default:
    return "none";
  }
}

std::string BuildObjc3TryExpressionProfile(const Expr &expr) {
  std::ostringstream out;
  out << "try-expression:kind="
      << TryOperatorKindSpelling(expr.try_operator_kind)
      << ";requires_throwing_context="
      << (expr.try_expression_requires_throwing_context ? "true" : "false")
      << ";normalized="
      << (expr.try_expression_is_normalized ? "true" : "false");
  return out.str();
}

}  // namespace

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

std::unique_ptr<Expr> BuildObjc3UnaryLoweringBinaryExpr(
    const Objc3LexToken &op,
    const std::string &lowered_binary_op,
    int synthetic_lhs_value,
    std::unique_ptr<Expr> rhs) {
  auto synthetic_lhs = BuildObjc3NumericLiteralExpr(synthetic_lhs_value, op);
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::Binary;
  node->op = lowered_binary_op;
  node->line = op.line;
  node->column = op.column;
  node->left = std::move(synthetic_lhs);
  node->right = std::move(rhs);
  return node;
}

std::unique_ptr<Expr> BuildObjc3LogicalNotExpr(
    const Objc3LexToken &op,
    std::unique_ptr<Expr> rhs) {
  auto zero = BuildObjc3NumericLiteralExpr(0, op);
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::Binary;
  node->op = "==";
  node->line = op.line;
  node->column = op.column;
  node->left = std::move(rhs);
  node->right = std::move(zero);
  return node;
}

std::unique_ptr<Expr> BuildObjc3BitwiseNotExpr(
    const Objc3LexToken &op,
    std::unique_ptr<Expr> rhs) {
  auto minus_one = BuildObjc3NumericLiteralExpr(-1, op);
  auto node = std::make_unique<Expr>();
  node->kind = Expr::Kind::Binary;
  node->op = "^";
  node->line = op.line;
  node->column = op.column;
  node->left = std::move(rhs);
  node->right = std::move(minus_one);
  return node;
}

std::unique_ptr<Expr> BuildObjc3TryExpression(
    const Objc3LexToken &try_token,
    Expr::TryOperatorKind try_kind,
    bool requires_throwing_context,
    std::unique_ptr<Expr> rhs) {
  auto expr = std::make_unique<Expr>();
  expr->kind = Expr::Kind::Call;
  expr->ident = "__objc3_try_expr";
  expr->line = try_token.line;
  expr->column = try_token.column;
  expr->try_expression_enabled = true;
  expr->try_operator_kind = try_kind;
  expr->try_expression_requires_throwing_context = requires_throwing_context;
  expr->try_expression_is_normalized = true;
  expr->args.push_back(std::move(rhs));
  expr->try_expression_profile = BuildObjc3TryExpressionProfile(*expr);
  return expr;
}

}  // namespace objc3c::parse
