  bool ParseLocalResourceAttribute(
      LetStmt &stmt,
      const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      return RejectLocalStorageAttributeAt(
          token, "O3P304", "missing '(' after objc_resource");
    }
    bool saw_close = false;
    bool saw_invalid = false;
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P305", "invalid objc_resource clause label");
      }
      const Token clause = Advance();
      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P306",
            "missing '=' after objc_resource clause label");
      }
      const std::string value_text = ParseAttributeArgumentText();
      if (value_text.empty()) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P307",
            "objc_resource clause value must not be empty");
      }
      if (clause.text == "close") {
        if (saw_close) {
          return RejectLocalStorageAttributeAt(
              clause, "O3P308", "duplicate objc_resource close clause");
        }
        stmt.resource_close_symbol = value_text;
        saw_close = true;
      } else if (clause.text == "invalid") {
        if (saw_invalid) {
          return RejectLocalStorageAttributeAt(
              clause, "O3P309", "duplicate objc_resource invalid clause");
        }
        stmt.resource_invalid_expression = value_text;
        saw_invalid = true;
      } else {
        return RejectLocalStorageAttributeAt(
            clause, "O3P310",
            "unsupported objc_resource clause '" + clause.text + "'");
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
      return RejectLocalStorageAttributeAt(
          token, "O3P311",
          "missing ')))' after objc_resource local annotation");
    }
    if (!saw_close || !saw_invalid) {
      return RejectLocalStorageAttributeAt(
          attribute_name, "O3P312",
          "objc_resource requires close and invalid clauses");
    }
    stmt.resource_attribute_declared = true;
    stmt.resource_profile_is_normalized = true;
    stmt.resource_profile = BuildResourceAttributeProfile(
        true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
    return true;
  }
