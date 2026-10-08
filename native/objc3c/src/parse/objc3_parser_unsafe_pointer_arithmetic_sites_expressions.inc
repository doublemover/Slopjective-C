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
  case Expr::Kind::CollectionLiteral:
    for (const auto &key : expr->collection_keys) {
      CollectPointerArithmeticExprSites(key.get(), sites);
    }
    for (const auto &value : expr->collection_values) {
      CollectPointerArithmeticExprSites(value.get(), sites);
    }
    return;
  case Expr::Kind::IndexAccess:
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    return;
  case Expr::Kind::Call:
  case Expr::Kind::Try:
  case Expr::Kind::Throw:
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
  case Expr::Kind::StringInterpolation:
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
    return;
  case Expr::Kind::MatchExpression:
    CollectPointerArithmeticExprSites(expr->match_expression_scrutinee.get(),
                                      sites);
    for (const auto &arm : expr->match_expression_arms) {
      CollectPointerArithmeticExprSites(arm.guard_condition.get(), sites);
      CollectPointerArithmeticExprSites(arm.value.get(), sites);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::KeyPathLiteral:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  case Expr::Kind::StringLiteral:
  default:
    return;
  }
}
