  bool ParseResourceLocalStorageSugarPayload(LetStmt &stmt) {
    if (!Match(TokenKind::LParen)) {
      return RejectMissingResourceSugarOpenParen();
    }
    if (!At(TokenKind::Identifier)) {
      return RejectMissingResourceSugarCleanupIdentifier();
    }
    stmt.resource_close_symbol = Advance().text;
    if (!Match(TokenKind::Comma)) {
      return RejectMissingResourceSugarCleanupSeparator();
    }
    if (!At(TokenKind::Identifier) || Peek().text != "invalid") {
      return RejectMissingResourceSugarInvalidClause();
    }
    Advance();
    if (!Match(TokenKind::Colon)) {
      return RejectMissingResourceSugarInvalidSeparator();
    }
    const std::string invalid_expression = ParseAttributeArgumentText();
    if (invalid_expression.empty()) {
      return RejectEmptyResourceSugarInvalidExpression();
    }
    stmt.resource_invalid_expression = invalid_expression;
    if (!Match(TokenKind::RParen)) {
      return RejectMissingResourceSugarCloseParen();
    }
    PublishResourceLocalStorageSugarProfile(stmt);
    return true;
  }
