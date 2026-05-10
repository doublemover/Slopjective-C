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
