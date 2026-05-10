  bool MatchBlockLiteralParameterListOpening() {
    return Match(TokenKind::LParen);
  }

  bool AtBlockLiteralParameterListClosing() {
    return At(TokenKind::RParen);
  }

  bool ConsumeBlockLiteralParameterListClosing() {
    if (!Match(TokenKind::RParen)) {
      return ReportMissingBlockLiteralParameterListClosing();
    }
    return true;
  }
