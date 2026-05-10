  bool ParseLocalStorageAttribute(LetStmt &stmt) {
    if (!Match(TokenKind::LParen) || !Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P314",
          "malformed __attribute__ local annotation"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P315",
          "missing local annotation name after __attribute__(("));
      return false;
    }
    const Token attribute_name = Advance();
    if (attribute_name.text == "cleanup") {
      return ParseLocalCleanupAttribute(stmt, attribute_name);
    }
    if (attribute_name.text == "objc_resource") {
      return ParseLocalResourceAttribute(stmt, attribute_name);
    }
    diagnostics_.push_back(MakeDiag(
        attribute_name.line, attribute_name.column, "O3P316",
        "only cleanup and objc_resource are supported on local let bindings in this tranche"));
    return false;
  }

  bool ParseLocalStorageSugar(LetStmt &stmt) {
    if (Match(TokenKind::KwAtCleanup)) {
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P317",
            "missing '(' after @cleanup"));
        return false;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P318",
            "@cleanup requires cleanup function identifier"));
        return false;
      }
      stmt.cleanup_function_symbol = Advance().text;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P319",
            "missing ')' after @cleanup payload"));
        return false;
      }
      stmt.cleanup_attribute_declared = true;
      stmt.cleanup_sugar_declared = true;
      stmt.cleanup_profile_is_normalized = true;
      stmt.cleanup_profile =
          BuildCleanupAttributeProfile(true, stmt.cleanup_function_symbol);
      return true;
    }
    if (Match(TokenKind::KwAtResource)) {
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P320",
            "missing '(' after @resource"));
        return false;
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P321",
            "@resource requires cleanup function identifier"));
        return false;
      }
      stmt.resource_close_symbol = Advance().text;
      if (!Match(TokenKind::Comma)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P322",
            "missing ',' after @resource cleanup function"));
        return false;
      }
      if (!At(TokenKind::Identifier) || Peek().text != "invalid") {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P323",
            "@resource requires invalid clause"));
        return false;
      }
      Advance();
      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P324",
            "missing ':' after invalid in @resource"));
        return false;
      }
      const std::string invalid_expression = ParseAttributeArgumentText();
      if (invalid_expression.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P325",
            "@resource invalid expression must not be empty"));
        return false;
      }
      stmt.resource_invalid_expression = invalid_expression;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P326",
            "missing ')' after @resource payload"));
        return false;
      }
      stmt.resource_attribute_declared = true;
      stmt.resource_sugar_declared = true;
      stmt.resource_profile_is_normalized = true;
      stmt.resource_profile = BuildResourceAttributeProfile(
          true, stmt.resource_close_symbol, stmt.resource_invalid_expression);
      return true;
    }
    return false;
  }
