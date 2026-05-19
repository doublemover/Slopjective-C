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
