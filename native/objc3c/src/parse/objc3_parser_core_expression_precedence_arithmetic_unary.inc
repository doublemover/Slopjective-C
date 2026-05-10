  std::unique_ptr<Expr> ParseAdditive() {
    auto expr = ParseMultiplicative();
    while (expr != nullptr && (At(TokenKind::Plus) || At(TokenKind::Minus))) {
      const Token op = Advance();
      auto rhs = ParseMultiplicative();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

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

  std::unique_ptr<Expr> ParseUnary() {
    if (Match(TokenKind::KwTry)) {
      const Token try_token = Previous();
      Expr::TryOperatorKind try_kind = Expr::TryOperatorKind::Propagate;
      bool requires_throwing_context = true;
      if (Match(TokenKind::Question)) {
        try_kind = Expr::TryOperatorKind::Optional;
        requires_throwing_context = false;
      } else if (Match(TokenKind::Bang)) {
        try_kind = Expr::TryOperatorKind::Forced;
        requires_throwing_context = false;
      }
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3TryExpression(
          try_token, try_kind, requires_throwing_context, std::move(rhs));
    }
    if (Match(TokenKind::KwAwait)) {
      const Token await_token = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      rhs->await_expression_enabled = true;
      if (rhs->line == 0u) {
        rhs->line = await_token.line;
      }
      if (rhs->column == 0u) {
        rhs->column = await_token.column;
      }
      return rhs;
    }
    if (Match(TokenKind::Bang)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3LogicalNotExpr(op, std::move(rhs));
    }
    if (Match(TokenKind::Plus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3UnaryLoweringBinaryExpr(op, "+", 0, std::move(rhs));
    }
    if (Match(TokenKind::Minus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3UnaryLoweringBinaryExpr(op, "-", 0, std::move(rhs));
    }
    if (Match(TokenKind::Tilde)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      return BuildObjc3BitwiseNotExpr(op, std::move(rhs));
    }
    return ParsePostfix();
  }
