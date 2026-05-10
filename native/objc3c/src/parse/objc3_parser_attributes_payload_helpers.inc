  std::string ParseAttributeArgumentText() {
    std::string text;
    while (!At(TokenKind::Eof) &&
           !At(TokenKind::Comma) &&
           !At(TokenKind::RParen)) {
      text += Advance().text;
    }
    return text;
  }

  bool ParseNamedStringAttributePayload(
      const Token &attribute_name,
      const char *attribute_spelling,
      std::string &named_value_out) {
    (void)attribute_name;
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P340",
          std::string("missing '(' after ") + attribute_spelling + " attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "named") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P341",
          std::string(attribute_spelling) + " requires named(\"...\") payload"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P342",
          std::string("missing '(' after ") + attribute_spelling + " named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P343",
          std::string(attribute_spelling) + " named payload requires string literal"));
      return false;
    }
    named_value_out = Advance().text;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P344",
          std::string("missing '))' after ") + attribute_spelling + " named payload"));
      return false;
    }
    return true;
  }

  template <typename TCallableDecl>
  bool ParseExecutorAttributePayload(
      TCallableDecl &decl,
      const Token &attribute_token) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P287",
          "missing '(' after objc_executor attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P288",
          "invalid objc_executor payload"));
      return false;
    }

    const Token kind = Advance();
    decl.executor_affinity_declared = true;
    decl.executor_affinity_kind = kind.text;
    decl.executor_affinity_named = false;
    decl.executor_affinity_name.clear();

    if (kind.text == "main" || kind.text == "global") {
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(
            token.line, token.column, "O3P289",
            "missing ')' after objc_executor payload"));
        return false;
      }
      return true;
    }

    if (kind.text != "named") {
      diagnostics_.push_back(MakeDiag(
          attribute_token.line, attribute_token.column, "O3P290",
          "unsupported objc_executor payload '" + kind.text + "'"));
      return false;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P291",
          "missing '(' after objc_executor named payload"));
      return false;
    }
    if (!At(TokenKind::String)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P292",
          "objc_executor named payload requires string literal"));
      return false;
    }
    decl.executor_affinity_name = Advance().text;
    decl.executor_affinity_named = true;
    if (!Match(TokenKind::RParen) || !Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P293",
          "missing '))' after objc_executor named payload"));
      return false;
    }
    return true;
  }

  template <typename TCallableDecl>
  bool ParseAbiAlignmentAttributePayload(
      TCallableDecl &decl,
      const Token &attribute_name) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P361",
          "missing '(' after objc_abi_align attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "bytes") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P362",
          "objc_abi_align requires bytes: integer payload"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P363",
          "missing ':' after objc_abi_align bytes label"));
      return false;
    }
    if (!At(TokenKind::Number)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P364",
          "objc_abi_align bytes payload requires integer literal"));
      return false;
    }
    const Token bytes_token = Advance();
    std::size_t alignment = 0;
    try {
      alignment =
          static_cast<std::size_t>(std::stoull(bytes_token.text, nullptr, 0));
    } catch (...) {
      diagnostics_.push_back(MakeDiag(
          bytes_token.line, bytes_token.column, "O3P364",
          "objc_abi_align bytes payload requires integer literal"));
      return false;
    }
    if (alignment == 0u || (alignment & (alignment - 1u)) != 0u) {
      diagnostics_.push_back(MakeDiag(
          bytes_token.line, bytes_token.column, "O3P365",
          "objc_abi_align bytes payload must be a non-zero power of two"));
      return false;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P366",
          "missing ')' after objc_abi_align payload"));
      return false;
    }
    decl.objc_abi_align_declared = true;
    decl.objc_abi_alignment_bytes = alignment;
    (void)attribute_name;
    return true;
  }

  template <typename TCallableDecl>
  bool ParseReturnsBorrowedAttributePayload(TCallableDecl &decl) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P296",
          "missing '(' after objc_returns_borrowed attribute"));
      return false;
    }
    if (!At(TokenKind::Identifier) || Peek().text != "owner_index") {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P297",
          "objc_returns_borrowed requires owner_index clause"));
      return false;
    }
    Advance();
    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P298",
          "missing '=' after owner_index in objc_returns_borrowed"));
      return false;
    }
    if (!At(TokenKind::Number)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P299",
          "objc_returns_borrowed owner_index requires integer literal"));
      return false;
    }
    decl.objc_returns_borrowed_owner_index =
        static_cast<std::size_t>(std::max(0, std::stoi(Advance().text)));
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(
          token.line, token.column, "O3P300",
          "missing ')' after objc_returns_borrowed payload"));
      return false;
    }
    decl.objc_returns_borrowed_declared = true;
    return true;
  }
