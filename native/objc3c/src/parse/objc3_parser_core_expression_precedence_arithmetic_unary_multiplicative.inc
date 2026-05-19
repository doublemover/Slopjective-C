  std::unique_ptr<Expr> ParseMultiplicative() {
    auto expr = ParseUnary();
    while (expr != nullptr &&
           (At(TokenKind::Star) || At(TokenKind::Slash) ||
            At(TokenKind::Percent))) {
      const Token op = Advance();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }
