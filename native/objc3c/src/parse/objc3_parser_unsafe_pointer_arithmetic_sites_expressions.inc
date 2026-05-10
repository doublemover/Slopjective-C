void CollectPointerArithmeticExprSites(const Expr *expr, std::size_t &sites) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
  case Expr::Kind::Binary:
    if (expr->op == "+" || expr->op == "-") {
      sites += 1u;
    }
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    return;
  case Expr::Kind::Conditional:
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    CollectPointerArithmeticExprSites(expr->third.get(), sites);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectPointerArithmeticExprSites(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}
