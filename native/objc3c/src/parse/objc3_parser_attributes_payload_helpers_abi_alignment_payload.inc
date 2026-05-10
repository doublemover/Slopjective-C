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
