  std::unique_ptr<Expr> ParseConditional() {
    auto expr = ParseNilCoalescing();
    if (expr == nullptr) {
      return nullptr;
    }
    // Part 3 source-closure anchor: Question currently feeds only
    // conditional expressions here and nullability suffix parsing elsewhere.
    // Optional chaining and nil-coalescing are still fail-closed until A002/C002.
    if (!Match(TokenKind::Question)) {
      return expr;
    }

    const Token question = Previous();
    auto when_true = ParseExpression();
    if (when_true == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P107",
          "missing ':' in conditional expression"));
      return nullptr;
    }
    auto when_false = ParseConditional();
    if (when_false == nullptr) {
      return nullptr;
    }

    return BuildObjc3ConditionalExpr(
        question, std::move(expr), std::move(when_true), std::move(when_false));
  }
