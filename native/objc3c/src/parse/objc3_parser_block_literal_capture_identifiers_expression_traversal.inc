void CollectBlockLiteralExprIdentifiers(
    const Expr *expr,
    std::vector<std::string> &used_identifiers) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Identifier:
    if (!expr->ident.empty()) {
      used_identifiers.push_back(expr->ident);
    }
    return;
  case Expr::Kind::Binary:
    CollectBlockLiteralExprIdentifiers(expr->left.get(), used_identifiers);
    CollectBlockLiteralExprIdentifiers(expr->right.get(), used_identifiers);
    return;
  case Expr::Kind::Conditional:
    CollectBlockLiteralExprIdentifiers(expr->left.get(), used_identifiers);
    CollectBlockLiteralExprIdentifiers(expr->right.get(), used_identifiers);
    CollectBlockLiteralExprIdentifiers(expr->third.get(), used_identifiers);
    return;
  case Expr::Kind::Call:
  case Expr::Kind::Try:
  case Expr::Kind::Throw:
    for (const auto &arg : expr->args) {
      CollectBlockLiteralExprIdentifiers(arg.get(), used_identifiers);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectBlockLiteralExprIdentifiers(expr->receiver.get(), used_identifiers);
    for (const auto &arg : expr->args) {
      CollectBlockLiteralExprIdentifiers(arg.get(), used_identifiers);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::KeyPathLiteral:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::StringLiteral:
  default:
    return;
  }
}
