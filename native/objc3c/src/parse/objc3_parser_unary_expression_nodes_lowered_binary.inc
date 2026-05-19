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
