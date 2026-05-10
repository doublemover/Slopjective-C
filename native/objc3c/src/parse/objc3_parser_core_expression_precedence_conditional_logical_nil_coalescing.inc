  std::unique_ptr<Expr> ParseNilCoalescing() {
    auto expr = ParseLogicalOr();
    if (expr == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::QuestionQuestion)) {
      return expr;
    }
    const Token op = Previous();
    auto rhs = ParseNilCoalescing();
    if (rhs == nullptr) {
      return nullptr;
    }
    return BuildObjc3BinaryExpr(op, std::move(expr), std::move(rhs));
  }
