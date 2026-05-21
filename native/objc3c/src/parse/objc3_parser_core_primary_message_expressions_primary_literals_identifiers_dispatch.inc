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
    if (Match(TokenKind::String)) {
      matched_primary = true;
      const Token &string_token = Previous();
      std::string value;
      int unit_count = 0;
      if (!TryDecodeObjc3StringTokenText(string_token.text, value) ||
          !TryCountObjc3Utf8Scalars(value, unit_count)) {
        diagnostics_.push_back(MakeDiag(
            string_token.line, string_token.column, "O3P104",
            "invalid canonical string literal token"));
        return nullptr;
      }
      return BuildObjc3StringLiteralExpr(
          value, static_cast<int>(value.size()), unit_count, string_token);
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
