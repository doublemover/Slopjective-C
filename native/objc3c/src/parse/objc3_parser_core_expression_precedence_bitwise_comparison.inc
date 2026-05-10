  std::unique_ptr<Expr> ParseBitwiseOr() {
    auto expr = ParseBitwiseXor();
    while (expr != nullptr && Match(TokenKind::Pipe)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseXor();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseXor() {
    auto expr = ParseBitwiseAnd();
    while (expr != nullptr && Match(TokenKind::Caret)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseAnd() {
    auto expr = ParseEquality();
    while (expr != nullptr && Match(TokenKind::Ampersand)) {
      const Token op = Previous();
      auto rhs = ParseEquality();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseEquality() {
    auto expr = ParseRelational();
    while (expr != nullptr &&
           (Match(TokenKind::EqualEqual) || Match(TokenKind::BangEqual))) {
      const Token op = Previous();
      auto rhs = ParseRelational();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseRelational() {
    auto expr = ParseShift();
    while (expr != nullptr &&
           (Match(TokenKind::Less) || Match(TokenKind::LessEqual) ||
            Match(TokenKind::Greater) || Match(TokenKind::GreaterEqual))) {
      const Token op = Previous();
      auto rhs = ParseShift();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseShift() {
    auto expr = ParseAdditive();
    while (expr != nullptr &&
           (Match(TokenKind::LessLess) || Match(TokenKind::GreaterGreater))) {
      const Token op = Previous();
      auto rhs = ParseAdditive();
      if (rhs == nullptr) {
        return nullptr;
      }
      expr = BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
    }
    return expr;
  }
