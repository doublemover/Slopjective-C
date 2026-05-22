std::size_t CountFailableCallSitesInExpr(const Expr *expr) {
  if (expr == nullptr) {
    return 0;
  }
  switch (expr->kind) {
  case Expr::Kind::Call: {
    std::size_t count = IsFailableCallSymbol(expr->ident) ? 1u : 0u;
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::Try:
  case Expr::Kind::Throw: {
    std::size_t count = 0;
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::MessageSend: {
    std::size_t count = IsFailableCallSymbol(expr->selector) ? 1u : 0u;
    count += CountFailableCallSitesInExpr(expr->receiver.get());
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::Binary:
    return CountFailableCallSitesInExpr(expr->left.get()) +
           CountFailableCallSitesInExpr(expr->right.get());
  case Expr::Kind::Conditional:
    return CountFailableCallSitesInExpr(expr->left.get()) +
           CountFailableCallSitesInExpr(expr->right.get()) +
           CountFailableCallSitesInExpr(expr->third.get());
  case Expr::Kind::CollectionLiteral: {
    std::size_t count = 0;
    for (const auto &key : expr->collection_keys) {
      count += CountFailableCallSitesInExpr(key.get());
    }
    for (const auto &value : expr->collection_values) {
      count += CountFailableCallSitesInExpr(value.get());
    }
    return count;
  }
  case Expr::Kind::IndexAccess:
    return CountFailableCallSitesInExpr(expr->left.get()) +
           CountFailableCallSitesInExpr(expr->right.get());
  case Expr::Kind::StringInterpolation: {
    std::size_t count = 0;
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::KeyPathLiteral:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::StringLiteral:
  default:
    return 0;
  }
}
