  std::unique_ptr<Expr> ParseKeyPathLiteralExpression(const Token &keypath_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after @keypath"));
      return nullptr;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected key-path root identifier"));
      return nullptr;
    }
    const Token root = Advance();
    if (!Match(TokenKind::Comma)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "missing ',' after key-path root"));
      return nullptr;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected key-path component"));
      return nullptr;
    }
    std::vector<std::string> components;
    components.push_back(Advance().text);
    while (Match(TokenKind::Dot)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "expected identifier after '.' in key path"));
        return nullptr;
      }
      components.push_back(Advance().text);
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after @keypath"));
      return nullptr;
    }
    return BuildObjc3TypedKeyPathLiteralExpr(
        keypath_token, root, std::move(components));
  }

  std::unique_ptr<Expr> ParsePrimaryLiteralOrIdentifier(bool &matched_primary) {
    matched_primary = false;
    if (Match(TokenKind::Number)) {
      matched_primary = true;
      int value = 0;
      const Token &number_token = Previous();
      if (!ParseIntegerLiteralValue(number_token.text, value)) {
        diagnostics_.push_back(MakeDiag(
            number_token.line, number_token.column, "O3P103",
            "invalid numeric literal '" + number_token.text + "'"));
        return nullptr;
      }
      return BuildObjc3NumericLiteralExpr(value, number_token);
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      matched_primary = true;
      const Token &bool_token = Previous();
      return BuildObjc3BoolLiteralExpr(
          bool_token.kind == TokenKind::KwTrue, bool_token);
    }
    if (Match(TokenKind::KwNil)) {
      matched_primary = true;
      return BuildObjc3NilLiteralExpr(Previous());
    }
    if (Match(TokenKind::Caret)) {
      matched_primary = true;
      return ParseBlockLiteralExpression();
    }
    if (Match(TokenKind::KwAtKeypath)) {
      matched_primary = true;
      const Token keypath_token = Previous();
      return ParseKeyPathLiteralExpression(keypath_token);
    }
    if (Match(TokenKind::Identifier)) {
      matched_primary = true;
      const Token &identifier = Previous();
      return BuildObjc3IdentifierExpr(identifier.text, identifier);
    }
    return nullptr;
  }
