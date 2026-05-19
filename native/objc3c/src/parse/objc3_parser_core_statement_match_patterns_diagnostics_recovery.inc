  bool ReportUnsupportedTypeTestMatchPatternIfPresent() {
    if (At(TokenKind::Identifier) && Peek().text == "is") {
      const Token token = Advance();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P158", "unsupported type-test match pattern"));
      return true;
    }
    return false;
  }

  void ReportInvalidMatchPattern() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid match pattern"));
  }
