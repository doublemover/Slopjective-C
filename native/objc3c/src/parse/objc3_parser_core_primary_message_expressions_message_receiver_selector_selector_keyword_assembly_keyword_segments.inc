  bool ParseAdditionalMessageSelectorKeywords(Expr &message) {
    while (true) {
      if (RejectUnexpectedMessageSelectorQualifier(
              "unexpected qualifier 'pure' in keyword selector segment position",
              "unexpected qualifier 'extern' in keyword selector segment position",
              "unexpected qualifier 'async' in keyword selector segment position")) {
        return false;
      }
      if (!At(TokenKind::Identifier)) {
        break;
      }
      const Token keyword = Advance();
      if (!Match(TokenKind::Colon)) {
        return ReportMissingKeywordSelectorColon(keyword);
      }
      Expr::MessageSendSelectorPiece keyword_piece;
      keyword_piece.keyword = keyword.text;
      keyword_piece.has_argument = true;
      keyword_piece.line = keyword.line;
      keyword_piece.column = keyword.column;
      message.selector_lowering_pieces.push_back(std::move(keyword_piece));
      message.selector += keyword.text;
      message.selector += ":";
      if (!ParseMessageSelectorArgumentExpression(message)) {
        return false;
      }
    }
    return true;
  }
