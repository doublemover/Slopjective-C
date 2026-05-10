  template <typename TCallableDecl>
  bool ParseStatusCodeBridgeAttributePayload(
      TCallableDecl &decl,
      const Token &attribute_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P271",
          "missing '(' after objc_status_code attribute"));
      return false;
    }

    bool saw_success = false;
    bool saw_error_type = false;
    bool saw_mapping = false;
    while (!At(TokenKind::Eof) && !At(TokenKind::RParen)) {
      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P272",
            "invalid objc_status_code clause label"));
        return false;
      }
      const Token clause = Advance();
      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P273",
            "missing ':' after objc_status_code clause label"));
        return false;
      }
      const std::string value_text = ParseAttributeArgumentText();
      if (value_text.empty()) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P274",
            "objc_status_code clause value must not be empty"));
        return false;
      }

      if (clause.text == "success") {
        if (saw_success) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P275",
              "duplicate objc_status_code success clause"));
          return false;
        }
        decl.objc_status_code_success_literal = value_text;
        saw_success = true;
      } else if (clause.text == "error_type") {
        if (saw_error_type) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P276",
              "duplicate objc_status_code error_type clause"));
          return false;
        }
        decl.objc_status_code_error_type_spelling = value_text;
        saw_error_type = true;
      } else if (clause.text == "mapping") {
        if (saw_mapping) {
          diagnostics_.push_back(MakeDiag(
              clause.line, clause.column, "O3P277",
              "duplicate objc_status_code mapping clause"));
          return false;
        }
        decl.objc_status_code_mapping_symbol = value_text;
        saw_mapping = true;
      } else {
        diagnostics_.push_back(MakeDiag(
            clause.line, clause.column, "O3P278",
            "unsupported objc_status_code clause '" + clause.text + "'"));
        return false;
      }

      if (At(TokenKind::Comma)) {
        Advance();
      } else {
        break;
      }
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P279",
          "missing ')' after objc_status_code attribute payload"));
      return false;
    }

    if (!saw_success || !saw_error_type || !saw_mapping) {
      diagnostics_.push_back(MakeDiag(
          attribute_token.line, attribute_token.column, "O3P280",
          "objc_status_code requires success, error_type, and mapping clauses"));
      return false;
    }

    decl.objc_status_code_declared = true;
    return true;
  }
