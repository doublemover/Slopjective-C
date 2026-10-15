void WalkObjc3ProfileSymbolsInExpr(
    const Expr *expr,
    const Objc3ProfileSymbolWalker &walker) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    walker.Visit(expr->ident);
    for (const auto &arg : expr->args) {
      WalkObjc3ProfileSymbolsInExpr(arg.get(), walker);
    }
    return;
  case Expr::Kind::Try:
  case Expr::Kind::Throw:
    for (const auto &arg : expr->args) {
      WalkObjc3ProfileSymbolsInExpr(arg.get(), walker);
    }
    return;
  case Expr::Kind::MessageSend:
    walker.Visit(expr->selector);
    WalkObjc3ProfileSymbolsInExpr(expr->receiver.get(), walker);
    for (const auto &arg : expr->args) {
      WalkObjc3ProfileSymbolsInExpr(arg.get(), walker);
    }
    return;
  case Expr::Kind::Binary:
    WalkObjc3ProfileSymbolsInExpr(expr->left.get(), walker);
    WalkObjc3ProfileSymbolsInExpr(expr->right.get(), walker);
    return;
  case Expr::Kind::Conditional:
    WalkObjc3ProfileSymbolsInExpr(expr->left.get(), walker);
    WalkObjc3ProfileSymbolsInExpr(expr->right.get(), walker);
    WalkObjc3ProfileSymbolsInExpr(expr->third.get(), walker);
    return;
  case Expr::Kind::CollectionLiteral:
    for (const auto &key : expr->collection_keys) {
      WalkObjc3ProfileSymbolsInExpr(key.get(), walker);
    }
    for (const auto &value : expr->collection_values) {
      WalkObjc3ProfileSymbolsInExpr(value.get(), walker);
    }
    return;
  case Expr::Kind::IndexAccess:
    WalkObjc3ProfileSymbolsInExpr(expr->left.get(), walker);
    WalkObjc3ProfileSymbolsInExpr(expr->right.get(), walker);
    return;
  case Expr::Kind::StringInterpolation:
    for (const auto &arg : expr->args) {
      WalkObjc3ProfileSymbolsInExpr(arg.get(), walker);
    }
    return;
  case Expr::Kind::MatchExpression:
    WalkObjc3ProfileSymbolsInExpr(
        expr->match_expression_scrutinee.get(), walker);
    for (const auto &arm : expr->match_expression_arms) {
      WalkObjc3ProfileSymbolsInExpr(arm.guard_condition.get(), walker);
      WalkObjc3ProfileSymbolsInExpr(arm.value.get(), walker);
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
