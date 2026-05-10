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
