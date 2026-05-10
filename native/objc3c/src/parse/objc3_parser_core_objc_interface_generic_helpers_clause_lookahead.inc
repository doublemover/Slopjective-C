  bool IsGenericVarianceMarker(const std::string &text) const {
    return text == "__covariant" || text == "__contravariant" ||
           text == "__invariant";
  }

  bool LooksLikeObjcInterfaceGenericParameterClause() const {
    if (!At(TokenKind::Less)) {
      return false;
    }
    int depth = 0;
    for (std::size_t offset = 0; index_ + offset < tokens_.size(); ++offset) {
      const Token &token = tokens_[index_ + offset];
      if (token.kind == TokenKind::Less) {
        ++depth;
        continue;
      }
      if (token.kind == TokenKind::Greater) {
        --depth;
        if (depth == 0) {
          return false;
        }
        continue;
      }
      if (depth == 1 &&
          (token.kind == TokenKind::Colon ||
           (token.kind == TokenKind::Identifier &&
            IsGenericVarianceMarker(token.text)))) {
        return true;
      }
      if (token.kind == TokenKind::Eof) {
        return false;
      }
    }
    return false;
  }
