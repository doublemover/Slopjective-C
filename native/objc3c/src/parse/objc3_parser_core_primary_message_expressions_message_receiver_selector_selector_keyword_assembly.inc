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

  void PublishUnaryMessageSelectorHead(
      Expr &message, const Expr::MessageSendSelectorPiece &head_piece) {
    message.message_send_form = Expr::MessageSendForm::Unary;
    message.selector_lowering_pieces.push_back(head_piece);
  }

  void PublishKeywordMessageSelectorHead(
      Expr &message, Expr::MessageSendSelectorPiece head_piece) {
    message.message_send_form = Expr::MessageSendForm::Keyword;
    head_piece.has_argument = true;
    message.selector_lowering_pieces.push_back(head_piece);
    message.selector += ":";
  }

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

  bool ParseMessageSelectorAndArguments(Expr &message) {
    Expr::MessageSendSelectorPiece head_piece;
    if (!ParseMessageSelectorHead(message, head_piece)) {
      return false;
    }

    if (Match(TokenKind::Colon)) {
      PublishKeywordMessageSelectorHead(message, head_piece);
      if (!ParseMessageSelectorArgumentExpression(message)) {
        return false;
      }
      if (!ParseAdditionalMessageSelectorKeywords(message)) {
        return false;
      }
    } else {
      PublishUnaryMessageSelectorHead(message, head_piece);
    }
    FinalizeMessageSelectorLoweringMetadata(message);
    return true;
  }
