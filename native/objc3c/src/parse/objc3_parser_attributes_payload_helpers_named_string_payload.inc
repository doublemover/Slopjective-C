  bool ParseNamedStringAttributePayload(
      const Token &attribute_name,
      const char *attribute_spelling,
      std::string &named_value_out) {
    (void)attribute_name;
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P340",
          std::string("missing '(' after ") + attribute_spelling + " attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "named") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P341",
          std::string(attribute_spelling) + " requires named(\"...\") payload"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P342",
          std::string("missing '(' after ") + attribute_spelling + " named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P343",
          std::string(attribute_spelling) + " named payload requires string literal"));
      return false;
    }
    named_value_out = Advance().text;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P344",
          std::string("missing '))' after ") + attribute_spelling + " named payload"));
      return false;
    }
    return true;
  }
