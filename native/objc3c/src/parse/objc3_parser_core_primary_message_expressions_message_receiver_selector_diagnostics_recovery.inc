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

  bool ReportMissingMessageSelectorIdentifier() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                    "expected selector identifier in message send"));
    return false;
  }

  bool ReportMissingKeywordSelectorColon(const Token &keyword) {
    diagnostics_.push_back(MakeDiag(keyword.line, keyword.column, "O3P113",
                                    "missing ':' in keyword selector segment"));
    return false;
  }
