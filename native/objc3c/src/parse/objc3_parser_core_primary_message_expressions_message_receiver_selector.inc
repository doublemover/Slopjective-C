  bool ParseMessageReceiverAndOptionalSendMetadata(Expr &message, const Token &open) {
    const std::size_t receiver_diag_count = diagnostics_.size();
    message.receiver = ParsePostfix();
    if (message.receiver == nullptr) {
      if (diagnostics_.size() == receiver_diag_count) {
        diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P113",
                                        "invalid receiver expression in message send"));
      }
      return false;
    }
    if (Match(TokenKind::Question)) {
      message.optional_send_enabled = true;
      message.optional_send_symbol = BuildOptionalSendSymbol(true);
      message.optional_send_is_normalized = true;
    } else {
      message.optional_send_symbol = BuildOptionalSendSymbol(false);
      message.optional_send_is_normalized = true;
    }
    return true;
  }

  bool RejectUnexpectedMessageSelectorQualifier(
      const char *pure_message, const char *extern_message, const char *async_message) {
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
      const Token qualifier = Advance();
      const std::string message_text =
          qualifier.kind == TokenKind::KwPure
              ? pure_message
              : qualifier.kind == TokenKind::KwExtern ? extern_message : async_message;
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
      return true;
    }
    return false;
  }

  bool ParseMessageSelectorAndArguments(Expr &message) {
    if (RejectUnexpectedMessageSelectorQualifier(
            "unexpected qualifier 'pure' in message selector position",
            "unexpected qualifier 'extern' in message selector position",
            "unexpected qualifier 'async' in message selector position")) {
      return false;
    }

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "expected selector identifier in message send"));
      return false;
    }

    const Token selector_head = Advance();
    message.selector = selector_head.text;
    Expr::MessageSendSelectorPiece head_piece;
    head_piece.keyword = selector_head.text;
    head_piece.line = selector_head.line;
    head_piece.column = selector_head.column;
    if (Match(TokenKind::Colon)) {
      message.message_send_form = Expr::MessageSendForm::Keyword;
      head_piece.has_argument = true;
      message.selector_lowering_pieces.push_back(head_piece);
      message.selector += ":";
      auto first_arg = ParseExpressionWithBlockLiteralSourceUse(
          BlockLiteralSourceUseKind::MessageArgument);
      if (first_arg == nullptr) {
        return false;
      }
      message.args.push_back(std::move(first_arg));

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
          diagnostics_.push_back(MakeDiag(keyword.line, keyword.column, "O3P113",
                                          "missing ':' in keyword selector segment"));
          return false;
        }
        Expr::MessageSendSelectorPiece keyword_piece;
        keyword_piece.keyword = keyword.text;
        keyword_piece.has_argument = true;
        keyword_piece.line = keyword.line;
        keyword_piece.column = keyword.column;
        message.selector_lowering_pieces.push_back(std::move(keyword_piece));
        message.selector += keyword.text;
        message.selector += ":";
        auto arg = ParseExpressionWithBlockLiteralSourceUse(
            BlockLiteralSourceUseKind::MessageArgument);
        if (arg == nullptr) {
          return false;
        }
        message.args.push_back(std::move(arg));
      }
    } else {
      message.message_send_form = Expr::MessageSendForm::Unary;
      message.selector_lowering_pieces.push_back(head_piece);
    }
    message.message_send_form_symbol = BuildMessageSendFormSymbol(message.message_send_form);
    message.selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message.selector_lowering_pieces);
    message.selector_lowering_is_normalized = true;
    return true;
  }
