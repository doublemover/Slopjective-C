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
