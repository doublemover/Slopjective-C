  bool ParseLocalStorageSugar(LetStmt &stmt) {
    if (Match(TokenKind::KwAtCleanup)) {
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P317", "missing '(' after @cleanup");
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P318", "@cleanup requires cleanup function identifier");
      }
      stmt.cleanup_function_symbol = Advance().text;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P319", "missing ')' after @cleanup payload");
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
        return RejectLocalStorageAttributeAt(
            token, "O3P320", "missing '(' after @resource");
      }
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P321", "@resource requires cleanup function identifier");
      }
      stmt.resource_close_symbol = Advance().text;
      if (!Match(TokenKind::Comma)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P322",
            "missing ',' after @resource cleanup function");
      }
      if (!At(TokenKind::Identifier) || Peek().text != "invalid") {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P323", "@resource requires invalid clause");
      }
      Advance();
      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P324", "missing ':' after invalid in @resource");
      }
      const std::string invalid_expression = ParseAttributeArgumentText();
      if (invalid_expression.empty()) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P325",
            "@resource invalid expression must not be empty");
      }
      stmt.resource_invalid_expression = invalid_expression;
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        return RejectLocalStorageAttributeAt(
            token, "O3P326", "missing ')' after @resource payload");
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
