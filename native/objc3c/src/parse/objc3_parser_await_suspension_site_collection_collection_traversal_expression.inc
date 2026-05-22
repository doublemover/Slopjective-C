void CollectAwaitSuspensionExprSites(
    const Expr *expr,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  if (expr->await_expression_enabled) {
    counts.await_keyword_sites += 1u;
    counts.await_suspension_point_sites += 1u;
    counts.await_continuation_sites += 1u;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectAwaitSuspensionSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Try:
  case Expr::Kind::Throw:
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectAwaitSuspensionSitesFromSymbol(expr->selector, counts);
    CollectAwaitSuspensionExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    CollectAwaitSuspensionExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::CollectionLiteral:
    for (const auto &key : expr->collection_keys) {
      CollectAwaitSuspensionExprSites(key.get(), counts);
    }
    for (const auto &value : expr->collection_values) {
      CollectAwaitSuspensionExprSites(value.get(), counts);
    }
    return;
  case Expr::Kind::IndexAccess:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::StringInterpolation:
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
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
