  std::string ParseAttributeArgumentText() {
    std::string text;
    while (!At(TokenKind::Eof) &&
           !At(TokenKind::Comma) &&
           !At(TokenKind::RParen)) {
      text += Advance().text;
    }
    return text;
  }
