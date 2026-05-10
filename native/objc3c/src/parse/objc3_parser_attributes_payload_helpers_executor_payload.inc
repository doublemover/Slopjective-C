  template <typename TCallableDecl>
  bool ParseExecutorAttributePayload(
      TCallableDecl &decl,
      const Token &attribute_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P287",
          "missing '(' after objc_executor attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P288",
          "invalid objc_executor payload"));
      return false;
    }

    const Token kind = Advance();
    decl.executor_affinity_declared = true;
    decl.executor_affinity_kind = kind.text;
    decl.executor_affinity_named = false;
    decl.executor_affinity_name.clear();

    if (kind.text == "main" || kind.text == "global") {
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P289",
            "missing ')' after objc_executor payload"));
        return false;
      }
      return true;
    }

    if (kind.text != "named") {
      diagnostics_.push_back(MakeDiag(
          attribute_token.line, attribute_token.column, "O3P290",
          "unsupported objc_executor payload '" + kind.text + "'"));
      return false;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P291",
          "missing '(' after objc_executor named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P292",
          "objc_executor named payload requires string literal"));
      return false;
    }
    decl.executor_affinity_name = Advance().text;
    decl.executor_affinity_named = true;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P293",
          "missing '))' after objc_executor named payload"));
      return false;
    }
    return true;
  }
