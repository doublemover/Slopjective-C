    const Token expr_anchor = Peek();
    auto expr = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::ExpressionSite);
    if (expr == nullptr) {
      return nullptr;
    }
    auto stmt = BuildObjc3ExpressionStatement(expr_anchor, std::move(expr));
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          BuildObjc3MissingSemicolonDiagnostic(token, "expression"));
      return nullptr;
    }
    return stmt;
