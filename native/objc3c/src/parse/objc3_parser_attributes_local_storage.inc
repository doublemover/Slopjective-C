  bool ParseLocalCleanupAttribute(LetStmt &stmt, const Token &) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P301",
          "missing '(' after cleanup"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P302",
          "cleanup requires cleanup function identifier"));
      return false;
    }
    stmt.cleanup_function_symbol = Advance().text;
    if (!Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P303",
          "missing ')))' after cleanup local annotation"));
      return false;
    }
    stmt.cleanup_attribute_declared = true;
    stmt.cleanup_profile_is_normalized = true;
    stmt.cleanup_profile =
        BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
    return true;
  }

  bool ParseLocalResourceAttribute(
      LetStmt &stmt,
      const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P304",
          "missing '(' after objc_resource"));
      return false;
    }
    bool saw_close = false;
    bool saw_invalid = false;
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P305",
            "invalid objc_resource clause label"));
        return false;
      }
      const Token clause = Advance();
      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P306",
            "missing '=' after objc_resource clause label"));
        return false;
      }
      const std::string value_text = ParseAttributeArgumentText();
      if (value_text.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P307",
            "objc_resource clause value must not be empty"));
        return false;
      }
      if (clause.text == "close") {
        if (saw_close) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P308",
              "duplicate objc_resource close clause"));
          return false;
        }
        stmt.resource_close_symbol = value_text;
        saw_close = true;
      } else if (clause.text == "invalid") {
        if (saw_invalid) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P309",
              "duplicate objc_resource invalid clause"));
          return false;
        }
        stmt.resource_invalid_expression = value_text;
        saw_invalid = true;
      } else {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P310",
            "unsupported objc_resource clause '" + clause.text + "'"));
        return false;
      }
      if (At(TokenKind::Comma)) {
        Advance();
      } else {
        break;
      }
    }
    if (!Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen) ||
        !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P311",
          "missing ')))' after objc_resource local annotation"));
      return false;
    }
    if (!saw_close || !saw_invalid) {
      diagnostics_.push_back(MakeDiag(
          attribute_name.line, attribute_name.column, "O3P312",
          "objc_resource requires close and invalid clauses"));
      return false;
    }
    stmt.resource_attribute_declared = true;
    stmt.resource_profile_is_normalized = true;
    stmt.resource_profile = BuildResourceAttributeProfile(
        true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
    return true;
  }
