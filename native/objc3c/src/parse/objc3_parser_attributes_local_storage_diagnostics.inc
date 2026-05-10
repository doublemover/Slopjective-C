  bool RejectLocalStorageAttributeAt(
      const Token &token,
      const char *code,
      const std::string &message) {
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, code, message));
    return false;
  }
