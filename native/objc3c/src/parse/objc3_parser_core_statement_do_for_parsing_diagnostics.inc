    auto emit_do_for_diagnostic = [this](const Token &token, const char *code,
                                         const char *message) {
      diagnostics_.push_back(MakeDiag(token.line, token.column, code, message));
    };
