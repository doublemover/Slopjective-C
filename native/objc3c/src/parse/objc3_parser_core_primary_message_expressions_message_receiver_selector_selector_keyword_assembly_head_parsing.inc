  bool ParseMessageSelectorHead(Expr &message, Expr::MessageSendSelectorPiece &head_piece) {
    if (RejectUnexpectedMessageSelectorQualifier(
            "unexpected qualifier 'pure' in message selector position",
            "unexpected qualifier 'extern' in message selector position",
            "unexpected qualifier 'async' in message selector position")) {
      return false;
    }

    if (!At(TokenKind::Identifier)) {
      return ReportMissingMessageSelectorIdentifier();
    }

    const Token selector_head = Advance();
    message.selector = selector_head.text;
    head_piece.keyword = selector_head.text;
    head_piece.line = selector_head.line;
    head_piece.column = selector_head.column;
    return true;
  }
