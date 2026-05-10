  void ConfigurePostfixOptionalMemberSelector(
      Expr &message, const Token &member) {
    message.selector = member.text;
    message.message_send_form = Expr::MessageSendForm::Unary;
    Expr::MessageSendSelectorPiece head_piece;
    head_piece.keyword = member.text;
    head_piece.has_argument = false;
    head_piece.line = member.line;
    head_piece.column = member.column;
    message.selector_lowering_pieces.push_back(head_piece);
    message.message_send_form_symbol =
        BuildMessageSendFormSymbol(message.message_send_form);
    message.selector_lowering_symbol =
        BuildMessageSendSelectorLoweringSymbol(
            message.selector_lowering_pieces);
    message.selector_lowering_is_normalized = true;
  }

  std::unique_ptr<Expr> BuildPostfixOptionalMemberMessage(
      std::unique_ptr<Expr> receiver,
      const Token &access,
      const Token &member) {
    auto message = std::make_unique<Expr>();
    message->kind = Expr::Kind::MessageSend;
    message->line = access.line;
    message->column = access.column;
    message->receiver = std::move(receiver);
    message->optional_send_enabled = true;
    message->optional_member_access_enabled = true;
    message->optional_send_symbol = BuildOptionalSendSymbol(true);
    message->optional_send_is_normalized = true;
    ConfigurePostfixOptionalMemberSelector(*message, member);
    ConfigurePostfixOptionalMemberDispatchAbi(*message);
    ConfigurePostfixOptionalMemberNilReceiverSemantics(*message);
    return message;
  }

  std::unique_ptr<Expr> ParsePostfixOptionalMemberAccess(
      std::unique_ptr<Expr> receiver,
      const Token &access) {
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P103",
          "expected identifier after '?.'"));
      return nullptr;
    }
    const Token member = Advance();
    return BuildPostfixOptionalMemberMessage(
        std::move(receiver), access, member);
  }
