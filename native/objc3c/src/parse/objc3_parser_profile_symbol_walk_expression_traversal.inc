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
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::KeyPathLiteral:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}
