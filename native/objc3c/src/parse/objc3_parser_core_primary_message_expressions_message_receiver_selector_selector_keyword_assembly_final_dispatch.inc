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
